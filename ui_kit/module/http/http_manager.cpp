#include "stdafx.h"
#include "http_manager.h"
#include "shared/threads.h"
#include "shared/closure.h"

#define HTTP_MANAGER_CB_UI_THREAD	1


namespace nim_comp
{
	HTTP_RESULT HttpManager::AsynDownloadUpdateFile(std::string url, std::string& save_path, DownloadEventCb cb)
	{
		auto task = nbase::Bind(&HttpManager::Download, this, url, save_path, cb);
		nbase::ThreadManager::PostTask(shared::kThreadDatabase, task);
		return HTTP_RESULT_FAILE;
	}
	void HttpManager::StopDownload()
	{
		//cb = nullptr;
		is_cancel_ = true;
	}

	void HttpManager::Download(std::string url, std::string& save_path, DownloadEventCb cb)
	{
		HTTP_RESULT ret = DownloadIpml(url, save_path, cb);
		//cb(ret);
		return /*HTTP_RESULT_FAILE*/;
	}

	HTTP_RESULT HttpManager::DownloadIpml(std::string url, std::string& save_path, DownloadEventCb cb)
	{
		std::string ip = "122.227.241.42";
		USHORT uport = 80;

		WSADATA WsaData;
		WSAStartup(0x0202, &WsaData);

		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(uport);
		sin.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
		{
			if (cb)
			{
#if HTTP_MANAGER_CB_UI_THREAD
				shared::Post2UI(nbase::Bind(cb, DOWNLOAD_EVENT_ERR, HTTP_RESULT_FAILE_SOCK, 0));
#else
				cb(DOWNLOAD_EVENT_ERR, HTTP_RESULT_FAILE_SOCK, 0);
#endif
			}
			return HTTP_RESULT_FAILE_SOCK;
		}

		if (connect(sock, (const struct sockaddr *)&sin, sizeof(sockaddr_in)) == -1)
		{
			if (cb)
			{
#if HTTP_MANAGER_CB_UI_THREAD
				shared::Post2UI(nbase::Bind(cb, DOWNLOAD_EVENT_ERR, HTTP_RESULT_FAILE_CONNECT, 0));
#else
				cb(DOWNLOAD_EVENT_ERR, HTTP_RESULT_FAILE_CONNECT, 0);
#endif
			}
			closesocket(sock);
			return HTTP_RESULT_FAILE_CONNECT;
		}

		char *pHttpPost = "GET %s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Content-Length: %d\r\n\r\n"
			"%s";

		char cHttpPost[1024] = { 0 };
		sprintf(cHttpPost, pHttpPost, url.c_str(), ip.c_str(), uport, 0, "");

		if (send(sock, cHttpPost, strlen(cHttpPost), 0) == -1)
		{
			if (cb)
			{
#if HTTP_MANAGER_CB_UI_THREAD
				shared::Post2UI(nbase::Bind(cb, DOWNLOAD_EVENT_ERR, HTTP_RESULT_FAILE_SEND, 0));
#else
				cb(DOWNLOAD_EVENT_ERR, HTTP_RESULT_FAILE_SEND, 0);
#endif
			}
			closesocket(sock);
			return HTTP_RESULT_FAILE_SEND;
		}

		char recv_buf[1024 * 128] = { 0 };
		struct fd_set read_set;
		struct timeval timeout;
		//bool start = false;
		int number = 0;
		int write_size = 0;
		int file_size = 0;
		FILE *file = NULL;
		DOWNLOAD_EVENT download_ret = DOWNLOAD_EVENT_FINISH;
		HTTP_RESULT http_ret = HTTP_RESULT_SUCC;
		std::string tmp;
		clock_t tick_base = 0;
		clock_t tick;

		is_runing_ = true;
		is_cancel_ = false;
		update_file_status_ = 0;
		update_file_path_ = "";

		std::string file_path = save_path;
		if (file_path.empty())
		{
			std::wstring dir_w = QPath::GetNimAppDataDir(L"");
			dir_w += L"update\\";
			if (!::PathIsDirectory(dir_w.c_str()))
			{
				::CreateDirectory(dir_w.c_str(), NULL);
			}

			nbase::win32::UnicodeToMBCS(dir_w.c_str(), file_path);
			const char *file_name = strrchr(url.c_str(), '/') + 1;
			file_path += file_name;
		}
		
		do
		{
			FD_ZERO(&read_set);
			FD_SET(sock, &read_set);

			timeout.tv_sec = 0;
			timeout.tv_usec = 100000;

			switch (select(sock + 1, &read_set, NULL, NULL, &timeout))
			{
			case -1:			//select错误，退出
				is_runing_ = false;
				download_ret = DOWNLOAD_EVENT_ERR;
				http_ret = HTTP_RESULT_FAILE_SELECT;
				break;
			case 0:				//select超时，再次轮询
				/*run = false;
				ret = HTTP_RESULT_FAILE_BREAK_OFF;*/
				break;
			default:
				if (FD_ISSET(sock, &read_set)){
					number = recv(sock, recv_buf, sizeof(recv_buf), 0);
					if (number <= 0){
						is_runing_ = false;
						download_ret = DOWNLOAD_EVENT_ERR;
						http_ret = HTTP_RESULT_FAILE_RECV;
						break;
					}
					//buf = recv_buf
					if (!file)
					{
						//int len = strlen(recv_buf);
						char cLen[10] = {};
						char *start;
						char *end = strstr(recv_buf, "\r\n\r\n");
						char *L = strstr(recv_buf, "Length: ");
						if (!end || !L)
						{
							is_runing_ = false;
							download_ret = DOWNLOAD_EVENT_ERR;
							http_ret = HTTP_RESULT_FAILE_PARSE;
							break;
						}

						if (number <= (end - recv_buf + 4))		//recv_buf不够长
						{
							is_runing_ = false;
							download_ret = DOWNLOAD_EVENT_ERR;
							http_ret = HTTP_RESULT_FAILE_RECV_TOO_SHORT;
							break;
						}

						L += strlen("Length: ");

						if (end - L > 9)	//999 999 999 (1000M)
						{
							is_runing_ = false;
							download_ret = DOWNLOAD_EVENT_ERR;
							http_ret = HTTP_RESULT_FAILE_PARSE_LENGTH_TOO_LONG;
							break;
						}

						if (end <= L)
						{
							is_runing_ = false;
							download_ret = DOWNLOAD_EVENT_ERR;
							http_ret = HTTP_RESULT_FAILE_PARSE_LENGTH;
							break;
						}

						memcpy(cLen, L, end - L);
						file_size = atoi(cLen);
						if (file_size <= 0)
						{
							is_runing_ = false;
							download_ret = DOWNLOAD_EVENT_ERR;
							http_ret = HTTP_RESULT_FAILE_PARSE_LENGTH;
							break;
						}

						file = fopen(file_path.c_str(), "wb");
						//file = fopen("c:\\x2.exe", "wb");
						if (!file)
						{
							is_runing_ = false;
							download_ret = DOWNLOAD_EVENT_ERR;
							http_ret = HTTP_RESULT_FAILE_OPEN_FILE;
							break;
						}
						start = end + 4;
						int tmpsize = number - (start - recv_buf);
						fwrite(start, tmpsize, 1, file);
						write_size += tmpsize;
						if (cb)
						{
							float f = (float)file_size / 1000000;
							char cPrint[128] = {};
							sprintf(cPrint, "%.1f", f);
#if HTTP_MANAGER_CB_UI_THREAD
							shared::Post2UI(nbase::Bind(cb, DOWNLOAD_EVENT_START, HTTP_RESULT_SUCC, (WPARAM)cPrint));
#else
							cb(DOWNLOAD_EVENT_START, HTTP_RESULT_SUCC, (WPARAM)cPrint);
#endif
						}
						tick_base = clock();
						update_file_status_ = 1;
						update_file_path_ = file_path;
					}
					else
					{
						fwrite(recv_buf, number, 1, file);
						write_size += number;
						if (write_size == file_size)
						{
							is_runing_ = false;
							break;
						}

						if (write_size > file_size)
						{
							is_runing_ = false;
							download_ret = DOWNLOAD_EVENT_ERR;
							http_ret = HTTP_RESULT_FAILE_OPEN_FILE;
							break;
						}
						tick = clock();
						if (tick - tick_base > 1000)
						{
							if (cb)
							{
								//float f = (float)file_size / 1000000;
								float f1 = (float)write_size / 1000000;
								char cPrint[128] = {};
								sprintf(cPrint, "%.1f", f1);
#if HTTP_MANAGER_CB_UI_THREAD
								shared::Post2UI(nbase::Bind(cb, DOWNLOAD_EVENT_DOWNLOADING, HTTP_RESULT_SUCC, (WPARAM)cPrint));
#else
								cb(DOWNLOAD_EVENT_DOWNLOADING, HTTP_RESULT_SUCC, (WPARAM)cPrint);
#endif
							}
							tick_base = tick;
						}
					}
					break;
				}
			}
		} while (is_runing_ && !is_cancel_);
		//printf("%d", recv_str);
		if (is_cancel_ && cb)
		{
			download_ret = DOWNLOAD_EVENT_CANCEL;
			http_ret = HTTP_RESULT_SUCC;
		}
		FD_CLR(sock, &read_set);
		if ( sock!=-1 )
		{
			closesocket(sock);
		}
		WSACleanup();

		if (file)
		{
			fclose(file);
		}

		if (http_ret == HTTP_RESULT_SUCC)
		{
			save_path = file_path;
		}

		if (cb)
		{
			if (download_ret == DOWNLOAD_EVENT_FINISH && http_ret == HTTP_RESULT_SUCC)
			{
				update_file_status_ = 9;
			}
#if HTTP_MANAGER_CB_UI_THREAD
			shared::Post2UI(nbase::Bind(cb, download_ret, http_ret, 0));
#else
			cb(download_ret, http_ret, 0);
#endif
		}

		return http_ret;
	}
}
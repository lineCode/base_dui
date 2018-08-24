#include "stdafx.h"
#include <WinSock2.h>
#include "http_interface.h"
#include "module/login/login_manager.h"
#include "base/network/network_util.h"

#define MODE_QCLOUD 1		//腾讯云上的pgsql

#if MODE_QCLOUD
#if 11
#define INTERFACE_ADDR		"www.yiyuntong.com"	
#define INTERFACE_PORT		80

#define ERP_INTERFACE_ADDR	"www.yiyuntong.com"	
#define ERP_INTERFACE_PORT	80

#define WORK_CENTER_INTERFACE_ADDR	"www.yiyuntong.com"
#define WORK_CENTER_INTERFACE_PORT	80
#else
#define INTERFACE_ADDR		"www.yiyuntong.net"	
#define INTERFACE_PORT		80

#define ERP_INTERFACE_ADDR	"www.yiyuntong.net"	
#define ERP_INTERFACE_PORT	80

#define WORK_CENTER_INTERFACE_ADDR	"www.yiyuntong.net"
#define WORK_CENTER_INTERFACE_PORT	80
#endif
#elif 1	//冯

#define INTERFACE_ADDR		"192.168.5.25"			
#define INTERFACE_PORT		8080

#define ERP_INTERFACE_ADDR	"192.168.5.25"		
#define ERP_INTERFACE_PORT	8080		

#define WORK_CENTER_INTERFACE_ADDR	"www.weigongzi.net"
#define WORK_CENTER_INTERFACE_IP	"114.55.55.62"		//微工资ip
#define WORK_CENTER_INTERFACE_PORT	80

#else	//阿里云

#define INTERFACE_IP		"101.37.173.42"			
#define INTERFACE_PORT		8081

#define ERP_INTERFACE_IP	"101.37.173.42"				//供应链
#define ERP_INTERFACE_PORT	8080		

#define WORK_CENTER_INTERFACE_ADDR	"www.weigongzi.net"
#define WORK_CENTER_INTERFACE_IP	"114.55.55.62"		//微工资ip
#define WORK_CENTER_INTERFACE_PORT	80

#endif

//测试库
//"122.227.209.181"
//7000

std::string http_Send(std::string reque, std::string param)
{
	std::string Login = "WeiweiInterfaceController/loginIM";
	std::string getFriendList = "WeiweiInterfaceController/getFriendTreeList";
	std::string getCustomerList = "WeiweiInterfaceController/searchOdbcByCust";		//客服客户用户列表（客户公司列表由用户分类获得）
	std::string Personal_info = "WeiweiInterfaceController/changeDetail";
	std::string Password_set = "WeiweiInterfaceController/changePwd";
	std::string SearchVVOdbc = "WeiweiInterfaceController/searchVvOdbc";			//查询我的客户公司的数据库列表
	//std::string getCust =		"sap/getCust";					//我的客户(供应链管理)
	//std::string getSupplier =	"sap/getSupplier";				//我的供应商(供应链管理)
	std::string getFriendTreeListBydbcid = "sap/getFriendTreeListBydbcid";	//客户/供应商的联系人列表(供应链管理)
	std::string getWorkCenterInfo = "sys/tool/waitdonum";
	std::string InsertSqlOperation = "WeiweiInterfaceController/saveSqlLog";	//插入数据库操作记录
	std::string server_addr = INTERFACE_ADDR;
	unsigned short server_port = INTERFACE_PORT;
	if (/*!strcmp(reque.c_str(), getCust.c_str()) || !strcmp(reque.c_str(), getSupplier.c_str()) || */!strcmp(reque.c_str(), getFriendTreeListBydbcid.c_str()))
	{
		server_addr = ERP_INTERFACE_ADDR;
		server_port = ERP_INTERFACE_PORT;
	}
	char cUrl[256] = { 0 };
	char cData[1024] = { 0 };
	if (!strcmp(reque.c_str(), getWorkCenterInfo.c_str()))
	{
		server_addr = WORK_CENTER_INTERFACE_ADDR;
		server_port = WORK_CENTER_INTERFACE_PORT;
		sprintf(cUrl, "http://%s/wd3/%s", server_addr.c_str(), reque.c_str());
	}
	else 
	{
		sprintf(cUrl, "http://%s/weiwei/%s", server_addr.c_str(), reque.c_str());
	}
	sprintf(cData, "%s", param.c_str());

	WSADATA WsaData;
	WSAStartup(0x0101, &WsaData);

	struct hostent* host_addr = gethostbyname(server_addr.c_str());
	if (host_addr == NULL)
	{
		WSACleanup();
		return "";
	}

	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(server_port);
	sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		WSACleanup();
		return "";
	}

	if (connect(sock, (const struct sockaddr *)&sin, sizeof(sockaddr_in)) == -1)
	{
		closesocket(sock);
		WSACleanup();
		return "";
	}

	char *pHttpPost = "POST %s HTTP/1.0\r\n"
		"Host: %s:%d\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: %d\r\n\r\n"
		"%s";

	char strHttpPost[1024] = { 0 };
	sprintf(strHttpPost, pHttpPost, cUrl, server_addr.c_str(), server_port, strlen(cData), cData);

	if (send(sock, strHttpPost, strlen(strHttpPost), 0) == -1)
	{
		closesocket(sock);
		WSACleanup();
		return "";
	}

	std::string buf, tmp_buf;
	if (!strcmp(reque.c_str(), Login.c_str()) || !strcmp(reque.c_str(), getFriendList.c_str()) || !strcmp(reque.c_str(), getCustomerList.c_str()) || !strcmp(reque.c_str(), SearchVVOdbc.c_str())
		|| !strcmp(reque.c_str(), getFriendTreeListBydbcid.c_str()) /*|| !strcmp(reque.c_str(), getCust.c_str()) || !strcmp(reque.c_str(), getSupplier.c_str())*/ || !strcmp(reque.c_str(), getWorkCenterInfo.c_str()))
	{
		char recv_str[4096] = { 0 };
		struct fd_set fdreads;
		struct timeval timeout;
		bool body = false, finish = false;
		int recv_len, body_len = 0/*, write_pos = 0*/;
		std::string tmp;

		//int nBegin, nEnd;

		//int all_recv_size = 0;

		while (!finish)
		{
			FD_ZERO(&fdreads);
			FD_SET(sock, &fdreads);

			timeout.tv_sec = 0;
			timeout.tv_usec = 20000;

			switch (select(0, &fdreads, NULL, NULL, &timeout))
			{
			case -1://select错误，退出
				break;
			case 0: //select超时，再次轮询
				break;
			default:
				if (FD_ISSET(sock, &fdreads))
				{
					ZeroMemory(recv_str, sizeof(recv_str));
					tmp_buf.clear();

					recv_len = recv(sock, recv_str, sizeof(recv_str)-1, 0);
					if (recv_len <= 0 || recv_len > sizeof(recv_str)-1)
					{
						finish = true;
						break;
					}

					recv_str[recv_len] = 0;
					if (!body)
					{
						tmp += recv_str;
						int begin_pos = tmp.find("\r\n\r\n");
						if (begin_pos > -1)
						{
							body = true;

							std::string header = tmp.substr(0, begin_pos);
							int content_length_pos = header.find("Content-Length:");
							if (content_length_pos > 0)
							{
								int content_length_end = header.find("\r\n", content_length_pos);
								std::string strbody_len = header.substr(content_length_pos + strlen("Content-Length:"), content_length_end - content_length_pos - strlen("Content-Length:"));

								body_len = atoi(strbody_len.c_str());
							}

							if (recv_len > begin_pos + 4)
							{
								buf = recv_str + (begin_pos + 4);

								//int nBegin = buf.find_first_of("{");	//对于没有ContentLength的接口, 会有类似'238\r\n'夹杂
								//if (nBegin > 0)
								//{
								//	buf = buf.substr(nBegin);
								//	if (body_len >= nBegin)
								//	{
								//		body_len -= nBegin;
								//	}
								//}
							}

							if (body_len > 0 && buf.length() >= body_len)
							{
								finish = true;
								break;
							}
						}
					}
					else
					{
						tmp_buf = recv_str;

						int nBegin = 0, nEnd = 0;
						/*nBegin  = tmp_buf.find(url);
						int nEnd = tmp_buf.find(data);
						if (nBegin != -1){
						tmp_buf.erase(nBegin - 5, nEnd - nBegin + 8 + strlen(data_));
						}*/

						nBegin = tmp_buf.find("\r\n");
						if (nBegin > -1)
						{
							int nBegin2 = tmp_buf.find("\r\n", nBegin + 2);
							if (nBegin2 > -1)
							{
								tmp_buf = tmp_buf.erase(nBegin, nBegin2 - nBegin + 2);
								nBegin = 0;
							}
						}


						buf += tmp_buf;
						if (body_len > 0 && buf.length() >= body_len)
						{
							finish = true;
							break;
						}
					}
				}
				else
					break;
			}

		}
		FD_CLR(sock, &fdreads);
	}
	else if (!strcmp(reque.c_str(), Personal_info.c_str()) || !strcmp(reque.c_str(), Password_set.c_str()) || !strcmp(reque.c_str(), InsertSqlOperation.c_str()))
	{
		char recv_str[4096] = { 0 };
		if (recv(sock, recv_str, sizeof(recv_str)-1, 0) == -1)
		{
			closesocket(sock);
			WSACleanup();
			return "";
		}
		buf = recv_str;
		QLOG_APP(L"recv: {0}") << recv_str;
	}
//end:
	closesocket(sock);
	WSACleanup();

	return buf;
}

std::string http_Send2(std::string reque, std::string param)
{
	std::string getPLMToken = "cert/v1/tickets";
	std::string getQDFWInfo = "home/qidian/agreement";
	std::string inviteFriend = "home/sys/weisendsms";

	std::string server_addr = INTERFACE_ADDR;
	unsigned short server_port = INTERFACE_PORT;

	char cUrl[1024] = { 0 };
	char cData[1024] = { 0 };
#if 0
	if (reque == getPLMToken)
	{
		param = "username=VGLUTANN&password=2766EBB70B4B33604808D6493BC8C239";
	}
#endif
	sprintf(cData, "%s", param.c_str());

	sprintf(cUrl, "http://%s/%s?%s", server_addr.c_str(), reque.c_str(), param.c_str());

	WSADATA WsaData;
	WSAStartup(0x0101, &WsaData);

	struct hostent* host_addr = gethostbyname(server_addr.c_str());
	if (host_addr == NULL)
	{
		WSACleanup();
		return "";
	}

	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(server_port);
	sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		WSACleanup();
		return "";
	}

	if (connect(sock, (const struct sockaddr *)&sin, sizeof(sockaddr_in)) == -1)
	{
		closesocket(sock);
		WSACleanup();
		return "";
	}

	char *pHttpPost = "";
	if (getPLMToken == reque)
	{
		pHttpPost = "POST %s HTTP/1.0\r\n"
			"Host: %s:%d\r\n"
			"Content-Type: text/html;charset=UTF-8\r\n"	//"Content-Type: application/x-www-form-urlencoded\r\n"
			"Content-Length: %d\r\n\r\n"
			"%s";
	}
	else if (getQDFWInfo == reque)
	{
		pHttpPost = "GET %s HTTP/1.0\r\n"
			"Host: %s:%d\r\n"
			"Content-Type: text/html;charset=UTF-8\r\n"
			"Content-Length: %d\r\n\r\n"
			"%s";
	}
	else if (inviteFriend == reque)
	{
		pHttpPost = "GET %s HTTP/1.0\r\n"
			"Host: %s:%d\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Content-Length: %d\r\n\r\n"
			"%s";
		cData[0] = 0;
	}
	
	char strHttpPost[1024] = { 0 };
	sprintf(strHttpPost, pHttpPost, cUrl, server_addr.c_str(), server_port, strlen(cData), cData);

	if (send(sock, strHttpPost, strlen(strHttpPost), 0) == -1)
	{
		closesocket(sock);
		WSACleanup();
		return "";
	}

	std::string buf, tmp_buf;
	if (!strcmp(reque.c_str(), getPLMToken.c_str()) || !strcmp(reque.c_str(), getQDFWInfo.c_str()) || !strcmp(reque.c_str(), inviteFriend.c_str()))
	{
		char recv_str[4096] = { 0 };
		//char *pBody = NULL;
		struct fd_set fdreads;
		struct timeval timeout;
		bool body = false, finish = false;
		int recv_len, body_len = 0/*, write_pos = 0*/;
		std::string tmp;

		//int nBegin, nEnd;

		//int all_recv_size = 0;

		while (!finish)
		{
			FD_ZERO(&fdreads);
			FD_SET(sock, &fdreads);

			timeout.tv_sec = 0;
			timeout.tv_usec = 20000;

			switch (select(0, &fdreads, NULL, NULL, &timeout))
			{
			case -1://select错误，退出
				break;
			case 0: //select超时，再次轮询
				break;
			default:
				if (FD_ISSET(sock, &fdreads))
				{
					ZeroMemory(recv_str, sizeof(recv_str));
					tmp_buf.clear();

					recv_len = recv(sock, recv_str, sizeof(recv_str)-1, 0);
					if (recv_len <= 0 || recv_len > sizeof(recv_str)-1)
					{
						finish = true;
						break;
					}

					recv_str[recv_len] = 0;
					if (!body)
					{
						tmp += recv_str;
						int begin_pos = tmp.find("\r\n\r\n");
						if (begin_pos > -1)
						{
							body = true;

							std::string header = tmp.substr(0, begin_pos);
							int content_length_pos = header.find("Content-Length:");
							if (content_length_pos > 0)
							{
								int content_length_end = header.find("\r\n", content_length_pos);
								std::string strbody_len = header.substr(content_length_pos + strlen("Content-Length:"), content_length_end - content_length_pos - strlen("Content-Length:"));

								body_len = atoi(strbody_len.c_str());
							}

							if (recv_len > begin_pos + 4)
							{
								buf = recv_str + (begin_pos + 4);

								//int nBegin = buf.find_first_of("{");	//对于没有ContentLength的接口, 会有类似'238\r\n'夹杂
								//if (nBegin > 0)
								//{
								//	buf = buf.substr(nBegin);
								//	if (body_len >= nBegin)
								//	{
								//		body_len -= nBegin;
								//	}
								//}
							}

							if (body_len > 0 && buf.length() >= body_len)
							{
								finish = true;
								break;
							}
						}
					}
					else
					{
						tmp_buf = recv_str;

						int nBegin = 0, nEnd = 0;
						/*nBegin  = tmp_buf.find(url);
						int nEnd = tmp_buf.find(data);
						if (nBegin != -1){
						tmp_buf.erase(nBegin - 5, nEnd - nBegin + 8 + strlen(data_));
						}*/

						nBegin = tmp_buf.find("\r\n");
						if (nBegin > -1)
						{
							int nBegin2 = tmp_buf.find("\r\n", nBegin + 2);
							if (nBegin2 > -1)
							{
								tmp_buf = tmp_buf.erase(nBegin, nBegin2 - nBegin + 2);
								nBegin = 0;
							}
						}


						buf += tmp_buf;
						if (body_len > 0 && buf.length() >= body_len)
						{
							finish = true;
							break;
						}
					}
				}
				else
					break;
			}

		}
		FD_CLR(sock, &fdreads);
	}
//end:
	closesocket(sock);
	WSACleanup();

	return buf;
}

std::string http_Send3(std::string reque, std::string param)
{
	std::string ParamSeed = "GNFunction=GetAuthSeed";

	std::string server_ip = "122.227.241.40";
	unsigned short server_port = 8019;

	char cUrl[256] = { 0 };
	char cData[1024] = { 0 };

	//sprintf(cData, "%s", param.c_str());
	if (param == ParamSeed)
	{
		sprintf(cUrl, "%s?%s", reque.c_str(), param.c_str());
	}
	else
	{
		sprintf(cUrl, "%s", reque.c_str());
		sprintf(cData, "%s", param.c_str());
	}
		
	WSADATA WsaData;
	WSAStartup(0x0101, &WsaData);

	struct hostent* host_addr = gethostbyname(server_ip.c_str());
	if (host_addr == NULL)
	{
		WSACleanup();
		return "";
	}

	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(server_port);
	sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		WSACleanup();
		return "";
	}

	if (connect(sock, (const struct sockaddr *)&sin, sizeof(sockaddr_in)) == -1)
	{
		closesocket(sock);
		WSACleanup();
		return "";
	}

	char *pHttpPost = "GET %s HTTP/1.1\r\n"
		"Host: %s:%d\r\n"
		"Connection: keep-alive\r\n"
		"Accept: */*"
		"Content-Type: application/x-www-form-urlencoded\r\n\r\n"
		"%s";

	char strHttpPost[1024] = { 0 };
	sprintf(strHttpPost, pHttpPost, cUrl, server_ip.c_str(), server_port, cData);

	if (send(sock, strHttpPost, strlen(strHttpPost), 0) == -1)
	{
		closesocket(sock);
		WSACleanup();
		return "";
	}

	std::string buf, tmp_buf;
	//if (!strcmp(reque.c_str(), getPLMToken.c_str()))
	{
		char recv_str[4096] = { 0 };
		//char *pBody = NULL;
		struct fd_set fdreads;
		struct timeval timeout;
		bool body = false, finish = false;
		int recv_len, body_len = 0/*, write_pos = 0*/;
		std::string tmp;

		//int nBegin, nEnd;

		//int all_recv_size = 0;

		while (!finish)
		{
			FD_ZERO(&fdreads);
			FD_SET(sock, &fdreads);

			timeout.tv_sec = 0;
			timeout.tv_usec = 20000;

			switch (select(0, &fdreads, NULL, NULL, &timeout))
			{
			case -1://select错误，退出
				break;
			case 0: //select超时，再次轮询
				break;
			default:
				if (FD_ISSET(sock, &fdreads))
				{
					ZeroMemory(recv_str, sizeof(recv_str));
					tmp_buf.clear();

					recv_len = recv(sock, recv_str, sizeof(recv_str)-1, 0);
					if (recv_len <= 0 || recv_len > sizeof(recv_str)-1)
					{
						finish = true;
						break;
					}

					recv_str[recv_len] = 0;
					if (!body)
					{
						tmp += recv_str;
						int begin_pos = tmp.find("\r\n\r\n");
						if (begin_pos > -1)
						{
							body = true;

							std::string header = tmp.substr(0, begin_pos);
							int content_length_pos = header.find("Content-Length:");
							if (content_length_pos > 0)
							{
								int content_length_end = header.find("\r\n", content_length_pos);
								std::string strbody_len = header.substr(content_length_pos + strlen("Content-Length:"), content_length_end - content_length_pos - strlen("Content-Length:"));

								body_len = atoi(strbody_len.c_str());
							}

							if (recv_len > begin_pos + 4)
							{
								buf = recv_str + (begin_pos + 4);

								//int nBegin = buf.find_first_of("{");	//对于没有ContentLength的接口, 会有类似'238\r\n'夹杂
								//if (nBegin > 0)
								//{
								//	buf = buf.substr(nBegin);
								//	if (body_len >= nBegin)
								//	{
								//		body_len -= nBegin;
								//	}
								//}
							}

							if (body_len > 0 && buf.length() >= body_len)
							{
								finish = true;
								break;
							}
						}
						else	//没有httpheader
						{
							if (param == ParamSeed)
							{
								if (recv_len > 1 && buf.empty())
								{
									buf = recv_str;
								}
								else
								{
									buf.clear();
								}
							}
							else
							{
								buf += recv_str;
							}
						}
					}
					else
					{
						tmp_buf = recv_str;

						int nBegin = 0, nEnd = 0;
						/*nBegin  = tmp_buf.find(url);
						int nEnd = tmp_buf.find(data);
						if (nBegin != -1){
						tmp_buf.erase(nBegin - 5, nEnd - nBegin + 8 + strlen(data_));
						}*/

						nBegin = tmp_buf.find("\r\n");
						if (nBegin > -1)
						{
							int nBegin2 = tmp_buf.find("\r\n", nBegin + 2);
							if (nBegin2 > -1)
							{
								tmp_buf = tmp_buf.erase(nBegin, nBegin2 - nBegin + 2);
								nBegin = 0;
							}
						}


						buf += tmp_buf;
						if (body_len > 0 && buf.length() >= body_len)
						{
							finish = true;
							break;
						}
					}
				}
				else
					break;
			}

		}
		FD_CLR(sock, &fdreads);
	}
//end:
	closesocket(sock);
	WSACleanup();

	return buf;
}

#pragma once

//仅支持downloads

namespace nim_comp
{
	typedef std::function<void(int download_ret, int http_ret, WPARAM wp)> DownloadEventCb;
	enum DOWNLOAD_EVENT
	{
		DOWNLOAD_EVENT_FINISH = 0,
		DOWNLOAD_EVENT_ERR,
		DOWNLOAD_EVENT_START,
		DOWNLOAD_EVENT_DOWNLOADING,		//正在下载
		DOWNLOAD_EVENT_SUSPECT,			//暂停
		DOWNLOAD_EVENT_CONTINUE,		//暂停后继续
		DOWNLOAD_EVENT_CANCEL,			//
		
	};
	enum HTTP_RESULT
	{
		HTTP_RESULT_SUCC = 0,
		HTTP_RESULT_FAILE,
		HTTP_RESULT_FAILE_SOCK,
		HTTP_RESULT_FAILE_CONNECT,
		HTTP_RESULT_FAILE_SEND,
		HTTP_RESULT_FAILE_SELECT,
		HTTP_RESULT_FAILE_RECV,
		HTTP_RESULT_FAILE_RECV_TOO_SHORT,
		HTTP_RESULT_FAILE_TIMEOUT,						//下载超时（未开始下载）
		HTTP_RESULT_FAILE_BREAK_OFF,					//下载中断（下到一半）
		HTTP_RESULT_FAILE_RESOURCE,						//资源错误
		HTTP_RESULT_FAILE_OPEN_FILE,			//
		HTTP_RESULT_FAILE_PARSE,				//
		HTTP_RESULT_FAILE_PARSE_LENGTH,					//解析Length出错
		HTTP_RESULT_FAILE_PARSE_LENGTH_TOO_LONG,		//解析Length太长
		HTTP_RESULT_FAILE_FILE_SIZE_OVERFLOW,			//下载的文件总长度溢出
		HTTP_RESULT_MAX
	};

	class HttpManager : public nbase::SupportWeakCallback
	{
	public:
		SINGLETON_DEFINE(HttpManager);

		HttpManager() :is_runing_(false), is_cancel_(false), update_file_status_(0){};
		~HttpManager(){};
		int GetUpdateFileStatus(){ return update_file_status_; };
		std::string GetUpdateFilePath(){ return update_file_path_; };
	public:
		HTTP_RESULT AsynDownloadUpdateFile(std::string url, std::string& save_path, DownloadEventCb cb);
		void StopDownload();
	private:
		void Download(std::string url, std::string& save_path, DownloadEventCb cb);
		HTTP_RESULT DownloadIpml(std::string url, std::string& save_path, DownloadEventCb cb);
	private:
		bool is_runing_;		//这两个控制线程
		bool is_cancel_;

		int		update_file_status_;		//0-没有；1-没完成；9-完成
		std::string	update_file_path_;			//
	};
}
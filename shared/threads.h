#ifndef SHARED_THREADS_H_
#define SHARED_THREADS_H_

//#include <functional>
#include "log.h"

namespace shared
{
// thread ids
enum ThreadId
{
	kThreadUI,				//UI线程（主线程）
	kThreadDatabase,		//写DB线程
	kThreadGlobalMisc,		//全局Misc线程（比如：打开url）
	kThreadDuilibHelper,	//Duilib专用，UI辅助线程
	kThreadLiveStreaming,	//直播线程
	kThreadScreenCapture,	//取屏线程
	kThreadVideoPlay,		//视频播放线程
};

//杂项线程，处理一些琐事
class MiscThread : public nbase::FrameworkThread
{
public:
	MiscThread(shared::ThreadId thread_id, const char *name) : FrameworkThread(name), thread_id_(thread_id){};
	~MiscThread(void){};
private:
	virtual void Init() override{ nbase::ThreadManager::RegisterThread(thread_id_);};
	virtual void Cleanup() override{
		nbase::ThreadManager::UnregisterThread();
		QLOG_APP(L"MiscThread Cleanup");
	};
private:
	shared::ThreadId thread_id_;
};

//db线程
class DBThread : public nbase::FrameworkThread
{
public:
	DBThread(shared::ThreadId thread_id, const char *name) : FrameworkThread(name), thread_id_(thread_id){};
	~DBThread(void){};
private:
	virtual void Init() override{ nbase::ThreadManager::RegisterThread(thread_id_); };
	virtual void Cleanup() override{
		nbase::ThreadManager::UnregisterThread();
		QLOG_APP(L"DBThread Cleanup");
	};
private:
	shared::ThreadId thread_id_;
};

//视频播放线程
class VideoThread : public nbase::FrameworkThread
{
public:
	VideoThread(shared::ThreadId thread_id, const char *name) : FrameworkThread(name), thread_id_(thread_id){};
	~VideoThread(void){};
private:
	virtual void Init() override{ nbase::ThreadManager::RegisterThread(thread_id_); };
	virtual void Cleanup() override{
		nbase::ThreadManager::UnregisterThread();
		QLOG_APP(L"VideoThread Cleanup");
	};
private:
	shared::ThreadId thread_id_;
};




/*
* 主线程（UI线程）类，继承nbase::FrameworkThread
*/

typedef std::function<void()> MainThreadEventCallBack;

class MainThreadBase : public nbase::FrameworkThread
{
public:
	MainThreadBase() : nbase::FrameworkThread("MainThread") {}
	virtual ~MainThreadBase() {}

	/**
	* 程序崩溃时的处理函数
	* @param[in] ret	用户的处理方式（关闭或重新运行程序）
	* @return void	无返回值
	*/
	void RegMainThreadEvent(MainThreadEventCallBack init_event, MainThreadEventCallBack end_event, MainThreadEventCallBack err_event);

private:
	virtual void Init() override;
	virtual void Cleanup() override;

	/**
	* 主线程开始循环前，misc线程和db线程先开始循环
	* @return void	无返回值
	*/
	virtual void PreMessageLoop() = 0;

	/**
	* 主线程结束循环前，misc线程和db线程先结束循环
	* @return void	无返回值
	*/
	virtual void PostMessageLoop() = 0;
private:
	/*std::unique_ptr<MiscThread>	misc_thread_;
	std::unique_ptr<MiscThread>	screen_capture_thread_;
	std::unique_ptr<DBThread>	db_thread_;
	std::unique_ptr<MiscThread>	video_play_thread_;*/

	MainThreadEventCallBack init_event_cb_;
	MainThreadEventCallBack end_event_cb_;
	MainThreadEventCallBack err_event_cb_;
};

}

#endif // SHARED_THREADS_H_

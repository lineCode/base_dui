#ifndef _MAIN_THREAD_H_
#define _MAIN_THREAD_H_

#include "../shared/threads.h"

class MainThread : public shared::MainThreadBase
{
public:
	MainThread(){};
	virtual ~MainThread(){};

private:
	/**
	* 主线程开始循环前，misc线程和db线程先开始循环
	* @return void	无返回值
	*/
	virtual void PreMessageLoop();

	/**
	* 主线程结束循环前，misc线程和db线程先结束循环
	* @return void	无返回值
	*/
	virtual void PostMessageLoop();
private:
	std::unique_ptr<shared::MiscThread>	misc_thread_;
	std::unique_ptr<shared::MiscThread>	screen_capture_thread_;
	std::unique_ptr<shared::DBThread>	db_thread_;
};

#endif // _MAIN_THREAD_H_

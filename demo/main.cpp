// Demo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "shared/threads.h"
#include "gui/login/login_form.h"
#include "gui/main/main_form.h"
#include "module/init/init_manager.h"
#include "module/window/windows_manager.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	nim_comp::InitManager::GetInstance()->Init();

	shared::MainThreadEventCallBack init_event_cb = [](){
		nim_comp::WindowsManager::GetInstance()->SingletonShow<LoginForm>(LoginForm::kClassName);
		//nim_comp::WindowsManager::GetInstance()->SingletonShow<MainForm>(MainForm::kClassName);
		return;
	};

	shared::MainThread main_thread;
	main_thread.RegMainThreadEvent(init_event_cb, nullptr, nullptr);
	main_thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

	return 0;
}


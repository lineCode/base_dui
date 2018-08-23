// player.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "gui/main/player_main_form.h"
#include "thread/main_thread.h"
#include "shared/util.h"
#include "module/window/windows_manager.h"

int _tmain(int argc, _TCHAR* argv[])
{
	dui::UIManager::SetInstance(GetModuleHandle(NULL));
	dui::UIManager::SetGlobalResDir((dui::UIManager::GetInstancePath() + L"res/").c_str());

	shared::MainThreadEventCallBack init_event_cb = [](){
		nim_comp::WindowsManager::GetInstance()->SingletonShow<PlayerMainForm>(PlayerMainForm::kClassName);
		return;
	};

	MainThread main_thread;
	main_thread.RegMainThreadEvent(init_event_cb, nullptr, nullptr);
	main_thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

	return 0;
}


// Demo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "shared/threads.h"
#include "gui/login/login_form.h"
#include "module/init/init_manager.h"

using namespace DuiLib;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	nim_comp::InitManager::GetInstance()->Init();

	CPaintManagerUI::SetInstance(GetModuleHandle(NULL));
	CPaintManagerUI::SetResourcePath((DuiLib::CPaintManagerUI::GetInstancePath() + _T("res\\")).c_str());

	shared::MainThreadEventCallBack init_event_cb = [](){
		printf("shared::MainThreadEventCallBack init_event_cb");
		LoginForm *login_form = new LoginForm;
		login_form->Create(NULL, LoginForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
		login_form->CenterWindow();
		login_form->ShowWindow();
		return;
	};

	shared::MainThread main_thread;
	main_thread.RegMainThreadEvent(init_event_cb, nullptr, nullptr);
	main_thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

	return 0;
}


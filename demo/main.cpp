// Demo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "thread/main_thread.h"
#include "shared/util.h"
#include "gui/login/login_form.h"
#include "gui/main/main_form.h"
#include "module/init/init_manager.h"
#include "module/window/windows_manager.h"

using namespace std;

void AddDllPath()
{
	TCHAR path_envirom[4096] = { 0 };
	GetEnvironmentVariable(L"path", path_envirom, 4096);

	std::wstring path = QPath::GetAppPath();

	path += L"dll";

	if (!nbase::FilePathIsExist(path, true))
	{
		MessageBox(NULL, L"AddDllPath faile!", L"提示", MB_OK);
		exit(0);
	}
	std::wstring new_envirom = path_envirom;
	new_envirom.append(L";");
	new_envirom.append(path);
	SetEnvironmentVariable(L"path", new_envirom.c_str());
}

int _tmain(int argc, _TCHAR* argv[])
{
	AddDllPath();

	HRESULT hr = ::OleInitialize(NULL);
	if (FAILED(hr))
		return 0;

	nim_comp::InitManager::GetInstance()->Init();

	shared::MainThreadEventCallBack init_event_cb = [](){
		//nim_comp::WindowsManager::GetInstance()->SingletonShow<LoginForm>(LoginForm::kClassName);
		nim_comp::WindowsManager::GetInstance()->SingletonShow<MainForm>(MainForm::kClassName);
		return;
	};

	MainThread main_thread;
	main_thread.RegMainThreadEvent(init_event_cb, nullptr, nullptr);
	main_thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

	::OleUninitialize();

	return 0;
}


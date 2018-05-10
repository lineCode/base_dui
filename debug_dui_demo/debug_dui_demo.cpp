// debug_dui_demo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "login_form.h"

#pragma comment(lib, "../libs/duilib_d.lib")


int _tmain(int argc, _TCHAR* argv[])
{
	string s1 = "123456789012345678901234567890123456789012345678901234567890";
	wstring s2 = L"1234567890123456789012345678901234567890123456789012345678901234567890";
	int size = sizeof(std::string);
	size = sizeof(std::wstring);
	size = sizeof(s1);
	size = sizeof(s2);
	dui::CPaintManager::SetInstance(GetModuleHandle(NULL));
	dui::CPaintManager::SetGlobalResDir((dui::CPaintManager::GetInstancePath() + L"res/").c_str());

	LoginForm *form = new LoginForm;
	form->Create(NULL, LoginForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
	form->CenterWindow();
	form->ShowWindow();

	dui::CPaintManager::MessageLoop();

	return 0;
}


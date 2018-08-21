// player.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "gui/main/player_main_form.h"


int _tmain(int argc, _TCHAR* argv[])
{
	dui::UIManager::SetInstance(GetModuleHandle(NULL));
	dui::UIManager::SetGlobalResDir((dui::UIManager::GetInstancePath() + L"res/").c_str());

	PlayerMainForm *form = new PlayerMainForm;
	form->Create(NULL, PlayerMainForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
	form->CenterWindow();
	form->ShowWindow();

	dui::UIManager::MessageLoop();

	return 0;
}


#include "StdAfx.h"
#include "init_manager.h"
#include "module/db/user_db.h"


namespace nim_comp
{

void InitManager::Init(LPCTSTR dui_res_dir)
{
	UserDB::GetInstance()->Load();

	dui::CPaintManager::SetInstance(GetModuleHandle(NULL));
	dui::CPaintManager::SetGlobalResDir((dui::CPaintManager::GetInstancePath() + dui_res_dir).c_str());
}

void InitManager::Clean()
{
	UserDB::GetInstance()->Close();
	
}

}


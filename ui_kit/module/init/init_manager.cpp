#include "StdAfx.h"
#include "gui/emoji/emoji_info.h"
#include "module/db/user_db.h"

#include "init_manager.h"


namespace nim_comp
{

void InitManager::Init(LPCTSTR dui_res_dir)
{
	UserDB::GetInstance()->Load();

	dui::UIManager::SetInstance(GetModuleHandle(NULL));
	dui::UIManager::SetGlobalResDir((dui::UIManager::GetInstancePath() + dui_res_dir).c_str());

	//加载聊天表情
	LoadEmoji();
}

void InitManager::Clean()
{
	UserDB::GetInstance()->Close();
	
}

}


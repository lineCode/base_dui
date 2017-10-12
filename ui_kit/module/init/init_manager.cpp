#include "StdAfx.h"
#include "init_manager.h"
#include "module/db/user_db.h"


namespace nim_comp
{

void InitManager::Init(bool enable_subscribe_event)
{
	UserDB::GetInstance()->Load();
}

void InitManager::Clean()
{
	
}

}


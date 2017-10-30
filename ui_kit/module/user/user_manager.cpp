#include "stdafx.h"
#include <fstream>
#include "base/memory/singleton.h"
#include "shared/auto_unregister.h"
#include "user_define.h"

#include "user_manager.h"

namespace nim_comp
{
void UserManager::InvokeRegisterAccount(const std::string &username, const std::string &password, const std::string &nickname, const OnRegisterAccountCallback& cb)
{
	InvokeRegisterAccount(username, password, nickname, cb);
}

NIMFriendFlag UserManager::GetUserType(const std::string &id)
{
	return GetUserType(id);
}

bool UserManager::GetUserInfo(const std::string &account, UserNameCard &info)
{
	return GetUserInfo(account, info);
}

void UserManager::GetUserInfos(const std::list<std::string>& ids, std::list<UserNameCard>& uinfos)
{
	//nim_comp::UserService::GetInstance()->GetUserInfos(ids, uinfos);
}

const std::map<std::string, UserNameCard>& UserManager::GetAllUserInfos()
{
	return GetAllUserInfos();
}

std::wstring UserManager::GetUserName(const std::string &id, bool alias_prior/* = true */)
{
#if 0
	return nim_comp::UserService::GetInstance()->GetUserName(id, alias_prior);
#else
	return L"";
#endif
}

std::wstring UserManager::GetFriendAlias(const std::string & id)
{
	return GetFriendAlias(id);
}

UnregisterCallback UserManager::RegFriendListChange(const OnFriendListChangeCallback & callback)
{
	return RegFriendListChange(callback);
}

UnregisterCallback UserManager::RegUserInfoChange(const OnUserInfoChangeCallback& callback)
{
	return RegUserInfoChange(callback);
}

UnregisterCallback UserManager::RegMiscUInfoChange(const OnUserInfoChangeCallback & callback)
{
	return RegMiscUInfoChange(callback);
}

void UserManager::DoLoadFriends(const OnGetUserInfoCallback& cb)
{
	OnGetUserInfoCallback *pcb = new OnGetUserInfoCallback(cb);
	StdClosure task = std::bind(&UserManager::DoLoadFriendsAsyn, this, pcb);
	shared::Post2GlobalMisc(task);
}

void UserManager::DoLoadFriendsAsyn(const OnGetUserInfoCallback* pcb)
{
	printf("UserManager::DoLoadFriendsAsyn in thread GlobalMisc\n");

	//Sleep(1000);

	if (pcb)
	{
		if (*pcb)
		{
			Json::Reader reader;
			Json::Value root, value;
			std::string file, document;
			nbase::win32::UnicodeToMBCS(QPath::GetAppPath(), file);
			file += "other/friendlist.txt";
			ifstream is(file.c_str()); 

			if (is && reader.parse(is, root) && root.isObject() && root["code"].isInt() )
			{
				if (root["code"] != 120000)
				{
					printf("root[\"code\"] != 120000\n");
					goto faile;
				}
				if (!root["object"].isObject() || !root["object"]["userList"].isArray())
				{
					printf("Json::Reader parse, data error\n");
					goto faile;
				}
				value = root["object"]["userList"];
				int len = value.size();
				std::list<UserNameCard> list;
				UserNameCard card;
				for (int i = 0; i < len; i++)
				{
					card.SetAccId(value[i]["netEaseId"].asString());
					card.SetName(value[i]["friendName"].asString());
					card.SetIconUrl(value[i]["userPhoto"].asString());
					card.SetMobile(value[i]["friendMobile"].asString());
					list.push_back(card);
				}

				shared::Post2UI(std::bind(*pcb, list));
			}
			else
			{
				printf("Json::Reader parse error\n");
				goto faile;
			}
faile:
			is.close();
		}
		delete pcb;
	}
	return;
}

}


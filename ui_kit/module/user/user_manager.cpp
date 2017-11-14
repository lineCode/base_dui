#include "stdafx.h"
#include <fstream>
#include "user_manager.h"

namespace nim_comp
{
void UserManager::InvokeRegisterAccount(const std::string &username, const std::string &password, const std::string &nickname, const OnRegisterAccountCallback& cb)
{
	assert(0);
	InvokeRegisterAccount(username, password, nickname, cb);
}

NIMFriendFlag UserManager::GetUserType(const std::string &id)
{
	assert(0);
	return GetUserType(id);
}

bool UserManager::GetUserInfo(const std::string &account, UserNameCard &info)
{
	bool bFind = false;
	for (auto it = all_user_.cbegin(); it != all_user_.cend(); it++)
	{
		if (it->first == account)
		{
			info = it->second;
			bFind = true;
			break;
		}
	}

	return bFind;
}

void UserManager::GetUserInfos(const std::list<std::string>& ids, std::list<UserNameCard>& uinfos)
{
	//nim_comp::UserService::GetInstance()->GetUserInfos(ids, uinfos);
}

const std::map<std::string, UserNameCard>& UserManager::GetAllUserInfos()
{
	return all_user_;
}

std::wstring UserManager::GetUserName(const std::string &id, bool alias_prior/* = true */)
{
	wstring name = L"unknown";
	UserNameCard card;
	if (GetUserInfo(id, card))
	{
		nbase::win32::MBCSToUnicode(card.GetName(), name, CP_UTF8);
	}
	return name;
}

std::wstring UserManager::GetFriendAlias(const std::string & id)
{
	assert(0);
	return GetFriendAlias(id);
}

UnregisterCallback UserManager::RegFriendListChange(const OnFriendListChangeCallback & callback)
{
	assert(0);
	return RegFriendListChange(callback);
}

UnregisterCallback UserManager::RegUserInfoChange(const OnUserInfoChangeCallback& callback)
{
	assert(0);
	return RegUserInfoChange(callback);
}

UnregisterCallback UserManager::RegMiscUInfoChange(const OnUserInfoChangeCallback & callback)
{
	assert(0);
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

				all_user_.clear();
				for (auto it = list.cbegin(); it != list.cend(); it++)
				{
					all_user_[it->GetAccId()] = *it;
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


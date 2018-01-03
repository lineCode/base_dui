#pragma once

#include "module/user/user_define.h"

namespace nim_comp
{
/** @class FriendItem
  * @brief 好友列表项的UI类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class FriendItemEx : public dui::TreeNode
{
public:
	FriendItemEx(const UserNameCard &info);
	virtual ~FriendItemEx();
	virtual void Init();
	
	std::string GetAccid()
	{
		return info_.GetAccId();
	}

	/*bool Match(const UTF8String& search_key)
	{
		std::wstring ws_search_key = nbase::UTF8ToUTF16(search_key);
		ws_search_key = nbase::MakeLowerString(ws_search_key);
		if (nick_name_.find(ws_search_key) != std::wstring::npos
			|| nick_name_full_spell_.find(search_key) != UTF8String::npos
			|| nick_name_simple_spell_.find(search_key) != UTF8String::npos)
		{
			return true;
		}
		return false;
	}*/
	bool operator < (const FriendItemEx& elment2) const
	{
		static DWORD lcid = MAKELCID(MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRCP);
		return ::CompareStringA(lcid, 0, this->info_.GetName().c_str(), this->info_.GetName().length(), elment2.info_.GetName().c_str(), elment2.info_.GetName().length()) == 1;
	}
	bool operator == (const FriendItemEx& elment2) const
	{
		return this->info_.GetAccId() == elment2.info_.GetAccId();
	}

	/*bool OnClicked(dui::EventArgs* arg);
private:
	void OnTeamNameChange(const nim::TeamInfo& user_info);
	void OnUserPhotoReady(PhotoType type, const std::string& tid, const std::wstring &photo_path);*/

protected:
	//bool OnFriendHeadClicked(void *param);
private:
	dui::Label*		contact_;
	dui::Label*		post_;
	dui::Button		*btn_head_;

	/*bool			is_team_;
	std::string		id_;
	std::string		neteaseid_;

	std::wstring	nick_name_;
	std::string		post_name_;
	UTF8String		nick_name_full_spell_;
	UTF8String		nick_name_simple_spell_;
	std::string		head_image;*/

	//AutoUnregister unregister_cb;
	UserNameCard	info_;
};
}
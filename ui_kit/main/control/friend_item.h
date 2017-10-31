#pragma once

//#include "shared/pin_yin_helper.h"

namespace nim_comp
{
/** @class FriendItem
  * @brief 好友列表项的UI类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class FriendItem : public dui::TreeNode
{
public:
	FriendItem();
	virtual ~FriendItem();
	virtual void Init();
	/*bool GetIsTeam() const
	{
		return is_team_;
	}
	std::string GetId()
	{
		return id_;
	}
	std::string GetNeteaseId()
	{
		return neteaseid_;
	}

	bool Match(const UTF8String& search_key)
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
	}
	bool operator < (const FriendItem& elment2) const
	{
		static DWORD lcid = MAKELCID(MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRCP);
		return ::CompareString(lcid, 0, this->nick_name_.c_str(), this->nick_name_.length(), elment2.nick_name_.c_str(),elment2.nick_name_.length()) == 1;
	}
	bool operator == (const FriendItem& elment2) const 
	{
		return this->id_ == elment2.id_;
	}

	bool OnClicked(ui::EventArgs* arg);
private:
	void OnTeamNameChange(const nim::TeamInfo& user_info);
	void OnUserPhotoReady(PhotoType type, const std::string& tid, const std::wstring &photo_path);*/

private:
	dui::Label*		contact_;
	dui::Label*		post_;

	bool			is_team_;
	std::string		id_;
	std::string		neteaseid_;

	std::wstring	nick_name_;
	std::string	post_name_;
	UTF8String		nick_name_full_spell_;
	UTF8String		nick_name_simple_spell_;
	std::string		head_image;

	//AutoUnregister unregister_cb;
};
}
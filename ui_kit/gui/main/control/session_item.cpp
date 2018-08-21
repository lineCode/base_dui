#include "stdafx.h"
#include "session_item.h"
#include "module/user/user_manager.h"
#include "module/session/session_util.h"
#include "module/photo/photo_manager.h"

//#include "module/session/session_manager.h"
//#include "export/nim_ui_window_manager.h"
//#include "gui/main/main_form.h"

using namespace dui;

namespace nim_comp
{
	SessionItem::SessionItem(const SessionData &msg)
	{
		msg_ = msg;
	}

	SessionItem::~SessionItem()
	{

	}

	void SessionItem::Init()
	{

		auto OnItemClick = [this](Event *event){
			//printf("SessionItem OnItemClick, name:%s, phone:%s\n", event->pSender->GetName().c_str(), info_.GetMobile().c_str());
			return false;
		};
		this->AttachItemClick(std::bind(OnItemClick, std::placeholders::_1));
		
		label_name_ = (Label*) this->FindSubControl(L"label_name");
		label_msg_ = (Label*) this->FindSubControl(L"label_msg");
		label_time_ = (Label*) this->FindSubControl(L"label_time");

		wstring wname, wmsg, wtime;
		//nbase::win32::MBCSToUnicode(msg_.msg_sender_accid_, wname, CP_UTF8);
		wname = UserManager::GetInstance()->GetUserName(msg_.msg_sender_accid_);
		label_name_->SetText(wname.c_str());

		nbase::win32::MBCSToUnicode(msg_.msg_content_, wname, CP_UTF8);
		label_msg_->SetText(wname.c_str());

		std::wstring str = GetMessageTime(msg_.msg_timetag_, true);
		label_time_->SetText(str.c_str());

		btn_head_ = dynamic_cast<dui::Button *>(FindSubControl(L"btn_friend_head"));
		if (btn_head_)
		{
			auto OnFriendHeadClicked = [this](Event *event){
				//printf("SessionItem OnFriendHeadClicked, name:%s, phone:%s\n", event->pSender->GetName().c_str(), info_.GetMobile().c_str());
				return false; };
			
			btn_head_->AttachClick(std::bind(OnFriendHeadClicked, std::placeholders::_1));
		}

		wstring photopath = PhotoManager::GetInstance()->GetUserPhoto(msg_.msg_sender_accid_);
		btn_head_->SetBkImage(photopath.c_str());
	}
}
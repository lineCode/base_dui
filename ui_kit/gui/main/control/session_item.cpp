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
#if MODE_EVENTMAP
		auto OnItemClick = [this](TEvent *event){
			//printf("SessionItem OnItemClick, name:%s, phone:%s\n", event->pSender->GetName().c_str(), info_.GetMobile().c_str());
			return false;
		};
		this->AttachItemClick(std::bind(OnItemClick, std::placeholders::_1));
#endif
		
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
#if MODE_EVENTMAP
		btn_head_ = dynamic_cast<dui::Button *>(FindSubControl(L"btn_friend_head"));
		if (btn_head_)
		{
			auto OnFriendHeadClicked = [this](TEvent *event){
				//printf("SessionItem OnFriendHeadClicked, name:%s, phone:%s\n", event->pSender->GetName().c_str(), info_.GetMobile().c_str());
				return false; };
			
			btn_head_->AttachClick(std::bind(OnFriendHeadClicked, std::placeholders::_1));
		}
#endif
		wstring photopath = PhotoManager::GetInstance()->GetUserPhoto(msg_.msg_sender_accid_);
		btn_head_->SetBkImage(photopath.c_str());
		//is_team_ = is_team;
		//id_ = accid;

		//SetUTF8Name(id_);
		//SetUTF8DataID(id_);

		//if (is_team){
		//	nick_name_ = TeamService::GetInstance()->GetTeamName(id_);
		//}
		//else
		//{
		//	nick_name_ = UserService::GetInstance()->GetUserName(id_);
		//	neteaseid_ = UserService::GetInstance()->GetFriendnetEaseId(id_);
		//	post_name_ = UserService::GetInstance()->GetPostName(id_);
		//}
		//contact_->SetText(nick_name_);
		//if (post_name_ != "");
		//{
		//	wstring post_name_w;
		//	nbase::win32::MBCSToUnicode(post_name_, post_name_w, CP_UTF8);
		//	post_->SetVisible(true);
		//	post_->SetText(post_name_w);
		//}


		//auto head_ctrl = FindSubControl(L"head_image");
		//if (is_team)
		//{
		//	head_ctrl->SetBkImage(PhotoManager::GetInstance()->GetTeamPhoto(id_, false));
		//	head_ctrl->SetMouseEnabled(false); //群头像不响应点击
		//	head_ctrl->SetCursorType(ui::CursorType::kCursorArrow);
		//}
		//else
		//{
		//	//head_ctrl->SetBkImage(PhotoManager::GetInstance()->GetUserPhoto(accid));
		//	wstring photopath = PhotoManager::GetInstance()->GetUserPhoto(accid);
		//	if (photopath == QPath::GetAppPath() + L"res\\faces\\default\\default.png")		//自创默认头像
		//	{
		//		wstring file_w;
		//		string file = "default_face_";
		//		file = file + accid + ".bmp";
		//		nbase::win32::MBCSToUnicode(file, file_w);
		//		photopath = PhotoManager::GetInstance()->GetPhotoDir(kUser) + file_w;
		//		string utf8name;
		//		nbase::win32::UnicodeToMBCS(nick_name_, utf8name, CP_UTF8);
		//		if (!PhotoManager::GetInstance()->CreateHead(utf8name, photopath))
		//		{
		//			photopath = QPath::GetAppPath() + L"res\\faces\\default\\default.png";
		//		}
		//	}
		//	head_ctrl->SetBkImage(photopath);
		//}

		//nick_name_ = nbase::MakeLowerString(nick_name_);
		//nick_name_full_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(nick_name_));
		//nick_name_simple_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(nick_name_));

		//if (is_team_)
		//{
		//	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(nbase::Bind(&FriendItem::OnTeamNameChange, this, std::placeholders::_1)));
		//	unregister_cb.Add(PhotoManager::GetInstance()->RegPhotoReady(nbase::Bind(&FriendItem::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		//}
	}

	/*bool FriendItem::OnFriendHeadClicked(void *param)
	{
		printf("FriendItem OnFriendHeadClicked, phone:%s\n", info_.GetMobile().c_str());
		return false;
	}*/
#if 0
	bool FriendItem::OnClicked(ui::EventArgs* arg)
	{
		SessionBox *session = SessionManager::GetInstance()->OpenSessionBox(UserService::GetInstance()->GetFriendnetEaseId(id_), is_team_ ? nim::kNIMSessionTypeTeam : nim::kNIMSessionTypeP2P);
		if (session)
		{
			::SendMessage(nim_ui::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), WM_USER + 3, (WPARAM)session, 0);
		}
		return true;
	}

	void FriendItem::OnTeamNameChange(const nim::TeamInfo& user_info)
	{
		if (user_info.GetTeamID() == id_)
		{
			contact_->SetUTF8Text(user_info.GetName());

			nick_name_ = nbase::UTF8ToUTF16(user_info.GetName());
			nick_name_full_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(nick_name_));
			nick_name_simple_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(nick_name_));
		}
	}

	void FriendItem::OnUserPhotoReady(PhotoType type, const std::string& tid, const std::wstring &photo_path)
	{
		if (type == kTeam && tid == id_)
		{
			auto head_ctrl = FindSubControl(L"head_image");
			head_ctrl->SetBkImage(photo_path);
		}
	}
#endif
}
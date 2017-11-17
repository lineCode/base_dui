#include "stdafx.h"
#include "session_box.h"
//#include "gui/link/link_form.h"
//#include "gui/invoke_chat_form/invoke_chat_form.h"
//#include "gui/team_info/team_info.h"
//#include "gui/broad/broad_form.h"
//#include "gui/session/msg_record.h"
//#include "gui/profile_form/profile_form.h"
//#include "callback/session/session_callback.h"
//#include "callback/http/http_callback.h"
//#include "module/session/session_manager.h"
//#include "module/video/video_manager.h"
//#include "module/rts/rts_manager.h"
//#include "module/service/user_service.h"
//#include "capture_image/src/capture_manager.h"
//#include "shared/modal_wnd/file_dialog_ex.h"
//#include "shared/ui/ui_menu.h"
//#include "callback/team/team_callback.h"
//#include "service/remote_desktop/remote_desktop_manager.h"
//#include <commdlg.h>
//#include "export/nim_ui_user_config.h"
//#include <io.h>
//#include "base64.h"
//#include "gui/session/control/atme_view.h"

using namespace dui;

namespace nim_comp
{

SessionBox::SessionBox(std::string id, NIMSessionType type) :droptarget_(NULL)
{
}

SessionBox::~SessionBox() 
{

}

void SessionBox::Init()
{
	printf("SessionBox::Init");

	CheckBtn *chk_face = (CheckBtn*)FindSubControl(L"chk_face");
	Button* btn_image = (Button*)FindSubControl(L"btn_image");
	Button* btn_file = (Button*)FindSubControl(L"btn_file");
	Button* btn_jsb = (Button*)FindSubControl(L"btn_jsb");
	Button* btn_clip = (Button*)FindSubControl(L"btn_clip");
	Button* btn_msg_record = (Button*)FindSubControl(L"btn_msg_record");
	Button* btn_send = (Button*)FindSubControl(L"btn_send");

	chk_face->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_image->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_file->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_jsb->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_clip->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_msg_record->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_send->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));

	input_edit_ = (RichEdit*)FindSubControl(L"input_edit");

	InitRichEdit();
}

void SessionBox::InitRichEdit()
{
	if (!input_edit_)
	{
		return;
	}
	::RevokeDragDrop(m_pManager->GetPaintWindow());
	//::RevokeDragDrop(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND());
	if (!droptarget_)
	{
		/*ITextServices * text_services = input_edit_->GetTextServices();
		QLOG_APP(L"input_edit_->GetTextServices = {0}") << (int)text_services;
		if (!text_services)
		{
			return;
		}
		richeditolecallback_ = new IRichEditOleCallbackEx(text_services, std::function<void()>());
		text_services->Release();
		input_edit_->SetOleCallback(richeditolecallback_);
		richeditolecallback_->SetCustomMode(true);*/

		IDropTarget *pdt = input_edit_->GetTxDropTarget();
		QLOG_APP(L"input_edit_->GetTxDropTarget = {0}") << (int)pdt;
		if (pdt)
		{
			droptarget_ = pdt;
		}
		else
		{
			droptarget_ = NULL;
		}
	}
	::RegisterDragDrop(m_pManager->GetPaintWindow(), this);
	//::RegisterDragDrop(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), this);
}

bool SessionBox::OnClicked(dui::TEvent *event)
{
	if (event)
	{
		wprintf(L"SessionBox::OnClicked %s\n", event->pSender->GetName().c_str());
	}
	
	return false;
}


#if 0
LRESULT SessionBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam ,bool &bHandle)
{
	if (uMsg == WM_KEYDOWN)
	{
		if (wParam == 'V')
		{
			if (::GetKeyState(VK_CONTROL) < 0 && input_edit_->IsFocused())
			{
				input_edit_->PasteSpecial(CF_TEXT);
				bHandle = true;
				return 1;
			}
		}
		else if (wParam == VK_DOWN || wParam == VK_UP || wParam == VK_RETURN || wParam == VK_BACK)
		{
			if (HandleAtMsg(wParam, lParam))
			{
				bHandle = true;
				return 0;
			}
		}
	}
	//if (uMsg == WM_CHAR)
	//{
	//	if (wParam != VK_BACK)
	//	{
	//		//先让RichEdit处理完，然后再处理@消息
	//		LRESULT res = session_form_->WindowEx::HandleMessage(uMsg, wParam, lParam);
	//		HandleAtMsg(wParam, lParam);
	//		bHandle = true;
	//		return res;
	//	}
	//}
	else if (uMsg == WM_MOUSEWHEEL)
	{
		//处理@列表的滚动
		if (HandleAtMouseWheel(wParam, lParam))
		{
			bHandle = true;
			return 0;
		}
	}
	else if (uMsg == WM_LBUTTONDOWN || uMsg == WM_NCLBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_KILLFOCUS)
	{
		HideAtListForm();
	}
	else if (uMsg == WM_SETFOCUS)
	{
		if (input_edit_)
			input_edit_->SetFocus();
	}
	else if (uMsg == WM_USER + 1)
	{
		wchar_t* path = (wchar_t*)wParam;
		wstring path_ = path;
	}

	return 0;
}

void SessionBox::SendReceiptIfNeeded(bool auto_detect/* = false*/)
{
	//发送已读回执目前只支持P2P会话
	if (session_type_ != nim::kNIMSessionTypeP2P)
		return;

	if (auto_detect)
	{
		auto top_hwnd = GetForegroundWindow();

		//if (top_hwnd != this->GetHWND() || msg_list_ == nullptr || !msg_list_->IsAtEnd())
		if (top_hwnd != nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND() || msg_list_ == nullptr || !msg_list_->IsAtEnd())	

		{
			receipt_need_send_ = true;
			return;
		}
	}

	nim::IMMessage msg;
	if (GetLastNeedSendReceiptMsg(msg))
	{
		receipt_need_send_ = false;
		nim::MsgLog::SendReceiptAsync(msg.ToJsonString(false), [](const nim::MessageStatusChangedResult& res) {
			auto iter = res.results_.begin();
			QLOG_APP(L"mark receipt id:{0} time:{1} rescode:{2}") << iter->talk_id_ << iter->msg_timetag_ << res.rescode_;
		});
	}
}

bool SessionBox::Notify(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (param->Type == kEventScrollChange)
	{
		if (name == L"msg_list")
		{
			if (receipt_need_send_)
			{
				bool list_last = msg_list_->IsAtEnd();
				// 这里要判断当前窗口是否是激活状态，因为在非激活状态下的话也可能会收到Scroll消息导致误发回执消息
				if (list_last && nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND() == GetForegroundWindow())
					SendReceiptIfNeeded();
			}
		}
	} else if (param->Type == ui::kEventNotify)
	{
		MsgBubbleItem* item = dynamic_cast<MsgBubbleItem*>(param->pSender);
		assert(item);
		nim::IMMessage md = item->GetMsg();

		if (param->wParam == BET_RESEND)
		{
			RemoveMsgItem(md.client_msg_id_);
			ReSendMsg(md);
		}
		else if (param->wParam == BET_RELOAD)
		{
			item->SetLoadStatus(RS_LOADING);
			nim::NOS::FetchMedia(md, nim::NOS::DownloadMediaCallback(), nim::NOS::ProgressCallback());
		}
		else if (param->wParam == BET_DELETE)
		{
			RemoveMsgItem(md.client_msg_id_);
			//if (session_type_ == nim::kNIMSessionTypeTeam)
			//	nim::Session::DeleteRecentSession(nim::kNIMSessionTypeTeam, session_id_, nim::Session::DeleteRecentSessionCallabck());
			nim::MsgLog::DeleteAsync(session_id_, session_type_, md.client_msg_id_, nim::MsgLog::DeleteCallback());
		}
		else if (param->wParam == BET_RETWEET)
		{
			OnMenuRetweetMessage(md);
		}
		else if (param->wParam == BET_RECALL)
		{
			nim::Talk::RecallMsg(md, "test notify when recall", nbase::Bind(&nim_comp::TalkCallback::OnReceiveRecallMsgCallback, std::placeholders::_1, std::placeholders::_2));
		}
		else if (param->wParam == BET_TRANSFORM)
		{
			nim::IMMessage msg_data = ((MsgBubbleAudio*)param->pSender)->GetMsg();
			nim::IMAudio audio;
			nim::Talk::ParseAudioMessageAttach(msg_data, audio);
			nim::AudioInfo audio_info;
			audio_info.samplerate_ = "16000";
			audio_info.url_ = audio.url_;
			audio_info.duration_ = audio.duration_;
			audio_info.mime_type_ = audio.file_extension_;

			nim::Tool::GetAudioTextAsync(audio_info, ToWeakCallback([this](int rescode, const std::string& text) {
				if (rescode == nim::kNIMResSuccess) {
					ShowMsgBox(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), nbase::UTF8ToUTF16(text), MsgboxCallback(), L"转文字", L"确定", L"");
				}
				else {
					ShowMsgBox(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), L"语音转化失败", MsgboxCallback(), L"转文字", L"确定", L"");
					QLOG_ERR(L"audio convert to text failed errorcode={0}") << rescode;
				}
			}));
		}
		else if (param->wParam == BET_SHOWPROFILE)
		{
			if (session_type_ == nim::kNIMSessionTypeTeam)
			{
				nim::NIMTeamUserType type = team_member_info_list_[LoginManager::GetInstance()->GetAccount()].GetUserType();
				if (md.sender_accid_==nim_comp::LoginManager::GetInstance()->GetAccount())
				{
					ProfileForm::ShowProfileForm(session_id_, nim_comp::LoginManager::GetInstance()->GetUserId(), type);
				}
				else
					ProfileForm::ShowProfileForm(session_id_, nim_comp::UserService::GetInstance()->GetAccIdByNetEaseId(md.sender_accid_), type);

			}
			else
			{
				if (md.sender_accid_==nim_comp::LoginManager::GetInstance()->GetAccount())
				{
					//ProfileForm::ShowProfileForm(nim_comp::LoginManager::GetInstance()->GetUserId());
					ProfileForm::ShowProfileForm(nim_comp::LoginManager::GetInstance()->GetAccount());		//mod by scz
				}
				else
					ProfileForm::ShowProfileForm(nim_comp::UserService::GetInstance()->GetAccIdByNetEaseId(md.sender_accid_));
			}
		}
		else if (param->wParam == BET_MENUATTA)
		{
			AtSomeone at;
			at.uid_ = item->GetSenderId();
			std::string show_name;
			if (!at.is_robot_)
			{
				auto i = team_member_info_list_.find(at.uid_);
				if (i != team_member_info_list_.end())
				{
					show_name = i->second.GetNick();
				}
				if (show_name.empty())
				{
					show_name = nbase::UTF16ToUTF8(UserService::GetInstance()->GetUserName(at.uid_, false));
				}
			}

			uid_at_someone_[show_name] = at;

			std::wstring show_text = L"@";
			show_text.append(nbase::UTF8ToUTF16(show_name));
			show_text.append(L" ");

			input_edit_->ReplaceSel(show_text, false);
		}
	}
	else if (param->Type == ui::kEventTextChange)
	{
		if (name == L"input_edit")
		{
			if (GetRichText(input_edit_) == L"")
			{
				btn_send_->SetState(ui::ControlStateType::kControlStateDisabled);
				btn_send_->SetBkImage(L"send_disable.png");
			}
			else
			{
				btn_send_->SetState(ui::ControlStateType::kControlStateNormal);
				btn_send_->SetBkImage(L"send2.png");
			}
			//当用户正在输入框内输入文字时，发送给对方“对方正在输入”的消息
			if (session_type_ == nim::kNIMSessionTypeP2P && !IsFileTransPhone())
			{
				std::wstring wstr = GetRichText(input_edit_);
				if (wstr.empty())
				{

				}
				else
				{
					long long now = nbase::Time::Now().ToTimeT();
					if (writing_time_ == 0 || now - writing_time_ > kCellWritingTime)
					{
						writing_time_ = now;

						Json::Value json;
						Json::FastWriter writer;
						json["id"] = "1";

						nim::SysMessage msg;
						msg.receiver_accid_ = session_id_;
						msg.sender_accid_ = LoginManager::GetInstance()->GetAccount();
						msg.client_msg_id_ = QString::GetGUID();
						msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
						msg.attach_ = writer.write(json);
						msg.type_ = nim::kNIMSysMsgTypeCustomP2PMsg;

						nim::SystemMsg::SendCustomNotificationMsg(msg.ToJsonString());
					}
				}
			}
			else if (session_type_ == nim::kNIMSessionTypeTeam)
			{
				std::wstring wstr = GetRichText(input_edit_);

				if (wstr[wstr.length() - 1] == L'@')
				{
					HandleAtMsg(L'@', param->lParam);
				}
				else
				{
					HWND at_list = FindWindow(L"AtlistForm", L"");
					if (at_list != NULL)
						ShowWindow(at_list, SW_HIDE);
				}
			}
		}
	}
	else if (param->Type == ui::kEventMouseButtonDown)
	{
		HWND at_list = FindWindow(L"AtlistForm", L"");
		if (at_list != NULL)
			DestroyWindow(at_list);
	}
	return true;
}

bool SessionBox::OnClicked(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"btn_send")
	{
		if (btn_send_->GetBkImage() != L"send_disable.png")
			OnBtnSend();
	}
	else if (name == L"btn_image")
	{
		OnBtnImage(false);
	}
	else if (name == L"btn_snapchat")
	{
		OnBtnImage(true);
	}
	else if (name == L"btn_file")
	{
		OnBtnFile();
	}
	else if (name == L"btn_jsb")
	{
		OnBtnJsb();
	}
	else if (name == L"btn_audio")
	{
		OnBtnAudio();
	}
	else if (name == L"btn_video")
	{
		OnBtnVideo();
	}
	else if (name == L"btn_rts")
	{
		OnBtnRts();
	}
	else if (name == L"btn_tip")
	{
		SendTip(L"这是一条提醒消息");
	}
	else if (name == CELL_BTN_LOAD_MORE_MSG)
	{
		Control* ctrl = msg_list_->FindSubControl(CELL_LOAD_MORE_MSG);
		assert(ctrl);
		ctrl->SetVisible(false);

		InvokeShowMsgs(false);
	}
	else if (name == L"btn_clip")
	{
#if 1
		OnBtnClip();
#else
		if (CaptureManager::GetInstance()->IsCaptureTracking())
		{
			return false;
		}

		//::ShowWindow( m_hWnd, SW_SHOWMINIMIZED );
		StdClosure callback = nbase::Bind(&SessionBox::DoClip, this);
		nbase::ThreadManager::PostDelayedTask(kThreadUI, callback, nbase::TimeDelta::FromMilliseconds(500));
#endif
	}
	else if (name == L"btn_msg_record")
	{
		MsgRecordForm* f = WindowsManager::SingletonShow<MsgRecordForm>(MsgRecordForm::kClassName);
		f->RefreshRecord(session_id_, session_type_);
	}
	else if (name == L"btn_custom_msg")
	{
		ShowCustomMsgForm();
	}
	else if (name == L"btn_new_broad")
	{
		BroadForm* broad = WindowsManager::SingletonShow<BroadForm>(BroadForm::kClassName);
		broad->SetTid(session_id_);
	}
	else if (name == L"btn_refresh_member")
	{
		InvokeGetTeamMember();
	}
	else if (name == L"btn_addmember")
	{
		std::wstring session_id = nbase::UTF8ToUTF16(this->GetSessionID());
		nim_comp::TeamInfoForm* team_info_form = (nim_comp::TeamInfoForm*)nim_comp::WindowsManager::GetInstance()->GetWindow(nim_comp::TeamInfoForm::kClassName, session_id);

		nim::TeamInfo* pTeamInfo = this->GetSessionTeamInfo();
		std::wstring title = pTeamInfo->GetType() == nim::kNIMTeamTypeNormal ? L"讨论组资料" : L"群资料";
		team_info_form = new nim_comp::TeamInfoForm(false, pTeamInfo->GetType(), this->GetSessionID(), *pTeamInfo);
		team_info_form->Create(NULL, title.c_str(), WS_OVERLAPPEDWINDOW& ~WS_MAXIMIZEBOX, 0L);
		team_info_form->CenterWindow();
		team_info_form->ShowWindow(true);
	}
	else if (name == L"add_msglog")
	{
		std::wstring wstr = GetRichText(input_edit_);
		int num = 100;
		nbase::StringToInt(param->pSender->GetDataID(), &num);
		for (int i = 0; i < num; i++)
		{
			nim::IMMessage msg;
			PackageMsg(msg);
			//msg.status_ = nim::kNIMMsgLogStatusSent;
			msg.status_ = nim::kNIMMsgLogStatusRead;
			msg.sender_accid_ = session_id_;
			msg.type_ = nim::kNIMMessageTypeText;
			msg.content_ = nbase::StringPrintf("%d>>%s", i, nbase::UTF16ToUTF8(wstr).c_str());
			//发送消息不需要填写昵称
			//UserInfo user_info;
			//UserService::GetInstance()->GetUserInfo(msg.from_account, user_info);
			//msg.from_nick = user_info.name;
			nim::MsgLog::WriteMsglogToLocalAsync(session_id_, msg, false, nim::MsgLog::WriteMsglogCallback());
		}
	}
	else if (name == L"btn_remotedesktop")
	{
		OnBtnRemoteDesktop();
	}
	return true;
}

bool SessionBox::OnSelChanged(ui::EventArgs* param)
{
	EventType et = param->Type;
	std::wstring name = param->pSender->GetName();
	if (name == L"btn_face")
	{
		if (et == kEventSelect)
			OnBtnEmoji();
	}
	return true;
}

bool SessionBox::OnEditEnter(ui::EventArgs* param)
{
	if (!at_list_form->addAt){
		if (btn_send_->GetBkImage() != L"send_disable.png")
			OnBtnSend();
	}
	else
		at_list_form->addAt = false;
	return true;
}

bool SessionBox::OnFrameRightKillFouce(ui::EventArgs* param)
{
#if 1	//计算鼠标是否在frame_right内
	if (!is_frame_right_focus_)
	{
		return false;
	}
	ui::UiRect rect_window = GetPos(false);
	ui::UiRect rect_frame_right;
	rect_frame_right.left = rect_window.right - 180;
	rect_frame_right.top = 51;
	rect_frame_right.right = rect_window.right;
	rect_frame_right.bottom = rect_window.bottom;

	POINT pt = param->ptMouse;
	pt.x -= 14;
	pt.y -= 14;
	if (!PtInRect(&rect_frame_right, pt))
	{
		frame_right_->SetVisible(false);
		frame_right_->SelectItem(0);
	}
	else
	{
		//frame_right_->FindSubControl(L"group_member_info")->SetFocus();
		return false;
	}
#else
	//ShowGroupInfo();	//ShowGroupInfo is err
	frame_right_->SetVisible(false);
	frame_right_->SelectItem(0);
#endif
	return true;
}

bool SessionBox::OnFrameRightSetFouce(ui::EventArgs* param)
{
	is_frame_right_focus_ = true;
	return true;
}

void SessionBox::DoClip()
{
	std::wstring send_info;
	CaptureManager::CaptureCallback callback = nbase::Bind(&SessionBox::OnClipCallback, this, std::placeholders::_1, std::placeholders::_2);
	std::string acc = LoginManager::GetInstance()->GetAccount();
	assert(!acc.empty());
	std::wstring app_data_audio_path = QPath::GetUserAppDataDir(acc);
	if (CaptureManager::GetInstance()->StartCapture(callback, app_data_audio_path, send_info) == false)
	{
		OnClipCallback(FALSE, L"");
	}
}

void SessionBox::OnClipCallback(bool ret, const std::wstring& file_path)
{
	if (ret)
	{
		emoji::InsertImageToEdit(input_edit_, file_path);
	//	SendImage(file_path);
	}
};

void SessionBox::OnBtnSend()
{
	if (!LoginManager::GetInstance()->IsLinkActive())
	{
		AddTip(STT_LINK);

		ShowLinkForm();
		return;
	}
#if 0	//云信高版本3.6 代码
	ITextServices *text_service = input_edit_->GetTextServices();
	if (!text_service)
	{
		QLOG_ERR(L"DoBtnSend occur error. text service null.");
		return;
	}

	std::vector<RE_OLE_ITEM_CONTENT> content_list;
	Re_GetTextEx(text_service, content_list);
	text_service->Release();

	bool empty_msg = content_list.empty();
	for (UINT i = 0; i < content_list.size(); ++i)
	{
		RE_OLE_ITEM_CONTENT info = content_list.at(i);
		std::string msg_body = nbase::UTF16ToUTF8(info.content_);
		switch (info.type_)
		{
		case RE_OLE_TYPE_TEXT:
		{
								 if (CheckMsgContent(info.content_).empty())
								 {
									 if (content_list.size() == 1)
									 {
										 empty_msg = true;
									 }
								 }
								 else
								 {
									 SendText(msg_body);
								 }
		}
			break;
		case RE_OLE_TYPE_IMAGE:
		{
								  if (nbase::FilePathIsExist(info.image_src_, FALSE))
								  {
									  SendImage(info.image_src_);
								  }		
		}			
			break;
		case RE_OLE_TYPE_FILE:
		{
								 std::wstring file_path = info.content_;
								 if (CheckFileSize(file_path))
								 {
									 SendFile(file_path);
								 }
								 else
								 {
									 ShowMsgBox(this->GetWindow()->GetHWND(), MsgboxCallback(), L"STRID_SESSION_SUPPORT_15MB");
								 }
		}
			break;
		}
	}
#else
	std::wstring wstr = GetRichText(input_edit_);
	StringHelper::Trim(wstr);
	if (wstr.empty())
	{
		input_edit_->SetText(L"");
		ShowMsgBox(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), L"请输入内容后再发送哦", MsgboxCallback(),
			L"提示", L"确定", L"");
		return;
	}
	else
	{
		std::string text = Wstr2Str(wstr);
/*		if (wstr[0] == L'[' && wstr[wstr.length() - 1] == L']' && _access(text.substr(1,text.length()-2).c_str(),0) == 0)
		{
			wstr = wstr.substr(1, wstr.length() - 2);
			SendImage(wstr);
		}
		else
			SendText(nbase::UTF16ToUTF8(wstr));*/
		int i = 0;
		for (; i <	wstr.length(); i++)
		{
			if (wstr[i] == L'[')
			{
				int p = wstr.find_first_of(']', i);
				std::string mb;
				std::wstring uni = wstr.substr(i + 1, p - i - 1);
				nbase::win32::UnicodeToMBCS(uni, mb);
				if (p && _access(mb.c_str()/*nbase::UTF16ToUTF8(wstr.substr(i + 1, p - i - 1)).c_str()*/, 0) == 0)
				{
					if (i != 0)
					{
						wstring wstr_text = wstr.substr(0, i);
						SendText(nbase::UTF16ToUTF8(wstr_text));
					}
					wstring wstr_image = wstr.substr(i + 1, p - i - 1);
					SendImage(wstr_image);
					text = text.substr(p + 1);
					wstr = wstr.substr(p + 1);
					i = -1;
				}

			}
		}
		if (wstr.length() > 0)
			SendText(nbase::UTF16ToUTF8(wstr));

	}
#endif
	input_edit_->SetText(L"");
	input_edit_->SetFocus();
}

void SessionBox::FlashTaskbar()
{
	FLASHWINFO flash_info = { sizeof(FLASHWINFO), nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), FLASHW_TRAY, 3, 0 };
	::FlashWindowEx(&flash_info);
}

void SessionBox::AdjustSizeForAdvancedTeam()
{
#if 0
	if (session_type_ == nim::kNIMSessionTypeTeam && team_info_.GetType() == nim::kNIMTeamTypeAdvanced)
	{
		int width = 740, height = 600;
		this->SetMinInfo(width, height);
		this->SetPos(ui::UiRect(0, 0, width, height), SWP_NOMOVE | SWP_NOACTIVATE);
	}
#endif
}

void SessionBox::OnNewMsg(bool create, bool flash)
{
	if (flash || create)
	{
		HWND hwnd = nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND();

		bool need = false;
		if (create)
		{
			RECT rc;
			
			::GetWindowRect(hwnd, &rc);

			WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
			::GetWindowPlacement(hwnd, &wp);
			wp.showCmd = SW_SHOWMINNOACTIVE;
			wp.rcNormalPosition = rc;
			::SetWindowPlacement(hwnd, &wp);

			need = true;
		}
		else
		{
			if (::IsIconic(hwnd))
				need = true;
			else if (::GetForegroundWindow() != hwnd)
				need = true;
		}

		if (need)
			FlashTaskbar();
	}
}

void SessionBox::OnDropFile(HDROP hDrop)
{
	const int MAX_PATH_SIZE = 1024;

	wchar_t buff[MAX_PATH_SIZE] = { 0 };
	int iFile = DragQueryFileW(hDrop, 0xFFFFFFFF, buff, MAX_PATH_SIZE);
	bool large_file = false;
	for (int i = 0; i < iFile; i++)
	{
		int len = DragQueryFileW(hDrop, i, buff, MAX_PATH_SIZE);
		std::wstring path(buff, len);

		std::wstring mime_type = ZoomImage::GetMimeType(path);
		if (!mime_type.empty())
		{
			SendImage(path);
		}
		else if (CheckFileSize(path))
		{
			SendFile(path);
		}
		else
		{
			large_file = true;
			wprintf_s(L"Send File %s\n", path.c_str());
		}
	}
	if (large_file)
	{
		ShowMsgBox(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), L"暂时只支持小于60MB的非空文件", MsgboxCallback(), L"提示", L"确定", L"");
	}
	//目前是通过RichEdit的拖曳来处理
	//DragFinish(hDrop);
}

void SessionBox::OnBtnImage(bool is_snapchat)
{
	std::wstring file_type = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_PIC_FILE");
	LPCTSTR filter = L"*.jpg;*.jpeg;*.png;*.bmp";
	std::wstring text = nbase::StringPrintf(L"%s(%s)", file_type.c_str(), filter);
	std::map<LPCTSTR, LPCTSTR> filters;
	filters[text.c_str()] = filter;

	CFileDialogEx::FileDialogCallback2 cb = nbase::Bind(&SessionBox::OnImageSelected, this, is_snapchat, std::placeholders::_1, std::placeholders::_2);

	CFileDialogEx* file_dlg = new CFileDialogEx();
	file_dlg->SetFilter(filters);
	file_dlg->SetParentWnd(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND());
	file_dlg->AyncShowOpenFileDlg(cb);
}

void SessionBox::OnBtnFile()
{
	std::wstring file_type = L"文件格式(*.*)";
	LPCTSTR filter = L"*.*";
	std::map<LPCTSTR, LPCTSTR> filters;
	filters[file_type.c_str()] = filter;

	CFileDialogEx::FileDialogCallback2 cb = nbase::Bind(&SessionBox::OnFileSelected, this, std::placeholders::_1, std::placeholders::_2);

	CFileDialogEx* file_dlg = new CFileDialogEx();
	file_dlg->SetFilter(filters);
	file_dlg->SetParentWnd(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND());
	file_dlg->AyncShowOpenFileDlg(cb);
}

void SessionBox::OnImageSelected(bool is_snapchat, BOOL ret, std::wstring file_path)
{
	if (ret)
	{
		if (!is_snapchat)
		{
			emoji::InsertImageToEdit(input_edit_, file_path);
//			SendImage(file_path);
		}
		else {
			SendSnapChat(file_path);
		}
	}
}

void SessionBox::OnFileSelected(BOOL ret, std::wstring file_path)
{
	if (ret)
	{
		if (CheckFileSize(file_path))
		{
			SendFile(file_path);
		}
		else
		{
			ShowMsgBox(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), L"暂时只支持小于15MB的非空文件", MsgboxCallback(), L"提示", L"确定", L"");
		}
	}
}

void SessionBox::OnBtnJsb()
{
	int jsb = (rand() % 3 + rand() % 4 + rand() % 5) % 3 + 1;

	Json::Value json;
	Json::FastWriter writer;
	json["type"] = CustomMsgType_Jsb;
	json["data"]["value"] = jsb;

	SendJsb(writer.write(json));
}

void SessionBox::OnBtnEmoji()
{
	RECT rc = btn_face_->GetPos(true);
	POINT pt = { rc.left - 150, rc.top - 350 };
	::ClientToScreen(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), &pt);

	OnSelectEmotion sel = nbase::Bind(&SessionBox::OnEmotionSelected, this, std::placeholders::_1);
	OnSelectEmotion2 sel2 = nbase::Bind(&SessionBox::OnEmotionSelectedSticker, this, std::placeholders::_1, std::placeholders::_2);
	OnEmotionClose  cls = nbase::Bind(&SessionBox::OnEmotionClosed, this);

	EmojiForm* emoji_form = new EmojiForm;
	emoji_form->ShowEmoj(pt, sel, sel2, cls);
}

void SessionBox::OnEmotionSelected(std::wstring emo)
{
	std::wstring file;
	if (emoji::GetEmojiFileByTag(emo, file))
	{
		emoji::InsertFaceToEdit(input_edit_, file, emo);
	}
}
void SessionBox::OnEmotionSelectedSticker(const std::wstring &catalog, const std::wstring &name)
{
	SendSticker(nbase::UTF16ToUTF8(catalog), nbase::UTF16ToUTF8(name));
}

void SessionBox::OnEmotionClosed()
{
	btn_face_->Selected(false, false);

	input_edit_->SetFocus();
}
void SessionBox::OnBtnAudio()
{
	if (session_type_ == nim::kNIMSessionTypeP2P)
	{
		VideoManager::GetInstance()->ShowVideoChatForm(session_id_, false);
	}
}
void SessionBox::OnBtnVideo()
{
	if (session_type_ == nim::kNIMSessionTypeP2P)
	{
		VideoManager::GetInstance()->ShowVideoChatForm(session_id_, true);
	}
}
void SessionBox::OnBtnRts()
{
	if (session_type_ == nim::kNIMSessionTypeP2P)
	{
		RtsManager::GetInstance()->StartRtsForm(nim::kNIMRtsChannelTypeTcp | nim::kNIMRtsChannelTypeVchat, session_id_, "");
	}
}

void SessionBox::OnMenuRetweetMessage(const nim::IMMessage &msg)
{
	InvokeChatForm *contact_select_form = (InvokeChatForm *)WindowsManager::GetInstance()->GetWindow(InvokeChatForm::kClassName, nbase::UTF8ToUTF16(InvokeChatForm::kRetweetMessage));
	if (!contact_select_form)
	{
		contact_select_form = new InvokeChatForm(InvokeChatForm::kRetweetMessage, std::list<UTF8String>(), nbase::Bind(&SessionBox::OnSelectedRetweetList, this, msg, std::placeholders::_1/*, std::placeholders::_2*/)/*, true*/);
		contact_select_form->Create(NULL, L"", UI_WNDSTYLE_FRAME& ~WS_MAXIMIZEBOX, 0L);
		contact_select_form->CenterWindow();
	}
	else
	{
		contact_select_form->ActiveWindow();
	}
}

void SessionBox::OnBtnClip()
{
	if (CaptureManager::GetInstance()->IsCaptureTracking())
	{
		return;
	}

	//::ShowWindow( m_hWnd, SW_SHOWMINIMIZED );
	StdClosure callback = nbase::Bind(&SessionBox::DoClip, this);
	nbase::ThreadManager::PostDelayedTask(kThreadUI, callback, nbase::TimeDelta::FromMilliseconds(500));
}

void SessionBox::OnBtnRemoteDesktop()
{
	if (session_type_ == nim::kNIMSessionTypeP2P)
	{
		PopupRemoteDesktopMenu();
	}
}

void SessionBox::PopupRemoteDesktopMenu()
{
	POINT point;
	::GetCursorPos(&point);

	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"remote_desktop_menu.xml");
	pMenu->Init(xml, _T("xml"), point);

	CMenuElementUI* cop = (CMenuElementUI*)pMenu->FindControl(L"controlled");	//被控制
	cop->AttachSelect(nbase::Bind(&SessionBox::OnRemoteDesktopMenu, this, std::placeholders::_1));

	CMenuElementUI* del = (CMenuElementUI*)pMenu->FindControl(L"control");		//控制
	del->AttachSelect(nbase::Bind(&SessionBox::OnRemoteDesktopMenu, this, std::placeholders::_1));

	pMenu->Show();
}

bool SessionBox::OnRemoteDesktopMenu(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"control")
	{
		OnRemoteDesktopMenuControl(RDCHANNEL_TYPE::RDF_TYPE_REQ_CLIENT);
	}
	else if (name == L"controlled")
	{
		OnRemoteDesktopMenuControl(RDCHANNEL_TYPE::RDF_TYPE_REQ_SERVER);
	}
	return false;
}

void SessionBox::OnRemoteDesktopMenuControl(RDCHANNEL_TYPE type)
{
	//if (nim_comp::RemoteDesktopManager::GetInstance()->StartRemoting((RDCHANNEL_TYPE)type, LoginManager::GetInstance()->GetAccount(), session_id_))
	//{
	//	ShowRemoteDesktopTab(type);
	//}

	//nim_comp::REMOTEDESKTOP_RESULT ret;
	nim_comp::RemoteDesktopManager::GetInstance()->ShowRemotingForm(type, LoginManager::GetInstance()->GetAccount(), session_id_/*, ret*/);
}

void SessionBox::OnCloseInputMsgBoxCallback(MsgBoxRet ret)
{
	if (ret == MB_YES){
		//this->Close();
	}
}

void SessionBox::RemoveTip(SessionTipType type)
{
	if (type == STT_LINK)
	{
		Control* ctrl = msg_content_->FindSubControl(SESSION_TIP_LINK);
		if (ctrl)
			msg_content_->Remove(ctrl);
	}
	else if (type == STT_LEAVE)
	{
		Control* ctrl = msg_content_->FindSubControl(SESSION_TIP_LEAVE);
		if (ctrl)
			msg_content_->Remove(ctrl);
	}
	else if (type == STT_NOT_FRIEND)
	{
		Control* ctrl = msg_content_->FindSubControl(SESSION_TIP_NOT_FRIEND);
		if (ctrl)
			msg_content_->Remove(ctrl);
	}
	else if (type == STT_LOAD_MORE_MSG)
	{
		Control* ctrl = msg_list_->FindSubControl(CELL_LOAD_MORE_MSG);
		if (ctrl)
			msg_list_->Remove(ctrl);
	}
	else if (type == STT_WRITING)
	{
		Control* ctrl = msg_list_->FindSubControl(CELL_WRITING);
		if (ctrl)
			msg_list_->Remove(ctrl);
		has_writing_cell_ = false;
	}
}

void SessionBox::AddTip(SessionTipType type)
{
	if (type == STT_LINK)
	{
		RemoveTip(STT_LINK);

		HBox* hbox = new HBox;
		GlobalManager::FillBoxWithCache(hbox, L"session/tip_link.xml");
		msg_content_->Add(hbox);

		hbox->SetName(SESSION_TIP_LINK);
	}
	else if (type == STT_LEAVE)
	{
		RemoveTip(STT_LEAVE);

		HBox* hbox = new HBox;
		GlobalManager::FillBoxWithCache(hbox, L"session/tip_leave.xml");
		msg_content_->Add(hbox);

		hbox->SetName(SESSION_TIP_LEAVE);
	}
	else if (type == STT_NOT_FRIEND)
	{
		RemoveTip(STT_NOT_FRIEND);

		HBox* hbox = new HBox;
		GlobalManager::FillBoxWithCache(hbox, L"session/tip_not_friend.xml");
		msg_content_->Add(hbox);

		hbox->SetName(SESSION_TIP_NOT_FRIEND);
	}
	else if (type == STT_LOAD_MORE_MSG)
	{
		RemoveTip(STT_LOAD_MORE_MSG);

		ListContainerElement* cell = new ListContainerElement;
		GlobalManager::FillBoxWithCache(cell, L"session/cell_more_msg.xml");
		msg_list_->AddAt(cell, 0);

		cell->SetName(CELL_LOAD_MORE_MSG);
	}
}

void SessionBox::EnterTeamHandle()
{
	RemoveTip(STT_LEAVE);
	is_valid_ = true;

	//btn_header_->SetEnabled(true);
	input_edit_->SetEnabled(true);
	btn_send_->SetEnabled(true);
	FindSubControl(L"btn_custom_msg")->SetEnabled(true);
	FindSubControl(L"btn_msg_record")->SetEnabled(true);
	/*FindSubControl(L"btn_addmember")->SetVisible(true);
	FindSubControl(L"label_addmember")->SetVisible(true);*/
}

void SessionBox::LeaveTeamHandle()
{
	AddTip(STT_LEAVE);
	is_valid_ = false;

	/*btn_header_->SetEnabled(false);
	btn_invite_->SetEnabled(false);*/
	input_edit_->SetReadOnly(true);
	FindSubControl(L"bottom_panel")->SetEnabled(false);
	btn_new_broad_->SetEnabled(false);
	btn_refresh_member_->SetEnabled(false);
	FindSubControl(L"btn_addmember")->SetVisible(false);
	FindSubControl(L"label_addmember")->SetVisible(false);
}

void SessionBox::DismissTeamHandle()
{
	AddTip(STT_DISMISS);
	is_valid_ = false;

	/*btn_header_->SetEnabled(false);
	btn_invite_->SetEnabled(false);*/
	input_edit_->SetReadOnly(true);
	FindSubControl(L"bottom_panel")->SetEnabled(false);
	btn_new_broad_->SetEnabled(false);
	btn_refresh_member_->SetEnabled(false);
}

BOOL SessionBox::CheckDropEnable(POINTL pt)
{
	RECT rect = input_edit_->GetPos();
	ClientToScreen(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), (LPPOINT)&rect);
	ClientToScreen(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), ((LPPOINT)&rect) + 1);
	RECT rect_ = msg_content_->GetPos();
	ClientToScreen(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), (LPPOINT)&rect_);
	ClientToScreen(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), ((LPPOINT)&rect_) + 1);
	if (pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom)
	{
		return TRUE;
	}
	else if (pt.x >= rect_.left && pt.x <= rect_.right && pt.y >= rect_.top && pt.y <= rect_.bottom)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

HRESULT SessionBox::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	return droptarget_->QueryInterface(iid, ppvObject);
}

ULONG SessionBox::AddRef(void)
{
	if (droptarget_ == NULL)
	{
		return 0;
	}
	return droptarget_->AddRef();
}

ULONG SessionBox::Release(void)
{
	if (droptarget_ == NULL)
	{
		return 0;
	}
	return droptarget_->Release();
}

HRESULT SessionBox::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	if (!CheckDropEnable(pt))
	{
		*pdwEffect = DROPEFFECT_NONE;
	}
	return droptarget_->DragOver(grfKeyState, pt, pdwEffect);
}
HRESULT SessionBox::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	return droptarget_->DragEnter(pDataObject, grfKeyState, pt, pdwEffect);
}
HRESULT SessionBox::DragLeave(void)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	return droptarget_->DragLeave();
}

HRESULT SessionBox::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	if (pDataObj)
	{
		STGMEDIUM stgMedium;
		FORMATETC cFmt = { (CLIPFORMAT)CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		HRESULT hr = pDataObj->GetData(&cFmt, &stgMedium);
		if (hr == S_OK)
		{
			//拖曳文件
			HDROP file = NULL;
			file = (HDROP)GlobalLock(stgMedium.hGlobal);
			GlobalUnlock(stgMedium.hGlobal);
			OnDropFile(file);
			::ReleaseStgMedium(&stgMedium);
			droptarget_->DragLeave();
			return S_OK;
		}
	}
	return droptarget_->Drop(pDataObj, grfKeyState, pt, pdwEffect);
}

void SessionBox::UpdateSessionIcon(const std::wstring &icon)
{
#if 0
	if (!nbase::FilePathIsExist(icon, false))
		return;

	if (icon_handle_)
	{
		::DestroyIcon(icon_handle_);
	}

	Gdiplus::Bitmap bm(icon.c_str());
	bm.GetHICON(&icon_handle_);
	if (icon_handle_)
	{
		::SendMessage(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), WM_SETICON, (WPARAM)TRUE, (LPARAM)icon_handle_);
		::SendMessage(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), WM_SETICON, (WPARAM)FALSE, (LPARAM)icon_handle_);
	}
#endif
}

void SessionBox::CheckHeader()
{
	std::wstring name, photo;
	bool is_team = (session_type_ == nim::kNIMSessionTypeTeam);

	if (is_team)
	{
		//label_tid_->SetUTF8Text(session_id_);
		name = TeamService::GetInstance()->GetTeamName(session_id_);
		photo = PhotoService::GetInstance()->GetTeamPhoto(session_id_, true);

	}
	else
	{
		//label_tid_->SetVisible(false);
		name = IsFileTransPhone() ? L"我的手机" : UserService::GetInstance()->GetUserName(session_id_);
		photo = PhotoService::GetInstance()->GetUserPhoto(session_id_);
	}

	/*label_title_->SetText(name);
	SetTaskbarTitle(name);
	btn_header_->SetBkImage(photo);*/
	UpdateSessionIcon(photo);
}

void SessionBox::OnSelectedRetweetList(nim::IMMessage msg, const std::list<std::string>& friend_list/*, const std::list<std::string>& team_list*/)
{
#if 1
	std::list<std::string> team_list;
#endif
	if (friend_list.empty() && team_list.empty())
		return;

	auto retweet_cb = [this](const std::string &msg, const std::string &receiver_accid)
	{
		nim::IMMessage sending_msg;
		nim::Talk::ParseIMMessage(msg, sending_msg);
		sending_msg.sender_accid_ = LoginManager::GetInstance()->GetAccount();

		SessionBox *session = SessionManager::GetInstance()->FindSessionBox(receiver_accid);
		if (session)
		{
			session->AddRetweetMsg(sending_msg);
		}
		nim::Talk::SendMsg(msg);
	};

	for (auto &it : friend_list)
	{
		std::string send_msg = nim::Talk::CreateRetweetMessage(msg.ToJsonString(false), QString::GetGUID(), nim::kNIMSessionTypeP2P, it, msg.msg_setting_, 1000 * nbase::Time::Now().ToTimeT());
		retweet_cb(send_msg, it);
	}

	for (auto &it : team_list)
	{
		std::string send_msg = nim::Talk::CreateRetweetMessage(msg.ToJsonString(false), QString::GetGUID(), nim::kNIMSessionTypeTeam, it, msg.msg_setting_, 1000 * nbase::Time::Now().ToTimeT());
		retweet_cb(send_msg, it);
	}
}

bool SessionBox::OnBtnInvite(ui::EventArgs* param)
{
	std::wstring caption = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_INVITEUSERFORM_INVITE_JOINCHAT");
	std::wstring session_id = nbase::UTF8ToUTF16(session_id_);
	InvokeChatForm* invite_user_form = (InvokeChatForm*)WindowsManager::GetInstance()->GetWindow\
		(InvokeChatForm::kClassName, session_id);
	if (invite_user_form == NULL)
	{
		std::list<UTF8String> ids_;
		ids_.push_back(session_id_);
		invite_user_form = new InvokeChatForm(session_id_, ids_, nbase::Bind(&SessionBox::CreateGroup, this, std::placeholders::_1));
		invite_user_form->Create(NULL, caption.c_str(), WS_OVERLAPPEDWINDOW& ~WS_MAXIMIZEBOX, 0L);
		invite_user_form->CenterWindow();
		invite_user_form->ShowWindow(true);
	}
	else
	{
		invite_user_form->ActiveWindow();
	}

	return true;
}

void SessionBox::CreateGroup(const std::list<UTF8String>& _id_list)
{
	std::list<UTF8String> id_list = _id_list;
	id_list.push_back(session_id_);
	
	UTF16String user_names;
	auto it = id_list.cbegin();
	for (int i = 0; it != id_list.cend() && i < 2; it++, i++)
		user_names += UserService::GetInstance()->GetUserName(*it, false) + L";";
	user_names += UserService::GetInstance()->GetUserName(it == id_list.end() ? LoginManager::GetInstance()->GetAccount() : *it, false);

	nim::TeamInfo tinfo;
	tinfo.SetName(nbase::UTF16ToUTF8(user_names));
	tinfo.SetType(nim::kNIMTeamTypeNormal);
	nim::Team::CreateTeamAsync(tinfo, id_list, "", nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));

	//Close();
}

bool SessionBox::OnBtnHeaderClick(ui::EventArgs* param)
{
	if (session_type_ == nim::kNIMSessionTypeTeam) {
		std::wstring session_id = nbase::UTF8ToUTF16(session_id_);
		TeamInfoForm* team_info_form = (TeamInfoForm*)WindowsManager::GetInstance()->GetWindow\
			(TeamInfoForm::kClassName, session_id);
		if (team_info_form == NULL)
		{
			std::wstring title = team_info_.GetType() == nim::kNIMTeamTypeNormal ? L"讨论组资料" : L"群资料";
			team_info_form = new TeamInfoForm(false, team_info_.GetType(), session_id_, team_info_);
			team_info_form->Create(NULL, title.c_str(), WS_OVERLAPPEDWINDOW& ~WS_MAXIMIZEBOX, 0L);
			team_info_form->CenterWindow();
			team_info_form->ShowWindow(true);
		}
		else
		{
			team_info_form->ActiveWindow();
		}
	}
	else if (session_type_ == nim::kNIMSessionTypeP2P)
	{
		std::wstring session_id = nbase::UTF8ToUTF16(session_id_);
		ProfileForm::ShowProfileForm(nbase::UTF16ToUTF8(session_id));
	}
	return true;
}

bool SessionBox::IsFileTransPhone()

{
	if (session_type_ == nim::kNIMSessionTypeP2P && LoginManager::GetInstance()->IsEqual(session_id_))
	{
		return true;
	}
	return false;
}

void SessionBox::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	auto refresh_msglist = [this](const nim::UserNameCard& info) //更新消息列表中消息气泡的头像和名称
	{
		bool refresh_icon = info.ExistValue(nim::kUserNameCardKeyIconUrl);
		bool refresh_show_name = info.ExistValue(nim::kUserNameCardKeyName) && session_type_ == nim::kNIMSessionTypeTeam;
		if (refresh_show_name)
		{
			auto iter = team_member_info_list_.find(info.GetAccId());
			if (iter != team_member_info_list_.cend() && !iter->second.GetNick().empty()) //设置了群名片，不更新
				refresh_show_name = false;
		}
		
		if (refresh_icon)
		{
			int msg_count = msg_list_->GetCount();
			for (int i = 0; i < msg_count; i++)
			{
				MsgBubbleItem* bubble_item = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
				if (bubble_item != NULL && bubble_item->msg_.sender_accid_ == info.GetAccId())
					bubble_item->SetShowHeader();
			}
		}

		if (refresh_show_name)
			RefreshMsglistShowname(info.GetAccId());
	};

	for (auto info : uinfos)
	{
		if (session_type_ == nim::kNIMSessionTypeP2P)
		{
			if (info.GetAccId() == session_id_)
				CheckHeader();
		}
		else if (session_type_ == nim::kNIMSessionTypeTeam)
		{
			if (team_info_.GetType() == nim::kNIMTeamTypeAdvanced)
			{
				auto iter = team_member_info_list_.find(info.GetAccId());
				if (iter != team_member_info_list_.end())
				{
					//更新群成员列表信息
					TeamItem* item = (TeamItem*)member_list_->FindSubControl(nbase::UTF8ToUTF16(info.GetAccId()));
					if (item != NULL)
						item->InitInfo(iter->second);
				}
			}
		}

		refresh_msglist(info);
	}
}

void SessionBox::OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path)
{
	auto refresh_msglist_photo = [=]() //更新消息列表头像
	{
		int msg_count = msg_list_->GetCount();
		for (int i = 0; i < msg_count; i++)
		{
			MsgBubbleItem* bubble_item = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
			if (bubble_item != NULL && bubble_item->msg_.sender_accid_ == accid)
				bubble_item->msg_header_button_->SetBkImage(photo_path);
		}
	};

	if (accid == session_id_)
	{
		UpdateSessionIcon(photo_path);
		//btn_header_->SetBkImage(photo_path);
	}

	if (type == kUser)
	{
		// 群聊时，更新群成员列表和消息列表中用户头像
		if (session_type_ == nim::kNIMSessionTypeTeam)
		{
			if (team_info_.GetType() == nim::kNIMTeamTypeAdvanced && team_member_info_list_.find(accid) != team_member_info_list_.cend())
			{
				TeamItem* item = (TeamItem*)member_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
				if (item != NULL)
					item->FindSubControl(L"member_icon")->SetBkImage(photo_path);

				refresh_msglist_photo();
			}
			else if (team_info_.GetType() == nim::kNIMTeamTypeNormal)
				refresh_msglist_photo();
		}
		else
			refresh_msglist_photo();
	}
}

void SessionBox::ShowGroupInfo()
{
	if (session_type_ == nim::NIMSessionType::kNIMSessionTypeTeam)
	{
		if (!frame_right_->IsVisible())
			frame_right_->SetVisible(true);
		if (frame_right_->GetCurSel() == 0 || frame_right_->GetCurSel() == -1)
		{
			frame_right_->SelectItem(1);
			//frame_right_->SetFocus();
			//frame_right_->FindSubControl(L"group_member_info")->SetFocus();
		}
		else if (frame_right_->GetCurSel() == 1)
		{
			frame_right_->SelectItem(0);
		}
	}
}

bool SessionBox::OnShowGroupInfo(ui::EventArgs* param)
{
	if (frame_right_->IsVisible())
		ShowGroupInfo();
	return true;
}
#endif

HRESULT SessionBox::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	return droptarget_->QueryInterface(iid, ppvObject);
}

ULONG SessionBox::AddRef(void)
{
	if (droptarget_ == NULL)
	{
		return 0;
	}
	return droptarget_->AddRef();
}

ULONG SessionBox::Release(void)
{
	if (droptarget_ == NULL)
	{
		return 0;
	}
	return droptarget_->Release();
}

HRESULT SessionBox::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	/*if (!CheckDropEnable(pt))
	{
		*pdwEffect = DROPEFFECT_NONE;
	}*/
	return droptarget_->DragOver(grfKeyState, pt, pdwEffect);
}
HRESULT SessionBox::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	return droptarget_->DragEnter(pDataObject, grfKeyState, pt, pdwEffect);
}
HRESULT SessionBox::DragLeave(void)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	return droptarget_->DragLeave();
}

HRESULT SessionBox::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
{
	if (droptarget_ == NULL)
	{
		return -1;
	}
	if (pDataObj)
	{
		STGMEDIUM stgMedium;
		FORMATETC cFmt = { (CLIPFORMAT)CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		HRESULT hr = pDataObj->GetData(&cFmt, &stgMedium);
		if (hr == S_OK)
		{
			//拖曳文件
			HDROP file = NULL;
			file = (HDROP)GlobalLock(stgMedium.hGlobal);
			GlobalUnlock(stgMedium.hGlobal);
			//OnDropFile(file);
			::ReleaseStgMedium(&stgMedium);
			droptarget_->DragLeave();
			return S_OK;
		}
	}
	return droptarget_->Drop(pDataObj, grfKeyState, pt, pdwEffect);
}

} // namespace nim_comp


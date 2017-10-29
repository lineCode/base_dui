#include "stdafx.h"
#include "resource.h"
#include "main_form.h"
//#include "util/user.h"
//#include "callback/team/team_callback.h"
//#include "module/config/config_helper.h"
//#include "gui/about/about_form.h"
//#include "gui/msglogmanage/msglog_manage_form.h"
//#include "gui/contact_select_form/contact_select_form.h"
//#include "gui/chatroom_frontpage.h"
//#include "cef/cef_module/cef_manager.h"
//#include "gui/cef/cef_form.h"
//#include "gui/cef/cef_native_form.h"

using namespace dui;

const LPCTSTR MainForm::kClassName	= L"MainForm";

MainForm::MainForm()
{
	/*is_busy_ = false;
	is_trayicon_left_double_clicked_ = false;

	OnUserInfoChangeCallback cb1 = nbase::Bind(&MainForm::OnUserInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(nim_ui::UserManager::GetInstance()->RegUserInfoChange(cb1));

	OnPhotoReadyCallback cb2 = nbase::Bind(&MainForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(nim_ui::PhotoManager::GetInstance()->RegPhotoReady(cb2));*/
}

MainForm::~MainForm()
{

}

std::wstring MainForm::GetSkinFolder()
{
	return L"main_vv";
}

std::wstring MainForm::GetSkinFile()
{
	return L"main.xml";
}

LPCTSTR MainForm::GetWindowClassName() const
{
	return kClassName;
}

LPCTSTR MainForm::GetWindowId() const
{
	return kClassName;
}

UINT MainForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void MainForm::OnEsc( BOOL &bHandled )
{
	bHandled = TRUE;
}

void MainForm::OnFinalMessage(HWND hWnd)
{
	/*TrayIcon::GetInstance()->Destroy();
	nim_ui::SessionListManager::GetInstance()->AttachListBox(nullptr);
	nim_ui::ContactsListManager::GetInstance()->AttachFriendListBox(nullptr);
	nim_ui::ContactsListManager::GetInstance()->AttachGroupListBox(nullptr);*/

	__super::OnFinalMessage(hWnd);
}

LRESULT MainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	::ShowWindow(m_hWnd, SW_HIDE);
	return 0;
}

void MainForm::InitWindow()
{
	SetIcon(IDI_ICON1);
	SetTaskbarTitle(L"微微");

	btn_header_ = dynamic_cast<Button*>(m_PaintManager.FindControl(_T("btn_header")));
	tab_session_friend_ = dynamic_cast<TabLayout*>(m_PaintManager.FindControl(_T("tab_session_friend")));

	list_friend_ = dynamic_cast<List*>(m_PaintManager.FindControl(_T("list_friend")));

	//nim_comp::UserManager::GetInstance()->DoLoadUser();
}

void MainForm::OnClick(dui::TNotify& msg)
{
	bool bHandle = false;

	Control *pControl = msg.pSender;
	String name = pControl->GetName();
	wprintf(L"MainForm::OnClicked %s\n", name.c_str());
	if (name == _T("btnbox_opt_session"))
	{
		tab_session_friend_->SelectItem(0);
	}
	else if (name == _T("btnbox_opt_friends"))
	{
		tab_session_friend_->SelectItem(1);
	}
	else if (name == _T("btnbox_opt_groups"))
	{
		tab_session_friend_->SelectItem(2);
	}

	if (!bHandle)
	{
		__super::OnClick(msg);
	}
}
#if 0
bool MainForm::MainMenuButtonClick(ui::EventArgs* param)
{
	RECT rect = param->pSender->GetPos();
	CPoint point;
	point.x = rect.left - 15;
	point.y = rect.bottom + 10;
	ClientToScreen(m_hWnd, &point);
	PopupMainMenu(point);
	return true;
}

void MainForm::PopupMainMenu(POINT point)
{
	//创建菜单窗口
	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"main_menu.xml");
	pMenu->Init(xml, _T("xml"), point);
	//注册回调
	CMenuElementUI* look_log = (CMenuElementUI*)pMenu->FindControl(L"look_log");
	look_log->AttachSelect(nbase::Bind(&MainForm::LookLogMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* file_trans = (CMenuElementUI*)pMenu->FindControl(L"file_helper");
	file_trans->AttachSelect(nbase::Bind(&MainForm::FileTransMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* address = (CMenuElementUI*)pMenu->FindControl(L"address");
	address->AttachSelect(nbase::Bind(&MainForm::AddressMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* export_msglog = (CMenuElementUI*)pMenu->FindControl(L"export_msglog");
	export_msglog->AttachSelect(nbase::Bind(&MainForm::ExportMsglogMenuItemClick, this, std::placeholders::_1));
	CMenuElementUI* import_msglog = (CMenuElementUI*)pMenu->FindControl(L"import_msglog");
	import_msglog->AttachSelect(nbase::Bind(&MainForm::ImportMsglogMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* clear_chat_record = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record");
	clear_chat_record->AttachSelect(nbase::Bind(&MainForm::ClearChatRecordMenuItemClick, this, true, std::placeholders::_1));
	CMenuElementUI* clear_chat_record_ex = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record_ex");
	clear_chat_record_ex->AttachSelect(nbase::Bind(&MainForm::ClearChatRecordMenuItemClick, this, false, std::placeholders::_1));

	CMenuElementUI* clear_chat_record_p2p = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record_p2p");
	clear_chat_record_p2p->AttachSelect(nbase::Bind(&MainForm::ClearChatRecordBySessionTypeMenuItemClick, this, true, nim::kNIMSessionTypeP2P, std::placeholders::_1));
	CMenuElementUI* clear_chat_record_p2p_ex = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record_p2p_ex");
	clear_chat_record_p2p_ex->AttachSelect(nbase::Bind(&MainForm::ClearChatRecordBySessionTypeMenuItemClick, this, false, nim::kNIMSessionTypeP2P, std::placeholders::_1));
	CMenuElementUI* clear_chat_record_team = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record_team");
	clear_chat_record_team->AttachSelect(nbase::Bind(&MainForm::ClearChatRecordBySessionTypeMenuItemClick, this, true, nim::kNIMSessionTypeTeam, std::placeholders::_1));
	CMenuElementUI* clear_chat_record_team_ex = (CMenuElementUI*)pMenu->FindControl(L"clear_chat_record_team_ex");
	clear_chat_record_team_ex->AttachSelect(nbase::Bind(&MainForm::ClearChatRecordBySessionTypeMenuItemClick, this, false, nim::kNIMSessionTypeTeam, std::placeholders::_1));

	CMenuElementUI* vchat_setting = (CMenuElementUI*)pMenu->FindControl(L"vchat_setting");
	vchat_setting->AttachSelect(nbase::Bind(&MainForm::VChatSettingMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* rts_replay = (CMenuElementUI*)pMenu->FindControl(L"rts_replay");
	rts_replay->AttachSelect(nbase::Bind(&MainForm::RtsReplayMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* adapt_dpi = (CMenuElementUI*)pMenu->FindControl(L"adapt_dpi");
	adapt_dpi->AttachSelect(nbase::Bind(&MainForm::AdaptDpiMenuItemClick, this, std::placeholders::_1));
	CheckBox* check_dpi = (CheckBox*)adapt_dpi->FindSubControl(L"check_dpi");
	check_dpi->Selected(ConfigHelper::GetInstance()->IsAdaptDpi());

	CMenuElementUI* language = (CMenuElementUI*)pMenu->FindControl(L"language");
	language->AttachMouseEnter(nbase::Bind(&MainForm::ShowLanguageList, this, std::placeholders::_1));
	language->AttachMouseLeave(nbase::Bind(&MainForm::CloseLanguageList, this, std::placeholders::_1, true));
	pMenu->AttachWindowClose(nbase::Bind(&MainForm::CloseLanguageList, this, std::placeholders::_1, false));

	CMenuElementUI* about = (CMenuElementUI*)pMenu->FindControl(L"about");
	about->AttachSelect(nbase::Bind(&MainForm::AboutMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* logoff = (CMenuElementUI*)pMenu->FindControl(L"logoff");
	logoff->AttachSelect(nbase::Bind(&MainForm::LogoffMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* quit = (CMenuElementUI*)pMenu->FindControl(L"quit");
	quit->AttachSelect(nbase::Bind(&MainForm::QuitMenuItemClick, this, std::placeholders::_1));
	//显示
	pMenu->Show();
}

static void LookLogClick(HWND m_hWnd)
{
	//TODO：暂时显示用户数据所在的目录，方便收集用户反馈！
	std::wstring dir = nim_ui::UserConfig::GetInstance()->GetUserDataPath();
	std::wstring tip = nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_MENU_CURRENT_USER_DIR").c_str(), dir.c_str());
	ShowMsgBox(m_hWnd, MsgboxCallback(), tip, false, L"STRING_TIPS", true, L"STRID_MAINWINDOW_MENU_GOT_IT", true);
	HINSTANCE inst = ::ShellExecute(NULL, L"open", dir.c_str(), NULL, NULL, SW_SHOW);
	int ret = (int) inst;
	if(ret > 32)
	{
		QLOG_APP(L"open user data path: {0}") <<dir.c_str();
		return;
	}
	else
	{
		QLOG_ERR(L"failed to open user data path: {0}") <<dir.c_str();
		return;
	}
}

bool MainForm::LookLogMenuItemClick(ui::EventArgs* param)
{
	nbase::ThreadManager::PostTask(shared::kThreadUI, nbase::Bind(&LookLogClick, GetHWND()));
	return false;
}

bool MainForm::FileTransMenuItemClick(ui::EventArgs* param)
{
	nim_ui::SessionManager::GetInstance()->OpenSessionBox(nim_ui::LoginManager::GetInstance()->GetAccount(), nim::kNIMSessionTypeP2P);
	return true;
}

bool MainForm::AddressMenuItemClick(ui::EventArgs* param)
{
	if (!nim_ui::LoginManager::GetInstance()->IsLinkActive())
		nim_ui::WindowsManager::GetInstance()->ShowLinkForm();
	else
		nim_ui::ContactsListManager::GetInstance()->InvokeGetAllUserInfo();
	return true;
}
bool MainForm::ExportMsglogMenuItemClick(ui::EventArgs* param)
{
	MsglogManageForm *form = nim_ui::WindowsManager::SingletonShow<MsglogManageForm>(MsglogManageForm::kClassName);
	form->SetType(true);

	return true;
}
bool MainForm::ImportMsglogMenuItemClick(ui::EventArgs* param)
{
	MsglogManageForm *form = nim_ui::WindowsManager::SingletonShow<MsglogManageForm>(MsglogManageForm::kClassName);
	form->SetType(false);

	return true;
}

bool MainForm::ClearChatRecordMenuItemClick(bool del_session, ui::EventArgs* param)
{
	nim::MsgLog::DeleteAllAsync(del_session, nim::MsgLog::DeleteAllCallback());
	return true;
}

bool MainForm::ClearChatRecordBySessionTypeMenuItemClick(bool del_session, nim::NIMSessionType type, ui::EventArgs* param)
{
	nim::MsgLog::DeleteBySessionTypeAsync(del_session, type, nim::MsgLog::DeleteBySessionTypeCallback());
	return true;
}

bool MainForm::VChatSettingMenuItemClick(ui::EventArgs* param)
{
	nim_ui::WindowsManager::GetInstance()->ShowVideoSettingForm();
	return true;
}

bool MainForm::RtsReplayMenuItemClick(ui::EventArgs* param)
{
	nim_ui::WindowsManager::SingletonShow<nim_comp::RtsReplay>(nim_comp::RtsReplay::kClassName);
	return true;
}

bool MainForm::AdaptDpiMenuItemClick(ui::EventArgs* param)
{
	CMenuElementUI* menu_item = (CMenuElementUI*)(param->pSender);
	CheckBox* check_dpi = (CheckBox*)menu_item->FindSubControl(L"check_dpi");
	ConfigHelper::GetInstance()->SetAdaptDpi(!check_dpi->IsSelected());
	return true;
}

bool MainForm::ShowLanguageList(ui::EventArgs* param)
{
	std::wstring menu_name = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_MENU_LANGUAGE_LIST");
	HWND hWnd = ::FindWindow(L"MenuWnd", menu_name.c_str());
	if (hWnd) //语言列表已经打开
	{
		::ShowWindow(hWnd, SW_SHOWNOACTIVATE);
		return true;
	}

	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"language_list.xml");
	ui::UiRect menu_pos = param->pSender->GetWindow()->GetPos(true);
	ui::UiRect elem_pos = param->pSender->GetPos(true);
	ui::CPoint popup_pt(menu_pos.left + elem_pos.right + 2, menu_pos.top + elem_pos.top);
	pMenu->Init(xml, _T("xml"), popup_pt, CMenuWnd::RIGHT_BOTTOM, true);
	::SetWindowText(pMenu->GetHWND(), menu_name.c_str());
	//注册回调
	std::string current_language = ConfigHelper::GetInstance()->GetLanguage();
	Box* language_list = (Box*)((Box*)pMenu->GetRoot())->GetItemAt(0);
	if (language_list)
	for (int i = 0; i < language_list->GetCount(); i++)
	{
		CMenuElementUI* language_item = dynamic_cast<CMenuElementUI*>(language_list->GetItemAt(i));
		if (language_item)
		{
			if (current_language == language_item->GetUTF8Name())
				language_item->Selected(true, false);
			language_item->AttachSelect(nbase::Bind(&MainForm::OnSelectLanguage, this, std::placeholders::_1));
		}
	}

	//显示
	pMenu->Show();
	return true;
}

bool MainForm::CloseLanguageList(ui::EventArgs* param, bool check_mouse)
{
	std::wstring menu_name = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_MENU_LANGUAGE_LIST");
	HWND hWnd = ::FindWindow(L"MenuWnd", menu_name.c_str());
	if (!hWnd)
		return true;

	if (check_mouse)
	{
		RECT menu_rect;
		::GetWindowRect(hWnd, &menu_rect);
		POINT mouse_pt;
		::GetCursorPos(&mouse_pt);
		if (::PtInRect(&menu_rect, mouse_pt))
			return true; //鼠标在语言列表上，就不关闭
	}

	::DestroyWindow(hWnd);

	return true;
}

bool MainForm::OnSelectLanguage(ui::EventArgs* param)
{
	std::string current_language = ConfigHelper::GetInstance()->GetLanguage();
	std::string selected_language = param->pSender->GetUTF8Name();
	if (current_language == selected_language)
		return true;

	MsgboxCallback cb = nbase::Bind(&MainForm::OnSelectLanguageCallback, this, std::placeholders::_1, selected_language);
	ShowMsgBox(m_hWnd, cb, L"STRID_MAINWINDOW_CHANGE_LANGUAGE_TIP", true, L"STRING_TIPS", true, L"STRING_OK", true, L"STRING_NO", true);

	return true;
}

void MainForm::OnSelectLanguageCallback(MsgBoxRet ret, const std::string& language)
{
	if (ret == MB_YES)
	{
		ConfigHelper::GetInstance()->SetLanguage(language);
		LogoffMenuItemClick(NULL);
		QLOG_APP(L"Selected language: {0}") << nbase::UTF8ToUTF16(language);
	}
}

bool MainForm::SessionListMenuItemClick(ui::EventArgs* param)
{
	OptionBox* session = (OptionBox*) FindControl(L"btn_session_list");
	session->Selected(true, true);
	LeftClick();
	return true;
}

bool MainForm::AboutMenuItemClick(ui::EventArgs* param)
{
	nim_ui::WindowsManager::SingletonShow<AboutForm>(AboutForm::kClassName);
	return false;
}

bool MainForm::LogoffMenuItemClick(ui::EventArgs* param)
{
	QCommand::Set(kCmdRestart, L"true");
	std::wstring wacc = nbase::UTF8ToUTF16(nim_ui::LoginManager::GetInstance()->GetAccount());
	QCommand::Set(kCmdAccount, wacc);
	nim_ui::LoginManager::GetInstance()->DoLogout(false, nim::kNIMLogoutChangeAccout);
	return true;
}

bool MainForm::QuitMenuItemClick(ui::EventArgs* param)
{
	nim_ui::LoginManager::GetInstance()->DoLogout(false);
	return true;
}

bool MainForm::OnlineStateMenuButtonClick(ui::EventArgs* param)
{
	RECT rect = param->pSender->GetPos();
	CPoint point;
	point.x = rect.left - 15;
	point.y = rect.bottom + 10;
	ClientToScreen(m_hWnd, &point);
	
	//创建菜单窗口
	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"online_state_menu.xml");
	pMenu->Init(xml, _T("xml"), point);
	//注册回调
	CMenuElementUI* look_log = (CMenuElementUI*)pMenu->FindControl(L"online");
	look_log->AttachSelect(nbase::Bind(&MainForm::OnlineStateMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* file_trans = (CMenuElementUI*)pMenu->FindControl(L"busy");
	file_trans->AttachSelect(nbase::Bind(&MainForm::OnlineStateMenuItemClick, this, std::placeholders::_1));

	//显示
	pMenu->Show();
	return true;
}

bool MainForm::OnlineStateMenuItemClick(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"online")
	{		
		if (!is_busy_)
			return true;

		is_busy_ = false;
	}
	else if (name == L"busy")
	{
		if (is_busy_)
			return true;

		is_busy_ = true;
	}

	SetOnlineState();
	return true;
}

void MainForm::CheckOnlineState(const Json::Value& json)
{
	bool link = json["link"].asBool();
	if (link)
	{
		SetOnlineState();
	}
}

void MainForm::SetOnlineState()
{
	if (!nim_comp::SubscribeEventManager::GetInstance()->IsEnabled())
		return;

	nim::EventData event_data = nim_ui::SubscribeEventManager::GetInstance()->CreateBusyEvent(is_busy_);
	nim::SubscribeEvent::Publish(event_data,
		this->ToWeakCallback([this](nim::NIMResCode res_code, int event_type, const nim::EventData& event_data){
		if (res_code == nim::kNIMResSuccess)
		{
			if (is_busy_)
				btn_online_state_->SetBkImage(L"..\\menu\\icon_busy.png");
			else
				btn_online_state_->SetBkImage(L"..\\menu\\icon_online.png");
		}
		else
		{
			QLOG_ERR(L"OnlineStateMenuItemClick publish busy event error, code:{0}, event_type:{1}") << res_code << event_type;
		}
	}));
}
#endif
void MainForm::PopupTrayMenu(POINT point)
{
	//创建菜单窗口
	//CMenuWnd* pMenu = new CMenuWnd(NULL);
	//STRINGorID xml(L"tray_menu.xml");
	//pMenu->Init(xml, _T("xml"), point, CMenuWnd::RIGHT_TOP);
	////注册回调
	//CMenuElementUI* display_session_list = (CMenuElementUI*)pMenu->FindControl(L"display_session_list");
	//display_session_list->AttachSelect(nbase::Bind(&MainForm::SessionListMenuItemClick, this, std::placeholders::_1));

	//CMenuElementUI* logoff = (CMenuElementUI*)pMenu->FindControl(L"logoff");
	//logoff->AttachSelect(nbase::Bind(&MainForm::LogoffMenuItemClick, this, std::placeholders::_1));

	//CMenuElementUI* quit = (CMenuElementUI*)pMenu->FindControl(L"quit");
	//quit->AttachSelect(nbase::Bind(&MainForm::QuitMenuItemClick, this, std::placeholders::_1));
	////显示
	//pMenu->Show();
}


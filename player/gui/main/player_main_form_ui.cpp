#include "stdafx.h"
#include "resource.h"
#include "player_main_form.h"
#include "module/login/login_manager.h"
#include "module/tray/tray_manager.h"
#include "shared/modal_wnd/file_dialog_ex.h"

#include "../live555/testProgs/playCommon.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"

#include "rtsp/rtsp.h"

PlayerMainForm::PlayerMainForm()
{

}

PlayerMainForm::~PlayerMainForm()
{
	printf("~PlayerMainForm()\n");
}

void PlayerMainForm::Notify(dui::Event& msg)
{
	bool bHandle = false;

	Control *pControl = msg.pSender;
	String name = pControl->GetName();
	if (msg.Type == UIEVENT_ITEMCLICK)
	{
		if (name == _T("menu_logoff"))
			MenuLogoffClick(&msg);
		else if (name == _T("menu_logout"))
			MenuLogoutClick(&msg);
		bHandle = true;
	}
	if (!bHandle)
		__super::Notify(msg);
}

void PlayerMainForm::OnFinalMessage(HWND hWnd)
{
	m_video_screen->StopVideo(true);
	nim_comp::TrayManager::GetInstance()->Destroy();
	return __super::OnFinalMessage(hWnd);
}

LRESULT PlayerMainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	::ShowWindow(m_hWnd, SW_HIDE);
	return 0;
}

void PlayerMainForm::OnEsc(BOOL &bHandled)
{
	bHandled = TRUE;
}

void PlayerMainForm::InitWindow()
{
	nim_comp::TrayManager::GetInstance()->Init();
	nim_comp::TrayManager::GetInstance()->SetTrayIcon(::LoadIconW(nbase::win32::GetCurrentModuleHandle(), MAKEINTRESOURCE(IDI_ICON1)), L"视界");
	nim_comp::TrayManager::GetInstance()->RegEventCallback(std::bind(&PlayerMainForm::TrayLeftClick, this), nim_comp::TrayEventType_LeftClick);
	nim_comp::TrayManager::GetInstance()->RegEventCallback(std::bind(&PlayerMainForm::TrayRightClick, this), nim_comp::TrayEventType_RightClick);

	m_video_screen = static_cast<Screen*>(m_manager.FindControl(_T("video_screen")));
}

void PlayerMainForm::TrayLeftClick()
{
	::ShowWindow(m_hWnd, SW_SHOW);
	this->ActiveWindow();
	::SetForegroundWindow(m_hWnd);
	::BringWindowToTop(m_hWnd);
	/*nim_comp::TrayManager::GetInstance()->StopTrayIconAnimate();*/
}

void PlayerMainForm::TrayRightClick()
{
	POINT point;
	::GetCursorPos(&point);
	PopupTrayMenu(point);
}

void PlayerMainForm::PopupTrayMenu(POINT point)
{
	dui::CMenuWnd* pMenu = new dui::CMenuWnd;
	String xml(L"tray_menu.xml");
	pMenu->Init(NULL, xml, _T("menu"), point, &m_manager, NULL, eMenuAlignment_Left | eMenuAlignment_Bottom);
	pMenu->Show();
}

bool PlayerMainForm::MenuLogoffClick(dui::Event* param)
{
	return true;
}

bool PlayerMainForm::MenuLogoutClick(dui::Event* param)
{
	nim_comp::LoginManager::GetInstance()->DoLogout(false);
	return true;
}

void PlayerMainForm::OnClick(dui::Event& msg)
{
	if (msg.pSender->GetName() == _T("btnRecord"))
		/*StartVideo(_T("../bin/other/video.mp4"))*/;
	else if (msg.pSender->GetName() == _T("btnPausePlay"))
		PauseOrStartVideo();
	else if (msg.pSender->GetName() == _T("btnStopPlay"))
		StopVideo();
	else if (msg.pSender->GetName() == _T("btnOpenFile"))
		OnBtnOpenFileClicked();
	else if (msg.pSender->GetName() == _T("btnSelectedHF"))
		OnBtnOpenRTSPClicked();
	
	return __super::OnClick(msg);
}

void PlayerMainForm::OnBtnOpenFileClicked()
{
	std::wstring file_type = _T("图像文件");
	LPCTSTR filter = L"*.avi;*.mp4;*.mkv;*.remb";
	std::wstring text = nbase::StringPrintf(L"%s(%s)", file_type.c_str(), filter);
	std::map<LPCTSTR, LPCTSTR> filters;
	filters[text.c_str()] = filter;

	FileDialogEx::FileDialogCallback2 cb = std::bind(&PlayerMainForm::BtnOpenFileClickedCallback, this, std::placeholders::_1, std::placeholders::_2);

	FileDialogEx* file_dlg = new FileDialogEx();
	file_dlg->SetFilter(filters);
	file_dlg->SetParentWnd(GetHWND());
	file_dlg->AyncShowOpenFileDlg(cb);
}

void PlayerMainForm::OnBtnOpenRTSPClicked()
{
	start_rtsp_client();
}
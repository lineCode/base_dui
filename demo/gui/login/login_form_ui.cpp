#include "stdafx.h"
#include "resource.h"
#include "login_form.h"


using namespace dui;

const LPCTSTR LoginForm::kClassName	= L"LoginForm";

LoginForm::LoginForm()
{
}

LoginForm::~LoginForm()
{
}

String LoginForm::GetSkinFolder()
{
	return L"login";
}

String LoginForm::GetSkinFile()
{
	return L"login.xml";
}

//ui::UILIB_RESOURCETYPE LoginForm::GetResourceType() const
//{
//	return ui::UILIB_FILE;
//}
//
//std::wstring LoginForm::GetZIPFileName() const
//{
//	return L"login.zip";
//}

LPCTSTR LoginForm::GetWindowClassName() const 
{
	return kClassName;
}

LPCTSTR LoginForm::GetWindowId() const
{
	return kClassName;
}

UINT LoginForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void LoginForm::InitWindow()
{
	//m_PaintManager.AttachEvent(ui::kEventClick, nbase::Bind(&LoginForm::OnClicked, this, std::placeholders::_1));
	SetIcon(IDI_ICON1);
	SetTaskbarTitle(L"µÇÂ¼");

	re_account_ = (RichEdit*)m_PaintManager.FindControl(L"re_account");
	re_pwd_ = (RichEdit*)m_PaintManager.FindControl(L"re_pwd");
	label_info_ = (Label*)m_PaintManager.FindControl(L"label_info");
	/*re_account_->SetSelAllOnFocus(true);
	re_pwd_->SetSelAllOnFocus(true);*/
	btn_login_ = (Button*)m_PaintManager.FindControl(L"btn_login");
	btn_cancel_login_ = (Button*)m_PaintManager.FindControl(L"btn_cancel_login");
	chk_arrow_down_ = (CheckBtn*)m_PaintManager.FindControl(L"chk_arrow_down");

	this->RegLoginManagerCallback();

	re_account_->SetText(L"13777073806");
	re_pwd_->SetText(L"dingjunjie");
}

void LoginForm::Notify(dui::TNotify& msg)
{
	bool bHandle = false;
	Control *pControl = msg.pSender;
	if (pControl && msg.sType == DUI_MSGTYPE_WINDOWINIT)
	{
		bHandle = true;
		wprintf(_T("LoginForm::Notify Name:%s MSG:%s\n"), pControl->GetName().c_str(), DUI_MSGTYPE_WINDOWINIT);
	}
	else if (pControl && msg.sType == DUI_MSGTYPE_ITEMCLICK)
	{
		bHandle = true;
		wprintf(_T("LoginForm::Notify Name:%s MSG:%s\n"), pControl->GetName().c_str(), DUI_MSGTYPE_ITEMCLICK);
		ImportDataToDB();
	}
	if (!bHandle)
	{
		__super::Notify(msg);
	}
}

void LoginForm::OnClick(TNotify& msg)
{
	bool bHandle = false;
	//std::wstring name = msg.pSender->GetName();
	if (msg.pSender == btn_login_)
	{
		bHandle = true;
		StartLogin();
	}
	else if (msg.pSender == btn_cancel_login_)
	{
		bHandle = true;
		CancelLogin();
	}
	else if (msg.pSender == chk_arrow_down_)
	{
		bHandle = true;
		POINT pt = msg.ptMouse;
		::ClientToScreen(GetHWND(), &pt);
		ShowMenu(chk_arrow_down_->GetCheck(), pt);
		return;
	}
	
	if (!bHandle)
	{
		return __super::OnClick(msg);
	}
}

LRESULT LoginForm::OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled)
{
	::PostQuitMessage(0);
	return 0;
}

void LoginForm::ShowMenu(bool show, POINT pt)
{
	CMenuWnd *menu_wnd = new CMenuWnd;
	menu_wnd->Init(NULL, _T("menu.xml"), _T("menu"), pt, &m_PaintManager);
	
}

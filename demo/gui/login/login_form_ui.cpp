#include "stdafx.h"
#include "login_form.h"


using namespace DuiLib;

const LPCTSTR LoginForm::kClassName	= L"LoginForm";

LoginForm::LoginForm()
{
}

LoginForm::~LoginForm()
{

}

CDuiString LoginForm::GetSkinFolder()
{
	return L"";
}

CDuiString LoginForm::GetSkinFile()
{
	return L"login\\login.xml";
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
	
	re_account_ = (CRichEditUI*)m_PaintManager.FindControl(L"re_account");
	re_pwd_ = (CRichEditUI*)m_PaintManager.FindControl(L"re_pwd");
	label_info_ = (CLabelUI*)m_PaintManager.FindControl(L"label_info");
	/*re_account_->SetSelAllOnFocus(true);
	re_pwd_->SetSelAllOnFocus(true);*/
	btn_login_ = (CButtonUI*)m_PaintManager.FindControl(L"btn_login");
	btn_cancel_login_ = (CButtonUI*)m_PaintManager.FindControl(L"btn_cancel_login");
	chk_arrow_down_ = (CCheckBoxUI*)m_PaintManager.FindControl(L"chk_arrow_down");

	this->RegLoginManagerCallback();

	re_account_->SetText(L"13777073806");
	re_pwd_->SetText(L"dingjunjie");
}

void LoginForm::OnClick(TNotifyUI& msg)
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
		ShowMenu(chk_arrow_down_->GetCheck());
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

void LoginForm::ShowMenu(bool show)
{
	CMenuWnd *menu_wnd = new CMenuWnd;
	menu_wnd->Init(NULL, _T("login/menu.xml"), { 200, 10 }, &m_PaintManager);
	
}

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

//std::wstring LoginForm::GetWindowId() const
//{
//	return kClassName;
//}

UINT LoginForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void LoginForm::InitWindow()
{
	//m_PaintManager.AttachEvent(ui::kEventClick, nbase::Bind(&LoginForm::OnClicked, this, std::placeholders::_1));
	
	re_account_ = (CRichEditUI*)m_PaintManager.FindControl(L"re_account");
	re_pwd_ = (CRichEditUI*)m_PaintManager.FindControl(L"re_pwd");
	/*re_account_->SetSelAllOnFocus(true);
	re_pwd_->SetSelAllOnFocus(true);*/
	btn_login_ = (CButtonUI*)m_PaintManager.FindControl(L"btn_login");

	this->RegLoginManagerCallback();
}

void LoginForm::OnClick(DuiLib::TNotifyUI& msg)
{
	//std::wstring name = msg.pSender->GetName();
	if (msg.pSender == btn_login_)
	{
		StartLogin("djj", "123");
	}
	else if (msg.pSender->GetName() == _T("closebtn"))
	{
		Close();
		return;
	}
	else if (msg.pSender->GetName() == _T("minbtn"))
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		return;
	}
	else if (msg.pSender->GetName() == _T("maxbtn"))
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		return;
	}
	else if (msg.pSender->GetName() == _T("restorebtn"))
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		return;
	}
}

LRESULT LoginForm::OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled)
{
	::PostQuitMessage(0);
	return 0;
}

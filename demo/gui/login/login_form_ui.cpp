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
	
}

LRESULT LoginForm::OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled)
{
	::PostQuitMessage(0);
	return 0;
}

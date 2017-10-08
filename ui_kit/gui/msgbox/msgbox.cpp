#include "stdafx.h"
#include "shared/closure.h"
#include "msgbox.h"


using namespace DuiLib;

void ShowMsgBox(HWND hwnd, MsgboxCallback cb,
	const std::wstring &content, bool content_is_id,
	const std::wstring &title, bool title_is_id,
	const std::wstring &yes, bool btn_yes_is_id,
	const std::wstring &no, bool btn_no_is_id)
{
	MsgBox* msgbox = new MsgBox;
	HWND hWnd = msgbox->Create(hwnd, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
	if(hWnd == NULL)
		return;
#if 1
	msgbox->SetTitle(title);
	msgbox->SetContent(content);
	msgbox->SetButton(yes, no);
	msgbox->Show(hwnd, cb);
#else
	MutiLanSupport *multilan = MutiLanSupport::GetInstance();
	msgbox->SetTitle(title_is_id ? multilan->GetStringViaID(title) : title);
	msgbox->SetContent(content_is_id ? multilan->GetStringViaID(content) : content);
	msgbox->SetButton(btn_yes_is_id ? multilan->GetStringViaID(yes) : yes, btn_no_is_id ? multilan->GetStringViaID(no) : no);
	msgbox->Show(hwnd, cb);
#endif
}

const LPCTSTR MsgBox::kClassName = L"MsgBox";

MsgBox::MsgBox()
{
}

MsgBox::~MsgBox()
{
}

std::wstring MsgBox::GetSkinFolder()
{
	return L"msgbox";
}

std::wstring MsgBox::GetSkinFile()
{
	return L"msgbox.xml";
}

DuiLib::UILIB_RESOURCETYPE MsgBox::GetResourceType() const
{
	return DuiLib::UILIB_FILE;
}

std::wstring MsgBox::GetZIPFileName() const
{
	return (L"msgbox.zip");
}

LPCTSTR MsgBox::GetWindowClassName() const
{
	return kClassName;
}

std::wstring MsgBox::GetWindowId() const
{
	return kClassName;
}

UINT MsgBox::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

LRESULT MsgBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//if(uMsg == WM_DESTROY)
	//{
	//	HWND hWndParent = GetWindowOwner(m_hWnd);
	//	if(hWndParent)
	//	{
	//		::EnableWindow(hWndParent, TRUE);
	//		::SetForegroundWindow(hWndParent);
	//		::SetFocus(hWndParent);
	//	}
	//}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void MsgBox::OnEsc( BOOL &bHandled )
{
	bHandled = TRUE;
	EndMsgBox(MB_NO);
}
void MsgBox::Close(UINT nRet)
{
	// 提示框关闭之前先Enable父窗口，防止父窗口隐到后面去。
	HWND hWndParent = GetWindowOwner(m_hWnd);
	if (hWndParent)
	{
		::EnableWindow(hWndParent, TRUE);
		::SetFocus(hWndParent);
	}

	__super::Close(nRet);
}

void MsgBox::InitWindow()
{
#if 0
	m_pRoot->AttachBubbledEvent(DuiLib::kEventClick, nbase::Bind(&MsgBox::OnClicked, this, std::placeholders::_1));
#endif
	title_ = (CLabelUI*)m_PaintManager.FindControl(L"title");
	content_ = (CRichEditUI*)m_PaintManager.FindControl(L"content");
	btn_yes_ = (CButtonUI*)m_PaintManager.FindControl(L"btn_yes");
	btn_no_ = (CButtonUI*)m_PaintManager.FindControl(L"btn_no");
}

bool MsgBox::OnClicked(DuiLib::TEventUI* msg)
{
	std::wstring name = msg->pSender->GetName();
	if(name == L"btn_yes")
	{
		EndMsgBox(MB_YES);
	}
	else if(name == L"btn_no")
	{
		EndMsgBox(MB_NO);
	}
	else if(name == L"closebtn")
	{
		EndMsgBox(MB_NO);
	}
	return true;
}

void MsgBox::SetTitle( const std::wstring &str )
{
	title_->SetText(str.c_str());
}

void MsgBox::SetContent( const std::wstring &str )
{
#if 0
	content_->SetText(str.c_str());

	int width = content_->GetFixedWidth();

	CDuiSize sz = content_->GetNaturalSize(width, 0);
	content_->SetFixedHeight(sz.cy, false);
#endif
}

void MsgBox::SetButton( const std::wstring &yes, const std::wstring &no )
{
	if( !yes.empty() )
	{
		btn_yes_->SetText(yes.c_str());
		btn_yes_->SetVisible(true);
		btn_yes_->SetFocus();
	}
	else
	{
		btn_yes_->SetVisible(false);
	}

	if( !no.empty() )
	{
		btn_no_->SetText(no.c_str());
		btn_no_->SetVisible(true);
		if( yes.empty() )
			btn_no_->SetFocus();
	}
	else
	{
		btn_no_->SetVisible(false);
	}
}

void MsgBox::Show( HWND hwnd, MsgboxCallback cb )
{
	msgbox_callback_ = cb;

	::EnableWindow(hwnd, FALSE);
	CenterWindow();
	ShowWindow(true);
}

void MsgBox::EndMsgBox( MsgBoxRet ret )
{
	this->Close(0);

	if( msgbox_callback_ )
	{
		Post2UI( nbase::Bind(msgbox_callback_, ret) );
	}
}

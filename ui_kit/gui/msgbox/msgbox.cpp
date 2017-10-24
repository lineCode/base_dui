#include "stdafx.h"
#include "shared/closure.h"
#include "msgbox.h"


using namespace dui;

void ShowMsgBox(HWND hParentWnd, MsgboxCallback cb,
	const std::wstring &content, bool content_is_id,
	const std::wstring &title, bool title_is_id,
	const std::wstring &yes, bool btn_yes_is_id,
	const std::wstring &no, bool btn_no_is_id)
{
	MsgBox* msgbox = new MsgBox(cb);
	HWND hWnd = msgbox->Create(hParentWnd, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
	if(hWnd == NULL)
		return;
#if 1
	msgbox->SetTitle(title);
	msgbox->SetContent(content);
	msgbox->SetButton(yes, no);
	msgbox->Show();
#else
	MutiLanSupport *multilan = MutiLanSupport::GetInstance();
	msgbox->SetTitle(title_is_id ? multilan->GetStringViaID(title) : title);
	msgbox->SetContent(content_is_id ? multilan->GetStringViaID(content) : content);
	msgbox->SetButton(btn_yes_is_id ? multilan->GetStringViaID(yes) : yes, btn_no_is_id ? multilan->GetStringViaID(no) : no);
	msgbox->Show(hwnd, cb);
#endif
}

const LPCTSTR MsgBox::kClassName = L"MsgBox";

MsgBox::MsgBox(MsgboxCallback cb/* = nullptr*/) :msgbox_callback_(cb)
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

dui::UILIB_RESOURCETYPE MsgBox::GetResourceType() const
{
	return dui::UILIB_FILE;
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

void MsgBox::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
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

LRESULT MsgBox::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	EndMsgBox(MB_NO);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

void MsgBox::InitWindow()
{
#if 0
	m_pRoot->AttachBubbledEvent(dui::kEventClick, nbase::Bind(&MsgBox::OnClicked, this, std::placeholders::_1));
#endif
	title_ = (Label*)m_PaintManager.FindControl(L"title");
	content_ = (RichEdit*)m_PaintManager.FindControl(L"content");
	btn_yes_ = (Button*)m_PaintManager.FindControl(L"btn_yes");
	btn_no_ = (Button*)m_PaintManager.FindControl(L"btn_no");
}

void MsgBox::OnClick(dui::TNotify& msg)
{
	bool bHandle = false;
	std::wstring name = msg.pSender->GetName();
	if(name == L"btn_yes")
	{
		bHandle = true;
		EndMsgBox(MB_YES);
	}
	else if(name == L"btn_no")
	{
		bHandle = true;
		EndMsgBox(MB_NO);
	}
	
	if (!bHandle)
	{
		__super::OnClick(msg);
	}
}

void MsgBox::SetTitle( const std::wstring &str )
{
	title_->SetText(str.c_str());
}

void MsgBox::SetContent( const std::wstring &str )
{
	content_->SetText(str.c_str());
#if 0
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

void MsgBox::Show()
{
	::EnableWindow(GetParent(m_hWnd), FALSE);
	CenterWindow();
	ShowWindow(true);
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

void MsgBox::EndMsgBox( MsgBoxRet ret )
{
	this->Close(0);

	if( msgbox_callback_ )
	{
		shared::Post2UI( nbase::Bind(msgbox_callback_, ret) );
	}
}

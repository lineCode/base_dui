#include "stdafx.h"
#include "window_ex.h"
#include "module/window/windows_manager.h"

namespace nim_comp
{
WindowEx::WindowEx()
{
}

WindowEx::~WindowEx()
{
}

HWND WindowEx::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, bool isLayeredWindow, const dui::DuiRect& rc)
{
	if (!RegisterWnd())
	{
		return NULL;
	}

	if (isLayeredWindow)
	{
		m_PaintManager.GetShadow()->ShowShadow(true);
		m_PaintManager.GetShadow()->SetImage(_T("../public/bk/bk_shadow.png"));
		m_PaintManager.GetShadow()->SetSize(14);
		m_PaintManager.GetShadow()->SetShadowCorner({14,14,14,14});
	}
	HWND hwnd = __super::Create(hwndParent, pstrName, dwStyle, dwExStyle, rc);
	ASSERT(hwnd);
	return hwnd;
}

void WindowEx::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

LRESULT WindowEx::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UnRegisterWnd();
	return __super::OnDestroy(uMsg, wParam, lParam, bHandled);
}

void WindowEx::OnEsc( BOOL &bHandled )
{
	bHandled = FALSE;
}

bool WindowEx::RegisterWnd()
{
	std::wstring wnd_class_name = GetWindowClassName();
	std::wstring wnd_id = GetWindowId();
	if (!WindowsManager::GetInstance()->RegisterWindow(wnd_class_name, wnd_id, this))
	{
		return false;
	}
	return true;
}

void WindowEx::UnRegisterWnd()
{
	std::wstring wnd_class_name = GetWindowClassName();
	std::wstring wnd_id = GetWindowId();
	WindowsManager::GetInstance()->UnRegisterWindow(wnd_class_name, wnd_id, this);
}

LRESULT WindowEx::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_CLOSE)
	{
		if(!::IsWindowEnabled(m_hWnd))
		{
			::SetForegroundWindow(m_hWnd);
			return FALSE;
		}
	}
	else if(uMsg == WM_KILLFOCUS)
	{
#if 1
		//OnKillFocus(uMsg, wParam, lParam);
#endif
	}
	else if(uMsg == WM_KEYDOWN)
	{
		if(wParam == VK_ESCAPE)
		{
			BOOL bHandled = FALSE;
			OnEsc(bHandled);
			if( !bHandled )
				this->Close();
		}
	}
	return __super::HandleMessage(uMsg,wParam,lParam);
}

void WindowEx::ActiveWindow()
{
	if(::IsWindow(m_hWnd))
	{
		if(::IsIconic(m_hWnd))
			::ShowWindow(m_hWnd, SW_RESTORE);
		else 
			::SetForegroundWindow(m_hWnd);
	}
}

void WindowEx::SetTaskbarTitle( const std::wstring &title )
{
	::SetWindowTextW(m_hWnd, title.c_str());
}

POINT  WindowEx::GetPopupWindowPos()
{
	ASSERT(IsWindow(m_hWnd));

	//ÆÁÄ»´óÐ¡
	MONITORINFO oMonitor = { sizeof(oMonitor) };
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	RECT screen = oMonitor.rcWork;

	dui::DuiRect rect = GetPos(true);

	POINT pt = { 0, 0 };
	pt.x = screen.right - rect.GetWidth();
	pt.y = screen.bottom - rect.GetHeight();

	return pt;
}

void  WindowEx::ToTopMost(bool forever)
{
	ASSERT(::IsWindow(m_hWnd));
	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	if( !forever )
	{
		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}
}
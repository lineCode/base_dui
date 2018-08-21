#include "stdafx.h"

namespace dui {

ScrollBar::ScrollBar() :
	m_bHorizontal(false),
	m_nRange(100),
	m_nScrollPos(0),
	m_nLineSize(SCROLLBAR_LINESIZE), 
    m_nScrollUnit(1),
	m_pOwner(NULL),
	m_nLastScrollPos(0),
	m_nLastScrollOffset(0),
	m_nScrollRepeatDelay(0), 
	m_dwButton1Color(0),
	m_uButton1State(0),
	m_dwButton2Color(0),
	m_uButton2State(0), 
	m_dwThumbColor(0),
	m_uThumbState(0),
	m_bShowButton1(true),
	m_bShowButton2(true)
{
	m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
	ptLastMouse.x = ptLastMouse.y = 0;
	::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
	::ZeroMemory(&m_rcButton1, sizeof(m_rcButton1));
	::ZeroMemory(&m_rcButton2, sizeof(m_rcButton2));
}

LPCTSTR ScrollBar::GetClass() const
{
	return DUI_CTR_SCROLLBAR;
}

LPVOID ScrollBar::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_SCROLLBAR) == 0 ) return static_cast<ScrollBar*>(this);
	return Control::GetInterface(pstrName);
}

ScrollBox* ScrollBar::GetOwner() const
{
	return m_pOwner;
}

void ScrollBar::SetOwner(ScrollBox* pOwner)
{
	m_pOwner = pOwner;
}

void ScrollBar::SetVisible(bool bVisible)
{
	if( m_bVisible == bVisible ) return;

	bool v = IsVisible();
	m_bVisible = bVisible;
	if( m_bFocused ) m_bFocused = false;
}

void ScrollBar::SetEnabled(bool bEnable)
{
	Control::SetEnabled(bEnable);
	if( !IsEnabled() ) {
		m_uButton1State = 0;
		m_uButton2State = 0;
		m_uThumbState = 0;
	}
}

void ScrollBar::SetFocus()
{
	if( m_pOwner != NULL ) m_pOwner->SetFocus();
	else Control::SetFocus();
}

bool ScrollBar::IsHorizontal()
{
	return m_bHorizontal;
}

void ScrollBar::SetHorizontal(bool bHorizontal)
{
	if( m_bHorizontal == bHorizontal ) return;

	m_bHorizontal = bHorizontal;
	if( m_bHorizontal ) {
		if( m_cxyFixed.cy == 0 ) {
			m_cxyFixed.cx = 0;
			m_cxyFixed.cy = DEFAULT_SCROLLBAR_SIZE;
		}
	}
	else {
		if( m_cxyFixed.cx == 0 ) {
			m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
			m_cxyFixed.cy = 0;
		}
	}

	if( m_pOwner != NULL ) m_pOwner->NeedUpdate(); else NeedParentUpdate();
}

int ScrollBar::GetScrollRange() const
{
	return m_nRange;
}

void ScrollBar::SetScrollRange(int nRange)
{
	if( m_nRange == nRange ) return;

	m_nRange = nRange;
	if( m_nRange < 0 ) m_nRange = 0;
	if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
	SetPos(m_rcItem, true);
}

int ScrollBar::GetScrollPos() const
{
	return m_nScrollPos;
}

void ScrollBar::SetScrollPos(int nPos, bool bTriggerEvent)
{
	if( m_nScrollPos == nPos ) return;

	int iOldScrollPos = m_nScrollPos;
	m_nScrollPos = nPos;
	if( m_nScrollPos < 0 ) m_nScrollPos = 0;
    if( m_nScrollUnit > 1 ) {
        int iLeftOffset = m_nScrollPos % m_nScrollUnit;
        if( iLeftOffset != 0 ) {
            if( iLeftOffset >= m_nScrollUnit/2 ) m_nScrollPos += m_nScrollUnit - iLeftOffset;
            else m_nScrollPos -= iLeftOffset;   
        }
    }
	if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;

	SetPos(m_rcItem, true);

	if(bTriggerEvent && m_pManager != NULL) 
		m_pManager->SendNotify(this, UIEVENT_SCROLL, m_nScrollPos, iOldScrollPos, true, false);
}

int ScrollBar::GetLineSize() const
{
    if (m_nScrollUnit > 1) return m_nScrollUnit;
	return m_nLineSize;
}

void ScrollBar::SetLineSize(int nSize)
{
    if (nSize >= 0) m_nLineSize = nSize;
}

int ScrollBar::GetScrollUnit() const
{
    return m_nScrollUnit;
}

void ScrollBar::SetScrollUnit(int iUnit)
{
    if (iUnit >= 0) m_nScrollUnit = iUnit;
}

bool ScrollBar::GetShowButton1()
{
	return m_bShowButton1;
}

void ScrollBar::SetShowButton1(bool bShow)
{
	m_bShowButton1 = bShow;
	SetPos(m_rcItem, true);
}

DWORD ScrollBar::GetButton1Color() const
{
	return m_dwButton1Color;
}

void ScrollBar::SetButton1Color(DWORD dwColor)
{
	if( m_dwButton1Color == dwColor ) return;
	m_dwButton1Color = dwColor;
	Invalidate();
}

LPCTSTR ScrollBar::GetButton1NormalImage()
{
	return m_diButton1Normal.sDrawString.c_str();
}

void ScrollBar::SetButton1NormalImage(LPCTSTR pStrImage)
{
	if( m_diButton1Normal.sDrawString == pStrImage && m_diButton1Normal.pImageInfo != NULL ) return;
	m_diButton1Normal.Clear();
	m_diButton1Normal.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetButton1HotImage()
{
	return m_diButton1Hot.sDrawString.c_str();
}

void ScrollBar::SetButton1HotImage(LPCTSTR pStrImage)
{
	if( m_diButton1Hot.sDrawString == pStrImage && m_diButton1Hot.pImageInfo != NULL ) return;
	m_diButton1Hot.Clear();
	m_diButton1Hot.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetButton1PushedImage()
{
	return m_diButton1Pushed.sDrawString.c_str();
}

void ScrollBar::SetButton1PushedImage(LPCTSTR pStrImage)
{
	if( m_diButton1Pushed.sDrawString == pStrImage && m_diButton1Pushed.pImageInfo != NULL ) return;
	m_diButton1Pushed.Clear();
	m_diButton1Pushed.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetButton1DisabledImage()
{
	return m_diButton1Disabled.sDrawString.c_str();
}

void ScrollBar::SetButton1DisabledImage(LPCTSTR pStrImage)
{
	if( m_diButton1Disabled.sDrawString == pStrImage && m_diButton1Disabled.pImageInfo != NULL ) return;
	m_diButton1Disabled.Clear();
	m_diButton1Disabled.sDrawString = pStrImage;
	Invalidate();
}

bool ScrollBar::GetShowButton2()
{
	return m_bShowButton2;
}

void ScrollBar::SetShowButton2(bool bShow)
{
	m_bShowButton2 = bShow;
	SetPos(m_rcItem, true);
}

DWORD ScrollBar::GetButton2Color() const
{
	return m_dwButton2Color;
}

void ScrollBar::SetButton2Color(DWORD dwColor)
{
	if( m_dwButton2Color == dwColor ) return;
	m_dwButton2Color = dwColor;
	Invalidate();
}

LPCTSTR ScrollBar::GetButton2NormalImage()
{
	return m_diButton2Normal.sDrawString.c_str();
}

void ScrollBar::SetButton2NormalImage(LPCTSTR pStrImage)
{
	if( m_diButton2Normal.sDrawString == pStrImage && m_diButton2Normal.pImageInfo != NULL ) return;
	m_diButton2Normal.Clear();
	m_diButton2Normal.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetButton2HotImage()
{
	return m_diButton2Hot.sDrawString.c_str();
}

void ScrollBar::SetButton2HotImage(LPCTSTR pStrImage)
{
	if( m_diButton2Hot.sDrawString == pStrImage && m_diButton2Hot.pImageInfo != NULL ) return;
	m_diButton2Hot.Clear();
	m_diButton2Hot.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetButton2PushedImage()
{
	return m_diButton2Pushed.sDrawString.c_str();
}

void ScrollBar::SetButton2PushedImage(LPCTSTR pStrImage)
{
	if( m_diButton2Pushed.sDrawString == pStrImage && m_diButton2Pushed.pImageInfo != NULL ) return;
	m_diButton2Pushed.Clear();
	m_diButton2Pushed.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetButton2DisabledImage()
{
	return m_diButton2Disabled.sDrawString.c_str();
}

void ScrollBar::SetButton2DisabledImage(LPCTSTR pStrImage)
{
	if( m_diButton2Disabled.sDrawString == pStrImage && m_diButton2Disabled.pImageInfo != NULL ) return;
	m_diButton2Disabled.Clear();
	m_diButton2Disabled.sDrawString = pStrImage;
	Invalidate();
}

DWORD ScrollBar::GetThumbColor() const
{
	return m_dwThumbColor;
}

void ScrollBar::SetThumbColor(DWORD dwColor)
{
	if( m_dwThumbColor == dwColor ) return;
	m_dwThumbColor = dwColor;
	Invalidate();
}

LPCTSTR ScrollBar::GetThumbNormalImage()
{
	return m_diThumbNormal.sDrawString.c_str();
}

void ScrollBar::SetThumbNormalImage(LPCTSTR pStrImage)
{
	if( m_diThumbNormal.sDrawString == pStrImage && m_diThumbNormal.pImageInfo != NULL ) return;
	m_diThumbNormal.Clear();
	m_diThumbNormal.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetThumbHotImage()
{
	return m_diThumbHot.sDrawString.c_str();
}

void ScrollBar::SetThumbHotImage(LPCTSTR pStrImage)
{
	if( m_diThumbHot.sDrawString == pStrImage && m_diThumbHot.pImageInfo != NULL ) return;
	m_diThumbHot.Clear();
	m_diThumbHot.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetThumbPushedImage()
{
	return m_diThumbPushed.sDrawString.c_str();
}

void ScrollBar::SetThumbPushedImage(LPCTSTR pStrImage)
{
	if( m_diThumbPushed.sDrawString == pStrImage && m_diThumbPushed.pImageInfo != NULL ) return;
	m_diThumbPushed.Clear();
	m_diThumbPushed.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetThumbDisabledImage()
{
	return m_diThumbDisabled.sDrawString.c_str();
}

void ScrollBar::SetThumbDisabledImage(LPCTSTR pStrImage)
{
	if( m_diThumbDisabled.sDrawString == pStrImage && m_diThumbDisabled.pImageInfo != NULL ) return;
	m_diThumbDisabled.Clear();
	m_diThumbDisabled.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetRailNormalImage()
{
	return m_diRailNormal.sDrawString.c_str();
}

void ScrollBar::SetRailNormalImage(LPCTSTR pStrImage)
{
	if( m_diRailNormal.sDrawString == pStrImage && m_diRailNormal.pImageInfo != NULL ) return;
	m_diRailNormal.Clear();
	m_diRailNormal.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetRailHotImage()
{
	return m_diRailHot.sDrawString.c_str();
}

void ScrollBar::SetRailHotImage(LPCTSTR pStrImage)
{
	if( m_diRailHot.sDrawString == pStrImage && m_diRailHot.pImageInfo != NULL ) return;
	m_diRailHot.Clear();
	m_diRailHot.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetRailPushedImage()
{
	return m_diRailPushed.sDrawString.c_str();
}

void ScrollBar::SetRailPushedImage(LPCTSTR pStrImage)
{
	if( m_diRailPushed.sDrawString == pStrImage && m_diRailPushed.pImageInfo != NULL ) return;
	m_diRailPushed.Clear();
	m_diRailPushed.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetRailDisabledImage()
{
	return m_diRailDisabled.sDrawString.c_str();
}

void ScrollBar::SetRailDisabledImage(LPCTSTR pStrImage)
{
	if( m_diRailDisabled.sDrawString == pStrImage && m_diRailDisabled.pImageInfo != NULL ) return;
	m_diRailDisabled.Clear();
	m_diRailDisabled.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetBkNormalImage()
{
	return m_diBkNormal.sDrawString.c_str();
}

void ScrollBar::SetBkNormalImage(LPCTSTR pStrImage)
{
	if( m_diBkNormal.sDrawString == pStrImage && m_diBkNormal.pImageInfo != NULL ) return;
	m_diBkNormal.Clear();
	m_diBkNormal.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetBkHotImage()
{
	return m_diBkHot.sDrawString.c_str();
}

void ScrollBar::SetBkHotImage(LPCTSTR pStrImage)
{
	if( m_diBkHot.sDrawString == pStrImage && m_diBkHot.pImageInfo != NULL ) return;
	m_diBkHot.Clear();
	m_diBkHot.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetBkPushedImage()
{
	return m_diBkPushed.sDrawString.c_str();
}

void ScrollBar::SetBkPushedImage(LPCTSTR pStrImage)
{
	if( m_diBkPushed.sDrawString == pStrImage && m_diBkPushed.pImageInfo != NULL ) return;
	m_diBkPushed.Clear();
	m_diBkPushed.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ScrollBar::GetBkDisabledImage()
{
	return m_diBkDisabled.sDrawString.c_str();
}

void ScrollBar::SetBkDisabledImage(LPCTSTR pStrImage)
{
	if( m_diBkDisabled.sDrawString == pStrImage && m_diBkDisabled.pImageInfo != NULL ) return;
	m_diBkDisabled.Clear();
	m_diBkDisabled.sDrawString = pStrImage;
	Invalidate();
}

void ScrollBar::SetPos(RECT rc, bool bNeedInvalidate)
{
	Control::SetPos(rc, bNeedInvalidate);
	
	SIZE cxyFixed = m_cxyFixed;
	if (m_pManager != NULL) 
		GetManager()->GetDPIObj()->Scale(&cxyFixed);

	rc = m_rcItem;

	if( m_bHorizontal ) {
		int cx = rc.right - rc.left;
		if (m_bShowButton1) cx -= cxyFixed.cy;
		if (m_bShowButton2) cx -= cxyFixed.cy;
		if (cx > cxyFixed.cy) {
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + cxyFixed.cy;
				m_rcButton1.bottom = rc.top + cxyFixed.cy;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.top = rc.top;
			m_rcButton2.right = rc.right;
			if( m_bShowButton2 ) {
				m_rcButton2.left = rc.right - cxyFixed.cy;
				m_rcButton2.bottom = rc.top + cxyFixed.cy;
			}
			else {
				m_rcButton2.left = m_rcButton2.right;
				m_rcButton2.bottom = m_rcButton2.top;
			}

			m_rcThumb.top = rc.top;
			m_rcThumb.bottom = rc.top + cxyFixed.cy;
			if( m_nRange > 0 ) {
				int cxThumb = cx * (rc.right - rc.left) / (m_nRange + rc.right - rc.left);
				if( cxThumb < cxyFixed.cy ) cxThumb = cxyFixed.cy;

				m_rcThumb.left = m_nScrollPos * (cx - cxThumb) / m_nRange + m_rcButton1.right;
				m_rcThumb.right = m_rcThumb.left + cxThumb;
				if( m_rcThumb.right > m_rcButton2.left ) {
					m_rcThumb.left = m_rcButton2.left - cxThumb;
					m_rcThumb.right = m_rcButton2.left;
				}
			}
			else {
				m_rcThumb.left = m_rcButton1.right;
				m_rcThumb.right = m_rcButton2.left;
			}
		}
		else {
			int cxButton = (rc.right - rc.left) / 2;
			if( cxButton > cxyFixed.cy ) cxButton = cxyFixed.cy;
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + cxButton;
				m_rcButton1.bottom = rc.top + cxyFixed.cy;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.top = rc.top;
			m_rcButton2.right = rc.right;
			if( m_bShowButton2 ) {
				m_rcButton2.left = rc.right - cxButton;
				m_rcButton2.bottom = rc.top + cxyFixed.cy;
			}
			else {
				m_rcButton2.left = m_rcButton2.right;
				m_rcButton2.bottom = m_rcButton2.top;
			}

			::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		}
	}
	else {
		int cy = rc.bottom - rc.top;
		if( m_bShowButton1 ) cy -= m_cxyFixed.cx;
		if( m_bShowButton2 ) cy -= m_cxyFixed.cx;
		if( cy > m_cxyFixed.cx ) {
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + m_cxyFixed.cx;
				m_rcButton1.bottom = rc.top + m_cxyFixed.cx;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.left = rc.left;
			m_rcButton2.bottom = rc.bottom;
			if( m_bShowButton2 ) {
				m_rcButton2.top = rc.bottom - m_cxyFixed.cx;
				m_rcButton2.right = rc.left + m_cxyFixed.cx;
			}
			else {
				m_rcButton2.top = m_rcButton2.bottom;
				m_rcButton2.right = m_rcButton2.left;
			}

			m_rcThumb.left = rc.left;
			m_rcThumb.right = rc.left + m_cxyFixed.cx;
			if( m_nRange > 0 ) {
				int cyThumb = cy * (rc.bottom - rc.top) / (m_nRange + rc.bottom - rc.top);
				if( cyThumb < m_cxyFixed.cx ) cyThumb = m_cxyFixed.cx;

				m_rcThumb.top = m_nScrollPos * (cy - cyThumb) / m_nRange + m_rcButton1.bottom;
				m_rcThumb.bottom = m_rcThumb.top + cyThumb;
				if( m_rcThumb.bottom > m_rcButton2.top ) {
					m_rcThumb.top = m_rcButton2.top - cyThumb;
					m_rcThumb.bottom = m_rcButton2.top;
				}
			}
			else {
				m_rcThumb.top = m_rcButton1.bottom;
				m_rcThumb.bottom = m_rcButton2.top;
			}
		}
		else {
			int cyButton = (rc.bottom - rc.top) / 2;
			if( cyButton > m_cxyFixed.cx ) cyButton = m_cxyFixed.cx;
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + m_cxyFixed.cx;
				m_rcButton1.bottom = rc.top + cyButton;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.left = rc.left;
			m_rcButton2.bottom = rc.bottom;
			if( m_bShowButton2 ) {
				m_rcButton2.top = rc.bottom - cyButton;
				m_rcButton2.right = rc.left + m_cxyFixed.cx;
			}
			else {
				m_rcButton2.top = m_rcButton2.bottom;
				m_rcButton2.right = m_rcButton2.left;
			}

			::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		}
	}
}

void ScrollBar::DoEvent(Event& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
		else Control::DoEvent(event);
		return;
	}

	if( event.Type == UIEVENT_SETFOCUS ) 
	{
		return;
	}
	if( event.Type == UIEVENT_KILLFOCUS ) 
	{
		return;
	}
	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
	{
		if( !IsEnabled() ) return;

		m_nLastScrollOffset = 0;
		m_nScrollRepeatDelay = 0;
		m_pManager->SetTimer(this, DEFAULT_TIMERID, 50U);

		if( ::PtInRect(&m_rcButton1, event.ptMouse) ) {
			m_uButton1State |= UISTATE_PUSHED;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->LineUp(); 
				else SetScrollPos(m_nScrollPos - GetLineSize());
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->LineLeft(); 
				else SetScrollPos(m_nScrollPos - GetLineSize());
			}
		}
		else if( ::PtInRect(&m_rcButton2, event.ptMouse) ) {
			m_uButton2State |= UISTATE_PUSHED;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->LineDown(); 
				else SetScrollPos(m_nScrollPos + GetLineSize());
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->LineRight(); 
				else SetScrollPos(m_nScrollPos + GetLineSize());
			}
		}
		else if( ::PtInRect(&m_rcThumb, event.ptMouse) ) {
			m_uThumbState |= UISTATE_CAPTURED | UISTATE_PUSHED;
			ptLastMouse = event.ptMouse;
			m_nLastScrollPos = m_nScrollPos;
		}
		else {
			if( !m_bHorizontal ) {
				if( event.ptMouse.y < m_rcThumb.top ) {
					if( m_pOwner != NULL ) m_pOwner->PageUp(); 
					else SetScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
				}
				else if ( event.ptMouse.y > m_rcThumb.bottom ){
					if( m_pOwner != NULL ) m_pOwner->PageDown(); 
					else SetScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);                    
				}
			}
			else {
				if( event.ptMouse.x < m_rcThumb.left ) {
					if( m_pOwner != NULL ) m_pOwner->PageLeft(); 
					else SetScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
				}
				else if ( event.ptMouse.x > m_rcThumb.right ){
					if( m_pOwner != NULL ) m_pOwner->PageRight(); 
					else SetScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);                    
				}
			}
		}
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		m_nScrollRepeatDelay = 0;
		m_nLastScrollOffset = 0;
		m_pManager->KillTimer(this, DEFAULT_TIMERID);

		if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
			m_uThumbState &= ~( UISTATE_CAPTURED | UISTATE_PUSHED );
			Invalidate();
		}
		else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
			m_uButton1State &= ~UISTATE_PUSHED;
			Invalidate();
		}
		else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
			m_uButton2State &= ~UISTATE_PUSHED;
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEMOVE )
	{
		if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
			if( !m_bHorizontal ) {

				int vRange = m_rcItem.bottom - m_rcItem.top - m_rcThumb.bottom + m_rcThumb.top;
				if( m_bShowButton1 )
					vRange -= m_cxyFixed.cx;
				if( m_bShowButton2 )
					vRange -= m_cxyFixed.cx;

				if (vRange != 0)
					m_nLastScrollOffset = (event.ptMouse.y - ptLastMouse.y) * m_nRange / vRange;
					
			}
			else {

				int hRange = m_rcItem.right - m_rcItem.left - m_rcThumb.right + m_rcThumb.left;
				if( m_bShowButton1 )
					hRange -= m_cxyFixed.cy;
				if( m_bShowButton2 )
					hRange -= m_cxyFixed.cy;

				if (hRange != 0)
					m_nLastScrollOffset = (event.ptMouse.x - ptLastMouse.x) * m_nRange / hRange;
			}
		}
		else {
			if( (m_uThumbState & UISTATE_HOT) != 0 ) {
				if( !::PtInRect(&m_rcThumb, event.ptMouse) ) {
					m_uThumbState &= ~UISTATE_HOT;
					Invalidate();
				}
			}
			else {
				if( !IsEnabled() ) return;
				if( ::PtInRect(&m_rcThumb, event.ptMouse) ) {
					m_uThumbState |= UISTATE_HOT;
					Invalidate();
				}
			}
		}
		return;
	}
	if( event.Type == UIEVENT_CONTEXTMENU )
	{
		return;
	}
	if( event.Type == UIEVENT_TIMER && event.wParam == DEFAULT_TIMERID )
	{
		++m_nScrollRepeatDelay;
		if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->SetScrollPos(DuiSize(m_pOwner->GetScrollPos().cx, \
					m_nLastScrollPos + m_nLastScrollOffset)); 
				else SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->SetScrollPos(DuiSize(m_nLastScrollPos + m_nLastScrollOffset, \
					m_pOwner->GetScrollPos().cy)); 
				else SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
			}
			Invalidate();
		}
		else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
			if( m_nScrollRepeatDelay <= 5 ) return;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->LineUp(); 
				else SetScrollPos(m_nScrollPos - GetLineSize());
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->LineLeft(); 
				else SetScrollPos(m_nScrollPos - GetLineSize());
			}
		}
		else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
			if( m_nScrollRepeatDelay <= 5 ) return;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->LineDown(); 
				else SetScrollPos(m_nScrollPos + GetLineSize());
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->LineRight(); 
				else SetScrollPos(m_nScrollPos + GetLineSize());
			}
		}
		else {
			if( m_nScrollRepeatDelay <= 5 ) return;
			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(m_pManager->GetPaintWindow(), &pt);
			if( !m_bHorizontal ) {
				if( pt.y < m_rcThumb.top ) {
					if( m_pOwner != NULL ) m_pOwner->PageUp(); 
					else SetScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
				}
				else if ( pt.y > m_rcThumb.bottom ){
					if( m_pOwner != NULL ) m_pOwner->PageDown(); 
					else SetScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);                    
				}
			}
			else {
				if( pt.x < m_rcThumb.left ) {
					if( m_pOwner != NULL ) m_pOwner->PageLeft(); 
					else SetScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
				}
				else if ( pt.x > m_rcThumb.right ){
					if( m_pOwner != NULL ) m_pOwner->PageRight(); 
					else SetScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);                    
				}
			}
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
        if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( IsEnabled() ) {
                m_uButton1State |= UISTATE_HOT;
                m_uButton2State |= UISTATE_HOT;
                if( ::PtInRect(&m_rcThumb, event.ptMouse) ) m_uThumbState |= UISTATE_HOT;
                Invalidate();
            }
        }
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
        if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( IsEnabled() ) {
                m_uButton1State &= ~UISTATE_HOT;
                m_uButton2State &= ~UISTATE_HOT;
                m_uThumbState &= ~UISTATE_HOT;
                Invalidate();
            }
            if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
        }
        else {
            if (m_pManager) m_pManager->AddMouseLeaveNeeded(this);
            return;
        }
	}

	if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else Control::DoEvent(event);
}

void ScrollBar::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	/*if( _tcscmp(pstrName, _T("button1color")) == 0 ) {
		while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetButton1Color(clrColor);
	}
	else */if( _tcscmp(pstrName, _T("button1normalimage")) == 0 ) SetButton1NormalImage(pstrValue);
	else if( _tcscmp(pstrName, _T("button1hotimage")) == 0 ) SetButton1HotImage(pstrValue);
	else if( _tcscmp(pstrName, _T("button1pushedimage")) == 0 ) SetButton1PushedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("button1disabledimage")) == 0 ) SetButton1DisabledImage(pstrValue);
	/*else if( _tcscmp(pstrName, _T("button2color")) == 0 ) {
		while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetButton2Color(clrColor);
	}*/
	else if( _tcscmp(pstrName, _T("button2normalimage")) == 0 ) SetButton2NormalImage(pstrValue);
	else if( _tcscmp(pstrName, _T("button2hotimage")) == 0 ) SetButton2HotImage(pstrValue);
	else if( _tcscmp(pstrName, _T("button2pushedimage")) == 0 ) SetButton2PushedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("button2disabledimage")) == 0 ) SetButton2DisabledImage(pstrValue);
	/*else if( _tcscmp(pstrName, _T("thumbcolor")) == 0 ) {
		while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetThumbColor(clrColor);
	}*/
	else if( _tcscmp(pstrName, _T("thumbnormalimage")) == 0 ) SetThumbNormalImage(pstrValue);
	else if( _tcscmp(pstrName, _T("thumbhotimage")) == 0 ) SetThumbHotImage(pstrValue);
	else if( _tcscmp(pstrName, _T("thumbpushedimage")) == 0 ) SetThumbPushedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("thumbdisabledimage")) == 0 ) SetThumbDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, _T("railnormalimage")) == 0 ) SetRailNormalImage(pstrValue);
	else if( _tcscmp(pstrName, _T("railhotimage")) == 0 ) SetRailHotImage(pstrValue);
	else if( _tcscmp(pstrName, _T("railpushedimage")) == 0 ) SetRailPushedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("raildisabledimage")) == 0 ) SetRailDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, _T("bknormalimage")) == 0 ) SetBkNormalImage(pstrValue);
	else if( _tcscmp(pstrName, _T("bkhotimage")) == 0 ) SetBkHotImage(pstrValue);
	else if( _tcscmp(pstrName, _T("bkpushedimage")) == 0 ) SetBkPushedImage(pstrValue);
	else if( _tcscmp(pstrName, _T("bkdisabledimage")) == 0 ) SetBkDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, _T("hor")) == 0 ) SetHorizontal(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("linesize")) == 0 ) SetLineSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("range")) == 0 ) SetScrollRange(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("value")) == 0 ) SetScrollPos(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("scrollunit")) == 0 ) SetScrollUnit(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("showbutton1")) == 0 ) SetShowButton1(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("showbutton2")) == 0 ) SetShowButton2(_tcscmp(pstrValue, _T("true")) == 0);
	else Control::SetAttribute(pstrName, pstrValue);
}

bool ScrollBar::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
	PaintBkColor(hDC);
	PaintBkImage(hDC);
	PaintBk(hDC);
	PaintButton1(hDC);
	PaintButton2(hDC);
	PaintThumb(hDC);
	PaintRail(hDC);
	PaintBorder(hDC);
    return true;
}

void ScrollBar::PaintBk(HDC hDC)
{
	if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
	else m_uThumbState &= ~ UISTATE_DISABLED;

	if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
		if( DrawImage(hDC, m_diBkDisabled) ) return;
	}
	else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
		if( DrawImage(hDC, m_diBkPushed) ) return;
	}
	else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
		if( DrawImage(hDC, m_diBkHot) ) return;
	}

	if( DrawImage(hDC, m_diBkNormal) ) return;
}

void ScrollBar::PaintButton1(HDC hDC)
{
	if( !m_bShowButton1 ) return;

	if( !IsEnabled() ) m_uButton1State |= UISTATE_DISABLED;
	else m_uButton1State &= ~ UISTATE_DISABLED;

	RECT rc = { 0 };
	RECT rcButtuon1 = m_pManager->GetDPIObj()->Scale(m_rcButton1);
	rc.left = rcButtuon1.left - m_rcItem.left;
	rc.top = rcButtuon1.top - m_rcItem.top;
	rc.right = rcButtuon1.right - m_rcItem.left;
	rc.bottom = rcButtuon1.bottom - m_rcItem.top;

	if( m_dwButton1Color != 0 ) {
		if (m_dwButton1Color >= 0xFF000000) Render::DrawColor(hDC, rcButtuon1, GetAdjustColor(m_dwButton1Color));
		else Render::DrawColor(hDC, rcButtuon1, GetAdjustColor(m_dwButton1Color));
	}

	if( (m_uButton1State & UISTATE_DISABLED) != 0 ) {
		m_diButton1Disabled.rcDestOffset = rc;
		if( DrawImage(hDC, m_diButton1Disabled) ) return;
	}
	else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
		m_diButton1Pushed.rcDestOffset = rc;
		if( DrawImage(hDC, m_diButton1Pushed) ) return;
	}
	else if( (m_uButton1State & UISTATE_HOT) != 0 ) {
		m_diButton1Hot.rcDestOffset = rc;
		if( DrawImage(hDC, m_diButton1Hot) ) return;
	}

	m_diButton1Normal.rcDestOffset = rc;
	if( DrawImage(hDC, m_diButton1Normal) ) return;
}

void ScrollBar::PaintButton2(HDC hDC)
{
	if( !m_bShowButton2 ) return;

	if( !IsEnabled() ) m_uButton2State |= UISTATE_DISABLED;
	else m_uButton2State &= ~ UISTATE_DISABLED;

	RECT rc = { 0 };
	RECT rcButtuon2 = m_pManager->GetDPIObj()->Scale(m_rcButton2);
	rc.left = rcButtuon2.left - m_rcItem.left;
	rc.top = rcButtuon2.top - m_rcItem.top;
	rc.right = rcButtuon2.right - m_rcItem.left;
	rc.bottom = rcButtuon2.bottom - m_rcItem.top;

	if( m_dwButton2Color != 0 ) {
		if (m_dwButton2Color >= 0xFF000000) Render::DrawColor(hDC, rcButtuon2, GetAdjustColor(m_dwButton2Color));
		else Render::DrawColor(hDC, rcButtuon2, GetAdjustColor(m_dwButton2Color));
	}

	if( (m_uButton2State & UISTATE_DISABLED) != 0 ) {
		m_diButton2Disabled.rcDestOffset = rc;
		if( DrawImage(hDC, m_diButton2Disabled) ) return;
	}
	else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
		m_diButton2Pushed.rcDestOffset = rc;
		if( DrawImage(hDC, m_diButton2Pushed) ) return;
	}
	else if( (m_uButton2State & UISTATE_HOT) != 0 ) {
		m_diButton2Hot.rcDestOffset = rc;
		if( DrawImage(hDC, m_diButton2Hot) ) return;
	}

	m_diButton2Normal.rcDestOffset = rc;
	if( DrawImage(hDC, m_diButton2Normal) ) return;
}

void ScrollBar::PaintThumb(HDC hDC)
{
	if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
	if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
	else m_uThumbState &= ~ UISTATE_DISABLED;

	RECT rc = { 0 };
	RECT rcThumb = m_pManager->GetDPIObj()->Scale(m_rcThumb);
	rc.left = rcThumb.left - m_rcItem.left;
	rc.top = rcThumb.top - m_rcItem.top;
	rc.right = rcThumb.right - m_rcItem.left;
	rc.bottom = rcThumb.bottom - m_rcItem.top;

	if( m_dwThumbColor != 0 ) {
		if (m_dwThumbColor >= 0xFF000000) Render::DrawColor(hDC, rcThumb, GetAdjustColor(m_dwThumbColor));
		else Render::DrawColor(hDC, rcThumb, GetAdjustColor(m_dwThumbColor));
	}

	if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
		m_diThumbDisabled.rcDestOffset = rc;
		if( DrawImage(hDC, m_diThumbDisabled) ) return;
	}
	else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
		m_diThumbPushed.rcDestOffset = rc;
		if( DrawImage(hDC, m_diThumbPushed) ) return;
	}
	else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
		m_diThumbHot.rcDestOffset = rc;
		if( DrawImage(hDC, m_diThumbHot) ) return;
	}
	m_diThumbNormal.rcDestOffset = rc;
	if( DrawImage(hDC, m_diThumbNormal) ) return;
}

void ScrollBar::PaintRail(HDC hDC)
{
	if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
	if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
	else m_uThumbState &= ~ UISTATE_DISABLED;

	RECT rc = { 0 };
	RECT rcThumb = m_pManager->GetDPIObj()->Scale(m_rcThumb);
	if( !m_bHorizontal ) {
		rc.left = rcThumb.left - m_rcItem.left;
		rc.top = (rcThumb.top + rcThumb.bottom) / 2 - m_rcItem.top - EstimateSize({}).cx / 2;
		rc.right = rcThumb.right - m_rcItem.left;
		rc.bottom = (rcThumb.top + rcThumb.bottom) / 2 - m_rcItem.top + EstimateSize({}).cx - EstimateSize({}).cx / 2;
	}
	else {
		rc.left = (rcThumb.left + rcThumb.right) / 2 - m_rcItem.left - EstimateSize({}).cy / 2;
		rc.top = rcThumb.top - m_rcItem.top;
		rc.right = (rcThumb.left + rcThumb.right) / 2 - m_rcItem.left + EstimateSize({}).cy - EstimateSize({}).cy / 2;
		rc.bottom = rcThumb.bottom - m_rcItem.top;
	}

	if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
		m_diRailDisabled.rcDestOffset = rc;
		if( DrawImage(hDC, m_diRailDisabled) ) return;
	}
	else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
		m_diRailPushed.rcDestOffset = rc;
		if( DrawImage(hDC, m_diRailPushed) ) return;
	}
	else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
		m_diRailHot.rcDestOffset = rc;
		if( DrawImage(hDC, m_diRailHot) ) return;
	}
	m_diRailNormal.rcDestOffset = rc;
	if( DrawImage(hDC, m_diRailNormal) ) return;
}

} // namespace dui

#include "stdafx.h"

namespace dui {

/////////////////////////////////////////////////////////////////////////////////////
//
class ComboWnd : public WindowImplBase
{
public:
	ComboWnd();
	virtual ~ComboWnd();

	virtual String GetSkinFolder() override{ return m_folder; };
	virtual String GetSkinFile() override{ return m_xmlfile; };
	virtual LPCTSTR GetWindowClassName(void) const override{ return _T("DuiComboWnd"); };

	void Init(Combo* pOwner, String xml, String folder, POINT pt, UIManager* pMainPaintManager, bool isLayeredWindow = true);
	virtual void Notify(Event& msg);
	Control* CreateControl(LPCTSTR pstrClassName);

	virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

//#if(_WIN32_WINNT >= 0x0501)
//	virtual UINT GetClassStyle() const{ return __super::GetClassStyle() | CS_DROPSHADOW; };
//#endif

	void Show();

private:
	String			m_folder;
	String			m_xmlfile;
public:
    Combo*			m_pOwner;
    List*			m_pLayout;
    int				m_iOldSel;
	POINT			m_BasedPoint;

	UIManager*	m_pParentManager;
	bool			m_bLayeredWindow;
};

ComboWnd::ComboWnd() :
m_pOwner(NULL),
m_pLayout(NULL),
m_iOldSel(-1),
m_pParentManager(NULL),
m_bLayeredWindow(true)
{
	ZeroMemory(&m_BasedPoint, sizeof(POINT));
}

ComboWnd::~ComboWnd()
{

}


void ComboWnd::Init(Combo* pOwner, String xml, String folder, POINT pt, UIManager* pMainPaintManager, bool isLayeredWindow /*= true*/)
{
	assert(pOwner && pMainPaintManager && !xml.empty() && !folder.empty());
    m_pOwner = pOwner;
    m_pLayout = NULL;
	m_xmlfile = xml;
	m_folder = folder;
	m_BasedPoint = pt;
    m_iOldSel = m_pOwner->GetCurSel();
	m_pParentManager = pMainPaintManager;
	m_bLayeredWindow = isLayeredWindow;

	wprintf(L"CMenuWnd::Init ParentManager name:%s\n", m_pParentManager->GetName());	//菜单退出有时候会崩溃

	if (m_bLayeredWindow)
	{
		m_manager.GetShadow()->ShowShadow(true);
		m_manager.GetShadow()->SetImage(_T("../public/bk/bk_shadow.png"));
		m_manager.GetShadow()->SetSize(14);
		m_manager.GetShadow()->SetShadowCorner({ 14, 14, 14, 14 });
	}

    // Position the popup window in absolute space
    SIZE szDrop = m_pOwner->GetDropBoxSize();
	if (szDrop.cx == 0)
	{
		szDrop.cx = m_pOwner->GetWidth();
	}
    RECT rcOwner = pOwner->GetPos();
    RECT rc = rcOwner;
    rc.top = rc.bottom;		// 父窗口left、bottom位置作为弹出窗口起点
    rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
    if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

	Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
#if 0
    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    int cyFixed = 0;
	for (int it = 0; it < m_pLayout->GetCount(); it++) {
		Control* pControl = static_cast<Control*>(m_pLayout->GetItemAt(it));
        if( !pControl->IsVisible() ) continue;
        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;
    }
    cyFixed += 4; // VBox 默认的padding 调整
    rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

    ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    DuiRect rcWork = oMonitor.rcWork;
    if( rc.bottom > rcWork.bottom ) {
        rc.left = rcOwner.left;
        rc.right = rcOwner.right;
        if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;
        rc.top = rcOwner.top - MIN(cyFixed, szDrop.cy);
        rc.bottom = rcOwner.top;
        ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
    }
    
	Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
#endif
    // HACK: Don't deselect the parent's caption
	//放到Show()函数中去
   /*HWND hWndParent = ::GetParent(m_hWnd);
	while (::GetParent(hWndParent) != NULL) hWndParent = ::GetParent(hWndParent);
	if (hWndParent)
		::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);*/
}

void ComboWnd::OnFinalMessage(HWND hWnd)
{
    m_pOwner->m_pWindow = NULL;
    m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
    m_pOwner->Invalidate();
    delete this;
}

LRESULT ComboWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandle = false;
    if( uMsg == WM_CREATE ) {
		OnCreate(uMsg, wParam, lParam, bHandle);
        return 0;
    }
    else 
	if( uMsg == WM_CLOSE ) {
        //m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
		if( !m_pOwner->IsFloat() ) m_pOwner->SetPos(m_pOwner->GetPos(), false);
		else m_pOwner->SetPos(m_pOwner->GetRelativePos(), false);
        m_pOwner->SetFocus();
    }
    else if( uMsg == WM_KILLFOCUS ) {
        if( m_hWnd != (HWND) wParam ) {
            HWND hWnd = ::GetFocus();
            HWND hParentWnd = NULL;
            bool bIsChildFocus = false;
            while( hParentWnd = ::GetParent(hWnd) ) {
                if( m_hWnd == hParentWnd ) {
                    bIsChildFocus = true;
                    break;
                }
                hWnd = hParentWnd;
            }
            if (!bIsChildFocus) {
                PostMessage(WM_CLOSE);
                return 0;
            }
        }
    }

	if (!bHandle)
	{
		LRESULT lRes = 0;
		if (m_manager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	}
	else
		return 0;
   
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void ComboWnd::Notify(Event& msg)
{
	if (msg.Type == UIEVENT_ITEMSELECT)
	{
		if (m_pOwner)
		{
			m_pOwner->DoEvent(msg);
		}
		Close();
	}
}

Control* ComboWnd::CreateControl(LPCTSTR pstrClassName)
{
	return NULL;
}

LRESULT ComboWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	return 0;
}

void ComboWnd::EnsureVisible(int iIndex)
{
    if( m_pOwner->GetCurSel() < 0 ) return;
    m_pLayout->FindSelectable(m_pOwner->GetCurSel(), false);
    RECT rcItem = m_pLayout->GetItemAt(iIndex)->GetPos();
    RECT rcList = m_pLayout->GetPos();
	ScrollBar* pHorizontalScrollBar = m_pLayout->GetHorizontalScrollBar();
    if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();
    int iPos = m_pLayout->GetScrollPos().cy;
    if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
    int dx = 0;
    if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
    if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
    Scroll(0, dx);
}

void ComboWnd::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = m_pLayout->GetScrollPos();
    m_pLayout->SetScrollPos(DuiSize(sz.cx + dx, sz.cy + dy));
}


void ComboWnd::Show()
{
	assert(m_hWnd != NULL);
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	DuiRect rcWork = oMonitor.rcWork;
	DuiRect monitor_rect = oMonitor.rcMonitor;
	DuiSize szInit = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
	szInit = m_manager.GetRoot()->EstimateSize(szInit);

	/*RECT rcOwner = m_pOwner->GetPos();
	POINT ptBase = { rcOwner.left, rcOwner.bottom};
	::ClientToScreen(m_pParentManager->GetPaintWindow(), &ptBase);*/
	
	if (m_BasedPoint.y + szInit.cy > monitor_rect.bottom)
	{
		m_BasedPoint.y -= szInit.cy;
	}
	
	
	DuiRect rc;
	rc.left = m_BasedPoint.x;
	rc.top = m_BasedPoint.y;
	if (rc.top < monitor_rect.top)
	{
		rc.top = monitor_rect.top;
	}

	//判断是否超出屏幕
	if (rc.left > monitor_rect.right - szInit.cx)
	{
		rc.left = monitor_rect.right - szInit.cx;
	}
	if (rc.left < monitor_rect.left)
	{
		rc.left = monitor_rect.left;
	}
	rc.right = rc.left + szInit.cx;
	rc.bottom = rc.top + szInit.cy;

	//HACK: Don't deselect the parent's caption
	HWND hWndParent = ::GetParent(m_hWnd);
	while (::GetParent(hWndParent) != NULL) hWndParent = ::GetParent(hWndParent);
	if (hWndParent)
		::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);

	::SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, 0, 0, SWP_NOSIZE);
	ShowWindow();
}

////////////////////////////////////////////////////////
Combo::Combo() : m_pWindow(NULL), m_iCurSel(-1), m_uButtonState(0)
{
    m_szDropBox = DuiSize(0, 150);
    ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));

	m_bShowText = true;
	m_bSelectCloseFlag = true;
}

LPCTSTR Combo::GetClass() const
{
    return DUI_CTR_COMBO;
}

LPVOID Combo::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_COMBO) == 0 ) return static_cast<Combo*>(this);
    return __super::GetInterface(pstrName);
}

UINT Combo::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

int Combo::GetCurSel() const
{
    return m_iCurSel;
}

void Combo::DoEvent(Event& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else __super::DoEvent(event);
        return;
    }

	if (event.Type == UIEVENT_ITEMSELECT)
	{
		List *list = dynamic_cast<List*>(event.pSender);
		assert(list);
		IListItem* item = static_cast<IListItem*>(list->GetItemAt(event.wParam)->GetInterface(DUI_CTR_ILISTITEM));
		if (item)
		{
			SetText(item->GetItemText().c_str());
			m_iCurSel = event.wParam;

			if (m_event_map.find(UIEVENT_ITEMSELECT) != m_event_map.cend())
			{
				Event ev;
				ev.Type = UIEVENT_ITEMSELECT;
				ev.pSender = this;
				if (!m_event_map.find(UIEVENT_ITEMSELECT)->second(&ev)){
					return;
				}
			}
		}
		else
		{
			SetText(_T("unkouwn"));
			m_iCurSel = -1;
		}
	}
    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        Invalidate();
    }
    else if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        Invalidate();
    }
	else if (event.Type == UIEVENT_BUTTONDOWN)
    {
        if( IsEnabled() ) {
            Activate();
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
        }
        return;
    }
	else if (event.Type == UIEVENT_BUTTONUP)
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            m_uButtonState &= ~ UISTATE_CAPTURED;
            Invalidate();
        }
        return;
    }
	else if (event.Type == UIEVENT_MOUSEMOVE)
    {
        return;
    }
	else if (event.Type == UIEVENT_KEYDOWN)
    {
        if (IsKeyboardEnabled() && IsEnabled()) {
            switch( event.chKey ) {
			default:
				break;
            }
            return;
        }
    }
	else if (event.Type == UIEVENT_SCROLLWHEEL)
    {
        /*if (IsEnabled()) {
            bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
            SetSelectCloseFlag(false);
            SelectItem(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
            SetSelectCloseFlag(true);
            return;
        }*/
    }
	else if (event.Type == UIEVENT_CONTEXTMENU)
    {
        return;
    }
	else if (event.Type == UIEVENT_MOUSEENTER)
    {
        if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( IsEnabled() ) {
                if( (m_uButtonState & UISTATE_HOT) == 0  ) {
                    m_uButtonState |= UISTATE_HOT;
                    Invalidate();
                }
            }
        }
    }
	else if (event.Type == UIEVENT_MOUSELEAVE)
    {
        if( !::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( IsEnabled() ) {
                if( (m_uButtonState & UISTATE_HOT) != 0  ) {
                    m_uButtonState &= ~UISTATE_HOT;
                    Invalidate();
                }
            }
            if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
        }
        else {
            if (m_pManager) m_pManager->AddMouseLeaveNeeded(this);
            return;
        }
    }
    Control::DoEvent(event);
}

SIZE Combo::EstimateSize(SIZE szAvailable)
{
	if (m_FixedSize.cy == 0 && m_FixedSize.cx == 0) return DuiSize(m_pManager->GetDPIObj()->Scale(m_FixedSize.cx), m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8);
	return Control::EstimateSize(szAvailable);
}

bool Combo::Activate()
{
    if( !Control::Activate() ) return false;
#if 1
    if( m_pWindow ) return true;
    m_pWindow = new ComboWnd();
    ASSERT(m_pWindow);

	RECT rcOwner = GetPos();
	POINT ptBase = { rcOwner.left, rcOwner.bottom + 2 };
	::ClientToScreen(m_pManager->GetPaintWindow(), &ptBase);

	m_pWindow->Init(this, m_dropBoxFile, m_dropBoxFolder, ptBase, m_pManager);
	//if (m_pManager != NULL) m_pManager->SendNotify(this, UIEVENT_DROPDOWN);
    //Invalidate();
	m_pWindow->Show();
#else
	if (!m_pWindow)
	{
		m_pWindow = new ComboWnd;
		ASSERT(m_pWindow);
		m_pWindow->Init(this, m_dropBoxFile, m_dropBoxFolder, m_pManager);
	}
	else
		m_pWindow->ShowWindow();
	if (m_pManager != NULL) m_pManager->SendNotify(this, UIEVENT_DROPDOWN);
#endif
    return true;
}

//String Combo::GetText() const
//{
//    if( m_iCurSel < 0 ) return _T("");
//    Control* pControl = static_cast<Control*>(m_items[m_iCurSel]);
//    return pControl->GetText();
//}

void Combo::SetEnabled(bool bEnable)
{
    __super::SetEnabled(bEnable);
    if( !IsEnabled() ) m_uButtonState = 0;
}

SIZE Combo::GetDropBoxSize() const
{
    return m_szDropBox;
}

void Combo::SetDropBoxSize(SIZE szDropBox)
{
    m_szDropBox = szDropBox;
}

bool Combo::GetShowText() const
{
	return m_bShowText;
}

void Combo::SetShowText(bool flag)
{
	m_bShowText = flag;
	Invalidate();
}

RECT Combo::GetTextPadding() const
{
    return m_rcTextPadding;
}

void Combo::SetTextPadding(RECT rc)
{
    m_rcTextPadding = rc;
    Invalidate();
}

LPCTSTR Combo::GetNormalImage() const
{
	return m_diNormal.sDrawString.c_str();
}

void Combo::SetNormalImage(LPCTSTR pStrImage)
{
	if( m_diNormal.sDrawString == pStrImage && m_diNormal.pImageInfo != NULL ) return;
	m_diNormal.Clear();
	m_diNormal.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR Combo::GetHotImage() const
{
	return m_diHot.sDrawString.c_str();
}

void Combo::SetHotImage(LPCTSTR pStrImage)
{
	if( m_diHot.sDrawString == pStrImage && m_diHot.pImageInfo != NULL ) return;
	m_diHot.Clear();
	m_diHot.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR Combo::GetPushedImage() const
{
	return m_diPushed.sDrawString.c_str();
}

void Combo::SetPushedImage(LPCTSTR pStrImage)
{
	if( m_diPushed.sDrawString == pStrImage && m_diPushed.pImageInfo != NULL ) return;
	m_diPushed.Clear();
	m_diPushed.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR Combo::GetFocusedImage() const
{
	return m_diFocused.sDrawString.c_str();
}

void Combo::SetFocusedImage(LPCTSTR pStrImage)
{
	if( m_diFocused.sDrawString == pStrImage && m_diFocused.pImageInfo != NULL ) return;
	m_diFocused.Clear();
	m_diFocused.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR Combo::GetDisabledImage() const
{
	return m_diDisabled.sDrawString.c_str();
}

void Combo::SetDisabledImage(LPCTSTR pStrImage)
{
	if( m_diDisabled.sDrawString == pStrImage && m_diDisabled.pImageInfo != NULL ) return;
	m_diDisabled.Clear();
	m_diDisabled.sDrawString = pStrImage;
	Invalidate();
}

void Combo::SetPos(RECT rc, bool bNeedInvalidate)
{
    // Put all elements out of sight
    RECT rcNull = { 0 };
    for( int i = 0; i < m_items.GetSize(); i++ ) static_cast<Control*>(m_items[i])->SetPos(rcNull, false);
    // Position this control
    Control::SetPos(rc, bNeedInvalidate);
}

void Combo::Move(SIZE szOffset, bool bNeedInvalidate)
{
	Control::Move(szOffset, bNeedInvalidate);
}

void Combo::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = NULL;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetTextPadding(rcTextPadding);
    }
	else if( _tcscmp(pstrName, _T("showtext")) == 0 ) SetShowText(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
    else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
    else if( _tcscmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, _T("dropboxsize")) == 0)
	{
		SIZE szDropBoxSize = { 0 };
		LPTSTR pstr = NULL;
		szDropBoxSize.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		szDropBoxSize.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		SetDropBoxSize(szDropBoxSize);
	}
	else if (_tcscmp(pstrName, _T("dropboxfile")) == 0)
		SetDropBoxFile(pstrValue);
	else if (_tcscmp(pstrName, _T("dropboxfolder")) == 0)
		SetDropBoxFolder(pstrValue);
    else __super::SetAttribute(pstrName, pstrValue);
}

bool Combo::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
    return Control::DoPaint(hDC, rcPaint, pStopControl);
}

void Combo::PaintStatusImage(HDC hDC)
{
    if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
    else m_uButtonState &= ~ UISTATE_FOCUSED;
    if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
    else m_uButtonState &= ~ UISTATE_DISABLED;

    if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
        if (DrawImage(hDC, m_diDisabled)) return;
    }
    else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
        if (DrawImage(hDC, m_diPushed)) return;
    }
    else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        if (DrawImage(hDC, m_diHot)) return;
    }
    else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
        if (DrawImage(hDC, m_diFocused)) return;
    }

    DrawImage(hDC, m_diNormal);
}

void Combo::PaintText(HDC hDC)
{
	if (!m_bShowText) return;

    RECT rcText = m_rcItem;
    rcText.left += m_rcTextPadding.left;
    rcText.right -= m_rcTextPadding.right;
    rcText.top += m_rcTextPadding.top;
    rcText.bottom -= m_rcTextPadding.bottom;

	String sText = GetText();
	if (sText.empty()) return;
	int nLinks = 0;
	if (IsEnabled()) {
		Render::DrawText(hDC, m_pManager, rcText, sText.c_str(), 0xff000000, 2, 0);
	}
	else {
		Render::DrawText(hDC, m_pManager, rcText, sText.c_str(), 0xff000000, 2, 0);
	}
}

} // namespace dui

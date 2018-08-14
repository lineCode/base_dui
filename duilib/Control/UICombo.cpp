#include "StdAfx.h"

namespace dui {

/////////////////////////////////////////////////////////////////////////////////////
//
#if !ComboBody_EQUAL_LIST
class ComboBody : public ScrollContainer
{
public:
    ComboBody::ComboBody(Combo* pOwner);
    bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);

protected:
    Combo* m_pOwner;
};


ComboBody::ComboBody(Combo* pOwner) : m_pOwner(pOwner)
{
    ASSERT(m_pOwner);
}

bool ComboBody::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl) {
    RECT rcTemp = { 0 };
    if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return true;

    ListViewInfo* pListInfo = NULL;
    if( m_pOwner ) pListInfo = m_pOwner->GetListInfo();

    CRenderClip clip;
    CRenderClip::GenerateClip(hDC, rcTemp, clip);
    Control::DoPaint(hDC, rcPaint, pStopControl);

    if( m_items.GetSize() > 0 ) {
        RECT rc = m_rcItem;
        rc.left += m_rcPadding.left;
        rc.top += m_rcPadding.top;
        rc.right -= m_rcPadding.right;
        rc.bottom -= m_rcPadding.bottom;
        if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

        if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                Control* pControl = static_cast<Control*>(m_items[it]);
                if( pControl == pStopControl ) return false;
                if( !pControl->IsVisible() ) continue;
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
                }
            }
        }
        else {
            int iDrawIndex = 0;
            CRenderClip childClip;
            CRenderClip::GenerateClip(hDC, rcTemp, childClip);
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                Control* pControl = static_cast<Control*>(m_items[it]);
                if( pControl == pStopControl ) return false;
                if( !pControl->IsVisible() ) continue;
                if( !pControl->IsFloat() ) {
                    IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
                    if( pListItem != NULL ) {
                        pListItem->SetDrawIndex(iDrawIndex);
                        iDrawIndex += 1;
                    }
                    if (pListInfo && pListInfo->iHLineSize > 0) {
                        // 因为没有为最后一个预留分割条长度，如果list铺满，最后一条不会显示
                        RECT rcMargin = pControl->GetMargin();
                        const RECT& rcPos = pControl->GetPos();
                        RECT rcBottomLine = { rcPos.left, rcPos.bottom + rcMargin.bottom, rcPos.right, rcPos.bottom + rcMargin.bottom + pListInfo->iHLineSize };
                        if( ::IntersectRect(&rcTemp, &rcPaint, &rcBottomLine) ) {
                            rcBottomLine.top += pListInfo->iHLineSize / 2;
                            rcBottomLine.bottom = rcBottomLine.top;
                            CRenderEngine::DrawLine(hDC, rcBottomLine, pListInfo->iHLineSize, GetAdjustColor(pListInfo->dwHLineColor));
                        }
                    }
                }
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    CRenderClip::UseOldClipBegin(hDC, childClip);
                    if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
                    CRenderClip::UseOldClipEnd(hDC, childClip);
                }
                else {
                    if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
                    if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
                }
            }
        }
    }

    if( m_pVerticalScrollBar != NULL ) {
        if( m_pVerticalScrollBar == pStopControl ) return false;
        if (m_pVerticalScrollBar->IsVisible()) {
            if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
                if( !m_pVerticalScrollBar->Paint(hDC, rcPaint, pStopControl) ) return false;
            }
        }
    }

    if( m_pHorizontalScrollBar != NULL ) {
        if( m_pHorizontalScrollBar == pStopControl ) return false;
        if (m_pHorizontalScrollBar->IsVisible()) {
            if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
                if( !m_pHorizontalScrollBar->Paint(hDC, rcPaint, pStopControl) ) return false;
            }
        }
    }
    return true;
}
#endif
/////////////////////////////////////////////////////////////////////////////////////
//
//

class CComboWnd : public WindowImplBase
{
public:
	CComboWnd();
	virtual ~CComboWnd();

	virtual String GetSkinFolder() override{ return m_folder; };
	virtual String GetSkinFile() override{ return m_xmlfile; };
	virtual LPCTSTR GetWindowClassName(void) const override{ return _T("DuiComboWnd"); };

	void Init(Combo* pOwner, String xml, String folder, POINT pt, CPaintManager* pMainPaintManager, bool isLayeredWindow = true);
	virtual void Notify(TEvent& msg);
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
    bool			m_bScrollbarClicked;
	POINT			m_BasedPoint;

	CPaintManager*	m_pParentManager;
	bool			m_bLayeredWindow;
};

CComboWnd::CComboWnd() :
m_pOwner(NULL),
m_pLayout(NULL),
m_iOldSel(-1),
m_bScrollbarClicked(false),
m_pParentManager(NULL),
m_bLayeredWindow(true)
{
	ZeroMemory(&m_BasedPoint, sizeof(POINT));
}

CComboWnd::~CComboWnd()
{

}


void CComboWnd::Init(Combo* pOwner, String xml, String folder, POINT pt, CPaintManager* pMainPaintManager, bool isLayeredWindow /*= true*/)
{
	assert(pOwner && pMainPaintManager && !xml.empty() && !folder.empty());
    m_pOwner = pOwner;
    m_pLayout = NULL;
	m_xmlfile = xml;
	m_folder = folder;
	m_BasedPoint = pt;
    m_iOldSel = m_pOwner->GetCurSel();
    m_bScrollbarClicked = false;
	m_pParentManager = pMainPaintManager;
	m_bLayeredWindow = isLayeredWindow;

	wprintf(L"CMenuWnd::Init ParentManager name:%s\n", m_pParentManager->GetName());	//菜单退出有时候会崩溃

	if (m_bLayeredWindow)
	{
		m_PaintManager.GetShadow()->ShowShadow(true);
		m_PaintManager.GetShadow()->SetImage(_T("../public/bk/bk_shadow.png"));
		m_PaintManager.GetShadow()->SetSize(14);
		m_PaintManager.GetShadow()->SetShadowCorner({ 14, 14, 14, 14 });
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
    cyFixed += 4; // VerticalLayout 默认的padding 调整
    rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

    ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;
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
   /* HWND hWndParent = m_hWnd;
    while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
    ::ShowWindow(m_hWnd, SW_SHOW);
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);*/
}

void CComboWnd::OnFinalMessage(HWND hWnd)
{
    m_pOwner->m_pWindow = NULL;
    m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
    m_pOwner->Invalidate();
    delete this;
}

LRESULT CComboWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandle = false;
    if( uMsg == WM_CREATE ) {
		OnCreate(uMsg, wParam, lParam, bHandle);
		//m_pm.SetForceUseSharedRes(true);
  //      m_pm.Init(m_hWnd);
		//String strResourcePath = m_pm.GetGlobalResDir();
		//ASSERT(!strResourcePath.empty());
		//if (!strResourcePath.empty()){
		//	String folder = GetSkinFolder();
		//	if (folder.back() != _T('\\') && folder.back() != _T('/')){
		//		folder += _T('\\');
		//	}
		//	strResourcePath += folder;
		//	m_pm.SetThisResPath(strResourcePath.c_str());
		//}
  //      // The trick is to add the items to the new container. Their owner gets
  //      // reassigned by this operation - which is why it is important to reassign
  //      // the items back to the righfull owner/manager when the window closes.
  //      //m_pLayout = new ComboBody(m_pOwner);
  //      m_pLayout->SetManager(&m_pm, NULL, true);
  //      LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VBox"));
  //      if( pDefaultAttributes ) {
  //          m_pLayout->SetAttributeList(pDefaultAttributes);
  //      }
  //      m_pLayout->SetPadding(CDuiRect(1, 1, 1, 1));
  //      m_pLayout->SetBkColor(0xFFFFFFFF);
  //      m_pLayout->SetBorderColor(0xFFC6C7D2);
  //      m_pLayout->SetBorderSize(1);
  //      m_pLayout->SetAutoDestroy(false);
  //      m_pLayout->EnableScrollBar();
  //      
  //      for( int i = 0; i < m_pOwner->GetCount(); i++ ) {
  //          m_pLayout->Add(static_cast<Control*>(m_pOwner->GetItemAt(i)));
  //      }
  //      m_pm.AttachDialog(m_pLayout);
        
        return 0;
    }
    else 
	if( uMsg == WM_CLOSE ) {
        //m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
		if( !m_pOwner->IsFloat() ) m_pOwner->SetPos(m_pOwner->GetPos(), false);
		else m_pOwner->SetPos(m_pOwner->GetRelativePos(), false);
        m_pOwner->SetFocus();
    }
    else if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK ) {
        POINT pt = { 0 };
        ::GetCursorPos(&pt);
		::ScreenToClient(m_PaintManager.GetPaintWindow(), &pt);
		Control* pControl = m_PaintManager.FindControl(pt);
        if( pControl && _tcscmp(pControl->GetClass(), DUI_CTR_SCROLLBAR) == 0 ) {
            m_bScrollbarClicked = true;
        }
#if 0
		if (!m_PaintManager.IsNoActivate()) ::SetFocus(m_hWnd);
		/*if (m_pRoot == NULL) break;*/
		POINT pt2 = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		/*m_ptLastMousePos = pt;*/
		pControl = m_PaintManager.FindControl(pt2);
		if (pControl && pControl->GetManager() == &m_PaintManager)
		{
			//m_pEventClick = pControl;
			pControl->SetFocus();
			//SetCapture();
			TEvent event/* = { UIEVENT__FIRST }*/;
			event.Type = UIEVENT_BUTTONDOWN;
			event.pSender = pControl;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->Event(event);
			IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
			while (!pListItem)
			{
				pControl = pControl->GetParent();
				if (!pControl)
					break;
				pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
			}
			if (pListItem && m_pOwner)
			{
				pListItem->Select(true, false);
				m_pOwner->SelectItem(m_pLayout->GetCurSel(), false, false);
				bHandle = true;
			}
		}
#endif
    }
    else if( uMsg == WM_LBUTTONUP ) {
        if (m_bScrollbarClicked) {
            m_bScrollbarClicked = false;
        }
        else {
            POINT pt = { 0 };
            ::GetCursorPos(&pt);
            ::ScreenToClient(m_PaintManager.GetPaintWindow(), &pt);
			Control* pControl = m_PaintManager.FindControl(pt);
            if( pControl && _tcscmp(pControl->GetClass(), DUI_CTR_SCROLLBAR) != 0 ) PostMessage(WM_KILLFOCUS);
        }
    }
    else if( uMsg == WM_KEYDOWN ) {
        switch( wParam ) {
        case VK_ESCAPE:
            m_pOwner->SelectItem(m_iOldSel, true);
            EnsureVisible(m_iOldSel);
            // FALL THROUGH...
        case VK_RETURN:
            PostMessage(WM_KILLFOCUS);
            break;
        default:
            TEvent event;
            event.Type = UIEVENT_KEYDOWN;
            event.chKey = (TCHAR)wParam;
            m_pOwner->DoEvent(event);
            EnsureVisible(m_pOwner->GetCurSel());
            return 0;
        }
    }
    //else if( uMsg == WM_MOUSEWHEEL ) {
    //    int zDelta = (int) (short) HIWORD(wParam);
    //    TEvent event/* = { UIEVENT__FIRST }*/;
    //    event.Type = UIEVENT_SCROLLWHEEL;
    //    event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
    //    event.lParam = lParam;
    //    event.dwTimestamp = ::GetTickCount();
    //    //m_pOwner->DoEvent(event);
    //    EnsureVisible(m_pOwner->GetCurSel());
    //    return 0;
    //}
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
		if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	}
	else
		return 0;
   
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CComboWnd::Notify(TEvent& msg)
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

Control* CComboWnd::CreateControl(LPCTSTR pstrClassName)
{
	/*if (_tcsicmp(pstrClassName, _T("Menu")) == 0)
	{
		return new Menu();
	}
	else if (_tcsicmp(pstrClassName, _T("MenuElement")) == 0)
	{
		return new MenuElement();
	}*/
	return NULL;
}

LRESULT CComboWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	return 0;
}

void CComboWnd::EnsureVisible(int iIndex)
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

void CComboWnd::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = m_pLayout->GetScrollPos();
    m_pLayout->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
}


void CComboWnd::Show()
{
	assert(m_hWnd != NULL);
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	CDuiRect monitor_rect = oMonitor.rcMonitor;
	CDuiSize szInit = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
	szInit = m_PaintManager.GetRoot()->EstimateSize(szInit);

	/*RECT rcOwner = m_pOwner->GetPos();
	POINT ptBase = { rcOwner.left, rcOwner.bottom};
	::ClientToScreen(m_pParentManager->GetPaintWindow(), &ptBase);*/
	
	if (m_BasedPoint.y + szInit.cy > monitor_rect.bottom)
	{
		m_BasedPoint.y -= szInit.cy;
	}
	
	
	CDuiRect rc;
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
	HWND hWndParent = m_hWnd;
	while (::GetParent(hWndParent) != NULL) hWndParent = ::GetParent(hWndParent);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);

	::SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, 0, 0, SWP_NOSIZE);
	ShowWindow();
}

////////////////////////////////////////////////////////
Combo::Combo() : m_pWindow(NULL), m_iCurSel(-1), m_uButtonState(0)
{
    m_szDropBox = CDuiSize(0, 150);
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

void Combo::DoInit()
{
}

int Combo::GetCurSel() const
{
    return m_iCurSel;
}

bool Combo::GetSelectCloseFlag()
{
	return m_bSelectCloseFlag;
}

void Combo::SetSelectCloseFlag(bool flag)
{
	m_bSelectCloseFlag = flag;
}

bool Combo::SelectItem(int iIndex, bool bTakeFocus, bool bTriggerEvent)
{
    if( m_bSelectCloseFlag && m_pWindow != NULL ) m_pWindow->Close();
    if( iIndex == m_iCurSel ) return true;
    int iOldSel = m_iCurSel;
    if( m_iCurSel >= 0 ) {
        Control* pControl = static_cast<Control*>(m_items[m_iCurSel]);
        if( !pControl ) return false;
        IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) pListItem->Select(false, bTriggerEvent);
        m_iCurSel = -1;
    }
    if( iIndex < 0 ) return false;
    if( m_items.GetSize() == 0 ) return false;
    if( iIndex >= m_items.GetSize() ) iIndex = m_items.GetSize() - 1;
    Control* pControl = static_cast<Control*>(m_items[iIndex]);
    if( !pControl || !pControl->IsVisible() || !pControl->IsEnabled() ) return false;
    IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem == NULL ) return false;
	SetText(pControl->GetText().c_str());
    m_iCurSel = iIndex;
    if( m_pWindow != NULL || bTakeFocus ) pControl->SetFocus();
    pListItem->Select(true, bTriggerEvent);
	if (m_pManager != NULL && bTriggerEvent) 
		m_pManager->SendNotify(this, UIEVENT_ITEMSELECT, m_iCurSel, iOldSel);
	//----------------add by djj-----------------
	if (OnEvent.find(UIEVENT_ITEMSELECT) != OnEvent.cend()){
		TEvent event;
		event.Type = UIEVENT_ITEMSELECT;
		event.pSender = this;
		if (!OnEvent.find(UIEVENT_ITEMSELECT)->second(&event)){
			return false;
		}
	}

    Invalidate();

    return true;
}

bool Combo::ExpandItem(int iIndex, bool bExpand)
{
    return false;
}

int Combo::GetExpandedItem() const
{
    return -1;
}

bool Combo::SetItemIndex(Control* pControl, int iNewIndex)
{
    int iOrginIndex = GetItemIndex(pControl);
    if( iOrginIndex == -1 ) return false;
    if( iOrginIndex == iNewIndex ) return true;

    IListItem* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItem*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !__super::SetItemIndex(pControl, iNewIndex) ) return false;
    int iMinIndex = min(iOrginIndex, iNewIndex);
    int iMaxIndex = max(iOrginIndex, iNewIndex);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        Control* p = GetItemAt(i);
        IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
    return true;
}

bool Combo::SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex)
{
    if (pStartControl == NULL || iCount < 0 || iNewStartIndex < 0) return false;
    int iStartIndex = GetItemIndex(pStartControl);
    if (iStartIndex == iNewStartIndex) return true;
    if (iStartIndex + iCount > GetCount()) return false;
    if (iNewStartIndex + iCount > GetCount()) return false;

    IListItem* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItem*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
	if (!__super::SetMultiItemIndex(pStartControl, iCount, iNewStartIndex)) return false;
    int iMinIndex = min(iStartIndex, iNewStartIndex);
    int iMaxIndex = max(iStartIndex + iCount, iNewStartIndex + iCount);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        Control* p = GetItemAt(i);
        IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
    return true;
}

bool Combo::Add(Control* pControl)
{
	if (!pControl)
		return false;
    IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem != NULL ) 
    {
        pListItem->SetOwner(this);
        pListItem->SetIndex(m_items.GetSize());
    }
	else
	{
		printf("Combo::Add Control(%s) is not the interface of DUI_CTR_ILISTITEM\n", pControl->GetName());
	}
	return __super::Add(pControl);
}

bool Combo::AddAt(Control* pControl, int iIndex)
{
    if (!__super::AddAt(pControl, iIndex)) return false;

    // The list items should know about us
    IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem != NULL ) {
        pListItem->SetOwner(this);
        pListItem->SetIndex(iIndex);
    }

    for(int i = iIndex + 1; i < GetCount(); ++i) {
        Control* p = GetItemAt(i);
        pListItem = static_cast<IListItem*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= iIndex ) m_iCurSel += 1;
    return true;
}

bool Combo::Remove(Control* pControl, bool bDoNotDestroy)
{
    int iIndex = GetItemIndex(pControl);
    if (iIndex == -1) return false;

    if (!__super::RemoveAt(iIndex, bDoNotDestroy)) return false;

    for(int i = iIndex; i < GetCount(); ++i) {
        Control* p = GetItemAt(i);
        IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
    return true;
}

bool Combo::RemoveAt(int iIndex, bool bDoNotDestroy)
{
    if (!__super::RemoveAt(iIndex, bDoNotDestroy)) return false;

    for(int i = iIndex; i < GetCount(); ++i) {
        Control* p = GetItemAt(i);
        IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) pListItem->SetIndex(i);
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
    return true;
}

void Combo::RemoveAll()
{
    m_iCurSel = -1;
    __super::RemoveAll();
}

void Combo::DoEvent(TEvent& event)
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
		ListElement* item = dynamic_cast<ListElement*>(list->GetItemAt(event.wParam));
		if (item)
		{
			SetText(item->GetText().c_str());
			m_iCurSel = event.wParam;

			TEvent ev;
			ev.Type = UIEVENT_ITEMSELECT;
			ev.pSender = this;
			if (!OnEvent.find(UIEVENT_ITEMSELECT)->second(&ev)){
				return;
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
            case VK_F4:
                Activate();
            case VK_UP:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(m_iCurSel - 1, false));
                SetSelectCloseFlag(true);
            case VK_DOWN:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(m_iCurSel + 1, true));
                SetSelectCloseFlag(true);
            case VK_PRIOR:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(m_iCurSel - 1, false));
                SetSelectCloseFlag(true);
            case VK_NEXT:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(m_iCurSel + 1, true));
                SetSelectCloseFlag(true);
            case VK_HOME:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(0, false));
                SetSelectCloseFlag(true);
            case VK_END:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(GetCount() - 1, true));
                SetSelectCloseFlag(true);
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
	if (m_cxyFixed.cy == 0 && m_cxyFixed.cx == 0) return CDuiSize(m_pManager->GetDPIObj()->Scale(m_cxyFixed.cx), m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8);
	return Control::EstimateSize(szAvailable);
}

bool Combo::Activate()
{
    if( !Control::Activate() ) return false;
#if 1
    if( m_pWindow ) return true;
    m_pWindow = new CComboWnd();
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
		m_pWindow = new CComboWnd;
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
		CRenderEngine::DrawText(hDC, m_pManager, rcText, sText.c_str(), 0xff000000, 2, 0);
	}
	else {
		CRenderEngine::DrawText(hDC, m_pManager, rcText, sText.c_str(), 0xff000000, 2, 0);
	}
}

} // namespace dui

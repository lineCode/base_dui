#include "StdAfx.h"

#include "UIMenu.h"

namespace dui {

/////////////////////////////////////////////////////////////////////////////////////
//

Menu::Menu()
{
	if (GetHeader() != NULL)
		GetHeader()->SetVisible(false);
}

Menu::~Menu()
{}

LPCTSTR Menu::GetClass() const
{
    return _T("MenuUI");
}

LPVOID Menu::GetInterface(LPCTSTR pstrName)
{
    if( _tcsicmp(pstrName, _T("Menu")) == 0 ) return static_cast<Menu*>(this);
    return List::GetInterface(pstrName);
}

void Menu::DoEvent(TEvent& event)
{
	return __super::DoEvent(event);
}

bool Menu::Add(Control* pControl)
{
	MenuElement* pMenuItem = static_cast<MenuElement*>(pControl->GetInterface(_T("MenuElement")));
	if (pMenuItem == NULL)
		return false;

	for (int i = 0; i < pMenuItem->GetCount(); ++i)
	{
		if (pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL)
		{
			(static_cast<MenuElement*>(pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(false);
		}
	}
	return List::Add(pControl);
}

bool Menu::AddAt(Control* pControl, int iIndex)
{
	MenuElement* pMenuItem = static_cast<MenuElement*>(pControl->GetInterface(_T("MenuElement")));
	if (pMenuItem == NULL)
		return false;

	for (int i = 0; i < pMenuItem->GetCount(); ++i)
	{
		if (pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL)
		{
			(static_cast<MenuElement*>(pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(false);
		}
	}
	return List::AddAt(pControl, iIndex);
}

int Menu::GetItemIndex(Control* pControl) const
{
	MenuElement* pMenuItem = static_cast<MenuElement*>(pControl->GetInterface(_T("MenuElement")));
	if (pMenuItem == NULL)
		return -1;

	return __super::GetItemIndex(pControl);
}

bool Menu::SetItemIndex(Control* pControl, int iIndex)
{
	MenuElement* pMenuItem = static_cast<MenuElement*>(pControl->GetInterface(_T("MenuElement")));
	if (pMenuItem == NULL)
		return false;

	return __super::SetItemIndex(pControl, iIndex);
}

bool Menu::Remove(Control* pControl)
{
	MenuElement* pMenuItem = static_cast<MenuElement*>(pControl->GetInterface(_T("MenuElement")));
	if (pMenuItem == NULL)
		return false;

	return __super::Remove(pControl);
}

SIZE Menu::EstimateSize(SIZE szAvailable)
{
	int cxFixed = 0;
    int cyFixed = 0;
    for( int it = 0; it < GetCount(); it++ ) {
        Control* pControl = static_cast<Control*>(GetItemAt(it));
        if( !pControl->IsVisible() ) continue;
        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;
		if( cxFixed < sz.cx )
			cxFixed = sz.cx;
    }
    return CDuiSize(cxFixed, cyFixed);
}

void Menu::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	List::SetAttribute(pstrName, pstrValue);
}

/////////////////////////////////////////////////////////////////////////////////////
//

CMenuWnd::CMenuWnd():
m_pOwner(NULL),
m_pLayout(NULL),
m_xml(_T("")),
m_pParentManager(NULL)
{
	m_dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;
}

CMenuWnd::~CMenuWnd()
{

}
#if MENUWND_OBSERVER
BOOL CMenuWnd::Receive(ContextMenuParam param)
{
	switch (param.wParam)
	{
	case 1:
		Close();
		break;
	case 2:
		{
			HWND hParent = GetParent(m_hWnd);
			while (hParent != NULL)
			{
				if (hParent == param.hWnd)
				{
					Close();
					break;
				}
				hParent = GetParent(hParent);
			}
		}
		break;
	default:
		break;
	}

	return TRUE;
}
#endif
void CMenuWnd::Init(MenuElement* pOwner, STRINGorID xml, String folder, POINT point,
					CPaintManager* pMainPaintManager, std::map<String,bool>* pMenuCheckInfo/* = NULL*/,
					DWORD dwAlignment/* = eMenuAlignment_Left | eMenuAlignment_Top*/)
{
	m_BasedPoint = point;
    m_pOwner = pOwner;
    m_pLayout = NULL;
	m_xml = xml;
	m_dwAlignment = dwAlignment;
	m_pParentManager = pMainPaintManager;

	String strResourcePath = m_pm.GetGlobalResDir();
	ASSERT(!strResourcePath.empty());
	if (!strResourcePath.empty() && !folder.empty()){
		if (folder.back() != _T('\\') && folder.back() != _T('/')){
			folder += _T('\\');
		}
		strResourcePath += folder;
		m_pm.SetThisResPath(strResourcePath.c_str());
	}
#if MENUWND_OBSERVER
	// 如果是一级菜单的创建
	if (pOwner == NULL)
	{
		ASSERT(pMainPaintManager != NULL);
		CMenuWnd::GetGlobalContextMenuObserver().SetManger(pMainPaintManager);
		if (pMenuCheckInfo != NULL)
			CMenuWnd::GetGlobalContextMenuObserver().SetMenuCheckInfo(pMenuCheckInfo);
	}

	CMenuWnd::GetGlobalContextMenuObserver().AddReceiver(this);
#endif
	Create((m_pOwner == NULL) ? pMainPaintManager->GetPaintWindow() : m_pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP , WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CDuiRect());

	// HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;
    while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);

    ::ShowWindow(m_hWnd, SW_SHOW);
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CMenuWnd::GetWindowClassName() const
{
    return _T("DuiMenuWnd");
}


void CMenuWnd::Notify(TEvent& msg)
{
#if MENUWND_OBSERVER
	if( CMenuWnd::GetGlobalContextMenuObserver().GetManager() != NULL) 
	{
		if (msg.sType == DUI_MSGTYPE_CLICK || msg.sType == UIEVENT_ITEMCLICK/* || msg.sType == _T("valuechanged")*/)
		{
			CMenuWnd::GetGlobalContextMenuObserver().GetManager()->SendNotify(msg, false);
		}
	}
#else
	if (msg.Type == UIEVENT_ITEMCLICK)
	{
		if (m_pParentManager)
		{
			m_pParentManager->SendNotify(msg, false);
		}
		Close();
	}
#endif
}

Control* CMenuWnd::CreateControl( LPCTSTR pstrClassName )
{
		if (_tcsicmp(pstrClassName, _T("Menu")) == 0)
		{
			return new Menu();
		}
		else if (_tcsicmp(pstrClassName, _T("MenuElement")) == 0)
		{
			return new MenuElement();
		}
		return NULL;
}


void CMenuWnd::OnFinalMessage(HWND hWnd)
{
#if MENUWND_OBSERVER
	RemoveObserver();
#endif
	if( m_pOwner != NULL ) {
		for( int i = 0; i < m_pOwner->GetCount(); i++ ) {
			if( static_cast<MenuElement*>(m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement"))) != NULL ) {
				(static_cast<MenuElement*>(m_pOwner->GetItemAt(i)))->SetOwner(m_pOwner->GetParent());
				(static_cast<MenuElement*>(m_pOwner->GetItemAt(i)))->SetVisible(false);
				(static_cast<MenuElement*>(m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(false);
			}
		}
		m_pOwner->m_pWindow = NULL;
		m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
		m_pOwner->Invalidate();
	}
    //delete this;
}

LRESULT CMenuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_pOwner != NULL) {
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

		m_pm.Init(m_hWnd);
		// The trick is to add the items to the new container. Their owner gets
		// reassigned by this operation - which is why it is important to reassign
		// the items back to the righfull owner/manager when the window closes.
		m_pLayout = new Menu();
		//m_pm.UseParentResource(m_pOwner->GetManager());	//djj[20170425] : not defined.
		m_pLayout->SetManager(&m_pm, NULL, true);
		LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("Menu"));
		if( pDefaultAttributes ) {
			m_pLayout->SetAttributeList(pDefaultAttributes);
		}
		m_pLayout->SetAutoDestroy(false);

		for( int i = 0; i < m_pOwner->GetCount(); i++ ) {
			if(m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL ){
				(static_cast<MenuElement*>(m_pOwner->GetItemAt(i)))->SetOwner(m_pLayout);
				m_pLayout->Add(static_cast<Control*>(m_pOwner->GetItemAt(i)));
			}
		}

		CShadowUI *pShadow = m_pOwner->GetManager()->GetShadow();
		pShadow->CopyShadow(m_pm.GetShadow());

		pShadow->ShowShadow(false);

		m_pm.AttachDialog(m_pLayout);
		m_pm.AddNotifier(this);
		
		ResizeSubMenu();
	}
	else {
		m_pm.Init(m_hWnd);

		CDialogBuilder builder;

		Control* pRoot = builder.Create(m_xml,UINT(0), this, &m_pm);
		//m_pm.GetShadow()->ShowShadow(false);
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		ResizeMenu();
	}
#if 0		
	m_pm.GetShadow()->ShowShadow(true);
	m_pm.GetShadow()->Create(&m_pm);
#endif
	return 0;
}

Menu* CMenuWnd::GetMenuUI()
{
	return static_cast<Menu*>(m_pm.GetRoot());
}

void CMenuWnd::ResizeMenu()
{
	Control* pRoot = m_pm.GetRoot();

#if defined(WIN32) && !defined(UNDER_CE)
	MONITORINFO oMonitor = {}; 
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
#else
	CDuiRect rcWork;
	GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
	SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
	szAvailable = pRoot->EstimateSize(szAvailable);
	m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

	//必须是Menu标签作为xml的根节点
	Menu *pMenuRoot = static_cast<Menu*>(pRoot);
	ASSERT(pMenuRoot);

	SIZE szInit = m_pm.GetInitSize();
	CDuiRect rc;
	CDuiPoint point = m_BasedPoint;
	rc.left = point.x;
	rc.top = point.y;
	rc.right = rc.left + szInit.cx;
	rc.bottom = rc.top + szInit.cy;

	int nWidth = rc.GetWidth();
	int nHeight = rc.GetHeight();

	if (m_dwAlignment & eMenuAlignment_Right)
	{
		rc.right = point.x;
		rc.left = rc.right - nWidth;
	}

	if (m_dwAlignment & eMenuAlignment_Bottom)
	{
		rc.bottom = point.y;
		rc.top = rc.bottom - nHeight;
	}

	SetForegroundWindow(m_hWnd);
	MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
	SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top,
		rc.GetWidth(), rc.GetHeight() + pMenuRoot->GetPadding().bottom + pMenuRoot->GetPadding().top,
		SWP_SHOWWINDOW);
}

void CMenuWnd::ResizeSubMenu()
{
	// Position the popup window in absolute space
	RECT rcOwner = m_pOwner->GetPos();
	RECT rc = rcOwner;

	int cxFixed = 0;
	int cyFixed = 0;

#if defined(WIN32) && !defined(UNDER_CE)
	MONITORINFO oMonitor = {}; 
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
#else
	CDuiRect rcWork;
	GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
	SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

	for( int it = 0; it < m_pOwner->GetCount(); it++ ) {
		if(m_pOwner->GetItemAt(it)->GetInterface(_T("MenuElement")) != NULL ){
			Control* pControl = static_cast<Control*>(m_pOwner->GetItemAt(it));
			SIZE sz = pControl->EstimateSize(szAvailable);
			cyFixed += sz.cy;

			if( cxFixed < sz.cx )
				cxFixed = sz.cx;
		}
	}

	RECT rcWindow;
	GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWindow);

	rc.top = rcOwner.top;
	rc.bottom = rc.top + cyFixed;
	::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
	rc.left = rcWindow.right;
	rc.right = rc.left + cxFixed;
	rc.right += 2;

	bool bReachBottom = false;
	bool bReachRight = false;
	LONG chRightAlgin = 0;
	LONG chBottomAlgin = 0;

	RECT rcPreWindow = {0};
#if MENUWND_OBSERVER
	MenuObserverImpl::Iterator iterator(CMenuWnd::GetGlobalContextMenuObserver());
	MenuMenuReceiverImplBase* pReceiver = iterator.next();
	while( pReceiver != NULL ) {
		CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
		if( pContextMenu != NULL ) {
			GetWindowRect(pContextMenu->GetHWND(), &rcPreWindow);

			bReachRight = rcPreWindow.left >= rcWindow.right;
			bReachBottom = rcPreWindow.top >= rcWindow.bottom;
			if( pContextMenu->GetHWND() == m_pOwner->GetManager()->GetPaintWindow() 
				||  bReachBottom || bReachRight )
				break;
		}
		pReceiver = iterator.next();
	}
#endif
	if (bReachBottom)
	{
		rc.bottom = rcWindow.top;
		rc.top = rc.bottom - cyFixed;
	}

	if (bReachRight)
	{
		rc.right = rcWindow.left;
		rc.left = rc.right - cxFixed;
	}

	if( rc.bottom > rcWork.bottom )
	{
		rc.bottom = rc.top;
		rc.top = rc.bottom - cyFixed;
	}

	if (rc.right > rcWork.right)
	{
		rc.right = rcWindow.left;
		rc.left = rc.right - cxFixed;
	}

	if( rc.top < rcWork.top )
	{
		rc.top = rcOwner.top;
		rc.bottom = rc.top + cyFixed;
	}

	if (rc.left < rcWork.left)
	{
		rc.left = rcWindow.right;
		rc.right = rc.left + cxFixed;
	}

	MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + m_pLayout->GetPadding().top + m_pLayout->GetPadding().bottom, FALSE);

}


LRESULT CMenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if MENUWND_OBSERVER
	HWND hFocusWnd = (HWND)wParam;

	BOOL bInMenuWindowList = FALSE;
	ContextMenuParam param;
	param.hWnd = GetHWND();

	MenuObserverImpl::Iterator iterator(CMenuWnd::GetGlobalContextMenuObserver());
	MenuMenuReceiverImplBase* pReceiver = iterator.next();
	while( pReceiver != NULL ) {
		CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
		if( pContextMenu != NULL && pContextMenu->GetHWND() ==  hFocusWnd ) {
			bInMenuWindowList = TRUE;
			break;
		}
		pReceiver = iterator.next();
	}

	if( !bInMenuWindowList ) {
		param.wParam = 1;
		CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
		return 0;
	}
#else
	Close();
#endif
	return 0;
}
LRESULT CMenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pm.GetRoundCorner();
	if( !::IsIconic(*this) ) {
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg )
	{
	case WM_CREATE:       
		lRes = OnCreate(uMsg, wParam, lParam, bHandled); 
		break;
	case WM_KILLFOCUS:       
		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); 
		break;
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE || wParam == VK_LEFT)
			Close();
		break;
	case WM_SIZE:
		lRes = OnSize(uMsg, wParam, lParam, bHandled);
		break;
	case WM_CLOSE:
		if( m_pOwner != NULL )
		{
			m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
			m_pOwner->SetPos(m_pOwner->GetPos());
			m_pOwner->SetFocus();
		}
		bHandled = FALSE;
		break;
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		return 0L;
		break;
	default:
		bHandled = FALSE;
		break;
	}

	if (bHandled) return lRes;

    if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////////////
//

MenuElement::MenuElement():
m_pWindow(NULL),
m_bDrawLine(false),
m_dwLineColor(DEFAULT_LINE_COLOR),
m_bCheckItem(false),
m_bShowExplandIcon(false)
{
	m_cxyFixed.cy = ITEM_DEFAULT_HEIGHT;
	m_cxyFixed.cx = ITEM_DEFAULT_WIDTH;
	m_szIconSize.cy = ITEM_DEFAULT_ICON_SIZE;
	m_szIconSize.cx = ITEM_DEFAULT_ICON_SIZE;

	m_rcLinePadding.top = m_rcLinePadding.bottom = 0;
	m_rcLinePadding.left = DEFAULT_LINE_LEFT_PADDING;
	m_rcLinePadding.right = DEFAULT_LINE_RIGHT_PADDING;
}

MenuElement::~MenuElement()
{}

LPCTSTR MenuElement::GetClass() const
{
	return _T("MenuElementUI");
}

LPVOID MenuElement::GetInterface(LPCTSTR pstrName)
{
    if( _tcsicmp(pstrName, _T("MenuElement")) == 0 ) return static_cast<MenuElement*>(this);    
    return ListContainerElement::GetInterface(pstrName);
}

bool MenuElement::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return true;

	if(m_bDrawLine)
	{
		RECT rcLine = { m_rcItem.left +  m_rcLinePadding.left, m_rcItem.top + m_cxyFixed.cy/2, m_rcItem.right - m_rcLinePadding.right, m_rcItem.top + m_cxyFixed.cy/2 };
		CRenderEngine::DrawLine(hDC, rcLine, 1, m_dwLineColor);
	}
	else
	{
		MenuElement::DrawItemBk(hDC, m_rcItem);
		DrawItemText(hDC, m_rcItem);
		DrawItemIcon(hDC, m_rcItem);
		DrawItemExpland(hDC, m_rcItem);
		for (int i = 0; i < GetCount(); ++i)
		{
			if (GetItemAt(i)->GetInterface(_T("MenuElement")) == NULL)
				GetItemAt(i)->DoPaint(hDC, rcPaint, NULL);
		}
	}
	return true;
}

void MenuElement::DrawItemIcon(HDC hDC, const RECT& rcItem)
{
	if ( m_strIcon != _T("") )
	{
		if (!(m_bCheckItem && !GetChecked()))
		{
#if 1
			TCHAR cImage[526] = {};
			wsprintf(cImage, _T("file='%s' dest='%d,%d,%d,%d'"), m_strIcon.c_str(), 
				(ITEM_DEFAULT_ICON_WIDTH - m_szIconSize.cx)/2,
				(m_cxyFixed.cy - m_szIconSize.cy)/2,
				(ITEM_DEFAULT_ICON_WIDTH - m_szIconSize.cx)/2 + m_szIconSize.cx,
				(m_cxyFixed.cy - m_szIconSize.cy)/2 + m_szIconSize.cy);
			TDrawInfo drawinfo;
			drawinfo.sDrawString = cImage;
			drawinfo.rcDestOffset = rcItem;
#else
			String pStrImage;
			pStrImage.Format(_T("file='%s' dest='%d,%d,%d,%d'"), m_strIcon.c_str(), 
				(ITEM_DEFAULT_ICON_WIDTH - m_szIconSize.cx)/2,
				(m_cxyFixed.cy - m_szIconSize.cy)/2,
				(ITEM_DEFAULT_ICON_WIDTH - m_szIconSize.cx)/2 + m_szIconSize.cx,
				(m_cxyFixed.cy - m_szIconSize.cy)/2 + m_szIconSize.cy);
			TDrawInfo drawinfo;
			drawinfo.sDrawString = pStrImage;
			drawinfo.rcDestOffset = rcItem;
#endif
			CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, drawinfo);
		}			
	}
}

void MenuElement::DrawItemExpland(HDC hDC, const RECT& rcItem)
{
	if (m_bShowExplandIcon)
	{
		String strExplandIcon;
		strExplandIcon = GetManager()->GetDefaultAttributeList(_T("ExplandIcon"));
#if 1
		TCHAR cImage[526] = {};
		wsprintf(cImage, _T("file='%s' dest='%d,%d,%d,%d'"), strExplandIcon.c_str(),
			m_cxyFixed.cx - ITEM_DEFAULT_EXPLAND_ICON_WIDTH + (ITEM_DEFAULT_EXPLAND_ICON_WIDTH - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / 2,
			(m_cxyFixed.cy - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / 2,
			m_cxyFixed.cx - ITEM_DEFAULT_EXPLAND_ICON_WIDTH + (ITEM_DEFAULT_EXPLAND_ICON_WIDTH - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / 2 + ITEM_DEFAULT_EXPLAND_ICON_SIZE,
			(m_cxyFixed.cy - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / 2 + ITEM_DEFAULT_EXPLAND_ICON_SIZE);
		TDrawInfo drawinfo;
		drawinfo.sDrawString = cImage;
		drawinfo.rcDestOffset = rcItem;
#else
		String strBkImage;
		strBkImage.Format(_T("file='%s' dest='%d,%d,%d,%d'"), strExplandIcon.c_str(), 
			m_cxyFixed.cx - ITEM_DEFAULT_EXPLAND_ICON_WIDTH + (ITEM_DEFAULT_EXPLAND_ICON_WIDTH - ITEM_DEFAULT_EXPLAND_ICON_SIZE)/2,
			(m_cxyFixed.cy - ITEM_DEFAULT_EXPLAND_ICON_SIZE)/2,
			m_cxyFixed.cx - ITEM_DEFAULT_EXPLAND_ICON_WIDTH + (ITEM_DEFAULT_EXPLAND_ICON_WIDTH - ITEM_DEFAULT_EXPLAND_ICON_SIZE)/2 + ITEM_DEFAULT_EXPLAND_ICON_SIZE,
			(m_cxyFixed.cy - ITEM_DEFAULT_EXPLAND_ICON_SIZE)/2 + ITEM_DEFAULT_EXPLAND_ICON_SIZE);
		TDrawInfo drawinfo;
		drawinfo.sDrawString = strBkImage;
		drawinfo.rcDestOffset = rcItem;
#endif
		CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, drawinfo);
	}
}


void MenuElement::DrawItemText(HDC hDC, const RECT& rcItem)
{
	if (m_sText.empty()) return;

    if( m_pOwner == NULL ) return;
    TListInfo* pInfo = m_pOwner->GetListInfo();
    DWORD iTextColor = pInfo->dwTextColor;
    if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        iTextColor = pInfo->dwHotTextColor;
    }
    if( IsSelected() ) {
        iTextColor = pInfo->dwSelectedTextColor;
    }
    if( !IsEnabled() ) {
        iTextColor = pInfo->dwDisabledTextColor;
    }
    int nLinks = 0;
    RECT rcText = rcItem;
    rcText.left += pInfo->rcTextPadding.left;
    rcText.right -= pInfo->rcTextPadding.right;
    rcText.top += pInfo->rcTextPadding.top;
    rcText.bottom -= pInfo->rcTextPadding.bottom;

    if( pInfo->bShowHtml )
		CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText.c_str(), iTextColor, \
        NULL, NULL, nLinks, pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
    else
		CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText.c_str(), iTextColor, \
        pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
}


SIZE MenuElement::EstimateSize(SIZE szAvailable)
{
	SIZE cXY = {0};
	for( int it = 0; it < GetCount(); it++ ) {
		Control* pControl = static_cast<Control*>(GetItemAt(it));
		if( !pControl->IsVisible() ) continue;
		SIZE sz = pControl->EstimateSize(szAvailable);
		cXY.cy += sz.cy;
		if( cXY.cx < sz.cx )
			cXY.cx = sz.cx;
	}
	if(cXY.cy == 0) {
		TListInfo* pInfo = m_pOwner->GetListInfo();

		DWORD iTextColor = pInfo->dwTextColor;
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if( IsSelected() ) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if( !IsEnabled() ) {
			iTextColor = pInfo->dwDisabledTextColor;
		}

		RECT rcText = { 0, 0, MAX(szAvailable.cx, m_cxyFixed.cx), 9999 };
		rcText.left += pInfo->rcTextPadding.left;
		rcText.right -= pInfo->rcTextPadding.right;
		if( pInfo->bShowHtml ) {   
			int nLinks = 0;
			CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), iTextColor, NULL, NULL, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle);
		}
		else {
			CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), iTextColor, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle);
		}
		cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right + 20;
		cXY.cy = rcText.bottom - rcText.top + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
	}

	if( m_cxyFixed.cy != 0 ) cXY.cy = m_cxyFixed.cy;
	if ( cXY.cx < m_cxyFixed.cx )
		cXY.cx =  m_cxyFixed.cx;

	m_cxyFixed.cy = cXY.cy;
	m_cxyFixed.cx = cXY.cx;
	return cXY;
}

void MenuElement::DoEvent(TEvent& event)
{
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		ListContainerElement::DoEvent(event);
		if( m_pWindow ) return;
		bool hasSubMenu = false;
		for( int i = 0; i < GetCount(); ++i )
		{
			if( GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL )
			{
				(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
				(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(true);

				hasSubMenu = true;
			}
		}
		if( hasSubMenu )
		{
			m_pOwner->SelectItem(GetIndex(), true);
			CreateMenuWnd();
		}
		else
		{
#if MENUWND_OBSERVER
			ContextMenuParam param;
			param.hWnd = m_pManager->GetPaintWindow();
			param.wParam = 2;
			CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
			m_pOwner->SelectItem(GetIndex(), true);
#endif
		}
		return;
	}

	if( event.Type == UIEVENT_BUTTONUP )
	{
		if( IsEnabled() ){
			ListContainerElement::DoEvent(event);

			if( m_pWindow ) return;

			bool hasSubMenu = false;
			for( int i = 0; i < GetCount(); ++i ) {
				if( GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL ) {
					(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
					(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(true);

					hasSubMenu = true;
				}
			}
			if( hasSubMenu )
			{
				CreateMenuWnd();
			}
			else
			{
#if MENUWND_OBSERVER
				SetChecked(!GetChecked());
				if (CMenuWnd::GetGlobalContextMenuObserver().GetManager() != NULL)
				{
#if 0		//djj[20170425] : free pMenuCmd err when delete pMenuCmd outside;
					MenuCmd* pMenuCmd = new MenuCmd();
					lstrcpy(pMenuCmd->szName, GetName().c_str());
					lstrcpy(pMenuCmd->szUserData, GetUserData().c_str());
					lstrcpy(pMenuCmd->szText, GetText().c_str());
					pMenuCmd->bChecked = GetChecked();
					if (!PostMessage(CMenuWnd::GetGlobalContextMenuObserver().GetManager()->GetPaintWindow(), WM_MENUCLICK, (WPARAM)pMenuCmd, NULL))
					{
						delete pMenuCmd;
						pMenuCmd = NULL;
					}
#else
					MenuCmd menuCmd = {};
					lstrcpy(menuCmd.szName, GetName().c_str());
					lstrcpy(menuCmd.szUserData, GetUserData().c_str());
					lstrcpy(menuCmd.szText, GetText().c_str());
					menuCmd.bChecked = GetChecked();
					if (!SendMessage(CMenuWnd::GetGlobalContextMenuObserver().GetManager()->GetPaintWindow(), WM_MENUCLICK, (WPARAM)&menuCmd, NULL))
					{}
#endif		
				}
				ContextMenuParam param;
				param.hWnd = m_pManager->GetPaintWindow();
				param.wParam = 1;
				CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
#endif
			}
        }

        return;
    }

	if ( event.Type == UIEVENT_KEYDOWN && event.chKey == VK_RIGHT )
	{
		if( m_pWindow ) return;
		bool hasSubMenu = false;
		for( int i = 0; i < GetCount(); ++i )
		{
			if( GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL )
			{
				(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
				(static_cast<MenuElement*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(true);

				hasSubMenu = true;
			}
		}
		if( hasSubMenu )
		{
			m_pOwner->SelectItem(GetIndex(), true);
			CreateMenuWnd();
		}
		else
		{
#if MENUWND_OBSERVER
			ContextMenuParam param;
			param.hWnd = m_pManager->GetPaintWindow();
			param.wParam = 2;
			CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
			m_pOwner->SelectItem(GetIndex(), true);
#endif
		}

		return;
	}

    ListContainerElement::DoEvent(event);
}

CMenuWnd* MenuElement::GetMenuWnd()
{
	return m_pWindow;
}

void MenuElement::CreateMenuWnd()
{
	if( m_pWindow ) return;

	m_pWindow = new CMenuWnd();
	ASSERT(m_pWindow);
#if MENUWND_OBSERVER
	ContextMenuParam param;
	param.hWnd = m_pManager->GetPaintWindow();
	param.wParam = 2;
	CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
#endif
	m_pWindow->Init(static_cast<MenuElement*>(this), _T(""), _T(""), CDuiPoint(), NULL);
}

void MenuElement::SetLineType()
{
	m_bDrawLine = true;
	if (GetFixedHeight() == 0 || GetFixedHeight() == ITEM_DEFAULT_HEIGHT )
		SetFixedHeight(DEFAULT_LINE_HEIGHT);

	SetMouseChildEnabled(false);
	SetMouseEnabled(false);
	SetEnabled(false);
}

void MenuElement::SetLineColor(DWORD color)
{
	m_dwLineColor = color;
}

DWORD MenuElement::GetLineColor() const
{
	return m_dwLineColor;
}
void MenuElement::SetLinePadding(RECT rcPadding)
{
	m_rcLinePadding = rcPadding;
}

RECT MenuElement::GetLinePadding() const
{
	return m_rcLinePadding;
}

void MenuElement::SetIcon(LPCTSTR strIcon)
{
	if ( strIcon != _T("") )
		m_strIcon = strIcon;
}

void MenuElement::SetIconSize(LONG cx, LONG cy)
{
	m_szIconSize.cx = cx;
	m_szIconSize.cy = cy;
}

void MenuElement::SetChecked(bool bCheck/* = true*/)
{
#if MENUWND_OBSERVER
	if (!m_bCheckItem || CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo() == NULL )
		return;
	std::map<String,bool>::iterator it = CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->find(GetName());
	if (it == CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->end())
		CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->insert(std::map<String,bool>::value_type(GetName(),bCheck));
	else
		it->second = bCheck;
#endif
}

bool MenuElement::GetChecked() const
{
#if MENUWND_OBSERVER
	if (!m_bCheckItem || CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo() == NULL || CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->size() == 0)
		return false;

	std::map<String,bool>::iterator it = CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->find(GetName());
	if (it != CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->end())
	{
		return it->second;
	}
#endif
	return false;

}

void MenuElement::SetCheckItem(bool bCheckItem/* = false*/)
{
	m_bCheckItem = bCheckItem;
}

bool MenuElement::GetCheckItem() const
{
	return m_bCheckItem;
}

void MenuElement::SetShowExplandIcon(bool bShow)
{
	m_bShowExplandIcon = bShow;
}

void MenuElement::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("icon")) == 0){
		SetIcon(pstrValue);
	}
	else if( _tcscmp(pstrName, _T("iconsize")) == 0 ) {
		LPTSTR pstr = NULL;
		LONG cx = 0, cy = 0;
		cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);   
		SetIconSize(cx, cy);
	}
	else if( _tcscmp(pstrName, _T("checkitem")) == 0 ) {		
		SetCheckItem(_tcscmp(pstrValue, _T("true")) == 0 ? true : false);		
	}
	else if( _tcscmp(pstrName, _T("ischeck")) == 0 ) {		
#if MENUWND_OBSERVER
		if (CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo() != NULL && CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->find(GetName()) == CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->end())
		{
			SetChecked(_tcscmp(pstrValue, _T("true")) == 0 ? true : false);
		}	
#endif
	}	
	else if (_tcscmp(pstrName, _T("expland")) == 0) {
		SetShowExplandIcon(_tcscmp(pstrValue, _T("true")) == 0 ? true : false);
	}
	else if( _tcscmp(pstrName, _T("linetype")) == 0){
		if (_tcscmp(pstrValue, _T("true")) == 0)
			SetLineType();
	}
	else if( _tcscmp(pstrName, _T("linecolor")) == 0){
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetLineColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("linepadding")) == 0 ) {
		RECT rcPadding = { 0 };
		LPTSTR pstr = NULL;
		rcPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetLinePadding(rcPadding);
	}
	/*else if	( _tcscmp(pstrName, _T("height")) == 0){
		SetFixedHeight(_ttoi(pstrValue));
	}*/
	else
		ListContainerElement::SetAttribute(pstrName, pstrValue);
}

} // namespace dui

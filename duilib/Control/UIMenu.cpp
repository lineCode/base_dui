#include "StdAfx.h"

#include "UIMenu.h"

namespace dui {

/////////////////////////////////////////////////////////////////////////////////////
//
#if !MENU_EQUAL_LIST
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
#endif
/////////////////////////////////////////////////////////////////////////////////////
//

CMenuWnd::CMenuWnd():
m_pOwner(NULL),
m_pLayout(NULL),
//m_xml(_T("")),
m_pParentManager(NULL)
{
	m_dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;
}

CMenuWnd::~CMenuWnd()
{

}

String CMenuWnd::GetSkinFolder()
{
	return m_folder;
}

String CMenuWnd::GetSkinFile()
{
	return m_xmlfile;
}

LPCTSTR CMenuWnd::GetWindowClassName(void) const
{
	return _T("DuiMenuWnd");
}

void CMenuWnd::Init(MenuElement* pOwner, String xml, String folder, POINT point,
					CPaintManager* pMainPaintManager, std::map<String,bool>* pMenuCheckInfo/* = NULL*/,
					DWORD dwAlignment/* = eMenuAlignment_Left | eMenuAlignment_Top*/, bool isLayeredWindow/* = true*/)
{
	m_BasedPoint = point;
    m_pOwner = pOwner;
    m_pLayout = NULL;
	m_xmlfile = xml;
	m_folder = folder;
	m_dwAlignment = dwAlignment;
	m_pParentManager = pMainPaintManager;
	m_bLayeredWindow = isLayeredWindow;

	wprintf(L"CMenuWnd::Init ParentManager name:%s\n", m_pParentManager->GetName());	//²Ëµ¥ÍË³öÓÐÊ±ºò»á±ÀÀ£

	if (m_bLayeredWindow)
	{
		m_PaintManager.GetShadow()->ShowShadow(true);
		m_PaintManager.GetShadow()->SetImage(_T("../public/bk/bk_shadow.png"));
		m_PaintManager.GetShadow()->SetSize(14);
		m_PaintManager.GetShadow()->SetShadowCorner({ 14, 14, 14, 14 });
	}
#if 1	//mod by djj 20171121
	//when destroy parent wnd using clicking this menu item, the parent wnd destroys its child wnds, which include this menu wnd,and then throw exception;
	//so set parent wnd handle NULL;
	Create(NULL, L"MainTrayMenu", WS_POPUP , 0, CDuiRect(0,0,152,80));	
#else
	Create((m_pOwner == NULL) ? pMainPaintManager->GetPaintWindow() : m_pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP , WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CDuiRect());
#endif
}

void CMenuWnd::Notify(TEvent& msg)
{
	if (msg.Type == UIEVENT_ITEMCLICK)
	{
		if (m_pParentManager)
		{
			m_pParentManager->SendNotify(msg, false);
		}
		Close();
	}
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

Menu* CMenuWnd::GetMenuUI()
{
	return static_cast<Menu*>(m_PaintManager.GetRoot());
}

void CMenuWnd::Show()
{
	assert(m_hWnd != NULL);
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	CDuiRect monitor_rect = oMonitor.rcMonitor;
	CDuiSize szInit = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
	szInit = m_PaintManager.GetRoot()->EstimateSize(szInit);
#if 0
	if (m_bLayeredWindow)
	{
		assert(m_PaintManager.GetShadow());
		CShadowUI *shadow = m_PaintManager.GetShadow();
		szInit.cx += shadow->GetShadowCorner().left /*+ shadow->GetShadowCorner().right*/;
		szInit.cy += shadow->GetShadowCorner().top /*+ shadow->GetShadowCorner().bottom*/;
	}
#endif
	if (m_dwAlignment & eMenuAlignment_Bottom)
	{
		if (m_BasedPoint.y + szInit.cy > monitor_rect.bottom)
		{
			m_BasedPoint.y -= szInit.cy;
		}
	}
	else if (m_dwAlignment & eMenuAlignment_Top)
	{
		if (m_BasedPoint.y - szInit.cy >= monitor_rect.top)
		{
			m_BasedPoint.y -= szInit.cy;
		}
	}
	else
	{
		ASSERT(FALSE);
	}
	CDuiRect rc;
	rc.left = m_BasedPoint.x;
	rc.top = m_BasedPoint.y;
	if (rc.top < monitor_rect.top)
	{
		rc.top = monitor_rect.top;
	}

	//ÅÐ¶ÏÊÇ·ñ³¬³öÆÁÄ»
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
	Close();
	return 0;
}
//LRESULT CMenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	SIZE szRoundCorner = m_PaintManager.GetRoundCorner();
//	if( !::IsIconic(*this) ) {
//		CDuiRect rcWnd;
//		::GetWindowRect(*this, &rcWnd);
//		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
//		rcWnd.right++; rcWnd.bottom++;
//		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
//		::SetWindowRgn(*this, hRgn, TRUE);
//		::DeleteObject(hRgn);
//	}
//	bHandled = FALSE;
//	return 0;
//}

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

    if( m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
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
			TCHAR cImage[526] = {};
			wsprintf(cImage, _T("file='%s' dest='%d,%d,%d,%d'"), m_strIcon.c_str(), 
				(ITEM_DEFAULT_ICON_WIDTH - m_szIconSize.cx)/2,
				(m_cxyFixed.cy - m_szIconSize.cy)/2,
				(ITEM_DEFAULT_ICON_WIDTH - m_szIconSize.cx)/2 + m_szIconSize.cx,
				(m_cxyFixed.cy - m_szIconSize.cy)/2 + m_szIconSize.cy);
			TDrawInfo drawinfo;
			drawinfo.sDrawString = cImage;
			drawinfo.rcDestOffset = rcItem;

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

		TCHAR cImage[526] = {};
		wsprintf(cImage, _T("file='%s' dest='%d,%d,%d,%d'"), strExplandIcon.c_str(),
			m_cxyFixed.cx - ITEM_DEFAULT_EXPLAND_ICON_WIDTH + (ITEM_DEFAULT_EXPLAND_ICON_WIDTH - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / 2,
			(m_cxyFixed.cy - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / 2,
			m_cxyFixed.cx - ITEM_DEFAULT_EXPLAND_ICON_WIDTH + (ITEM_DEFAULT_EXPLAND_ICON_WIDTH - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / 2 + ITEM_DEFAULT_EXPLAND_ICON_SIZE,
			(m_cxyFixed.cy - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / 2 + ITEM_DEFAULT_EXPLAND_ICON_SIZE);
		TDrawInfo drawinfo;
		drawinfo.sDrawString = cImage;
		drawinfo.rcDestOffset = rcItem;

		CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, drawinfo);
	}
}


void MenuElement::DrawItemText(HDC hDC, const RECT& rcItem)
{
#if 1
	if (m_sText.empty()) return;

    if( m_pOwner == NULL ) return;
    ListViewInfo* pInfo = m_pOwner->GetListInfo();
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
#endif
}

#if 1
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
		ListViewInfo* pInfo = m_pOwner->GetListInfo();

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
	
	if (m_pManager) return m_pManager->GetDPIObj()->Scale(cXY);
	return cXY;
}
#endif
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
}

bool MenuElement::GetChecked() const
{
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

#include "stdafx.h"

namespace dui {

ListElement::ListElement() :
m_iIndex(-1),
m_pOwner(NULL),
m_bSelected(false),
m_uButtonState(0)
{
}

LPCTSTR ListElement::GetClass() const
{
	return DUI_CTR_LISTELEMENT;
}

UINT ListElement::GetControlFlags() const
{
	return UIFLAG_WANTRETURN;
}

LPVOID ListElement::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, DUI_CTR_ILISTITEM) == 0) return static_cast<IListItem*>(this);
	if (_tcscmp(pstrName, DUI_CTR_LISTELEMENT) == 0) return static_cast<ListElement*>(this);
	return Control::GetInterface(pstrName);
}

IList* ListElement::GetOwner()
{
	return m_pOwner;
}

void ListElement::SetOwner(Control *pOwner)
{
	if (pOwner != NULL) m_pOwner = static_cast<IList*>(pOwner->GetInterface(DUI_CTR_ILIST));
}

void ListElement::SetVisible(bool bVisible)
{
	Control::SetVisible(bVisible);
	if (!IsVisible() && m_bSelected)
	{
		m_bSelected = false;
		if (m_pOwner != NULL) m_pOwner->SelectItem(-1);
	}
}

void ListElement::SetEnabled(bool bEnable)
{
	Control::SetEnabled(bEnable);
	if (!IsEnabled()) {
		m_uButtonState = 0;
	}
}

int ListElement::GetIndex() const
{
	return m_iIndex;
}

void ListElement::SetIndex(int iIndex)
{
	m_iIndex = iIndex;
}

void ListElement::Invalidate()
{
	if (!IsVisible()) return;

	if (GetParent()) {
		ScrollBox* pParentContainer = static_cast<ScrollBox*>(GetParent()->GetInterface(DUI_CTR_SCROLLBOX));
		if (pParentContainer) {
			RECT rc = pParentContainer->GetPos();
			RECT rcPadding = pParentContainer->GetPadding();
			rc.left += rcPadding.left;
			rc.top += rcPadding.top;
			rc.right -= rcPadding.right;
			rc.bottom -= rcPadding.bottom;
			ScrollBar* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
			if (pVerticalScrollBar && pVerticalScrollBar->IsVisible() && !pParentContainer->GetScrollBarFloat()) rc.right -= pVerticalScrollBar->GetFixedWidth();
			ScrollBar* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
			if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() && !pParentContainer->GetScrollBarFloat()) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

			RECT invalidateRc = m_rcItem;
			if (!::IntersectRect(&invalidateRc, &m_rcItem, &rc))
			{
				return;
			}

			Control* pParent = GetParent();
			RECT rcTemp;
			RECT rcParent;
			while (pParent = pParent->GetParent())
			{
				rcTemp = invalidateRc;
				rcParent = pParent->GetPos();
				if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent))
				{
					return;
				}
			}

			if (m_pManager != NULL) m_pManager->Invalidate(invalidateRc);
		}
		else {
			Control::Invalidate();
		}
	}
	else {
		Control::Invalidate();
	}
}

bool ListElement::Activate()
{
	if (!Control::Activate()) return false;
	if (m_pManager != NULL) m_pManager->SendNotify(this, UIEVENT_ITEMCLICK);
	return true;
}

bool ListElement::IsSelected() const
{
	return m_bSelected;
}

bool ListElement::Select(bool bSelect, bool bTriggerEvent)
{
	if (!IsEnabled()) return false;
	if (bSelect == m_bSelected) return true;
	m_bSelected = bSelect;
	if (bSelect && m_pOwner != NULL) m_pOwner->SelectItem(m_iIndex, false,  bTriggerEvent);
	Invalidate();

	return true;
}

void ListElement::DoEvent(Event& event)
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
		if (m_pOwner != NULL) m_pOwner->DoEvent(event);
		else Control::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_BUTTONDOWN)
	{
		if (IsEnabled()) {
			Activate();
			Select();
			Invalidate();
		}
		return;
	}
	else if (event.Type == UIEVENT_BUTTONUP)
	{
		return;
	}
	else if (event.Type == UIEVENT_KEYDOWN)
	{
		if (IsKeyboardEnabled() && IsEnabled()) {
			if (event.chKey == VK_RETURN) {
				Activate();
				Invalidate();
				return;
			}
		}
	}
	else if (event.Type == UIEVENT_MOUSEMOVE)
	{
		return;
	}
	else if (event.Type == UIEVENT_MOUSEENTER)
	{
		if (::PtInRect(&m_rcItem, event.ptMouse)) {
			if (IsEnabled()) {
				if ((m_uButtonState & UISTATE_HOT) == 0) {
					m_uButtonState |= UISTATE_HOT;
					Invalidate();
				}
			}
		}
	}
	else if (event.Type == UIEVENT_MOUSELEAVE)
	{
		if (!::PtInRect(&m_rcItem, event.ptMouse)) {
			if (IsEnabled()) {
				if ((m_uButtonState & UISTATE_HOT) != 0) {
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

	// An important twist: The list-item will send the event not to its immediate
	// parent but to the "attached" list. A list may actually embed several components
	// in its path to the item, but key-presses etc. needs to go to the actual list.
	if (m_pOwner != NULL) m_pOwner->DoEvent(event); else Control::DoEvent(event);
}

void ListElement::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("selected")) == 0) Select();
	else if (_tcscmp(pstrName, _T("hotbkcolor")) == 0)
	{
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetHotBkColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("selectedbkcolor")) == 0)
	{
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetPushedBkColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("disabledbkcolor")) == 0)
	{
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetDisabledBkColor(clrColor);
	}
	else __super::SetAttribute(pstrName, pstrValue);
}

void ListElement::PaintBkColor(HDC hDC)
{
	DWORD color = GetBkColor();
	if (!IsEnabled()) {
		color = m_dwDisabledBkColor;
	}
	else if (IsSelected()){
		color = m_dwPushedBkColor;
	}
	else if ((m_uButtonState & UISTATE_HOT) != 0) {
		color = m_dwHotBkColor;
	}

	if (color != 0) {
		Render::DrawColor(hDC, m_rcItem, GetAdjustColor(color));
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
//
ListContainerElement::ListContainerElement() :
m_iIndex(-1),
m_pOwner(NULL),
m_bSelected(false),
m_uButtonState(0)
{
}

LPCTSTR ListContainerElement::GetClass() const
{
	return DUI_CTR_LISTCONTAINERELEMENT;
}

UINT ListContainerElement::GetControlFlags() const
{
	return UIFLAG_WANTRETURN;
}

LPVOID ListContainerElement::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, DUI_CTR_ILISTITEM) == 0) return static_cast<IListItem*>(this);
	if (_tcscmp(pstrName, DUI_CTR_LISTCONTAINERELEMENT) == 0) return static_cast<ListContainerElement*>(this);
	return __super::GetInterface(pstrName);
}

IList* ListContainerElement::GetOwner()
{
	return m_pOwner;
}

void ListContainerElement::SetOwner(Control* pOwner)
{
	if (pOwner != NULL) m_pOwner = static_cast<IListView*>(pOwner->GetInterface(DUI_CTR_ILIST));
}

void ListContainerElement::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);
	if (!IsVisible() && m_bSelected)
	{
		m_bSelected = false;
		if (m_pOwner != NULL) m_pOwner->SelectItem(-1);
	}
}

void ListContainerElement::SetEnabled(bool bEnable)
{
	Control::SetEnabled(bEnable);
	if (!IsEnabled()) {
		m_uButtonState = 0;
	}
}

int ListContainerElement::GetIndex() const
{
	return m_iIndex;
}

void ListContainerElement::SetIndex(int iIndex)
{
	m_iIndex = iIndex;
}

void ListContainerElement::Invalidate()
{
	if (!IsVisible()) return;

	if (GetParent()) {
		ScrollBox* pParentContainer = static_cast<ScrollBox*>(GetParent()->GetInterface(DUI_CTR_SCROLLBOX));
		if (pParentContainer) {
			RECT rc = pParentContainer->GetPos();
			RECT rcPadding = pParentContainer->GetPadding();
			rc.left += rcPadding.left;
			rc.top += rcPadding.top;
			rc.right -= rcPadding.right;
			rc.bottom -= rcPadding.bottom;
			ScrollBar* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
			if (pVerticalScrollBar && pVerticalScrollBar->IsVisible() && !pParentContainer->GetScrollBarFloat()) rc.right -= pVerticalScrollBar->GetFixedWidth();
			ScrollBar* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
			if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() && !pParentContainer->GetScrollBarFloat()) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

			RECT invalidateRc = m_rcItem;
			if (!::IntersectRect(&invalidateRc, &m_rcItem, &rc))
			{
				return;
			}

			Control* pParent = GetParent();
			RECT rcTemp;
			RECT rcParent;
			while (pParent = pParent->GetParent())
			{
				rcTemp = invalidateRc;
				rcParent = pParent->GetPos();
				if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent))
				{
					return;
				}
			}

			if (m_pManager != NULL) m_pManager->Invalidate(invalidateRc);
		}
		else {
			__super::Invalidate();
		}
	}
	else {
		__super::Invalidate();
	}
}

bool ListContainerElement::Activate()
{
	printf("ListContainerElement::Activate()\n");
	if (!__super::Activate()) return false;

	if (m_event_map.find(UIEVENT_ITEMCLICK) != m_event_map.cend()){
		Event event;
		event.Type = UIEVENT_ITEMCLICK;
		event.pSender = this;
		if (!m_event_map.find(UIEVENT_ITEMCLICK)->second(&event)){
			return false;
		}
	}

	if (m_pManager != NULL) m_pManager->SendNotify(this, UIEVENT_ITEMCLICK);
	return true;
}

bool ListContainerElement::IsSelected() const
{
	return m_bSelected;
}

bool ListContainerElement::Select(bool bSelect, bool bTriggerEvent)
{
	if (!IsEnabled()) return false;
	if (bSelect == m_bSelected) return true;
	m_bSelected = bSelect;
	if (bSelect && m_pOwner != NULL) m_pOwner->SelectItem(m_iIndex, bTriggerEvent, bTriggerEvent);
	Invalidate();

	return true;
}

void ListContainerElement::DoEvent(Event& event)
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
		if (m_pOwner != NULL) m_pOwner->DoEvent(event);
		else __super::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_BUTTONDOWN)
	{
		if (IsEnabled()) {
			Activate();
			Select();
			Invalidate();
		}
		return;
	}
	else if (event.Type == UIEVENT_BUTTONUP)
	{
		return;
	}
	else if (event.Type == UIEVENT_MOUSEMOVE)
	{
		return;
	}
	else if (event.Type == UIEVENT_MOUSEENTER)
	{
		if (::PtInRect(&m_rcItem, event.ptMouse)) {
			if (IsEnabled()) {
				if ((m_uButtonState & UISTATE_HOT) == 0) {
					m_uButtonState |= UISTATE_HOT;
					Invalidate();
				}
			}
		}
	}
	else if (event.Type == UIEVENT_MOUSELEAVE)
	{
		if (!::PtInRect(&m_rcItem, event.ptMouse)) {
			if (IsEnabled()) {
				if ((m_uButtonState & UISTATE_HOT) != 0) {
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

	// An important twist: The list-item will send the event not to its immediate
	// parent but to the "attached" list. A list may actually embed several components
	// in its path to the item, but key-presses etc. needs to go to the actual list.
	if (m_pOwner != NULL) m_pOwner->DoEvent(event); else Control::DoEvent(event);
}

void ListContainerElement::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("selected")) == 0) Select();
	else if (_tcscmp(pstrName, _T("hotbkcolor")) == 0)
	{
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetHotBkColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("selectedbkcolor")) == 0)
	{
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetPushedBkColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("disabledbkcolor")) == 0)
	{
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetDisabledBkColor(clrColor);
	}
	else __super::SetAttribute(pstrName, pstrValue);
}

void ListContainerElement::PaintBkColor(HDC hDC)
{
	DWORD color = GetBkColor();
	if (!IsEnabled()) {
		color = m_dwDisabledBkColor;
	}
	else if (IsSelected()){
		color = m_dwPushedBkColor;
	}
	else if ((m_uButtonState & UISTATE_HOT) != 0) {
		color = m_dwHotBkColor;
	}

	if (color != 0) {
		Render::DrawColor(hDC, m_rcItem, GetAdjustColor(color));
	}
}

//------------------------------------------------------------------------------------------------------
//
ListHeaderItem::ListHeaderItem() : m_bDragable(true), m_uButtonState(0), m_iSepWidth(4),
m_uTextStyle(DT_CENTER | DT_VCENTER | DT_SINGLELINE), m_dwTextColor(0), m_dwSepColor(0),
m_iFont(-1), m_bShowHtml(false)
{
	SetTextPadding(DuiRect(2, 0, 2, 0));
	ptLastMouse.x = ptLastMouse.y = 0;
	SetMinWidth(16);
}

LPCTSTR ListHeaderItem::GetClass() const
{
	return DUI_CTR_LISTHEADERITEM;
}

LPVOID ListHeaderItem::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, DUI_CTR_LISTHEADERITEM) == 0) return this;
	return Control::GetInterface(pstrName);
}

UINT ListHeaderItem::GetControlFlags() const
{
	if (IsEnabled() && m_iSepWidth != 0) return UIFLAG_SETCURSOR;
	else return 0;
}

void ListHeaderItem::SetEnabled(bool bEnable)
{
	Control::SetEnabled(bEnable);
	if (!IsEnabled()) {
		m_uButtonState = 0;
	}
}

bool ListHeaderItem::IsDragable() const
{
	return m_bDragable;
}

void ListHeaderItem::SetDragable(bool bDragable)
{
	m_bDragable = bDragable;
	if (!m_bDragable) m_uButtonState &= ~UISTATE_CAPTURED;
}

DWORD ListHeaderItem::GetSepWidth() const
{
	return m_iSepWidth;
}

void ListHeaderItem::SetSepWidth(int iWidth)
{
	m_iSepWidth = iWidth;
}

DWORD ListHeaderItem::GetTextStyle() const
{
	return m_uTextStyle;
}

void ListHeaderItem::SetTextStyle(UINT uStyle)
{
	m_uTextStyle = uStyle;
	Invalidate();
}

DWORD ListHeaderItem::GetTextColor() const
{
	return m_dwTextColor;
}


void ListHeaderItem::SetTextColor(DWORD dwTextColor)
{
	m_dwTextColor = dwTextColor;
	Invalidate();
}

DWORD ListHeaderItem::GetSepColor() const
{
	return m_dwSepColor;
}

void ListHeaderItem::SetSepColor(DWORD dwSepColor)
{
	m_dwSepColor = dwSepColor;
	Invalidate();
}

RECT ListHeaderItem::GetTextPadding() const
{
	return m_rcTextPadding;
}

void ListHeaderItem::SetTextPadding(RECT rc)
{
	m_rcTextPadding = rc;
	Invalidate();
}

void ListHeaderItem::SetFont(int index)
{
	m_iFont = index;
}

bool ListHeaderItem::IsShowHtml()
{
	return m_bShowHtml;
}

void ListHeaderItem::SetShowHtml(bool bShowHtml)
{
	if (m_bShowHtml == bShowHtml) return;

	m_bShowHtml = bShowHtml;
	Invalidate();
}

LPCTSTR ListHeaderItem::GetNormalImage() const
{
	return m_diNormal.sDrawString.c_str();
}

void ListHeaderItem::SetNormalImage(LPCTSTR pStrImage)
{
	if (m_diNormal.sDrawString == pStrImage && m_diNormal.pImageInfo != NULL) return;
	m_diNormal.Clear();
	m_diNormal.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ListHeaderItem::GetHotImage() const
{
	return m_diHot.sDrawString.c_str();
}

void ListHeaderItem::SetHotImage(LPCTSTR pStrImage)
{
	if (m_diHot.sDrawString == pStrImage && m_diHot.pImageInfo != NULL) return;
	m_diHot.Clear();
	m_diHot.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ListHeaderItem::GetPushedImage() const
{
	return m_diPushed.sDrawString.c_str();
}

void ListHeaderItem::SetPushedImage(LPCTSTR pStrImage)
{
	if (m_diPushed.sDrawString == pStrImage && m_diPushed.pImageInfo != NULL) return;
	m_diPushed.Clear();
	m_diPushed.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ListHeaderItem::GetFocusedImage() const
{
	return m_diFocused.sDrawString.c_str();
}

void ListHeaderItem::SetFocusedImage(LPCTSTR pStrImage)
{
	if (m_diFocused.sDrawString == pStrImage && m_diFocused.pImageInfo != NULL) return;
	m_diFocused.Clear();
	m_diFocused.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR ListHeaderItem::GetSepImage() const
{
	return m_diSep.sDrawString.c_str();
}

void ListHeaderItem::SetSepImage(LPCTSTR pStrImage)
{
	if (m_diSep.sDrawString == pStrImage && m_diSep.pImageInfo != NULL) return;
	m_diSep.Clear();
	m_diSep.sDrawString = pStrImage;
	Invalidate();
}

void ListHeaderItem::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("dragable")) == 0) SetDragable(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcscmp(pstrName, _T("texthalign")) == 0) {
		if (_tcsstr(pstrValue, _T("left")) != NULL) {
			m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
			m_uTextStyle |= DT_LEFT;
		}
		if (_tcsstr(pstrValue, _T("center")) != NULL) {
			m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
			m_uTextStyle |= DT_CENTER;
		}
		if (_tcsstr(pstrValue, _T("right")) != NULL) {
			m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
			m_uTextStyle |= DT_RIGHT;
		}
	}
	else if (_tcscmp(pstrName, _T("textvalign")) == 0)
	{
		if (_tcsstr(pstrValue, _T("top")) != NULL) {
			m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
			m_uTextStyle |= DT_TOP;
		}
		if (_tcsstr(pstrValue, _T("vcenter")) != NULL) {
			m_uTextStyle &= ~(DT_TOP | DT_BOTTOM);
			m_uTextStyle |= DT_VCENTER;
		}
		if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
			m_uTextStyle &= ~(DT_TOP | DT_VCENTER);
			m_uTextStyle |= DT_BOTTOM;
		}
	}
	else if (_tcscmp(pstrName, _T("endellipsis")) == 0) {
		if (_tcscmp(pstrValue, _T("true")) == 0) m_uTextStyle |= DT_END_ELLIPSIS;
		else m_uTextStyle &= ~DT_END_ELLIPSIS;
	}
	else if (_tcscmp(pstrName, _T("multiline")) == 0) {
		if (_tcscmp(pstrValue, _T("true")) == 0) {
			m_uTextStyle &= ~DT_SINGLELINE;
			m_uTextStyle |= DT_WORDBREAK;
		}
		else m_uTextStyle |= DT_SINGLELINE;
	}
	else if (_tcscmp(pstrName, _T("font")) == 0) SetFont(_ttoi(pstrValue));
	else if (_tcscmp(pstrName, _T("textcolor")) == 0 || _tcscmp(pstrName, _T("normaltextcolor")) == 0) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetTextColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("textpadding")) == 0) {
		RECT rcTextPadding = { 0 };
		LPTSTR pstr = NULL;
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
		SetTextPadding(rcTextPadding);
	}
	else if (_tcscmp(pstrName, _T("showhtml")) == 0) SetShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcscmp(pstrName, _T("normalimage")) == 0) SetNormalImage(pstrValue);
	else if (_tcscmp(pstrName, _T("hotimage")) == 0) SetHotImage(pstrValue);
	else if (_tcscmp(pstrName, _T("pushedimage")) == 0) SetPushedImage(pstrValue);
	else if (_tcscmp(pstrName, _T("focusedimage")) == 0) SetFocusedImage(pstrValue);
	else if (_tcscmp(pstrName, _T("sepwidth")) == 0) SetSepWidth(_ttoi(pstrValue));
	else if (_tcscmp(pstrName, _T("sepcolor")) == 0) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetSepColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("sepimage")) == 0) SetSepImage(pstrValue);
	else Control::SetAttribute(pstrName, pstrValue);
}

void ListHeaderItem::DoEvent(Event& event)
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
		if (m_pParent != NULL) m_pParent->DoEvent(event);
		else Control::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_SETFOCUS)
	{
		Invalidate();
	}
	if (event.Type == UIEVENT_KILLFOCUS)
	{
		Invalidate();
	}
	if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
	{
		if (!IsEnabled()) return;
		RECT rcSeparator = GetThumbRect();
		if (::PtInRect(&rcSeparator, event.ptMouse)) {
			if (m_bDragable) {
				m_uButtonState |= UISTATE_CAPTURED;
				ptLastMouse = event.ptMouse;
			}
		}
		else {
			m_uButtonState |= UISTATE_PUSHED;
			m_pManager->SendNotify(this, UIEVENT_LISTHEADERCLICK);
			Invalidate();
		}
		return;
	}
	if (event.Type == UIEVENT_BUTTONUP)
	{
		if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
			m_uButtonState &= ~UISTATE_CAPTURED;
			if (GetParent())
				GetParent()->NeedParentUpdate();
		}
		else if ((m_uButtonState & UISTATE_PUSHED) != 0) {
			m_uButtonState &= ~UISTATE_PUSHED;
			Invalidate();
		}
		return;
	}
	if (event.Type == UIEVENT_MOUSEMOVE)
	{
		if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
			RECT rc = m_rcItem;
			if (m_iSepWidth >= 0) {
				rc.right -= ptLastMouse.x - event.ptMouse.x;
			}
			else {
				rc.left -= ptLastMouse.x - event.ptMouse.x;
			}

			if (rc.right - rc.left > GetMinWidth()) {
				m_FixedSize.cx = rc.right - rc.left;
				ptLastMouse = event.ptMouse;
				if (GetParent())
					GetParent()->NeedParentUpdate();
			}
		}
		return;
	}
	if (event.Type == UIEVENT_SETCURSOR)
	{
		RECT rcSeparator = GetThumbRect();
		if (IsEnabled() && m_bDragable && ::PtInRect(&rcSeparator, event.ptMouse)) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
			return;
		}
	}
	if (event.Type == UIEVENT_MOUSEENTER)
	{
		if (::PtInRect(&m_rcItem, event.ptMouse)) {
			if (IsEnabled()) {
				if ((m_uButtonState & UISTATE_HOT) == 0) {
					m_uButtonState |= UISTATE_HOT;
					Invalidate();
				}
			}
		}
	}
	if (event.Type == UIEVENT_MOUSELEAVE)
	{
		if (!::PtInRect(&m_rcItem, event.ptMouse)) {
			if (IsEnabled()) {
				if ((m_uButtonState & UISTATE_HOT) != 0) {
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

SIZE ListHeaderItem::EstimateSize(SIZE szAvailable)
{
	if (m_FixedSize.cy == 0) return DuiSize(m_FixedSize.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8);
	return Control::EstimateSize(szAvailable);
}

RECT ListHeaderItem::GetThumbRect() const
{
	if (m_iSepWidth >= 0) return DuiRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
	else return DuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
}

void ListHeaderItem::PaintStatusImage(HDC hDC)
{
	if (IsFocused()) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~UISTATE_FOCUSED;

	if ((m_uButtonState & UISTATE_PUSHED) != 0) {
		if (!DrawImage(hDC, m_diPushed))  DrawImage(hDC, m_diNormal);
	}
	else if ((m_uButtonState & UISTATE_HOT) != 0) {
		if (!DrawImage(hDC, m_diHot))  DrawImage(hDC, m_diNormal);
	}
	else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {
		if (!DrawImage(hDC, m_diFocused))  DrawImage(hDC, m_diNormal);
	}
	else {
		DrawImage(hDC, m_diNormal);
	}

	if (m_iSepWidth > 0) {
		RECT rcThumb = GetThumbRect();
		m_diSep.rcDestOffset.left = rcThumb.left - m_rcItem.left;
		m_diSep.rcDestOffset.top = rcThumb.top - m_rcItem.top;
		m_diSep.rcDestOffset.right = rcThumb.right - m_rcItem.left;
		m_diSep.rcDestOffset.bottom = rcThumb.bottom - m_rcItem.top;
		if (!DrawImage(hDC, m_diSep)) {
			if (m_dwSepColor != 0) {
				RECT rcSepLine = { rcThumb.left + m_iSepWidth / 2, rcThumb.top, rcThumb.left + m_iSepWidth / 2, rcThumb.bottom };
				Render::DrawLine(hDC, rcSepLine, m_iSepWidth, GetAdjustColor(m_dwSepColor));
			}
		}
	}
	DrawImage(hDC, m_diFore);
}

void ListHeaderItem::PaintText(HDC hDC)
{
	if (m_dwTextColor == 0) m_dwTextColor = m_pManager->GetDefaultFontColor();

	RECT rcText = m_rcItem;
	rcText.left += m_rcTextPadding.left;
	rcText.top += m_rcTextPadding.top;
	rcText.right -= m_rcTextPadding.right;
	rcText.bottom -= m_rcTextPadding.bottom;

	if (m_sText.empty()) return;
	int nLinks = 0;
	if (m_bShowHtml)
		Render::DrawHtmlText(hDC, m_pManager, rcText, m_sText.c_str(), m_dwTextColor, \
		NULL, NULL, nLinks, m_iFont, m_uTextStyle);
	else
		Render::DrawText(hDC, m_pManager, rcText, m_sText.c_str(), m_dwTextColor, \
		m_iFont, m_uTextStyle);
}


} // namespace dui

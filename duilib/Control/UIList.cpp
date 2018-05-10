#include "StdAfx.h"

namespace dui {



List::List() :/* m_pCallback(NULL),*/ m_bScrollSelect(false), m_iCurSel(-1)/*, m_iExpandedItem(-1)*/
{
    //m_ListInfo.nColumns = 0;
    //m_ListInfo.uFixedHeight = 0;
    //m_ListInfo.nFont = -1;
    //m_ListInfo.uTextStyle = DT_VCENTER | DT_SINGLELINE; // m_uTextStyle(DT_VCENTER | DT_END_ELLIPSIS)
    //m_ListInfo.dwTextColor = 0xFF000000;
    //m_ListInfo.dwBkColor = 0;
    //m_ListInfo.bAlternateBk = false;
    //m_ListInfo.dwSelectedTextColor = 0xFF000000;
    //m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
    //m_ListInfo.dwHotTextColor = 0xFF000000;
    //m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
    //m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
    //m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
    //m_ListInfo.iHLineSize = 0;
    //m_ListInfo.dwHLineColor = 0xFF3C3C3C;
    //m_ListInfo.iVLineSize = 0;
    //m_ListInfo.dwVLineColor = 0xFF3C3C3C;
    //m_ListInfo.bShowHtml = false;
    //m_ListInfo.bMultiExpandable = false;
    //::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    //::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
}

LPCTSTR List::GetClass() const
{
    return DUI_CTR_LIST;
}

UINT List::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

LPVOID List::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_LIST) == 0 ) return static_cast<List*>(this);
    if( _tcscmp(pstrName, DUI_CTR_ILIST) == 0 ) return static_cast<IList*>(this);
	return __super::GetInterface(pstrName);
}

//Control* List::GetItemAt(int iIndex) const
//{
//    return m_pList->GetItemAt(iIndex);
//}
//
//int List::GetItemIndex(Control* pControl) const
//{
//	if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) return __super::GetItemIndex(pControl);
//    // We also need to recognize header sub-items
//    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->GetItemIndex(pControl);
//
//    return m_pList->GetItemIndex(pControl);
//}

bool List::SetItemIndex(Control* pControl, int iIndex)
{
    int iOrginIndex = GetItemIndex(pControl);
    if( iOrginIndex == -1 ) return false;
    if( iOrginIndex == iIndex ) return true;

    IListItem* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItem*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !SetItemIndex(pControl, iIndex) ) return false;
    int iMinIndex = min(iOrginIndex, iIndex);
    int iMaxIndex = max(iOrginIndex, iIndex);
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

bool List::SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex)
{
    if (pStartControl == NULL || iCount < 0 || iNewStartIndex < 0) return false;
	if (pStartControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) return __super::SetMultiItemIndex(pStartControl, iCount, iNewStartIndex);
    // We also need to recognize header sub-items
    if( _tcsstr(pStartControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return SetMultiItemIndex(pStartControl, iCount, iNewStartIndex);

    int iStartIndex = GetItemIndex(pStartControl);
    if (iStartIndex == iNewStartIndex) return true;
    if (iStartIndex + iCount > GetCount()) return false;
    if (iNewStartIndex + iCount > GetCount()) return false;

    IListItem* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItem*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !SetMultiItemIndex(pStartControl, iCount, iNewStartIndex) ) return false;
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

//int List::GetCount() const
//{
//    return m_pList->GetCount();
//}

bool List::Add(Control* pControl)
{
    // Override the Add() method so we can add items specifically to
    // the intended widgets. Headers are assumed to be
    // answer the correct interface so we can add multiple list headers.
  
    // The list items should know about us
	IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
	if( pListItem != NULL ) {
		pListItem->SetOwner(this);
		pListItem->SetIndex(GetCount());
    }
	else
	{
		assert(0 && "List::Add item is not listitem");
		return false;
	}
    return __super::Add(pControl);
}

bool List::AddAt(Control* pControl, int iIndex)
{
    // Override the AddAt() method so we can add items specifically to
    // the intended widgets. Headers and are assumed to be
    // answer the correct interface so we can add multiple list headers.
    
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

bool List::Remove(Control* pControl, bool bDoNotDestroy)
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

bool List::RemoveAt(int iIndex, bool bDoNotDestroy)
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

void List::RemoveAll()
{
    m_iCurSel = -1;
    //m_iExpandedItem = -1;
   __super::RemoveAll();
}

void List::SetPos(RECT rc, bool bNeedInvalidate)
{
	__super::SetPos(rc, bNeedInvalidate);

	/*if( m_pHeader == NULL ) return;*/

	rc = m_rcItem;
	RECT rcPadding = GetPadding();
	rc.left += rcPadding.left;
	rc.top += rcPadding.top;
	rc.right -= rcPadding.right;
	rc.bottom -= rcPadding.bottom;

	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
		rc.top -= m_pVerticalScrollBar->GetScrollPos();
		rc.bottom -= m_pVerticalScrollBar->GetScrollPos();
		rc.bottom += m_pVerticalScrollBar->GetScrollRange();
		rc.right -= m_pVerticalScrollBar->GetFixedWidth();
	}
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
		rc.left -= m_pHorizontalScrollBar->GetScrollPos();
		rc.right -= m_pHorizontalScrollBar->GetScrollPos();
		rc.right += m_pHorizontalScrollBar->GetScrollRange();
		rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
	}

	//m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
	//assert(m_ListInfo.nColumns == 1 && "List::SetPos list column num must be 1");
#if 0
	int iOffset = GetScrollPos().cx;
	for( int i = 0; i < m_ListInfo.nColumns; i++ ) {
		Control* pControl = static_cast<Control*>(m_pHeader->GetItemAt(i));
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) continue;

		RECT rcPos = pControl->GetPos();
		if( iOffset > 0 ) {
			rcPos.left -= iOffset;
			rcPos.right -= iOffset;
			pControl->SetPos(rcPos, false);
		}
		m_ListInfo.rcColumn[i] = pControl->GetPos();
	}
#endif
	__super::SetPos(__super::GetPos(), bNeedInvalidate);
}

//void List::Move(SIZE szOffset, bool bNeedInvalidate)
//{
//	__super::Move(szOffset, bNeedInvalidate);
//	if( !m_pHeader->IsVisible() ) m_pHeader->Move(szOffset, false);
//}

void List::DoEvent(TEvent& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
		else ScrollContainer::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        m_bFocused = true;
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        m_bFocused = false;
        return;
    }

    if( event.Type == UIEVENT_KEYDOWN )
    {
        if (IsKeyboardEnabled() && IsEnabled()) {
            switch( event.chKey ) {
            case VK_UP:
                SelectItem(FindSelectable(m_iCurSel - 1, false), true);
            case VK_DOWN:
                SelectItem(FindSelectable(m_iCurSel + 1, true), true);
            case VK_PRIOR:
                PageUp();
            case VK_NEXT:
                PageDown();
            case VK_HOME:
                SelectItem(FindSelectable(0, false), true);
            case VK_END:
                SelectItem(FindSelectable(GetCount() - 1, true), true);
            case VK_RETURN:
                if( m_iCurSel != -1 ) GetItemAt(m_iCurSel)->Activate();
            }
            return;
        }
    }

    if( event.Type == UIEVENT_SCROLLWHEEL )
    {
        if (IsEnabled()) {
            switch( LOWORD(event.wParam) ) {
            case SB_LINEUP:
                if( m_bScrollSelect ) SelectItem(FindSelectable(m_iCurSel - 1, false), true);
                else LineUp();
                return;
            case SB_LINEDOWN:
                if( m_bScrollSelect ) SelectItem(FindSelectable(m_iCurSel + 1, true), true);
                else LineDown();
                return;
            }
        }
    }

	ScrollContainer::DoEvent(event);
}

bool List::GetScrollSelect()
{
    return m_bScrollSelect;
}

void List::SetScrollSelect(bool bScrollSelect)
{
    m_bScrollSelect = bScrollSelect;
}

int List::GetCurSel() const
{
    return m_iCurSel;
}

bool List::SelectItem(int iIndex, bool bTakeFocus, bool bTriggerEvent)
{
    if( iIndex == m_iCurSel ) return true;

    int iOldSel = m_iCurSel;
    // We should first unselect the currently selected item
    if( m_iCurSel >= 0 ) {
        Control* pControl = GetItemAt(m_iCurSel);
        if( pControl != NULL) {
            IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
            if( pListItem != NULL ) pListItem->Select(false, bTriggerEvent);
        }

        m_iCurSel = -1;
    }
    if( iIndex < 0 ) return false;

    Control* pControl = GetItemAt(iIndex);
    if( pControl == NULL ) return false;
    if( !pControl->IsVisible() ) return false;
    if( !pControl->IsEnabled() ) return false;

    IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem == NULL ) return false;
    m_iCurSel = iIndex;
    if( !pListItem->Select(true, bTriggerEvent) ) {
        m_iCurSel = -1;
        return false;
    }
    EnsureVisible(m_iCurSel);
    if( bTakeFocus ) pControl->SetFocus();
    if( m_pManager != NULL && bTriggerEvent ) {
		m_pManager->SendNotify(this, UIEVENT_ITEMSELECT, m_iCurSel, iOldSel);
    }

    return true;
}

//ListViewInfo* List::GetListInfo()
//{
//    return &m_ListInfo;
//}

//int List::GetChildMargin() const
//{
//    return m_pList->GetChildMargin();
//}
//
//void List::SetChildMargin(int iPadding)
//{
//    m_pList->SetChildMargin(iPadding);
//}

//UINT List::GetItemFixedHeight()
//{
//    return m_ListInfo.uFixedHeight;
//}
//
//void List::SetItemFixedHeight(UINT nHeight)
//{
//    m_ListInfo.uFixedHeight = nHeight;
//    NeedUpdate();
//}
//
//int List::GetItemFont(int index)
//{
//    return m_ListInfo.nFont;
//}
//
//void List::SetItemFont(int index)
//{
//    m_ListInfo.nFont = index;
//    NeedUpdate();
//}
//
//UINT List::GetItemTextStyle()
//{
//    return m_ListInfo.uTextStyle;
//}
//
//void List::SetItemTextStyle(UINT uStyle)
//{
//    m_ListInfo.uTextStyle = uStyle;
//    NeedUpdate();
//}
//
//void List::SetItemTextPadding(RECT rc)
//{
//    m_ListInfo.rcTextPadding = rc;
//    NeedUpdate();
//}
//
//RECT List::GetItemTextPadding() const
//{
//	return m_ListInfo.rcTextPadding;
//}
//
//void List::SetItemTextColor(DWORD dwTextColor)
//{
//    m_ListInfo.dwTextColor = dwTextColor;
//    Invalidate();
//}
//
//void List::SetItemBkColor(DWORD dwBkColor)
//{
//    m_ListInfo.dwBkColor = dwBkColor;
//    Invalidate();
//}
//
//void List::SetItemBkImage(LPCTSTR pStrImage)
//{
//	if( m_ListInfo.diBk.sDrawString == pStrImage && m_ListInfo.diBk.pImageInfo != NULL ) return;
//	m_ListInfo.diBk.Clear();
//	m_ListInfo.diBk.sDrawString = pStrImage;
//	Invalidate();
//}
//
//bool List::IsAlternateBk() const
//{
//	return m_ListInfo.bAlternateBk;
//}
//
//void List::SetAlternateBk(bool bAlternateBk)
//{
//    m_ListInfo.bAlternateBk = bAlternateBk;
//    Invalidate();
//}
//
//DWORD List::GetItemTextColor() const
//{
//	return m_ListInfo.dwTextColor;
//}
//
//DWORD List::GetItemBkColor() const
//{
//	return m_ListInfo.dwBkColor;
//}
//
//LPCTSTR List::GetItemBkImage() const
//{
//	return m_ListInfo.diBk.sDrawString.c_str();
//}
//
//void List::SetSelectedItemTextColor(DWORD dwTextColor)
//{
//    m_ListInfo.dwSelectedTextColor = dwTextColor;
//    Invalidate();
//}
//
//void List::SetSelectedItemBkColor(DWORD dwBkColor)
//{
//    m_ListInfo.dwSelectedBkColor = dwBkColor;
//    Invalidate();
//}
//
//void List::SetSelectedItemImage(LPCTSTR pStrImage)
//{
//	if( m_ListInfo.diSelected.sDrawString == pStrImage && m_ListInfo.diSelected.pImageInfo != NULL ) return;
//	m_ListInfo.diSelected.Clear();
//	m_ListInfo.diSelected.sDrawString = pStrImage;
//	Invalidate();
//}
//
//DWORD List::GetSelectedItemTextColor() const
//{
//	return m_ListInfo.dwSelectedTextColor;
//}
//
//DWORD List::GetSelectedItemBkColor() const
//{
//	return m_ListInfo.dwSelectedBkColor;
//}
//
//LPCTSTR List::GetSelectedItemImage() const
//{
//	return m_ListInfo.diSelected.sDrawString.c_str();
//}
//
//void List::SetHotItemTextColor(DWORD dwTextColor)
//{
//    m_ListInfo.dwHotTextColor = dwTextColor;
//    Invalidate();
//}
//
//void List::SetHotItemBkColor(DWORD dwBkColor)
//{
//    m_ListInfo.dwHotBkColor = dwBkColor;
//    Invalidate();
//}
//
//void List::SetHotItemImage(LPCTSTR pStrImage)
//{
//	if( m_ListInfo.diHot.sDrawString == pStrImage && m_ListInfo.diHot.pImageInfo != NULL ) return;
//	m_ListInfo.diHot.Clear();
//	m_ListInfo.diHot.sDrawString = pStrImage;
//	Invalidate();
//}
//
//DWORD List::GetHotItemTextColor() const
//{
//	return m_ListInfo.dwHotTextColor;
//}
//DWORD List::GetHotItemBkColor() const
//{
//	return m_ListInfo.dwHotBkColor;
//}
//
//LPCTSTR List::GetHotItemImage() const
//{
//	return m_ListInfo.diHot.sDrawString.c_str();
//}
//
//void List::SetDisabledItemTextColor(DWORD dwTextColor)
//{
//    m_ListInfo.dwDisabledTextColor = dwTextColor;
//    Invalidate();
//}
//
//void List::SetDisabledItemBkColor(DWORD dwBkColor)
//{
//    m_ListInfo.dwDisabledBkColor = dwBkColor;
//    Invalidate();
//}
//
//void List::SetDisabledItemImage(LPCTSTR pStrImage)
//{
//	if( m_ListInfo.diDisabled.sDrawString == pStrImage && m_ListInfo.diDisabled.pImageInfo != NULL ) return;
//	m_ListInfo.diDisabled.Clear();
//	m_ListInfo.diDisabled.sDrawString = pStrImage;
//	Invalidate();
//}
//
//DWORD List::GetDisabledItemTextColor() const
//{
//	return m_ListInfo.dwDisabledTextColor;
//}
//
//DWORD List::GetDisabledItemBkColor() const
//{
//	return m_ListInfo.dwDisabledBkColor;
//}
//
//LPCTSTR List::GetDisabledItemImage() const
//{
//	return m_ListInfo.diDisabled.sDrawString.c_str();
//}
//
//int List::GetItemHLineSize() const
//{
//    return m_ListInfo.iHLineSize;
//}
//
//void List::SetItemHLineSize(int iSize)
//{
//    m_ListInfo.iHLineSize = iSize;
//    Invalidate();
//}
//
//DWORD List::GetItemHLineColor() const
//{
//    return m_ListInfo.dwHLineColor;
//}
//
//void List::SetItemHLineColor(DWORD dwLineColor)
//{
//    m_ListInfo.dwHLineColor = dwLineColor;
//    Invalidate();
//}
//
//bool List::IsItemShowHtml()
//{
//    return m_ListInfo.bShowHtml;
//}
//
//void List::SetItemShowHtml(bool bShowHtml)
//{
//    if( m_ListInfo.bShowHtml == bShowHtml ) return;
//
//    m_ListInfo.bShowHtml = bShowHtml;
//    NeedUpdate();
//}

void List::EnsureVisible(int iIndex)
{
    if( m_iCurSel < 0 ) return;
    RECT rcItem = GetItemAt(iIndex)->GetPos();
    RECT rcList = GetPos();
    RECT rcListPadding = GetPadding();

    rcList.left += rcListPadding.left;
    rcList.top += rcListPadding.top;
    rcList.right -= rcListPadding.right;
    rcList.bottom -= rcListPadding.bottom;

    ScrollBar* pHorizontalScrollBar = GetHorizontalScrollBar();
    if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();

    int iPos = GetScrollPos().cy;
    if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
    int dx = 0;
    if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
    if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
    Scroll(0, dx);
}

void List::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = GetScrollPos();
    SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
}

void List::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
#if 0
    /*if( _tcscmp(pstrName, _T("header")) == 0 ) GetHeader()->SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("headerbkimage")) == 0 ) GetHeader()->SetBkImage(pstrValue);
    else */if( _tcscmp(pstrName, _T("scrollselect")) == 0 ) SetScrollSelect(_tcscmp(pstrValue, _T("true")) == 0);
    //else if( _tcscmp(pstrName, _T("multiexpanding")) == 0 ) SetMultiExpanding(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("itemheight")) == 0 ) m_ListInfo.uFixedHeight = _ttoi(pstrValue);
    else if( _tcscmp(pstrName, _T("itemfont")) == 0 ) m_ListInfo.nFont = _ttoi(pstrValue);
    else if( _tcscmp(pstrName, _T("itemalign")) == 0 ) {
        if( _tcsstr(pstrValue, _T("left")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_ListInfo.uTextStyle |= DT_RIGHT;
        }
    }
    else if (_tcscmp(pstrName, _T("itemvalign")) == 0)
    {
        if (_tcsstr(pstrValue, _T("top")) != NULL) {
            m_ListInfo.uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
            m_ListInfo.uTextStyle |= DT_TOP;
        }
        if (_tcsstr(pstrValue, _T("vcenter")) != NULL) {
            m_ListInfo.uTextStyle &= ~(DT_TOP | DT_BOTTOM);
            m_ListInfo.uTextStyle |= DT_VCENTER;
        }
        if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
            m_ListInfo.uTextStyle &= ~(DT_TOP | DT_VCENTER);
            m_ListInfo.uTextStyle |= DT_BOTTOM;
        }
    }
    else if( _tcscmp(pstrName, _T("itemendellipsis")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_ListInfo.uTextStyle |= DT_END_ELLIPSIS;
        else m_ListInfo.uTextStyle &= ~DT_END_ELLIPSIS;
    }
    else if( _tcscmp(pstrName, _T("itemmultiline")) == 0 ) {
        if (_tcscmp(pstrValue, _T("true")) == 0) {
            m_ListInfo.uTextStyle &= ~DT_SINGLELINE;
            m_ListInfo.uTextStyle |= DT_WORDBREAK;
        }
        else m_ListInfo.uTextStyle |= DT_SINGLELINE;
    }
    else if( _tcscmp(pstrName, _T("itemtextpadding")) == 0 ) {
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = NULL;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetItemTextPadding(rcTextPadding);
    }
    else if( _tcscmp(pstrName, _T("itemtextcolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itembkcolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itembkimage")) == 0 ) SetItemBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemaltbk")) == 0 ) SetAlternateBk(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("itemselectedtextcolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetSelectedItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemselectedbkcolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetSelectedItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemselectedimage")) == 0 ) SetSelectedItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemhottextcolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetHotItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemhotbkcolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetHotItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemhotimage")) == 0 ) SetHotItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetDisabledItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetDisabledItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemdisabledimage")) == 0 ) SetDisabledItemImage(pstrValue);
   /* else if( _tcscmp(pstrName, _T("itemvlinesize")) == 0 ) {
        SetItemVLineSize(_ttoi(pstrValue));
    }
    else if( _tcscmp(pstrName, _T("itemvlinecolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetItemVLineColor(clrColor);
    }*/
    else if( _tcscmp(pstrName, _T("itemhlinesize")) == 0 ) {
        SetItemHLineSize(_ttoi(pstrValue));
    }
    else if( _tcscmp(pstrName, _T("itemhlinecolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetItemHLineColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcscmp(pstrName, _T("scrollbarfloat")) == 0) {
		SetScrollBarFloat(_tcscmp(pstrValue, _T("true")) == 0);
	}
	else 
#endif
		__super::SetAttribute(pstrName, pstrValue);
}

//IListCallback* List::GetTextCallback() const
//{
//    return m_pCallback;
//}
//
//void List::SetTextCallback(IListCallback* pCallback)
//{
//    m_pCallback = pCallback;
//}

//SIZE List::GetScrollPos() const
//{
//    return m_pList->GetScrollPos();
//}
//
//SIZE List::GetScrollRange() const
//{
//    return m_pList->GetScrollRange();
//}
//
//void List::SetScrollPos(SIZE szPos)
//{
//    m_pList->SetScrollPos(szPos);
//}
//
//void List::LineUp()
//{
//    m_pList->LineUp();
//}
//
//void List::LineDown()
//{
//    m_pList->LineDown();
//}
//
//void List::PageUp()
//{
//    m_pList->PageUp();
//}
//
//void List::PageDown()
//{
//    m_pList->PageDown();
//}
//
//void List::HomeUp()
//{
//    m_pList->HomeUp();
//}
//
//void List::EndDown()
//{
//    m_pList->EndDown();
//}
//
//void List::LineLeft()
//{
//    m_pList->LineLeft();
//}
//
//void List::LineRight()
//{
//    m_pList->LineRight();
//}
//
//void List::PageLeft()
//{
//    m_pList->PageLeft();
//}
//
//void List::PageRight()
//{
//    m_pList->PageRight();
//}
//
//void List::HomeLeft()
//{
//    m_pList->HomeLeft();
//}
//
//void List::EndRight()
//{
//    m_pList->EndRight();
//}
//
//void List::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
//{
//    m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);
//}
//
//ScrollBar* List::GetVerticalScrollBar() const
//{
//    return m_pList->GetVerticalScrollBar();
//}
//
//ScrollBar* List::GetHorizontalScrollBar() const
//{
//    return m_pList->GetHorizontalScrollBar();
//}

bool List::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData)
{
#if 0
	int iCurSel = m_iCurSel;
	bool bResult = m_pList->SortItems(pfnCompare, dwData, iCurSel);
	if (bResult) {
		m_iCurSel = iCurSel;
		EnsureVisible(m_iCurSel);
		NeedUpdate();
	}
	return bResult;
#else
	return false;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

ListElement::ListElement() :
m_iIndex(-1),
m_iDrawIndex(0),
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
	if (_tcscmp(pstrName, DUI_CTR_ILISTITEM) == 0) return static_cast<IListItem2*>(this);
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

int ListElement::GetDrawIndex() const
{
	return m_iDrawIndex;
}

void ListElement::SetDrawIndex(int iIndex)
{
	m_iDrawIndex = iIndex;
}

void ListElement::Invalidate()
{
	if (!IsVisible()) return;

	if (GetParent()) {
		ScrollContainer* pParentContainer = static_cast<ScrollContainer*>(GetParent()->GetInterface(DUI_CTR_SCROLLCONTAINER));
		if (pParentContainer) {
			RECT rc = pParentContainer->GetPos();
			RECT rcPadding = pParentContainer->GetPadding();
			rc.left += rcPadding.left;
			rc.top += rcPadding.top;
			rc.right -= rcPadding.right;
			rc.bottom -= rcPadding.bottom;
			ScrollBar* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
			if (pVerticalScrollBar && pVerticalScrollBar->IsVisible()) rc.right -= pVerticalScrollBar->GetFixedWidth();
			ScrollBar* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
			if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible()) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

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

bool ListElement::IsExpanded() const
{
	return false;
}

bool ListElement::Expand(bool /*bExpand = true*/)
{
	return false;
}

void ListElement::DoEvent(TEvent& event)
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
		if (m_pOwner != NULL) m_pOwner->DoEvent(event);
		else Control::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_DBLCLICK)
	{
		if (IsEnabled()) {
			Activate();
			Invalidate();
		}
		return;
	}
	if (event.Type == UIEVENT_KEYDOWN)
	{
		if (IsKeyboardEnabled() && IsEnabled()) {
			if (event.chKey == VK_RETURN) {
				Activate();
				Invalidate();
				return;
			}
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
	else Control::SetAttribute(pstrName, pstrValue);
}

void ListElement::DrawItemBk(HDC hDC, const RECT& rcItem)
{
#if 0
	ASSERT(m_pOwner);
	if (m_pOwner == NULL) return;
	ListViewInfo* pInfo = m_pOwner->GetListInfo();
	if (pInfo == NULL) return;
	DWORD iBackColor = 0;
	if (!pInfo->bAlternateBk || m_iDrawIndex % 2 == 0) iBackColor = pInfo->dwBkColor;
	if ((m_uButtonState & UISTATE_HOT) != 0) {
		iBackColor = pInfo->dwHotBkColor;
	}
	if (IsSelected()) {
		iBackColor = pInfo->dwSelectedBkColor;
	}
	if (!IsEnabled()) {
		iBackColor = pInfo->dwDisabledBkColor;
	}

	if (iBackColor != 0) {
		CRenderEngine::DrawColor(hDC, rcItem, GetAdjustColor(iBackColor));
	}

	if (!IsEnabled()) {
		if (DrawImage(hDC, pInfo->diDisabled)) return;
	}
	if (IsSelected()) {
		if (DrawImage(hDC, pInfo->diSelected)) return;
	}
	if ((m_uButtonState & UISTATE_HOT) != 0) {
		if (DrawImage(hDC, pInfo->diHot)) return;
	}

	if (!DrawImage(hDC, m_diBk)) {
		if (!pInfo->bAlternateBk || m_iDrawIndex % 2 == 0) {
			if (DrawImage(hDC, pInfo->diBk)) return;
		}
	}
#endif
}


} // namespace dui

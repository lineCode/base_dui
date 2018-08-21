#include "stdafx.h"

namespace dui {

List::List() : m_bScrollSelect(false), m_iCurSel(-1)/*, m_iExpandedItem(-1)*/
{
  
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

	__super::SetPos(__super::GetPos(), bNeedInvalidate);
}

void List::DoEvent(Event& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
		else ScrollBox::DoEvent(event);
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

	ScrollBox::DoEvent(event);
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
    SetScrollPos(DuiSize(sz.cx + dx, sz.cy + dy));
}

void List::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	__super::SetAttribute(pstrName, pstrValue);
}

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


} // namespace dui

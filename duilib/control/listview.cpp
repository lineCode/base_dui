#include "stdafx.h"

namespace dui {

ListView::ListView() : m_pCallback(NULL), m_bScrollSelect(false), m_iCurSel(-1)
{
    m_pHeader = new ListHeader;
    Add(m_pHeader);
	m_pHeader->SetVisible(false);

	m_pList = new ListBody(this);
    __super::Add(m_pList);

    m_ListInfo.nColumns = 0;
    m_ListInfo.uFixedHeight = 0;
    m_ListInfo.nFont = -1;
    m_ListInfo.uTextStyle = DT_VCENTER | DT_SINGLELINE; // m_uTextStyle(DT_VCENTER | DT_END_ELLIPSIS)
    m_ListInfo.dwTextColor = 0xFF000000;
    m_ListInfo.dwBkColor = 0;
    m_ListInfo.bAlternateBk = false;
    m_ListInfo.dwSelectedTextColor = 0xFF000000;
    m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
    m_ListInfo.dwHotTextColor = 0xFF000000;
    m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
    m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
    m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
    m_ListInfo.iHLineSize = 0;
    m_ListInfo.dwHLineColor = 0xFF3C3C3C;
    m_ListInfo.iVLineSize = 0;
    m_ListInfo.dwVLineColor = 0xFF3C3C3C;
    /*m_ListInfo.bShowHtml = false;
    m_ListInfo.bMultiExpandable = false;*/
    ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
}

LPCTSTR ListView::GetClass() const
{
    return DUI_CTR_LISTVIEW;
}

UINT ListView::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

LPVOID ListView::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, DUI_CTR_LISTVIEW) == 0) return static_cast<ListView*>(this);
	if (_tcscmp(pstrName, DUI_CTR_ILISTVIEW) == 0) return static_cast<IListView*>(this);
	if (_tcscmp(pstrName, DUI_CTR_ILIST) == 0) return static_cast<IList*>(this);
	return __super::GetInterface(pstrName);
}

Control* ListView::GetItemAt(int iIndex) const
{
    return m_pList->GetItemAt(iIndex);
}

int ListView::GetItemIndex(Control* pControl) const
{
	if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) return __super::GetItemIndex(pControl);
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->GetItemIndex(pControl);

    return m_pList->GetItemIndex(pControl);
}

bool ListView::SetItemIndex(Control* pControl, int iIndex)
{
#if 0
	if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) return __super::SetItemIndex(pControl, iIndex);
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->SetItemIndex(pControl, iIndex);

    int iOrginIndex = m_pList->GetItemIndex(pControl);
    if( iOrginIndex == -1 ) return false;
    if( iOrginIndex == iIndex ) return true;

	IListViewItem* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
		static_cast<IListViewItem*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !m_pList->SetItemIndex(pControl, iIndex) ) return false;
    int iMinIndex = min(iOrginIndex, iIndex);
    int iMaxIndex = max(iOrginIndex, iIndex);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        Control* p = m_pList->GetItemAt(i);
		IListViewItem* pListItem = static_cast<IListViewItem*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
#endif
    return true;
}

bool ListView::SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex)
{
#if 0
    if (pStartControl == NULL || iCount < 0 || iNewStartIndex < 0) return false;
	if (pStartControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) return __super::SetMultiItemIndex(pStartControl, iCount, iNewStartIndex);
    // We also need to recognize header sub-items
    if( _tcsstr(pStartControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->SetMultiItemIndex(pStartControl, iCount, iNewStartIndex);

    int iStartIndex = GetItemIndex(pStartControl);
    if (iStartIndex == iNewStartIndex) return true;
    if (iStartIndex + iCount > GetCount()) return false;
    if (iNewStartIndex + iCount > GetCount()) return false;

	IListViewItem* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
		static_cast<IListViewItem*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !m_pList->SetMultiItemIndex(pStartControl, iCount, iNewStartIndex) ) return false;
    int iMinIndex = min(iStartIndex, iNewStartIndex);
    int iMaxIndex = max(iStartIndex + iCount, iNewStartIndex + iCount);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        Control* p = m_pList->GetItemAt(i);
		IListViewItem* pListItem = static_cast<IListViewItem*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
#endif
    return true;
}

int ListView::GetCount() const
{
    return m_pList->GetCount();
}

bool ListView::Add(Control* pControl)
{
    // Override the Add() method so we can add items specifically to
    // the intended widgets. Headers are assumed to be
    // answer the correct interface so we can add multiple list headers.
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) {
        if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
			__super::Remove(m_pHeader);
            m_pHeader = static_cast<ListHeader*>(pControl);
        }
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
		return __super::AddAt(pControl, 0);
    }
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) {
        bool ret = m_pHeader->Add(pControl);
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return ret;
    }
    // The list items should know about us
	IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
	if( pListItem != NULL ) {
		pListItem->SetOwner(this);
		pListItem->SetIndex(GetCount());
    }
    return m_pList->Add(pControl);
}

bool ListView::AddAt(Control* pControl, int iIndex)
{
#if 1
	assert(0);
#else
    // Override the AddAt() method so we can add items specifically to
    // the intended widgets. Headers and are assumed to be
    // answer the correct interface so we can add multiple list headers.
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) {
        if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
			__super::Remove(m_pHeader);
            m_pHeader = static_cast<ListHeader*>(pControl);
        }
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
		return __super::AddAt(pControl, 0);
    }
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) {
        bool ret = m_pHeader->AddAt(pControl, iIndex);
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return ret;
    }
    if (!m_pList->AddAt(pControl, iIndex)) return false;

    // The list items should know about us
	IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem != NULL ) {
        pListItem->SetOwner(this);
        pListItem->SetIndex(iIndex);
    }

    for(int i = iIndex + 1; i < m_pList->GetCount(); ++i) {
        Control* p = m_pList->GetItemAt(i);
		pListItem = static_cast<IListItem*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= iIndex ) m_iCurSel += 1;
#endif
    return true;
}

bool ListView::Remove(Control* pControl, bool bDoNotDestroy)
{
	if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) return __super::Remove(pControl, bDoNotDestroy);
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->Remove(pControl, bDoNotDestroy);

    int iIndex = m_pList->GetItemIndex(pControl);
    if (iIndex == -1) return false;

    if (!m_pList->RemoveAt(iIndex, bDoNotDestroy)) return false;

    for(int i = iIndex; i < m_pList->GetCount(); ++i) {
        Control* p = m_pList->GetItemAt(i);
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

bool ListView::RemoveAt(int iIndex, bool bDoNotDestroy)
{
#if 1
	assert(0);
#else
    if (!m_pList->RemoveAt(iIndex, bDoNotDestroy)) return false;

    for(int i = iIndex; i < m_pList->GetCount(); ++i) {
        Control* p = m_pList->GetItemAt(i);
		IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) pListItem->SetIndex(i);
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
#endif
    return true;
}

void ListView::RemoveAll()
{
    m_iCurSel = -1;
    m_pList->RemoveAll();
}

void ListView::SetPos(RECT rc, bool bNeedInvalidate)
{
	if( m_pHeader != NULL ) { // 设置header各子元素x坐标,因为有些listitem的setpos需要用到(临时修复)
		RECT rcPadding = GetPadding();
		int iLeft = rc.left + rcPadding.left;
		int iRight = rc.right - rcPadding.right;

		m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

		if( !m_pHeader->IsVisible() ) {
			for( int it = m_pHeader->GetCount() - 1; it >= 0; it-- ) {
				static_cast<Control*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
			}
		}
		m_pHeader->SetPos(DuiRect(iLeft, 0, iRight, 0), false);
		int iOffset = m_pList->GetScrollPos().cx;
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
		if( !m_pHeader->IsVisible() ) {
			for( int it = m_pHeader->GetCount() - 1; it >= 0; it-- ) {
				static_cast<Control*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
			}
			m_pHeader->SetInternVisible(false);
		}
	}

	__super::SetPos(rc, bNeedInvalidate);

	if( m_pHeader == NULL ) return;

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

	m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

	if( !m_pHeader->IsVisible() ) {
		for( int it = m_pHeader->GetCount() - 1; it >= 0; it-- ) {
			static_cast<Control*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
		}
		m_pHeader->SetPos(DuiRect(rc.left, 0, rc.right, 0), false);
	}
	int iOffset = m_pList->GetScrollPos().cx;
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
	if( !m_pHeader->IsVisible() ) {
		for( int it = m_pHeader->GetCount() - 1; it >= 0; it-- ) {
			static_cast<Control*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
		}
		m_pHeader->SetInternVisible(false);
	}
	m_pList->SetPos(m_pList->GetPos(), bNeedInvalidate);
}

void ListView::Move(SIZE szOffset, bool bNeedInvalidate)
{
	__super::Move(szOffset, bNeedInvalidate);
	if( !m_pHeader->IsVisible() ) m_pHeader->Move(szOffset, false);
}

void ListView::DoEvent(Event& event)
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

ListHeader* ListView::GetHeader() const
{
    return m_pHeader;
}

ScrollBox* ListView::GetList() const
{
    return m_pList;
}

bool ListView::GetScrollSelect()
{
    return m_bScrollSelect;
}

void ListView::SetScrollSelect(bool bScrollSelect)
{
    m_bScrollSelect = bScrollSelect;
}

int ListView::GetCurSel() const
{
    return m_iCurSel;
}

bool ListView::SelectItem(int iIndex, bool bTakeFocus, bool bTriggerEvent)
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

UINT ListView::GetItemFixedHeight()
{
    return m_ListInfo.uFixedHeight;
}

void ListView::SetItemFixedHeight(UINT nHeight)
{
    m_ListInfo.uFixedHeight = nHeight;
    NeedUpdate();
}

int ListView::GetItemFont(int index)
{
    return m_ListInfo.nFont;
}

void ListView::SetItemFont(int index)
{
    m_ListInfo.nFont = index;
    NeedUpdate();
}

UINT ListView::GetItemTextStyle()
{
    return m_ListInfo.uTextStyle;
}

void ListView::SetItemTextStyle(UINT uStyle)
{
    m_ListInfo.uTextStyle = uStyle;
    NeedUpdate();
}

void ListView::SetItemTextPadding(RECT rc)
{
    m_ListInfo.rcTextPadding = rc;
    NeedUpdate();
}

RECT ListView::GetItemTextPadding() const
{
	return m_ListInfo.rcTextPadding;
}

void ListView::SetItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwTextColor = dwTextColor;
    Invalidate();
}

void ListView::SetItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwBkColor = dwBkColor;
    Invalidate();
}

void ListView::SetItemBkImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diBk.sDrawString == pStrImage && m_ListInfo.diBk.pImageInfo != NULL ) return;
	m_ListInfo.diBk.Clear();
	m_ListInfo.diBk.sDrawString = pStrImage;
	Invalidate();
}

bool ListView::IsAlternateBk() const
{
	return m_ListInfo.bAlternateBk;
}

void ListView::SetAlternateBk(bool bAlternateBk)
{
    m_ListInfo.bAlternateBk = bAlternateBk;
    Invalidate();
}

DWORD ListView::GetItemTextColor() const
{
	return m_ListInfo.dwTextColor;
}

DWORD ListView::GetItemBkColor() const
{
	return m_ListInfo.dwBkColor;
}

LPCTSTR ListView::GetItemBkImage() const
{
	return m_ListInfo.diBk.sDrawString.c_str();
}

void ListView::SetSelectedItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwSelectedTextColor = dwTextColor;
    Invalidate();
}

void ListView::SetSelectedItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwSelectedBkColor = dwBkColor;
    Invalidate();
}

void ListView::SetSelectedItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diSelected.sDrawString == pStrImage && m_ListInfo.diSelected.pImageInfo != NULL ) return;
	m_ListInfo.diSelected.Clear();
	m_ListInfo.diSelected.sDrawString = pStrImage;
	Invalidate();
}

DWORD ListView::GetSelectedItemTextColor() const
{
	return m_ListInfo.dwSelectedTextColor;
}

DWORD ListView::GetSelectedItemBkColor() const
{
	return m_ListInfo.dwSelectedBkColor;
}

LPCTSTR ListView::GetSelectedItemImage() const
{
	return m_ListInfo.diSelected.sDrawString.c_str();
}

void ListView::SetHotItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwHotTextColor = dwTextColor;
    Invalidate();
}

void ListView::SetHotItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwHotBkColor = dwBkColor;
    Invalidate();
}

void ListView::SetHotItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diHot.sDrawString == pStrImage && m_ListInfo.diHot.pImageInfo != NULL ) return;
	m_ListInfo.diHot.Clear();
	m_ListInfo.diHot.sDrawString = pStrImage;
	Invalidate();
}

DWORD ListView::GetHotItemTextColor() const
{
	return m_ListInfo.dwHotTextColor;
}
DWORD ListView::GetHotItemBkColor() const
{
	return m_ListInfo.dwHotBkColor;
}

LPCTSTR ListView::GetHotItemImage() const
{
	return m_ListInfo.diHot.sDrawString.c_str();
}

void ListView::SetDisabledItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwDisabledTextColor = dwTextColor;
    Invalidate();
}

void ListView::SetDisabledItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwDisabledBkColor = dwBkColor;
    Invalidate();
}

void ListView::SetDisabledItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diDisabled.sDrawString == pStrImage && m_ListInfo.diDisabled.pImageInfo != NULL ) return;
	m_ListInfo.diDisabled.Clear();
	m_ListInfo.diDisabled.sDrawString = pStrImage;
	Invalidate();
}

DWORD ListView::GetDisabledItemTextColor() const
{
	return m_ListInfo.dwDisabledTextColor;
}

DWORD ListView::GetDisabledItemBkColor() const
{
	return m_ListInfo.dwDisabledBkColor;
}

LPCTSTR ListView::GetDisabledItemImage() const
{
	return m_ListInfo.diDisabled.sDrawString.c_str();
}

int ListView::GetItemHLineSize() const
{
    return m_ListInfo.iHLineSize;
}

void ListView::SetItemHLineSize(int iSize)
{
    m_ListInfo.iHLineSize = iSize;
    Invalidate();
}

DWORD ListView::GetItemHLineColor() const
{
    return m_ListInfo.dwHLineColor;
}

void ListView::SetItemHLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwHLineColor = dwLineColor;
    Invalidate();
}

int ListView::GetItemVLineSize() const
{
    return m_ListInfo.iVLineSize;
}

void ListView::SetItemVLineSize(int iSize)
{
    m_ListInfo.iVLineSize = iSize;
    Invalidate();
}

DWORD ListView::GetItemVLineColor() const
{
    return m_ListInfo.dwVLineColor;
}

void ListView::SetItemVLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwVLineColor = dwLineColor;
    Invalidate();
}

void ListView::EnsureVisible(int iIndex)
{
    if( m_iCurSel < 0 ) return;
    RECT rcItem = m_pList->GetItemAt(iIndex)->GetPos();
    RECT rcList = m_pList->GetPos();
    RECT rcListPadding = m_pList->GetPadding();

    rcList.left += rcListPadding.left;
    rcList.top += rcListPadding.top;
    rcList.right -= rcListPadding.right;
    rcList.bottom -= rcListPadding.bottom;

	ScrollBar* pHorizontalScrollBar = m_pList->GetHorizontalScrollBar();
    if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();

    int iPos = m_pList->GetScrollPos().cy;
    if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
    int dx = 0;
    if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
    if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
    Scroll(0, dx);
}

void ListView::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = m_pList->GetScrollPos();
    m_pList->SetScrollPos(DuiSize(sz.cx + dx, sz.cy + dy));
}

void ListView::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("header")) == 0 ) GetHeader()->SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("headerbkimage")) == 0 ) GetHeader()->SetBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("scrollselect")) == 0 ) SetScrollSelect(_tcscmp(pstrValue, _T("true")) == 0);
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
    else if( _tcscmp(pstrName, _T("itemvlinesize")) == 0 ) {
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
    }
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
    //else if( _tcscmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcscmp(pstrName, _T("scrollbarfloat")) == 0) {
		if (m_pList)
			m_pList->SetScrollBarFloat(_tcscmp(pstrValue, _T("true")) == 0);
	}
	else __super::SetAttribute(pstrName, pstrValue);
}

IListViewCallback* ListView::GetTextCallback() const
{
    return m_pCallback;
}

void ListView::SetTextCallback(IListViewCallback* pCallback)
{
    m_pCallback = pCallback;
}

SIZE ListView::GetScrollPos() const
{
    return m_pList->GetScrollPos();
}

SIZE ListView::GetScrollRange() const
{
    return m_pList->GetScrollRange();
}

void ListView::SetScrollPos(SIZE szPos)
{
    m_pList->SetScrollPos(szPos);
}

void ListView::LineUp()
{
    m_pList->LineUp();
}

void ListView::LineDown()
{
    m_pList->LineDown();
}

void ListView::PageUp()
{
    m_pList->PageUp();
}

void ListView::PageDown()
{
    m_pList->PageDown();
}

void ListView::HomeUp()
{
    m_pList->HomeUp();
}

void ListView::EndDown()
{
    m_pList->EndDown();
}

void ListView::LineLeft()
{
    m_pList->LineLeft();
}

void ListView::LineRight()
{
    m_pList->LineRight();
}

void ListView::PageLeft()
{
    m_pList->PageLeft();
}

void ListView::PageRight()
{
    m_pList->PageRight();
}

void ListView::HomeLeft()
{
    m_pList->HomeLeft();
}

void ListView::EndRight()
{
    m_pList->EndRight();
}

void ListView::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
{
    m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);
}

ScrollBar* ListView::GetVerticalScrollBar() const
{
    return m_pList->GetVerticalScrollBar();
}

ScrollBar* ListView::GetHorizontalScrollBar() const
{
    return m_pList->GetHorizontalScrollBar();
}

bool ListView::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData)
{
	if (!m_pList) return false;
	int iCurSel = m_iCurSel;
	bool bResult = m_pList->SortItems(pfnCompare, dwData, iCurSel);
	if (bResult) {
		m_iCurSel = iCurSel;
		EnsureVisible(m_iCurSel);
		NeedUpdate();
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

ListBody::ListBody(IListView* pOwner) : m_pOwner(pOwner)
{
    ASSERT(m_pOwner);
}

bool ListBody::Add(Control* pControl)
{
	assert(m_pOwner && m_pOwner->GetListInfo());
	IListItem *item = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
	if (!item)
		return false;
	ListViewInfo *info = m_pOwner->GetListInfo();
	int column = info->nColumns;

	SingleLine *line = nullptr;
	if (GetCount() == 0){
		line = new SingleLine(m_pOwner, 0);
		__super::Add(line);
	}
	else
		line = dynamic_cast<SingleLine*>(GetItemAt(GetCount() - 1));
	if (!line)
		return false;
	if (line->GetCount() >= column)
	{
		line = new SingleLine(m_pOwner, GetCount());
		__super::Add(line);
	}
	assert(line && line->GetCount() < column);
	item->SetIndex(line->GetCount());
	return line->Add(pControl);
}

bool ListBody::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData, int& iCurSel)
{
	if (!pfnCompare) return false;
	m_pCompareFunc = pfnCompare;
	Control *pCurSelControl = GetItemAt(iCurSel);
	Control **pData = (Control **)m_items.GetData();
	qsort_s(m_items.GetData(), m_items.GetSize(), sizeof(Control*), ListBody::ItemComareFunc, this);
	if (pCurSelControl) iCurSel = GetItemIndex(pCurSelControl);
	IListItem *pItem = NULL;
	for (int i = 0; i < m_items.GetSize(); ++i)
	{
		pItem = (IListItem*)(static_cast<Control*>(m_items[i])->GetInterface(DUI_CTR_ILISTITEM));
		if (pItem)
		{
			pItem->SetIndex(i);
		}
	}

	return true;
}

int __cdecl ListBody::ItemComareFunc(void *pvlocale, const void *item1, const void *item2)
{
	ListBody *pThis = (ListBody*)pvlocale;
	if (!pThis || !item1 || !item2)
		return 0;
	return pThis->ItemComareFunc(item1, item2);
}

int __cdecl ListBody::ItemComareFunc(const void *item1, const void *item2)
{
	Control *pControl1 = *(Control**)item1;
	Control *pControl2 = *(Control**)item2;
	return m_pCompareFunc((UINT_PTR)pControl1, (UINT_PTR)pControl2, m_compareData);
}

void ListBody::SetScrollPos(SIZE szPos)
{
    int cx = 0;
    int cy = 0;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
        m_pVerticalScrollBar->SetScrollPos(szPos.cy);
        cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
    }

    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
        int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
        m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
        cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
    }

    if( cx == 0 && cy == 0 ) return;

    for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
        Control* pControl = static_cast<Control*>(m_items[it2]);
        if( !pControl->IsVisible() ) continue;
        if( pControl->IsFloat() ) continue;
		pControl->Move(DuiSize(-cx, -cy), false);
    }

    Invalidate();

    if( cx != 0 && m_pOwner ) {
        ListHeader* pHeader = m_pOwner->GetHeader();
        if( pHeader == NULL ) return;
        ListViewInfo* pInfo = m_pOwner->GetListInfo();
        pInfo->nColumns = MIN(pHeader->GetCount(), UILIST_MAX_COLUMNS);
        for( int i = 0; i < pInfo->nColumns; i++ ) {
            Control* pControl = static_cast<Control*>(pHeader->GetItemAt(i));
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) continue;
			pControl->Move(DuiSize(-cx, -cy), false);
			pInfo->rcColumn[i] = pControl->GetPos();
        }
		pHeader->Invalidate();
    }
}

void ListBody::SetPos(RECT rc, bool bNeedInvalidate)
{
    Control::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    // Adjust for padding
	RECT rcPadding = GetPadding();
	rc.left += rcPadding.left;
	rc.top += rcPadding.top;
	rc.right -= rcPadding.right;
	rc.bottom -= rcPadding.bottom;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() && !m_bScrollBarFloat) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
	if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() && !m_bScrollBarFloat) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

    // Determine the minimum size
    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
        szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

	int iChildMargin = m_iChildMargin;
    ListViewInfo* pInfo = NULL;
    if( m_pOwner ) {
        pInfo = m_pOwner->GetListInfo();
        if( pInfo != NULL ) {
            iChildMargin += pInfo->iHLineSize;
            if (pInfo->nColumns > 0) {
                szAvailable.cx = pInfo->rcColumn[pInfo->nColumns - 1].right - pInfo->rcColumn[0].left;
            }
        }
    }

    int cxNeeded = 0;
    int cyFixed = 0;
    int nEstimateNum = 0;
    SIZE szControlAvailable;
    int iControlMaxWidth = 0;
    int iControlMaxHeight = 0;
    for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
        Control* pControl = static_cast<Control*>(m_items[it1]);
        if( !pControl->IsVisible() ) continue;
        if( pControl->IsFloat() ) continue;
        szControlAvailable = szAvailable;
        RECT rcMargin = pControl->GetMargin();
        szControlAvailable.cx -= rcMargin.left + rcMargin.right;
        iControlMaxWidth = pControl->GetFixedWidth();
        iControlMaxHeight = pControl->GetFixedHeight();
        if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
        if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
        if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
        if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
        SIZE sz = pControl->EstimateSize(szAvailable);
        if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
        if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
        cyFixed += sz.cy + pControl->GetMargin().top + pControl->GetMargin().bottom;

        sz.cx = MAX(sz.cx, 0);
        if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
        if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
        cxNeeded = MAX(cxNeeded, sz.cx);
        nEstimateNum++;
    }
    cyFixed += (nEstimateNum - 1) * iChildMargin;

    if( m_pOwner ) {
        ListHeader* pHeader = m_pOwner->GetHeader();
        if( pHeader != NULL && pHeader->GetCount() > 0 ) {
            cxNeeded = MAX(0, pHeader->EstimateSize(DuiSize(rc.right - rc.left, rc.bottom - rc.top)).cx);
        }
    }

    // Place elements
    int cyNeeded = 0;
    // Position the elements
    SIZE szRemaining = szAvailable;
    int iPosY = rc.top;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        iPosY -= m_pVerticalScrollBar->GetScrollPos();
    }
    int iPosX = rc.left;
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
        iPosX -= m_pHorizontalScrollBar->GetScrollPos();
    }
    
    int iAdjustable = 0;
    for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
        Control* pControl = static_cast<Control*>(m_items[it2]);
        if( !pControl->IsVisible() ) continue;
        if( pControl->IsFloat() ) {
            SetFloatPos(it2);
            continue;
        }

        RECT rcMargin = pControl->GetMargin();
        szRemaining.cy -= rcMargin.top;
        szControlAvailable = szRemaining;
        szControlAvailable.cx -= rcMargin.left + rcMargin.right;
        iControlMaxWidth = pControl->GetFixedWidth();
        iControlMaxHeight = pControl->GetFixedHeight();
        if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
        if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
        if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
        if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
        SIZE sz = pControl->EstimateSize(szControlAvailable);
        if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
        if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
        sz.cx = pControl->GetMaxWidth();
        if( sz.cx == 0 ) sz.cx = szAvailable.cx - rcMargin.left - rcMargin.right;
        if( sz.cx < 0 ) sz.cx = 0;
        if( sz.cx > szControlAvailable.cx ) sz.cx = szControlAvailable.cx;
        if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();

        RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + rcMargin.left + sz.cx, iPosY + sz.cy + rcMargin.top + rcMargin.bottom };
        pControl->SetPos(rcCtrl, false);

        iPosY += sz.cy + iChildMargin + rcMargin.top + rcMargin.bottom;
        cyNeeded += sz.cy + rcMargin.top + rcMargin.bottom;
        szRemaining.cy -= sz.cy + iChildMargin + rcMargin.bottom;
    }
    cyNeeded += (nEstimateNum - 1) * iChildMargin;

    // Process the scrollbar
    ProcessScrollBar(rc, cxNeeded, cyNeeded);
}

void ListBody::DoEvent(Event& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else Control::DoEvent(event);
        return;
    }

    if( m_pOwner != NULL ) {
		if (event.Type == UIEVENT_SCROLLWHEEL) {
			if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled()) {
				RECT rcHorizontalScrollBar = m_pHorizontalScrollBar->GetPos();
				if( ::PtInRect(&rcHorizontalScrollBar, event.ptMouse) ) 
				{
					switch( LOWORD(event.wParam) ) {
					case SB_LINEUP:
						//m_pOwner->LineLeft();
						return;
					case SB_LINEDOWN:
						//m_pOwner->LineRight();
						return;
					}
				}
			}
		}
		m_pOwner->DoEvent(event); }
	else {
		Control::DoEvent(event);
	}
}

bool ListBody::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
    RECT rcTemp = { 0 };
    if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return true;

	ListViewInfo* pListInfo = NULL;
	if( m_pOwner ) pListInfo = m_pOwner->GetListInfo();

	RenderClip clip;
	RenderClip::GenerateClip(hDC, rcTemp, clip);
	Control::DoPaint(hDC, rcPaint, pStopControl);

	if( m_items.GetSize() > 0 ) {
        RECT rc = m_rcItem;
        rc.left += m_rcPadding.left;
        rc.top += m_rcPadding.top;
        rc.right -= m_rcPadding.right;
        rc.bottom -= m_rcPadding.bottom;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() && !m_bScrollBarFloat) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() && !m_bScrollBarFloat) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

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
            //int iDrawIndex = 0;
            RenderClip childClip;
            RenderClip::GenerateClip(hDC, rcTemp, childClip);
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                Control* pControl = static_cast<Control*>(m_items[it]);
                if( pControl == pStopControl ) return false;
                if( !pControl->IsVisible() ) continue;
                if( !pControl->IsFloat() ) {
					IListItem* pListItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
                    if (pListInfo && pListInfo->iHLineSize > 0) {
                        // 因为没有为最后一个预留分割条长度，如果list铺满，最后一条不会显示
                        RECT rcMargin = pControl->GetMargin();
                        const RECT& rcPos = pControl->GetPos();
                        RECT rcBottomLine = { rcPos.left, rcPos.bottom + rcMargin.bottom, rcPos.right, rcPos.bottom + rcMargin.bottom + pListInfo->iHLineSize };
                        if( ::IntersectRect(&rcTemp, &rcPaint, &rcBottomLine) ) {
                            rcBottomLine.top += pListInfo->iHLineSize / 2;
                            rcBottomLine.bottom = rcBottomLine.top;
                            Render::DrawLine(hDC, rcBottomLine, pListInfo->iHLineSize, GetAdjustColor(pListInfo->dwHLineColor));
                        }
                    }
                }
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    RenderClip::UseOldClipBegin(hDC, childClip);
                    if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
                    RenderClip::UseOldClipEnd(hDC, childClip);
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

/////////////////////////////////////////////////////////////////////////////////////
//
//

SIZE ListHeader::EstimateSize(SIZE szAvailable)
{
    SIZE cXY = {0, m_FixedSize.cy};
	if( cXY.cy == 0 && m_pManager != NULL ) {
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			cXY.cy = MAX(cXY.cy,static_cast<Control*>(m_items[it])->EstimateSize(szAvailable).cy);
		}
		int nMin = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;
		cXY.cy = MAX(cXY.cy,nMin);
	}

    for( int it = 0; it < m_items.GetSize(); it++ ) {
        cXY.cx +=  static_cast<Control*>(m_items[it])->EstimateSize(szAvailable).cx;
    }

    return cXY;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//
LPCTSTR ListBody::SingleLine::GetClass() const
{
	//assert(0);
	return _T("")/*DUI_CTR_LISTHBOXELEMENT*/;
}

LPVOID ListBody::SingleLine::GetInterface(LPCTSTR pstrName)
{
	//if( _tcscmp(pstrName, DUI_CTR_LISTHBOXELEMENT) == 0 ) return static_cast<ListHBoxElement*>(this);
	//return ListContainerElement::GetInterface(pstrName);
	//assert(0);
	return __super::GetInterface(pstrName);
}

void ListBody::SingleLine::SetPos(RECT rc, bool bNeedInvalidate)
{
	printf("ListBody::SingleLine::SetPos()\n");
	__super::SetPos(rc, bNeedInvalidate);
	if( m_pOwner == NULL ) return __super::SetPos(rc, bNeedInvalidate);

	Control::SetPos(rc, bNeedInvalidate);
	rc = m_rcItem;

	ListViewInfo* pInfo = m_pOwner->GetListInfo();
	if (pInfo == NULL) return;
	if (pInfo->nColumns > 0) {
		int iColumnIndex = 0;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			Control* pControl = static_cast<Control*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}
			if( iColumnIndex >= pInfo->nColumns ) continue;

			RECT rcMargin = pControl->GetMargin();
			RECT rcItem = { pInfo->rcColumn[iColumnIndex].left + rcMargin.left, m_rcItem.top + rcMargin.top, 
				pInfo->rcColumn[iColumnIndex].right - rcMargin.right, m_rcItem.bottom - rcMargin.bottom };
			if (pInfo->iVLineSize > 0 && iColumnIndex < pInfo->nColumns - 1) {
				rcItem.right -= pInfo->iVLineSize;
			}
			pControl->SetPos(rcItem, false);
			iColumnIndex += 1;
		}
	}
	else {
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			Control* pControl = static_cast<Control*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}

			RECT rcMargin = pControl->GetMargin();
			RECT rcItem = { m_rcItem.left + rcMargin.left, m_rcItem.top + rcMargin.top, 
				m_rcItem.right - rcMargin.right, m_rcItem.bottom - rcMargin.bottom };
			pControl->SetPos(rcItem, false);
		}
	}
}

bool ListBody::SingleLine::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
	/*ASSERT(m_pOwner);
	if( m_pOwner == NULL ) return true;
	ListViewInfo* pInfo = m_pOwner->GetListInfo();
	if( pInfo == NULL ) return true;

	DrawItemBk(hDC, m_rcItem);
	for( int i = 0; i < pInfo->nColumns; i++ ) {
		RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };
		if (pInfo->iVLineSize > 0 && i < pInfo->nColumns - 1) {
			RECT rcLine = { rcItem.right - pInfo->iVLineSize / 2, rcItem.top, rcItem.right - pInfo->iVLineSize / 2, rcItem.bottom};
			Render::DrawLine(hDC, rcLine, pInfo->iVLineSize, GetAdjustColor(pInfo->dwVLineColor));
		}
	}*/
	return __super::DoPaint(hDC, rcPaint, pStopControl);
}

SIZE ListBody::SingleLine::EstimateSize(SIZE szAvailable)
{
	SIZE cXY = { 0, m_FixedSize.cy };
	if (cXY.cy == 0 && m_pManager != NULL) {
		for (int it = 0; it < m_items.GetSize(); it++) {
			cXY.cy = MAX(cXY.cy, static_cast<Control*>(m_items[it])->EstimateSize(szAvailable).cy);
		}
		int nMin = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;
		cXY.cy = MAX(cXY.cy, nMin);
	}

	for (int it = 0; it < m_items.GetSize(); it++) {
		cXY.cx += static_cast<Control*>(m_items[it])->EstimateSize(szAvailable).cx;
	}

	return cXY;
};

} // namespace dui

#include "StdAfx.h"

namespace dui {


/////////////////////////////////////////////////////////////////////////////////////
//

class ListBody : public VerticalLayout
{
public:
    ListBody(List* pOwner);

    void SetScrollPos(SIZE szPos);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void DoEvent(TEvent& event);
    bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);
    bool SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData, int& iCurSel);

protected:
    static int __cdecl ItemComareFunc(void *pvlocale, const void *item1, const void *item2);
    int __cdecl ItemComareFunc(const void *item1, const void *item2);

protected:
    List* m_pOwner;
    PULVCompareFunc m_pCompareFunc;
    UINT_PTR m_compareData;
};

/////////////////////////////////////////////////////////////////////////////////////
//
//

List::List() : m_pCallback(NULL), m_bScrollSelect(false), m_iCurSel(-1), m_iExpandedItem(-1)
{
    m_pList = new ListBody(this);
    m_pHeader = new ListHeader;

    Add(m_pHeader);
    VerticalLayout::Add(m_pList);

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
    m_ListInfo.bShowHtml = false;
    m_ListInfo.bMultiExpandable = false;
    ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
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
    if( _tcscmp(pstrName, DUI_CTR_ILISTOWNER) == 0 ) return static_cast<IListOwner*>(this);
    return VerticalLayout::GetInterface(pstrName);
}

Control* List::GetItemAt(int iIndex) const
{
    return m_pList->GetItemAt(iIndex);
}

int List::GetItemIndex(Control* pControl) const
{
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return VerticalLayout::GetItemIndex(pControl);
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->GetItemIndex(pControl);

    return m_pList->GetItemIndex(pControl);
}

bool List::SetItemIndex(Control* pControl, int iIndex)
{
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return VerticalLayout::SetItemIndex(pControl, iIndex);
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->SetItemIndex(pControl, iIndex);

    int iOrginIndex = m_pList->GetItemIndex(pControl);
    if( iOrginIndex == -1 ) return false;
    if( iOrginIndex == iIndex ) return true;

    IListItem* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItem*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !m_pList->SetItemIndex(pControl, iIndex) ) return false;
    int iMinIndex = min(iOrginIndex, iIndex);
    int iMaxIndex = max(iOrginIndex, iIndex);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        Control* p = m_pList->GetItemAt(i);
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
    if( pStartControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return VerticalLayout::SetMultiItemIndex(pStartControl, iCount, iNewStartIndex);
    // We also need to recognize header sub-items
    if( _tcsstr(pStartControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->SetMultiItemIndex(pStartControl, iCount, iNewStartIndex);

    int iStartIndex = GetItemIndex(pStartControl);
    if (iStartIndex == iNewStartIndex) return true;
    if (iStartIndex + iCount > GetCount()) return false;
    if (iNewStartIndex + iCount > GetCount()) return false;

    IListItem* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItem*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !m_pList->SetMultiItemIndex(pStartControl, iCount, iNewStartIndex) ) return false;
    int iMinIndex = min(iStartIndex, iNewStartIndex);
    int iMaxIndex = max(iStartIndex + iCount, iNewStartIndex + iCount);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        Control* p = m_pList->GetItemAt(i);
        IListItem* pListItem = static_cast<IListItem*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
    return true;
}

int List::GetCount() const
{
    return m_pList->GetCount();
}

bool List::Add(Control* pControl)
{
    // Override the Add() method so we can add items specifically to
    // the intended widgets. Headers are assumed to be
    // answer the correct interface so we can add multiple list headers.
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) {
        if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
            VerticalLayout::Remove(m_pHeader);
            m_pHeader = static_cast<ListHeader*>(pControl);
        }
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return VerticalLayout::AddAt(pControl, 0);
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

bool List::AddAt(Control* pControl, int iIndex)
{
    // Override the AddAt() method so we can add items specifically to
    // the intended widgets. Headers and are assumed to be
    // answer the correct interface so we can add multiple list headers.
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) {
        if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
            VerticalLayout::Remove(m_pHeader);
            m_pHeader = static_cast<ListHeader*>(pControl);
        }
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return VerticalLayout::AddAt(pControl, 0);
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
    return true;
}

bool List::Remove(Control* pControl, bool bDoNotDestroy)
{
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return VerticalLayout::Remove(pControl, bDoNotDestroy);
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

bool List::RemoveAt(int iIndex, bool bDoNotDestroy)
{
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
    return true;
}

void List::RemoveAll()
{
    m_iCurSel = -1;
    m_iExpandedItem = -1;
    m_pList->RemoveAll();
}

void List::SetPos(RECT rc, bool bNeedInvalidate)
{
	if( m_pHeader != NULL ) { // 设置header各子元素x坐标,因为有些listitem的setpos需要用到(临时修复)
		int iLeft = rc.left + m_rcPadding.left;
		int iRight = rc.right - m_rcPadding.right;

		m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

		if( !m_pHeader->IsVisible() ) {
			for( int it = m_pHeader->GetCount() - 1; it >= 0; it-- ) {
				static_cast<Control*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
			}
		}
		m_pHeader->SetPos(CDuiRect(iLeft, 0, iRight, 0), false);
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

	VerticalLayout::SetPos(rc, bNeedInvalidate);

	if( m_pHeader == NULL ) return;

	rc = m_rcItem;
	rc.left += m_rcPadding.left;
	rc.top += m_rcPadding.top;
	rc.right -= m_rcPadding.right;
	rc.bottom -= m_rcPadding.bottom;

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
		m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0), false);
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
}

void List::Move(SIZE szOffset, bool bNeedInvalidate)
{
	VerticalLayout::Move(szOffset, bNeedInvalidate);
	if( !m_pHeader->IsVisible() ) m_pHeader->Move(szOffset, false);
}

void List::DoEvent(TEvent& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else VerticalLayout::DoEvent(event);
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

    VerticalLayout::DoEvent(event);
}

ListHeader* List::GetHeader() const
{
    return m_pHeader;
}

ScrollContainer* List::GetList() const
{
    return m_pList;
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
        m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSel, iOldSel);
    }

    return true;
}

TListInfo* List::GetListInfo()
{
    return &m_ListInfo;
}

int List::GetChildMargin() const
{
    return m_pList->GetChildMargin();
}

void List::SetChildMargin(int iPadding)
{
    m_pList->SetChildMargin(iPadding);
}

UINT List::GetItemFixedHeight()
{
    return m_ListInfo.uFixedHeight;
}

void List::SetItemFixedHeight(UINT nHeight)
{
    m_ListInfo.uFixedHeight = nHeight;
    NeedUpdate();
}

int List::GetItemFont(int index)
{
    return m_ListInfo.nFont;
}

void List::SetItemFont(int index)
{
    m_ListInfo.nFont = index;
    NeedUpdate();
}

UINT List::GetItemTextStyle()
{
    return m_ListInfo.uTextStyle;
}

void List::SetItemTextStyle(UINT uStyle)
{
    m_ListInfo.uTextStyle = uStyle;
    NeedUpdate();
}

void List::SetItemTextPadding(RECT rc)
{
    m_ListInfo.rcTextPadding = rc;
    NeedUpdate();
}

RECT List::GetItemTextPadding() const
{
	return m_ListInfo.rcTextPadding;
}

void List::SetItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwTextColor = dwTextColor;
    Invalidate();
}

void List::SetItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwBkColor = dwBkColor;
    Invalidate();
}

void List::SetItemBkImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diBk.sDrawString == pStrImage && m_ListInfo.diBk.pImageInfo != NULL ) return;
	m_ListInfo.diBk.Clear();
	m_ListInfo.diBk.sDrawString = pStrImage;
	Invalidate();
}

bool List::IsAlternateBk() const
{
	return m_ListInfo.bAlternateBk;
}

void List::SetAlternateBk(bool bAlternateBk)
{
    m_ListInfo.bAlternateBk = bAlternateBk;
    Invalidate();
}

DWORD List::GetItemTextColor() const
{
	return m_ListInfo.dwTextColor;
}

DWORD List::GetItemBkColor() const
{
	return m_ListInfo.dwBkColor;
}

LPCTSTR List::GetItemBkImage() const
{
	return m_ListInfo.diBk.sDrawString.c_str();
}

void List::SetSelectedItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwSelectedTextColor = dwTextColor;
    Invalidate();
}

void List::SetSelectedItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwSelectedBkColor = dwBkColor;
    Invalidate();
}

void List::SetSelectedItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diSelected.sDrawString == pStrImage && m_ListInfo.diSelected.pImageInfo != NULL ) return;
	m_ListInfo.diSelected.Clear();
	m_ListInfo.diSelected.sDrawString = pStrImage;
	Invalidate();
}

DWORD List::GetSelectedItemTextColor() const
{
	return m_ListInfo.dwSelectedTextColor;
}

DWORD List::GetSelectedItemBkColor() const
{
	return m_ListInfo.dwSelectedBkColor;
}

LPCTSTR List::GetSelectedItemImage() const
{
	return m_ListInfo.diSelected.sDrawString.c_str();
}

void List::SetHotItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwHotTextColor = dwTextColor;
    Invalidate();
}

void List::SetHotItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwHotBkColor = dwBkColor;
    Invalidate();
}

void List::SetHotItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diHot.sDrawString == pStrImage && m_ListInfo.diHot.pImageInfo != NULL ) return;
	m_ListInfo.diHot.Clear();
	m_ListInfo.diHot.sDrawString = pStrImage;
	Invalidate();
}

DWORD List::GetHotItemTextColor() const
{
	return m_ListInfo.dwHotTextColor;
}
DWORD List::GetHotItemBkColor() const
{
	return m_ListInfo.dwHotBkColor;
}

LPCTSTR List::GetHotItemImage() const
{
	return m_ListInfo.diHot.sDrawString.c_str();
}

void List::SetDisabledItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwDisabledTextColor = dwTextColor;
    Invalidate();
}

void List::SetDisabledItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwDisabledBkColor = dwBkColor;
    Invalidate();
}

void List::SetDisabledItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diDisabled.sDrawString == pStrImage && m_ListInfo.diDisabled.pImageInfo != NULL ) return;
	m_ListInfo.diDisabled.Clear();
	m_ListInfo.diDisabled.sDrawString = pStrImage;
	Invalidate();
}

DWORD List::GetDisabledItemTextColor() const
{
	return m_ListInfo.dwDisabledTextColor;
}

DWORD List::GetDisabledItemBkColor() const
{
	return m_ListInfo.dwDisabledBkColor;
}

LPCTSTR List::GetDisabledItemImage() const
{
	return m_ListInfo.diDisabled.sDrawString.c_str();
}

int List::GetItemHLineSize() const
{
    return m_ListInfo.iHLineSize;
}

void List::SetItemHLineSize(int iSize)
{
    m_ListInfo.iHLineSize = iSize;
    Invalidate();
}

DWORD List::GetItemHLineColor() const
{
    return m_ListInfo.dwHLineColor;
}

void List::SetItemHLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwHLineColor = dwLineColor;
    Invalidate();
}

int List::GetItemVLineSize() const
{
    return m_ListInfo.iVLineSize;
}

void List::SetItemVLineSize(int iSize)
{
    m_ListInfo.iVLineSize = iSize;
    Invalidate();
}

DWORD List::GetItemVLineColor() const
{
    return m_ListInfo.dwVLineColor;
}

void List::SetItemVLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwVLineColor = dwLineColor;
    Invalidate();
}

bool List::IsItemShowHtml()
{
    return m_ListInfo.bShowHtml;
}

void List::SetItemShowHtml(bool bShowHtml)
{
    if( m_ListInfo.bShowHtml == bShowHtml ) return;

    m_ListInfo.bShowHtml = bShowHtml;
    NeedUpdate();
}

void List::SetMultiExpanding(bool bMultiExpandable)
{
    m_ListInfo.bMultiExpandable = bMultiExpandable;
}

bool List::ExpandItem(int iIndex, bool bExpand /*= true*/)
{
    if( m_iExpandedItem >= 0 && !m_ListInfo.bMultiExpandable) {
        Control* pControl = GetItemAt(m_iExpandedItem);
        if( pControl != NULL ) {
            IListItem* pItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
            if( pItem != NULL ) pItem->Expand(false);
        }
        m_iExpandedItem = -1;
    }
    if( bExpand ) {
        Control* pControl = GetItemAt(iIndex);
        if( pControl == NULL ) return false;
        if( !pControl->IsVisible() ) return false;
        IListItem* pItem = static_cast<IListItem*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
        if( pItem == NULL ) return false;
        m_iExpandedItem = iIndex;
        if( !pItem->Expand(true) ) {
            m_iExpandedItem = -1;
            return false;
        }
    }
    NeedUpdate();
    return true;
}

int List::GetExpandedItem() const
{
    return m_iExpandedItem;
}

void List::EnsureVisible(int iIndex)
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

void List::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = m_pList->GetScrollPos();
    m_pList->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
}

void List::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("header")) == 0 ) GetHeader()->SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("headerbkimage")) == 0 ) GetHeader()->SetBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("scrollselect")) == 0 ) SetScrollSelect(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("multiexpanding")) == 0 ) SetMultiExpanding(_tcscmp(pstrValue, _T("true")) == 0);
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
    else if( _tcscmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
    else VerticalLayout::SetAttribute(pstrName, pstrValue);
}

IListCallback* List::GetTextCallback() const
{
    return m_pCallback;
}

void List::SetTextCallback(IListCallback* pCallback)
{
    m_pCallback = pCallback;
}

SIZE List::GetScrollPos() const
{
    return m_pList->GetScrollPos();
}

SIZE List::GetScrollRange() const
{
    return m_pList->GetScrollRange();
}

void List::SetScrollPos(SIZE szPos)
{
    m_pList->SetScrollPos(szPos);
}

void List::LineUp()
{
    m_pList->LineUp();
}

void List::LineDown()
{
    m_pList->LineDown();
}

void List::PageUp()
{
    m_pList->PageUp();
}

void List::PageDown()
{
    m_pList->PageDown();
}

void List::HomeUp()
{
    m_pList->HomeUp();
}

void List::EndDown()
{
    m_pList->EndDown();
}

void List::LineLeft()
{
    m_pList->LineLeft();
}

void List::LineRight()
{
    m_pList->LineRight();
}

void List::PageLeft()
{
    m_pList->PageLeft();
}

void List::PageRight()
{
    m_pList->PageRight();
}

void List::HomeLeft()
{
    m_pList->HomeLeft();
}

void List::EndRight()
{
    m_pList->EndRight();
}

void List::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
{
    m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);
}

ScrollBar* List::GetVerticalScrollBar() const
{
    return m_pList->GetVerticalScrollBar();
}

ScrollBar* List::GetHorizontalScrollBar() const
{
    return m_pList->GetHorizontalScrollBar();
}

bool List::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData)
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

ListBody::ListBody(List* pOwner) : m_pOwner(pOwner)
{
    ASSERT(m_pOwner);
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
		pItem = (IListItem*)(static_cast<Control*>(m_items[i])->GetInterface(TEXT("ListItem")));
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
		pControl->Move(CDuiSize(-cx, -cy), false);
    }

    Invalidate();

    if( cx != 0 && m_pOwner ) {
        ListHeader* pHeader = m_pOwner->GetHeader();
        if( pHeader == NULL ) return;
        TListInfo* pInfo = m_pOwner->GetListInfo();
        pInfo->nColumns = MIN(pHeader->GetCount(), UILIST_MAX_COLUMNS);
        for( int i = 0; i < pInfo->nColumns; i++ ) {
            Control* pControl = static_cast<Control*>(pHeader->GetItemAt(i));
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) continue;
			pControl->Move(CDuiSize(-cx, -cy), false);
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
    rc.left += m_rcPadding.left;
    rc.top += m_rcPadding.top;
    rc.right -= m_rcPadding.right;
    rc.bottom -= m_rcPadding.bottom;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

    // Determine the minimum size
    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
        szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

	int iChildMargin = m_iChildMargin;
    TListInfo* pInfo = NULL;
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
            cxNeeded = MAX(0, pHeader->EstimateSize(CDuiSize(rc.right - rc.left, rc.bottom - rc.top)).cx);
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

void ListBody::DoEvent(TEvent& event)
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
						m_pOwner->LineLeft();
						return;
					case SB_LINEDOWN:
						m_pOwner->LineRight();
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

	TListInfo* pListInfo = NULL;
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

/////////////////////////////////////////////////////////////////////////////////////
//
//

ListHeader::ListHeader()
{
}

LPCTSTR ListHeader::GetClass() const
{
    return DUI_CTR_LISTHEADER;
}

LPVOID ListHeader::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_LISTHEADER) == 0 ) return this;
    return HorizontalLayout::GetInterface(pstrName);
}

SIZE ListHeader::EstimateSize(SIZE szAvailable)
{
    SIZE cXY = {0, m_cxyFixed.cy};
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

ListHeaderItem::ListHeaderItem() : m_bDragable(true), m_uButtonState(0), m_iSepWidth(4),
m_uTextStyle(DT_CENTER | DT_VCENTER | DT_SINGLELINE), m_dwTextColor(0), m_dwSepColor(0), 
m_iFont(-1), m_bShowHtml(false)
{
	SetTextPadding(CDuiRect(2, 0, 2, 0));
    ptLastMouse.x = ptLastMouse.y = 0;
    SetMinWidth(16);
}

LPCTSTR ListHeaderItem::GetClass() const
{
    return DUI_CTR_LISTHEADERITEM;
}

LPVOID ListHeaderItem::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_LISTHEADERITEM) == 0 ) return this;
    return Control::GetInterface(pstrName);
}

UINT ListHeaderItem::GetControlFlags() const
{
    if( IsEnabled() && m_iSepWidth != 0 ) return UIFLAG_SETCURSOR;
    else return 0;
}

void ListHeaderItem::SetEnabled(bool bEnable)
{
    Control::SetEnabled(bEnable);
    if( !IsEnabled() ) {
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
    if ( !m_bDragable ) m_uButtonState &= ~UISTATE_CAPTURED;
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
    if( m_bShowHtml == bShowHtml ) return;

    m_bShowHtml = bShowHtml;
    Invalidate();
}

LPCTSTR ListHeaderItem::GetNormalImage() const
{
	return m_diNormal.sDrawString.c_str();
}

void ListHeaderItem::SetNormalImage(LPCTSTR pStrImage)
{
	if( m_diNormal.sDrawString == pStrImage && m_diNormal.pImageInfo != NULL ) return;
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
	if( m_diHot.sDrawString == pStrImage && m_diHot.pImageInfo != NULL ) return;
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
	if( m_diPushed.sDrawString == pStrImage && m_diPushed.pImageInfo != NULL ) return;
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
	if( m_diFocused.sDrawString == pStrImage && m_diFocused.pImageInfo != NULL ) return;
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
	if( m_diSep.sDrawString == pStrImage && m_diSep.pImageInfo != NULL ) return;
	m_diSep.Clear();
	m_diSep.sDrawString = pStrImage;
	Invalidate();
}

void ListHeaderItem::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("dragable")) == 0 ) SetDragable(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("halign")) == 0 ) {
        if( _tcsstr(pstrValue, _T("left")) != NULL ) {
            m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_uTextStyle |= DT_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) {
            m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_uTextStyle |= DT_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL ) {
            m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_uTextStyle |= DT_RIGHT;
        }
    }
    else if (_tcscmp(pstrName, _T("valign")) == 0)
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
    else if( _tcscmp(pstrName, _T("endellipsis")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_uTextStyle |= DT_END_ELLIPSIS;
        else m_uTextStyle &= ~DT_END_ELLIPSIS;
    }  
    else if( _tcscmp(pstrName, _T("multiline")) == 0 ) {
        if (_tcscmp(pstrValue, _T("true")) == 0) {
            m_uTextStyle  &= ~DT_SINGLELINE;
            m_uTextStyle |= DT_WORDBREAK;
        }
        else m_uTextStyle |= DT_SINGLELINE;
    }
    else if( _tcscmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
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
	else if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
		RECT rcTextPadding = { 0 };
		LPTSTR pstr = NULL;
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetTextPadding(rcTextPadding);
	}
    else if( _tcscmp(pstrName, _T("showhtml")) == 0 ) SetShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
    else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
    else if( _tcscmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("sepwidth")) == 0 ) SetSepWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("sepcolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetSepColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("sepimage")) == 0 ) SetSepImage(pstrValue);
    else Control::SetAttribute(pstrName, pstrValue);
}

void ListHeaderItem::DoEvent(TEvent& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else Control::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        Invalidate();
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        Invalidate();
    }
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
    {
        if( !IsEnabled() ) return;
        RECT rcSeparator = GetThumbRect();
        if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
            if( m_bDragable ) {
                m_uButtonState |= UISTATE_CAPTURED;
                ptLastMouse = event.ptMouse;
            }
        }
        else {
            m_uButtonState |= UISTATE_PUSHED;
            m_pManager->SendNotify(this, DUI_MSGTYPE_HEADERCLICK);
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            m_uButtonState &= ~UISTATE_CAPTURED;
            if( GetParent() ) 
                GetParent()->NeedParentUpdate();
        }
        else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
            m_uButtonState &= ~UISTATE_PUSHED;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            RECT rc = m_rcItem;
            if( m_iSepWidth >= 0 ) {
                rc.right -= ptLastMouse.x - event.ptMouse.x;
            }
            else {
                rc.left -= ptLastMouse.x - event.ptMouse.x;
            }
            
            if( rc.right - rc.left > GetMinWidth() ) {
                m_cxyFixed.cx = rc.right - rc.left;
                ptLastMouse = event.ptMouse;
                if( GetParent() ) 
                    GetParent()->NeedParentUpdate();
            }
        }
        return;
    }
    if( event.Type == UIEVENT_SETCURSOR )
    {
        RECT rcSeparator = GetThumbRect();
        if( IsEnabled() && m_bDragable && ::PtInRect(&rcSeparator, event.ptMouse) ) {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
            return;
        }
    }
    if( event.Type == UIEVENT_MOUSEENTER )
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
    if( event.Type == UIEVENT_MOUSELEAVE )
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

SIZE ListHeaderItem::EstimateSize(SIZE szAvailable)
{
    if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8);
    return Control::EstimateSize(szAvailable);
}

RECT ListHeaderItem::GetThumbRect() const
{
    if( m_iSepWidth >= 0 ) return CDuiRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
    else return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
}

void ListHeaderItem::PaintStatusImage(HDC hDC)
{
	if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;

	if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
		if( !DrawImage(hDC, m_diPushed) )  DrawImage(hDC, m_diNormal);
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( !DrawImage(hDC, m_diHot) )  DrawImage(hDC, m_diNormal);
	}
	else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
		if( !DrawImage(hDC, m_diFocused) )  DrawImage(hDC, m_diNormal);
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
        if( !DrawImage(hDC, m_diSep) ) {
            if (m_dwSepColor != 0) {
                RECT rcSepLine = { rcThumb.left + m_iSepWidth/2, rcThumb.top, rcThumb.left + m_iSepWidth/2, rcThumb.bottom};
                CRenderEngine::DrawLine(hDC, rcSepLine, m_iSepWidth, GetAdjustColor(m_dwSepColor));
            }
        }
    }
	DrawImage(hDC, m_diFore);
}

void ListHeaderItem::PaintText(HDC hDC)
{
    if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();

	RECT rcText = m_rcItem;
	rcText.left += m_rcTextPadding.left;
	rcText.top += m_rcTextPadding.top;
	rcText.right -= m_rcTextPadding.right;
	rcText.bottom -= m_rcTextPadding.bottom;

	if (m_sText.empty()) return;
    int nLinks = 0;
    if( m_bShowHtml )
		CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText.c_str(), m_dwTextColor, \
        NULL, NULL, nLinks, m_iFont, m_uTextStyle);
    else
		CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText.c_str(), m_dwTextColor, \
        m_iFont, m_uTextStyle);
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
    if( _tcscmp(pstrName, DUI_CTR_ILISTITEM) == 0 ) return static_cast<IListItem*>(this);
	if( _tcscmp(pstrName, DUI_CTR_LISTELEMENT) == 0 ) return static_cast<ListElement*>(this);
    return Control::GetInterface(pstrName);
}

IListOwner* ListElement::GetOwner()
{
    return m_pOwner;
}

void ListElement::SetOwner(Control* pOwner)
{
    if (pOwner != NULL) m_pOwner = static_cast<IListOwner*>(pOwner->GetInterface(DUI_CTR_ILISTOWNER));
}

void ListElement::SetVisible(bool bVisible)
{
    Control::SetVisible(bVisible);
    if( !IsVisible() && m_bSelected)
    {
        m_bSelected = false;
        if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
    }
}

void ListElement::SetEnabled(bool bEnable)
{
    Control::SetEnabled(bEnable);
    if( !IsEnabled() ) {
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
    if( !IsVisible() ) return;

    if( GetParent() ) {
        ScrollContainer* pParentContainer = static_cast<ScrollContainer*>(GetParent()->GetInterface(DUI_CTR_SCROLLCONTAINER));
        if( pParentContainer ) {
            RECT rc = pParentContainer->GetPos();
            RECT rcPadding = pParentContainer->GetPadding();
            rc.left += rcPadding.left;
            rc.top += rcPadding.top;
            rc.right -= rcPadding.right;
            rc.bottom -= rcPadding.bottom;
            ScrollBar* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
            if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
            ScrollBar* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
            if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

            RECT invalidateRc = m_rcItem;
            if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
            {
                return;
            }

            Control* pParent = GetParent();
            RECT rcTemp;
            RECT rcParent;
            while( pParent = pParent->GetParent() )
            {
                rcTemp = invalidateRc;
                rcParent = pParent->GetPos();
                if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
                {
                    return;
                }
            }

            if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
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
    if( !Control::Activate() ) return false;
    if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
    return true;
}

bool ListElement::IsSelected() const
{
    return m_bSelected;
}

bool ListElement::Select(bool bSelect, bool bTriggerEvent)
{
    if( !IsEnabled() ) return false;
    if( bSelect == m_bSelected ) return true;
    m_bSelected = bSelect;
    if( bSelect && m_pOwner != NULL ) m_pOwner->SelectItem(m_iIndex, bTriggerEvent);
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
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else Control::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_DBLCLICK )
    {
        if( IsEnabled() ) {
            Activate();
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_KEYDOWN )
    {
        if (IsKeyboardEnabled() && IsEnabled()) {
            if( event.chKey == VK_RETURN ) {
                Activate();
                Invalidate();
                return;
            }
        }
    }
    // An important twist: The list-item will send the event not to its immediate
    // parent but to the "attached" list. A list may actually embed several components
    // in its path to the item, but key-presses etc. needs to go to the actual list.
    if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else Control::DoEvent(event);
}

void ListElement::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("selected")) == 0 ) Select();
    else Control::SetAttribute(pstrName, pstrValue);
}

void ListElement::DrawItemBk(HDC hDC, const RECT& rcItem)
{
    ASSERT(m_pOwner);
    if( m_pOwner == NULL ) return;
    TListInfo* pInfo = m_pOwner->GetListInfo();
    if( pInfo == NULL ) return;
    DWORD iBackColor = 0;
    if( !pInfo->bAlternateBk || m_iDrawIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;
    if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        iBackColor = pInfo->dwHotBkColor;
    }
    if( IsSelected() ) {
        iBackColor = pInfo->dwSelectedBkColor;
    }
    if( !IsEnabled() ) {
        iBackColor = pInfo->dwDisabledBkColor;
    }

    if ( iBackColor != 0 ) {
        CRenderEngine::DrawColor(hDC, rcItem, GetAdjustColor(iBackColor));
    }

    if( !IsEnabled() ) {
        if( DrawImage(hDC, pInfo->diDisabled) ) return;
    }
    if( IsSelected() ) {
        if( DrawImage(hDC, pInfo->diSelected) ) return;
    }
    if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        if( DrawImage(hDC, pInfo->diHot) ) return;
    }

    if( !DrawImage(hDC, m_diBk) ) {
        if( !pInfo->bAlternateBk || m_iDrawIndex % 2 == 0 ) {
            if( DrawImage(hDC, pInfo->diBk) ) return;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

ListLabelElement::ListLabelElement() : m_bNeedEstimateSize(true), m_uFixedHeightLast(0),
    m_nFontLast(-1), m_uTextStyleLast(0)
{
    m_szAvailableLast.cx = m_szAvailableLast.cy = 0;
    m_cxyFixedLast.cx = m_cxyFixedLast.cy = 0;
    ::ZeroMemory(&m_rcTextPaddingLast, sizeof(m_rcTextPaddingLast));
}

LPCTSTR ListLabelElement::GetClass() const
{
    return DUI_CTR_LISTLABELELEMENT;
}

LPVOID ListLabelElement::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_LISTLABELELEMENT) == 0 ) return static_cast<ListLabelElement*>(this);
    return ListElement::GetInterface(pstrName);
}

void ListLabelElement::SetOwner(Control* pOwner)
{
    m_bNeedEstimateSize = true;
    ListElement::SetOwner(pOwner);
}

void ListLabelElement::SetFixedWidth(int cx)
{
    m_bNeedEstimateSize = true;
    Control::SetFixedWidth(cx);
}

void ListLabelElement::SetFixedHeight(int cy)
{
    m_bNeedEstimateSize = true;
    Control::SetFixedHeight(cy);
}

void ListLabelElement::SetText(LPCTSTR pstrText)
{
    m_bNeedEstimateSize = true;
    Control::SetText(pstrText);
}

void ListLabelElement::DoEvent(TEvent& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else ListElement::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
        if( IsEnabled() ) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
            Select();
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE ) 
    {
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
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
    if( event.Type == UIEVENT_MOUSELEAVE )
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
    ListElement::DoEvent(event);
}

SIZE ListLabelElement::EstimateSize(SIZE szAvailable)
{
    if( m_pOwner == NULL ) return CDuiSize(0, 0);
    TListInfo* pInfo = m_pOwner->GetListInfo();
    if (pInfo == NULL) return CDuiSize(0, 0);
    if (m_cxyFixed.cx > 0) {
        if (m_cxyFixed.cy > 0) return m_cxyFixed;
        else if (pInfo->uFixedHeight > 0) return CDuiSize(m_cxyFixed.cx, pInfo->uFixedHeight);
    }

    if ((pInfo->uTextStyle & DT_SINGLELINE) == 0 && 
        (szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy)) {
            m_bNeedEstimateSize = true;
    }
    if (m_uFixedHeightLast != pInfo->uFixedHeight || m_nFontLast != pInfo->nFont ||
        m_uTextStyleLast != pInfo->uTextStyle ||
        m_rcTextPaddingLast.left != pInfo->rcTextPadding.left || m_rcTextPaddingLast.right != pInfo->rcTextPadding.right ||
        m_rcTextPaddingLast.top != pInfo->rcTextPadding.top || m_rcTextPaddingLast.bottom != pInfo->rcTextPadding.bottom) {
            m_bNeedEstimateSize = true;
    }

    if (m_bNeedEstimateSize) {
        m_bNeedEstimateSize = false;
        m_szAvailableLast = szAvailable;
        m_uFixedHeightLast = pInfo->uFixedHeight;
        m_nFontLast = pInfo->nFont;
        m_uTextStyleLast = pInfo->uTextStyle;
        m_rcTextPaddingLast = pInfo->rcTextPadding;
        
        m_cxyFixedLast = m_cxyFixed;
        if (m_cxyFixedLast.cy == 0) {
            m_cxyFixedLast.cy = pInfo->uFixedHeight;
        }

        if ((pInfo->uTextStyle & DT_SINGLELINE) != 0) {
            if( m_cxyFixedLast.cy == 0 ) {
                m_cxyFixedLast.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
                m_cxyFixedLast.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
            }
            if (m_cxyFixedLast.cx == 0) {
                RECT rcText = { 0, 0, 9999, m_cxyFixedLast.cy };
                if( pInfo->bShowHtml ) {
                    int nLinks = 0;
					CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), 0, NULL, NULL, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                else {
					CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), 0, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                m_cxyFixedLast.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;
            }
        }
        else {
            if( m_cxyFixedLast.cx == 0 ) {
                m_cxyFixedLast.cx = szAvailable.cx;
            }
            RECT rcText = { 0, 0, m_cxyFixedLast.cx, 9999 };
            rcText.left += pInfo->rcTextPadding.left;
            rcText.right -= pInfo->rcTextPadding.right;
            if( pInfo->bShowHtml ) {
                int nLinks = 0;
				CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), 0, NULL, NULL, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            else {
				CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), 0, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            m_cxyFixedLast.cy = rcText.bottom - rcText.top + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
        }
    }
    return m_cxyFixedLast;
}

bool ListLabelElement::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
    DrawItemBk(hDC, m_rcItem);
    DrawItemText(hDC, m_rcItem);
	PaintStatusImage(hDC);
    return true;
}

void ListLabelElement::DrawItemText(HDC hDC, const RECT& rcItem)
{
	if (m_sText.empty()) return;

    if( m_pOwner == NULL ) return;
    TListInfo* pInfo = m_pOwner->GetListInfo();
    if( pInfo == NULL ) return;
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
        NULL, NULL, nLinks, pInfo->nFont, pInfo->uTextStyle);
    else
		CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText.c_str(), iTextColor, \
        pInfo->nFont, pInfo->uTextStyle);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

ListTextElement::ListTextElement() : m_nLinks(0), m_nHoverLink(-1), m_pOwner(NULL)
{
    ::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
}

ListTextElement::~ListTextElement()
{
    String* pText;
    for( int it = 0; it < m_aTexts.GetSize(); it++ ) {
        pText = static_cast<String*>(m_aTexts[it]);
        if( pText ) delete pText;
    }
    m_aTexts.Empty();
}

LPCTSTR ListTextElement::GetClass() const
{
    return DUI_CTR_LISTTEXTELEMENT;
}

LPVOID ListTextElement::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_LISTTEXTELEMENT) == 0 ) return static_cast<ListTextElement*>(this);
    return ListLabelElement::GetInterface(pstrName);
}

UINT ListTextElement::GetControlFlags() const
{
    return UIFLAG_WANTRETURN | ( (IsEnabled() && m_nLinks > 0) ? UIFLAG_SETCURSOR : 0);
}

LPCTSTR ListTextElement::GetText(int iIndex) const
{
    String* pText = static_cast<String*>(m_aTexts.GetAt(iIndex));
    if( pText ) return pText->c_str();
    return NULL;
}

void ListTextElement::SetText(int iIndex, LPCTSTR pstrText)
{
    if( m_pOwner == NULL ) return;
    TListInfo* pInfo = m_pOwner->GetListInfo();
    if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
    m_bNeedEstimateSize = true;
    
    while( m_aTexts.GetSize() < pInfo->nColumns ) { m_aTexts.Add(NULL); }

    String* pText = static_cast<String*>(m_aTexts[iIndex]);
    if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

	if ( pText ) //by cddjr 2011/10/20
		pText->assign(pstrText);
	else
		m_aTexts.SetAt(iIndex, new String(pstrText));
    Invalidate();
}

void ListTextElement::SetOwner(Control* pOwner)
{
    if (pOwner != NULL) {
        m_bNeedEstimateSize = true;
        ListElement::SetOwner(pOwner);
        m_pOwner = static_cast<IList*>(pOwner->GetInterface(DUI_CTR_ILIST));
    }
}

String* ListTextElement::GetLinkContent(int iIndex)
{
    if( iIndex >= 0 && iIndex < m_nLinks ) return &m_sLinks[iIndex];
    return NULL;
}

void ListTextElement::DoEvent(TEvent& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else ListLabelElement::DoEvent(event);
        return;
    }

    // When you hover over a link
    if( event.Type == UIEVENT_SETCURSOR ) {
        for( int i = 0; i < m_nLinks; i++ ) {
            if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
                return;
            }
        }      
    }
    if( event.Type == UIEVENT_BUTTONUP && IsEnabled() ) {
        for( int i = 0; i < m_nLinks; i++ ) {
            if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
                m_pManager->SendNotify(this, DUI_MSGTYPE_LINK, i);
                return;
            }
        }
    }
    if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE ) {
        int nHoverLink = -1;
        for( int i = 0; i < m_nLinks; i++ ) {
            if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
                nHoverLink = i;
                break;
            }
        }

        if(m_nHoverLink != nHoverLink) {
            Invalidate();
            m_nHoverLink = nHoverLink;
        }
    }
    if( m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE ) {
        if(m_nHoverLink != -1) {
            if( !::PtInRect(&m_rcLinks[m_nHoverLink], event.ptMouse) ) {
                m_nHoverLink = -1;
                Invalidate();
                if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
            }
            else {
                if (m_pManager) m_pManager->AddMouseLeaveNeeded(this);
                return;
            }
        }
    }
    ListLabelElement::DoEvent(event);
}

SIZE ListTextElement::EstimateSize(SIZE szAvailable)
{
    if( m_pOwner == NULL ) return CDuiSize(0, 0);
    TListInfo* pInfo = m_pOwner->GetListInfo();
    if (pInfo == NULL) return CDuiSize(0, 0);
    SIZE cxyFixed = m_cxyFixed;
    if (cxyFixed.cx == 0 && pInfo->nColumns > 0) {
        cxyFixed.cx = pInfo->rcColumn[pInfo->nColumns - 1].right - pInfo->rcColumn[0].left;
        if (m_cxyFixedLast.cx != cxyFixed.cx) m_bNeedEstimateSize = true;
    }
    if (cxyFixed.cx > 0) {
        if (cxyFixed.cy > 0) return cxyFixed;
        else if (pInfo->uFixedHeight > 0) return CDuiSize(cxyFixed.cx, pInfo->uFixedHeight);
    }
    
    if ((pInfo->uTextStyle & DT_SINGLELINE) == 0 && 
        (szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy)) {
            m_bNeedEstimateSize = true;
    }
    if (m_uFixedHeightLast != pInfo->uFixedHeight || m_nFontLast != pInfo->nFont ||
        m_uTextStyleLast != pInfo->uTextStyle ||
        m_rcTextPaddingLast.left != pInfo->rcTextPadding.left || m_rcTextPaddingLast.right != pInfo->rcTextPadding.right ||
        m_rcTextPaddingLast.top != pInfo->rcTextPadding.top || m_rcTextPaddingLast.bottom != pInfo->rcTextPadding.bottom) {
            m_bNeedEstimateSize = true;
    }

    String strText;
    IListCallback* pCallback = m_pOwner->GetTextCallback();
    if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, 0);
	else if (m_aTexts.GetSize() > 0) strText.assign(GetText(0));
    else strText = m_sText;
    if (m_sTextLast != strText) m_bNeedEstimateSize = true;

    if (m_bNeedEstimateSize) {
        m_bNeedEstimateSize = false;
        m_szAvailableLast = szAvailable;
        m_uFixedHeightLast = pInfo->uFixedHeight;
        m_nFontLast = pInfo->nFont;
        m_uTextStyleLast = pInfo->uTextStyle;
        m_rcTextPaddingLast = pInfo->rcTextPadding;
        m_sTextLast = strText;

        m_cxyFixedLast = m_cxyFixed;
        if (m_cxyFixedLast.cx == 0 && pInfo->nColumns > 0) {
            m_cxyFixedLast.cx = pInfo->rcColumn[pInfo->nColumns - 1].right - pInfo->rcColumn[0].left;
        }
        if (m_cxyFixedLast.cy == 0) {
            m_cxyFixedLast.cy = pInfo->uFixedHeight;
        }

        if ((pInfo->uTextStyle & DT_SINGLELINE) != 0) {
            if( m_cxyFixedLast.cy == 0 ) {
                m_cxyFixedLast.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
                m_cxyFixedLast.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
            }
            if (m_cxyFixedLast.cx == 0) {
                RECT rcText = { 0, 0, 9999, m_cxyFixedLast.cy };
                if( pInfo->bShowHtml ) {
                    int nLinks = 0;
					CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, strText.c_str(), 0, NULL, NULL, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                else {
					CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, strText.c_str(), 0, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                m_cxyFixedLast.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;
            }
        }
        else {
            if( m_cxyFixedLast.cx == 0 ) {
                m_cxyFixedLast.cx = szAvailable.cx;
            }
            RECT rcText = { 0, 0, m_cxyFixedLast.cx, 9999 };
            rcText.left += pInfo->rcTextPadding.left;
            rcText.right -= pInfo->rcTextPadding.right;
            if( pInfo->bShowHtml ) {
                int nLinks = 0;
				CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, strText.c_str(), 0, NULL, NULL, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            else {
				CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, strText.c_str(), 0, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            m_cxyFixedLast.cy = rcText.bottom - rcText.top + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
        }
    }
    return m_cxyFixedLast;
}

void ListTextElement::PaintStatusImage(HDC hDC)
{
	DrawImage(hDC, m_diFore);
}

void ListTextElement::DrawItemText(HDC hDC, const RECT& rcItem)
{
    if( m_pOwner == NULL ) return;
    TListInfo* pInfo = m_pOwner->GetListInfo();
    if (pInfo == NULL) return;
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
    IListCallback* pCallback = m_pOwner->GetTextCallback();

    m_nLinks = 0;
    int nLinks = lengthof(m_rcLinks);
    if (pInfo->nColumns > 0) {
        for( int i = 0; i < pInfo->nColumns; i++ )
        {
            RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };
            if (pInfo->iVLineSize > 0 && i < pInfo->nColumns - 1) {
                RECT rcLine = { rcItem.right - pInfo->iVLineSize / 2, rcItem.top, rcItem.right - pInfo->iVLineSize / 2, rcItem.bottom};
                CRenderEngine::DrawLine(hDC, rcLine, pInfo->iVLineSize, GetAdjustColor(pInfo->dwVLineColor));
                rcItem.right -= pInfo->iVLineSize;
            }

			rcItem.left += pInfo->rcTextPadding.left;
			rcItem.right -= pInfo->rcTextPadding.right;
			rcItem.top += pInfo->rcTextPadding.top;
			rcItem.bottom -= pInfo->rcTextPadding.bottom;

            String strText;//不使用LPCTSTR，否则限制太多 by cddjr 2011/10/20
            if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);
			else strText.assign(GetText(i));
            if( pInfo->bShowHtml )
                CRenderEngine::DrawHtmlText(hDC, m_pManager, rcItem, strText.c_str(), iTextColor, \
                &m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], nLinks, pInfo->nFont, pInfo->uTextStyle);
            else
                CRenderEngine::DrawText(hDC, m_pManager, rcItem, strText.c_str(), iTextColor, \
                pInfo->nFont, pInfo->uTextStyle);

            m_nLinks += nLinks;
            nLinks = lengthof(m_rcLinks) - m_nLinks; 
        }
    }
    else {
        RECT rcItem = m_rcItem;
        rcItem.left += pInfo->rcTextPadding.left;
        rcItem.right -= pInfo->rcTextPadding.right;
        rcItem.top += pInfo->rcTextPadding.top;
        rcItem.bottom -= pInfo->rcTextPadding.bottom;

        String strText;
        if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, 0);
		else if (m_aTexts.GetSize() > 0) strText.assign(GetText(0));
        else strText = m_sText;
        if( pInfo->bShowHtml )
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rcItem, strText.c_str(), iTextColor, \
            &m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], nLinks, pInfo->nFont, pInfo->uTextStyle);
        else
            CRenderEngine::DrawText(hDC, m_pManager, rcItem, strText.c_str(), iTextColor, \
            pInfo->nFont, pInfo->uTextStyle);

        m_nLinks += nLinks;
        nLinks = lengthof(m_rcLinks) - m_nLinks; 
    }

    for( int i = m_nLinks; i < lengthof(m_rcLinks); i++ ) {
        ::ZeroMemory(m_rcLinks + i, sizeof(RECT));
		((String*)(m_sLinks + i))->clear();
    }
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

ListContainerElement::ListContainerElement() : 
m_iIndex(-1),
m_iDrawIndex(0),
m_pOwner(NULL), 
m_bSelected(false),
m_bExpandable(false),
m_bExpand(false),
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
    if( _tcscmp(pstrName, DUI_CTR_ILISTITEM) == 0 ) return static_cast<IListItem*>(this);
	if( _tcscmp(pstrName, DUI_CTR_LISTCONTAINERELEMENT) == 0 ) return static_cast<ListContainerElement*>(this);
    return ScrollContainer::GetInterface(pstrName);
}

IListOwner* ListContainerElement::GetOwner()
{
    return m_pOwner;
}

void ListContainerElement::SetOwner(Control* pOwner)
{
    if (pOwner != NULL) m_pOwner = static_cast<IListOwner*>(pOwner->GetInterface(DUI_CTR_ILISTOWNER));
}

void ListContainerElement::SetVisible(bool bVisible)
{
    ScrollContainer::SetVisible(bVisible);
    if( !IsVisible() && m_bSelected)
    {
        m_bSelected = false;
        if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
    }
}

void ListContainerElement::SetEnabled(bool bEnable)
{
    Control::SetEnabled(bEnable);
    if( !IsEnabled() ) {
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

int ListContainerElement::GetDrawIndex() const
{
    return m_iDrawIndex;
}

void ListContainerElement::SetDrawIndex(int iIndex)
{
    m_iDrawIndex = iIndex;
}

void ListContainerElement::Invalidate()
{
    if( !IsVisible() ) return;

    if( GetParent() ) {
        ScrollContainer* pParentContainer = static_cast<ScrollContainer*>(GetParent()->GetInterface(DUI_CTR_SCROLLCONTAINER));
        if( pParentContainer ) {
            RECT rc = pParentContainer->GetPos();
            RECT rcPadding = pParentContainer->GetPadding();
            rc.left += rcPadding.left;
            rc.top += rcPadding.top;
            rc.right -= rcPadding.right;
            rc.bottom -= rcPadding.bottom;
            ScrollBar* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
            if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
            ScrollBar* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
            if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

            RECT invalidateRc = m_rcItem;
            if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
            {
                return;
            }

            Control* pParent = GetParent();
            RECT rcTemp;
            RECT rcParent;
            while( pParent = pParent->GetParent() )
            {
                rcTemp = invalidateRc;
                rcParent = pParent->GetPos();
                if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
                {
                    return;
                }
            }

            if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
        }
        else {
            ScrollContainer::Invalidate();
        }
    }
    else {
        ScrollContainer::Invalidate();
    }
}

bool ListContainerElement::Activate()
{
    if( !ScrollContainer::Activate() ) return false;
#if MODE_EVENTMAP
	if (OnEvent.find(UIEVENT_ITEMCLICK) != OnEvent.cend()){
		TEvent event;
		event.Type = UIEVENT_ITEMCLICK;
		event.pSender = this;
		if (!OnEvent.find(UIEVENT_ITEMCLICK)->second(&event)){
			return false;
		}
	}
#endif
	if (m_pManager != NULL) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
    return true;
}

bool ListContainerElement::IsSelected() const
{
    return m_bSelected;
}

bool ListContainerElement::Select(bool bSelect, bool bTriggerEvent)
{
    if( !IsEnabled() ) return false;
    if( bSelect == m_bSelected ) return true;
    m_bSelected = bSelect;
    if( bSelect && m_pOwner != NULL ) m_pOwner->SelectItem(m_iIndex, bTriggerEvent);
    Invalidate();

    return true;
}

bool ListContainerElement::IsExpandable() const
{
    return m_bExpandable;
}

void ListContainerElement::SetExpandable(bool bExpandable)
{
    m_bExpandable = bExpandable;
}

bool ListContainerElement::IsExpanded() const
{
    return m_bExpand;
}

bool ListContainerElement::Expand(bool bExpand)
{
    ASSERT(m_pOwner);
    if( m_pOwner == NULL ) return false;  
    if( bExpand == m_bExpand ) return true;
    m_bExpand = bExpand;
    if( m_bExpandable ) {
        if( !m_pOwner->ExpandItem(m_iIndex, bExpand) ) return false;
        if( m_pManager != NULL ) {
            if( bExpand ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMEXPAND, false);
            else m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCOLLAPSE, false);
        }
    }

    return true;
}

void ListContainerElement::DoEvent(TEvent& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else ScrollContainer::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_DBLCLICK )
    {
#if 0
        if( IsEnabled() ) {
            Activate();
            Invalidate();
        }
#endif
        return;
    }
    if( event.Type == UIEVENT_KEYDOWN )
    {
#if 0
        if (IsKeyboardEnabled() && IsEnabled()) {
            if( event.chKey == VK_RETURN ) {
                Activate();
                Invalidate();
                return;
            }
        }
#endif
    }
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
        if( IsEnabled() ) {
#if MODE_EVENTMAP
			Activate();
#else
            m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
#endif	
            Select();
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP ) 
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
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
    if( event.Type == UIEVENT_MOUSELEAVE )
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

    // An important twist: The list-item will send the event not to its immediate
    // parent but to the "attached" list. A list may actually embed several components
    // in its path to the item, but key-presses etc. needs to go to the actual list.
    if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else Control::DoEvent(event);
}

void ListContainerElement::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("selected")) == 0 ) Select();
	else if( _tcscmp(pstrName, _T("expandable")) == 0 ) SetExpandable(_tcscmp(pstrValue, _T("true")) == 0);
	else ScrollContainer::SetAttribute(pstrName, pstrValue);
}

bool ListContainerElement::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
    DrawItemBk(hDC, m_rcItem);
    return ScrollContainer::DoPaint(hDC, rcPaint, pStopControl);
}

void ListContainerElement::DrawItemText(HDC hDC, const RECT& rcItem)
{
    return;
}

void ListContainerElement::DrawItemBk(HDC hDC, const RECT& rcItem)
{
	ASSERT(m_pOwner);
	if( m_pOwner == NULL ) return;
	TListInfo* pInfo = m_pOwner->GetListInfo();
	if( pInfo == NULL ) return;
	DWORD iBackColor = 0;
	if( !pInfo->bAlternateBk || m_iDrawIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;

	if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		iBackColor = pInfo->dwHotBkColor;
	}
	if( IsSelected() ) {
		iBackColor = pInfo->dwSelectedBkColor;
	}
	if( !IsEnabled() ) {
		iBackColor = pInfo->dwDisabledBkColor;
	}
	if ( iBackColor != 0 ) {
		CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
	}

	if( !IsEnabled() ) {
		if( DrawImage(hDC, pInfo->diDisabled) ) return;
	}
	if( IsSelected() ) {
		if( DrawImage(hDC, pInfo->diSelected) ) return;
	}
	if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( DrawImage(hDC, pInfo->diHot) ) return;
	}
	if( !DrawImage(hDC, m_diBk) ) {
		if( !pInfo->bAlternateBk || m_iDrawIndex % 2 == 0 ) {
			if( DrawImage(hDC, pInfo->diBk) ) return;
		}
	}
}

SIZE ListContainerElement::EstimateSize(SIZE szAvailable)
{
    TListInfo* pInfo = NULL;
    if( m_pOwner ) pInfo = m_pOwner->GetListInfo();

    SIZE cXY = m_cxyFixed;

    if( cXY.cy == 0 ) {
        cXY.cy = pInfo->uFixedHeight;
    }

    return cXY;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

ListHBoxElement::ListHBoxElement()
{
    
}

LPCTSTR ListHBoxElement::GetClass() const
{
    return DUI_CTR_LISTHBOXELEMENT;
}

LPVOID ListHBoxElement::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_LISTHBOXELEMENT) == 0 ) return static_cast<ListHBoxElement*>(this);
    return ListContainerElement::GetInterface(pstrName);
}

void ListHBoxElement::SetPos(RECT rc, bool bNeedInvalidate)
{
    if( m_pOwner == NULL ) return ListContainerElement::SetPos(rc, bNeedInvalidate);

    Control::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    TListInfo* pInfo = m_pOwner->GetListInfo();
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

bool ListHBoxElement::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
    ASSERT(m_pOwner);
    if( m_pOwner == NULL ) return true;
    TListInfo* pInfo = m_pOwner->GetListInfo();
    if( pInfo == NULL ) return true;

    DrawItemBk(hDC, m_rcItem);
    for( int i = 0; i < pInfo->nColumns; i++ ) {
        RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };
        if (pInfo->iVLineSize > 0 && i < pInfo->nColumns - 1) {
            RECT rcLine = { rcItem.right - pInfo->iVLineSize / 2, rcItem.top, rcItem.right - pInfo->iVLineSize / 2, rcItem.bottom};
            CRenderEngine::DrawLine(hDC, rcLine, pInfo->iVLineSize, GetAdjustColor(pInfo->dwVLineColor));
        }
    }
    return ScrollContainer::DoPaint(hDC, rcPaint, pStopControl);
}

} // namespace dui

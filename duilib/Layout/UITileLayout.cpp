#include "stdafx.h"
#include "UITileLayout.h"

namespace dui
{
	TileLayout::TileLayout() : m_nColumns(1), m_nRows(0), m_nColumnsFixed(0), m_iChildVPadding(0),
		m_bIgnoreItemPadding(true)
	{
		m_szItem.cx = m_szItem.cy = 80;
	}

	LPCTSTR TileLayout::GetClass() const
	{
		return DUI_CTR_TILELAYOUT;
	}

	LPVOID TileLayout::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_TILELAYOUT) == 0 ) return static_cast<TileLayout*>(this);
		return ScrollContainer::GetInterface(pstrName);
	}

	int TileLayout::GetFixedColumns() const
	{
		return m_nColumnsFixed;
	}

	void TileLayout::SetFixedColumns(int iColums)
	{
		if( iColums < 0 ) return;
		m_nColumnsFixed = iColums;
		NeedUpdate();
	}

	int TileLayout::GetChildVPadding() const
	{
		return m_iChildVPadding;
	}

	void TileLayout::SetChildVPadding(int iPadding)
	{
		m_iChildVPadding = iPadding;
		if (m_iChildVPadding < 0) m_iChildVPadding = 0;
		NeedUpdate();
	}

	SIZE TileLayout::GetItemSize() const
	{
		return m_szItem;
	}

	void TileLayout::SetItemSize(SIZE szSize)
	{
		if( m_szItem.cx != szSize.cx || m_szItem.cy != szSize.cy ) {
			m_szItem = szSize;
			NeedUpdate();
		}
	}

	int TileLayout::GetColumns() const
	{
		return m_nColumns;
	}

	int TileLayout::GetRows() const
	{
		return m_nRows;
	}

	void TileLayout::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("itemsize")) == 0 ) {
			SIZE szItem = { 0 };
			LPTSTR pstr = NULL;
			szItem.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szItem.cy = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);     
			SetItemSize(szItem);
		}
		else if( _tcscmp(pstrName, _T("columns")) == 0 ) SetFixedColumns(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("childvpadding")) == 0 ) SetChildVPadding(_ttoi(pstrValue));
		else ScrollContainer::SetAttribute(pstrName, pstrValue);
	}

	void TileLayout::SetPos(RECT rc, bool bNeedInvalidate)
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

		if( m_items.GetSize() == 0) {
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
			szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
			szAvailable.cy += m_pVerticalScrollBar->GetScrollRange();

		int nEstimateNum = 0;
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			Control* pControl = static_cast<Control*>(m_items[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			nEstimateNum++;
		}

		int cxNeeded = 0;
		int cyNeeded = 0;
		int iChildMargin = m_iChildMargin;
		if (m_nColumnsFixed == 0) { 
			if (rc.right - rc.left >= m_szItem.cx) {
				m_nColumns = (rc.right - rc.left)/m_szItem.cx;
				cxNeeded = rc.right - rc.left;
				if (m_nColumns > 1) {
					if (iChildMargin <= 0) {
						iChildMargin = (cxNeeded-m_nColumns*m_szItem.cx)/(m_nColumns-1);
					}
					if (iChildMargin < 0) iChildMargin = 0;
				}
				else {
					iChildMargin = 0;
				}
			}
			else {
				m_nColumns = 1;
				cxNeeded = m_szItem.cx;
			}

			m_nRows = (nEstimateNum-1)/m_nColumns+1;
			cyNeeded = m_nRows*m_szItem.cy + (m_nRows-1)*m_iChildVPadding;
		}
		else {
			m_nColumns = m_nColumnsFixed;
			if (m_nColumns > 1) {
				if (iChildMargin <= 0) {
					if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() && rc.right - rc.left >= m_nColumns*m_szItem.cx) {
						iChildMargin = (rc.right - rc.left-m_nColumns*m_szItem.cx)/(m_nColumns-1);
					}
					else {
						iChildMargin = (szAvailable.cx-m_nColumns*m_szItem.cx)/(m_nColumns-1);
					}
				}
				if (iChildMargin < 0) iChildMargin = 0;
			}
			else iChildMargin = 0;

			if (nEstimateNum >= m_nColumns) cxNeeded = m_nColumns*m_szItem.cx + (m_nColumns-1)*iChildMargin;
			else cxNeeded = nEstimateNum*m_szItem.cx + (nEstimateNum-1)*iChildMargin;
			m_nRows = (nEstimateNum-1)/m_nColumns+1;
			cyNeeded = m_nRows*m_szItem.cy + (m_nRows-1)*m_iChildVPadding;
		}

		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
			Control* pControl = static_cast<Control*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it1);
				continue;
			}

			RECT rcMargin = pControl->GetMargin();
			SIZE sz = m_szItem;
			sz.cx -= rcMargin.left + rcMargin.right;
			sz.cy -= rcMargin.top + rcMargin.bottom;
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			if( sz.cx < 0) sz.cx = 0;
			if( sz.cy < 0) sz.cy = 0;

			UINT iChildAlign = GetChildAlign(); 
			UINT iChildVAlign = GetChildVAlign();
			int iColumnIndex = it1/m_nColumns;
			int iRowIndex = it1%m_nColumns;
			int iPosX = rc.left + iRowIndex*(m_szItem.cx+iChildMargin);
			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
				iPosX -= m_pHorizontalScrollBar->GetScrollPos();
			}
			int iPosY = rc.top + iColumnIndex*(m_szItem.cy+m_iChildVPadding);
			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
				iPosY -= m_pVerticalScrollBar->GetScrollPos();
			}
			if (iChildAlign == DT_CENTER) {
				if (iChildVAlign == DT_VCENTER) {
					RECT rcCtrl = { iPosX + (m_szItem.cx-sz.cx)/2+rcMargin.left, iPosY + (m_szItem.cy-sz.cy)/2+rcMargin.top, iPosX + (m_szItem.cx-sz.cx)/2 + sz.cx-rcMargin.right, iPosY + (m_szItem.cy-sz.cy)/2 + sz.cy-rcMargin.bottom };
					pControl->SetPos(rcCtrl, false);
				}
				else if (iChildVAlign == DT_BOTTOM) {
					RECT rcCtrl = { iPosX + (m_szItem.cx-sz.cx)/2+rcMargin.left, iPosY + m_szItem.cy - sz.cy+rcMargin.top, iPosX + (m_szItem.cx-sz.cx)/2 + sz.cx-rcMargin.right, iPosY + m_szItem.cy-rcMargin.bottom };
					pControl->SetPos(rcCtrl, false);
				}
				else {
					RECT rcCtrl = { iPosX + (m_szItem.cx-sz.cx)/2+rcMargin.left, iPosY+rcMargin.top, iPosX + (m_szItem.cx-sz.cx)/2 + sz.cx-rcMargin.right, iPosY + sz.cy-rcMargin.bottom };
					pControl->SetPos(rcCtrl, false);
				}
			}
			else if (iChildAlign == DT_RIGHT) {
				if (iChildVAlign == DT_VCENTER) {
					RECT rcCtrl = { iPosX + m_szItem.cx - sz.cx+rcMargin.left, iPosY + (m_szItem.cy-sz.cy)/2+rcMargin.top, iPosX + m_szItem.cx-rcMargin.right, iPosY + (m_szItem.cy-sz.cy)/2 + sz.cy-rcMargin.bottom };
					pControl->SetPos(rcCtrl, false);
				}
				else if (iChildVAlign == DT_BOTTOM) {
					RECT rcCtrl = { iPosX + m_szItem.cx - sz.cx+rcMargin.left, iPosY + m_szItem.cy - sz.cy+rcMargin.top, iPosX + m_szItem.cx-rcMargin.right, iPosY + m_szItem.cy-rcMargin.bottom };
					pControl->SetPos(rcCtrl, false);
				}
				else {
					RECT rcCtrl = { iPosX + m_szItem.cx - sz.cx+rcMargin.left, iPosY+rcMargin.top, iPosX + m_szItem.cx-rcMargin.right, iPosY + sz.cy-rcMargin.bottom };
					pControl->SetPos(rcCtrl, false);
				}
			}
			else {
				if (iChildVAlign == DT_VCENTER) {
					RECT rcCtrl = { iPosX+rcMargin.left, iPosY + (m_szItem.cy-sz.cy)/2+rcMargin.top, iPosX + sz.cx-rcMargin.right, iPosY + (m_szItem.cy-sz.cy)/2 + sz.cy-rcMargin.bottom };
					pControl->SetPos(rcCtrl, false);
				}
				else if (iChildVAlign == DT_BOTTOM) {
					RECT rcCtrl = { iPosX+rcMargin.left, iPosY + m_szItem.cy - sz.cy+rcMargin.top, iPosX + sz.cx-rcMargin.right, iPosY + m_szItem.cy-rcMargin.bottom };
					pControl->SetPos(rcCtrl, false);
				}
				else {
					RECT rcCtrl = { iPosX+rcMargin.left, iPosY+rcMargin.top, iPosX + sz.cx-rcMargin.right, iPosY + sz.cy-rcMargin.bottom };
					pControl->SetPos(rcCtrl, false);
				}
			}
		}

		// Process the scrollbar
		ProcessScrollBar(rc, cxNeeded, cyNeeded);



	//	// Position the elements
	//	if( m_szItem.cx > 0 ) {
	//		m_nColumns = (rc.right - rc.left) / m_szItem.cx;
	//		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
	//			m_nColumns = (rc.right - rc.left + m_pHorizontalScrollBar->GetScrollRange() ) / m_szItem.cx;
	//	}

	//	if( m_nColumns == 0 ) m_nColumns = 1;

	//	int cyNeeded = 0;
	//	int cxWidth = (rc.right - rc.left) / m_nColumns;
	//	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
	//		cxWidth = (rc.right - rc.left + m_pHorizontalScrollBar->GetScrollRange() ) / m_nColumns;

	//	int cyHeight = 0;
	//	int iCount = 0;
	//	POINT ptTile = { rc.left, rc.top };
	//	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
	//		ptTile.y -= m_pVerticalScrollBar->GetScrollPos();
	//	}
	//	int iPosX = rc.left;
	//	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
	//		iPosX -= m_pHorizontalScrollBar->GetScrollPos();
	//		ptTile.x = iPosX;
	//	}
	//	for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
	//		Control* pControl = static_cast<Control*>(m_items[it1]);
	//		if( !pControl->IsVisible() ) continue;
	//		if( pControl->IsFloat() ) {
	//			SetFloatPos(it1);
	//			continue;
	//		}

	//		// Determine size
	//		RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
	//		if( (iCount % m_nColumns) == 0 )
	//		{
	//			int iIndex = iCount;
	//			for( int it2 = it1; it2 < m_items.GetSize(); it2++ ) {
	//				Control* pLineControl = static_cast<Control*>(m_items[it2]);
	//				if( !pLineControl->IsVisible() ) continue;
	//				if( pLineControl->IsFloat() ) continue;

	//				RECT rcMargin = pLineControl->GetMargin();
	//				SIZE szAvailable = { rcTile.right - rcTile.left - rcMargin.left - rcMargin.right, 9999 };

	//				if( szAvailable.cx < pControl->GetMinWidth() ) szAvailable.cx = pControl->GetMinWidth();
	//				if( szAvailable.cx > pControl->GetMaxWidth() ) szAvailable.cx = pControl->GetMaxWidth();

	//				SIZE szTile = pLineControl->EstimateSize(szAvailable);
	//				if( szTile.cx == 0 ) szTile.cx = m_szItem.cx;
	//				if( szTile.cy == 0 ) szTile.cy = m_szItem.cy;
	//				if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
	//				if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
	//				if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
	//				if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();

	//				cyHeight = MAX(cyHeight, szTile.cy + rcMargin.top + rcMargin.bottom);
	//				if( (++iIndex % m_nColumns) == 0) break;
	//			}
	//		}

	//		RECT rcMargin = pControl->GetMargin();

	//		rcTile.left += rcMargin.left;
	//		rcTile.right -= rcMargin.right;

	//		// Set position
	//		rcTile.top = ptTile.y + rcMargin.top;
	//		rcTile.bottom = ptTile.y + cyHeight;

	//		SIZE szAvailable = { rcTile.right - rcTile.left, rcTile.bottom - rcTile.top };
	//		SIZE szTile = pControl->EstimateSize(szAvailable);
	//		if( szTile.cx == 0 ) szTile.cx = m_szItem.cx;
	//		if( szTile.cy == 0 ) szTile.cy = m_szItem.cy;
	//		if( szTile.cx == 0 ) szTile.cx = szAvailable.cx;
	//		if( szTile.cy == 0 ) szTile.cy = szAvailable.cy;
	//		if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
	//		if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
	//		if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
	//		if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();
	//		RECT rcPos = {(rcTile.left + rcTile.right - szTile.cx) / 2, (rcTile.top + rcTile.bottom - szTile.cy) / 2,
	//			(rcTile.left + rcTile.right - szTile.cx) / 2 + szTile.cx, (rcTile.top + rcTile.bottom - szTile.cy) / 2 + szTile.cy};
	//		pControl->SetPos(rcPos, false);

	//		if( (++iCount % m_nColumns) == 0 ) {
	//			ptTile.x = iPosX;
	//			ptTile.y += cyHeight + iChildMargin;
	//			cyHeight = 0;
	//		}
	//		else {
	//			ptTile.x += cxWidth;
	//		}
	//		cyNeeded = rcTile.bottom - rc.top;
	//		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) cyNeeded += m_pVerticalScrollBar->GetScrollPos();
	//	}

	//	// Process the scrollbar
	//	ProcessScrollBar(rc, 0, cyNeeded);
	}
}

#include "stdafx.h"

namespace dui
{
	HBox::HBox() : m_iSepWidth(0), m_uButtonState(0), m_bImmMode(false)
	{
		m_ptLastMouse.x = m_ptLastMouse.y = 0;
		::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
	}

	LPCTSTR HBox::GetClass() const
	{
		return DUI_CTR_HBOX;
	}

	LPVOID HBox::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_HBOX) == 0 ) return static_cast<HBox*>(this);
		return __super::GetInterface(pstrName);
	}

	UINT HBox::GetControlFlags() const
	{
		if( IsEnabled() && m_iSepWidth != 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void HBox::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		rc = m_rcItem;

		// Adjust for padding
		RECT rcPadding = GetPadding();
		rc.left += rcPadding.left;
		rc.top += rcPadding.top;
		rc.right -= rcPadding.right;
		rc.bottom -= rcPadding.bottom;

		if( m_items.GetSize() == 0) {
			return;
		}

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

		int cyNeeded = 0;
		int nAdjustables = 0;	//未确定长或宽的Control个数
		int cxFixed = 0;
		int nEstimateNum = 0;	//已确定长或宽的Control个数
		SIZE szControlAvailable;
		int iControlMaxWidth = 0;
		int iControlMaxHeight = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
			Control* pControl = static_cast<Control*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			szControlAvailable = szAvailable;
			RECT rcMargin = pControl->GetMargin();
			szControlAvailable.cy -= rcMargin.top + rcMargin.bottom;
			iControlMaxWidth = pControl->GetFixedWidth();
			iControlMaxHeight = pControl->GetFixedHeight();
			if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
			if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
			if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
			if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
			SIZE sz = { 0 };
			if (pControl->GetFixedWidth() == 0) {
				nAdjustables++;
				sz.cy = pControl->GetFixedHeight();
			}
			else {
				sz = pControl->EstimateSize(szControlAvailable);
				if (sz.cx == 0) {
					nAdjustables++;
				}
				else {
					if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
					if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();
				}
			}

			cxFixed += sz.cx + pControl->GetMargin().left + pControl->GetMargin().right;

			sz.cy = MAX(sz.cy, 0);
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			cyNeeded = MAX(cyNeeded, sz.cy + rcMargin.top + rcMargin.bottom);
			nEstimateNum++;
		}
		cxFixed += (nEstimateNum - 1) * m_iChildMargin;

		// Place elements
		int cxNeeded = 0;
		int cxExpand = 0;
		if( nAdjustables > 0 ) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosX = rc.left;
		int iEstimate = 0;
		int iAdjustable = 0;
		int cxFixedRemaining = cxFixed;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			Control* pControl = static_cast<Control*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}
			
			iEstimate += 1;
			RECT rcMargin = pControl->GetMargin();
			szRemaining.cx -= rcMargin.left;

			szControlAvailable = szRemaining;
			szControlAvailable.cy -= rcMargin.top + rcMargin.bottom;
			iControlMaxWidth = pControl->GetFixedWidth();
			iControlMaxHeight = pControl->GetFixedHeight();
			if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
			if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
			if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
			if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
      cxFixedRemaining = cxFixedRemaining - (rcMargin.left + rcMargin.right);
			if (iEstimate > 1) cxFixedRemaining = cxFixedRemaining - m_iChildMargin;
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if (pControl->GetFixedWidth() == 0 || sz.cx == 0) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cx = MAX(0, szRemaining.cx - rcMargin.right - cxFixedRemaining);
				} 
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			else {
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				cxFixedRemaining -= sz.cx;
			}

			sz.cy = pControl->GetMaxHeight();
			if( sz.cy == 0 ) sz.cy = szAvailable.cy - rcMargin.top - rcMargin.bottom;
			if( sz.cy < 0 ) sz.cy = 0;
			if( sz.cy > szControlAvailable.cy ) sz.cy = szControlAvailable.cy;
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
#if 1
			UINT iChildAlign = GetChildVAlign(); 
			UINT pos_style = pControl->GetPosStyle();
			if (pos_style & POS_STYLE_RIGHT)
			{
				assert(pControl == m_items.GetAt(m_items.GetSize() - 1));	//只有最后一个可靠右
				rcMargin.left = szAvailable.cx - iPosX - sz.cx - rcMargin.right;
			}

			if (iChildAlign == DT_VCENTER || (pos_style & POS_STYLE_VCENTER)) {
				int iPosY = (rc.bottom + rc.top) / 2;
				RECT rcCtrl = { iPosX + rcMargin.left, iPosY - sz.cy/2, iPosX + sz.cx + rcMargin.left, iPosY + sz.cy - sz.cy/2 };
				pControl->SetPos(rcCtrl, false);
			}
			else if (iChildAlign == DT_BOTTOM || (pos_style & POS_STYLE_BOTTOM)) {
				int iPosY = rc.bottom;
				RECT rcCtrl = { iPosX + rcMargin.left, iPosY - rcMargin.bottom - sz.cy, iPosX + sz.cx + rcMargin.left, iPosY - rcMargin.bottom };
				pControl->SetPos(rcCtrl, false);
			}
			else {
				int iPosY = rc.top;
				RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + sz.cx + rcMargin.left, iPosY + sz.cy + rcMargin.top };
				pControl->SetPos(rcCtrl, false);
			}

			
#else
			UINT iChildAlign = GetChildVAlign(); 
			if (iChildAlign == DT_VCENTER) {
				int iPosY = (rc.bottom + rc.top) / 2;
				RECT rcCtrl = { iPosX + rcMargin.left, iPosY - sz.cy/2, iPosX + sz.cx + rcMargin.left, iPosY + sz.cy - sz.cy/2 };
				pControl->SetPos(rcCtrl, false);
			}
			else if (iChildAlign == DT_BOTTOM) {
				int iPosY = rc.bottom;
				RECT rcCtrl = { iPosX + rcMargin.left, iPosY - rcMargin.bottom - sz.cy, iPosX + sz.cx + rcMargin.left, iPosY - rcMargin.bottom };
				pControl->SetPos(rcCtrl, false);
			}
			else {
				int iPosY = rc.top;
				RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + sz.cx + rcMargin.left, iPosY + sz.cy + rcMargin.top };
				pControl->SetPos(rcCtrl, false);
			}
#endif
			iPosX += sz.cx + m_iChildMargin + rcMargin.left + rcMargin.right;
			cxNeeded += sz.cx + rcMargin.left + rcMargin.right;
			szRemaining.cx -= sz.cx + m_iChildMargin + rcMargin.right;
		}
		cxNeeded += (nEstimateNum - 1) * m_iChildMargin;

	}

	void HBox::DoPostPaint(HDC hDC, const RECT& rcPaint)
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) {
			RECT rcSeparator = GetThumbRect(true);
			Render::DrawColor(hDC, rcSeparator, 0xAA000000);
		}
	}

	void HBox::SetSepWidth(int iWidth)
	{
		m_iSepWidth = iWidth;
	}

	int HBox::GetSepWidth() const
	{
		return m_iSepWidth;
	}

	void HBox::SetSepImmMode(bool bImmediately)
	{
		if( m_bImmMode == bImmediately ) return;
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
			m_pManager->RemovePostPaint(this);
		}

		m_bImmMode = bImmediately;
	}

	bool HBox::IsSepImmMode() const
	{
		return m_bImmMode;
	}

	void HBox::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("sepwidth")) == 0 ) SetSepWidth(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("sepimm")) == 0 ) SetSepImmMode(_tcscmp(pstrValue, _T("true")) == 0);
		else __super::SetAttribute(pstrName, pstrValue);
	}

	void HBox::DoEvent(Event& event)
	{
		if( m_iSepWidth != 0 ) {
			if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
			{
				RECT rcSeparator = GetThumbRect(false);
				if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
					m_uButtonState |= UISTATE_CAPTURED;
					m_ptLastMouse = event.ptMouse;
					m_rcNewPos = m_rcItem;
					if( !m_bImmMode && m_pManager ) m_pManager->AddPostPaint(this);
					return;
				}
			}
			if( event.Type == UIEVENT_BUTTONUP )
			{
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
					m_uButtonState &= ~UISTATE_CAPTURED;
					m_rcItem = m_rcNewPos;
					if( !m_bImmMode && m_pManager ) m_pManager->RemovePostPaint(this);
					NeedParentUpdate();
					return;
				}
			}
			if( event.Type == UIEVENT_MOUSEMOVE )
			{
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
					LONG cx = event.ptMouse.x - m_ptLastMouse.x;
					m_ptLastMouse = event.ptMouse;
					RECT rc = m_rcNewPos;
					if( m_iSepWidth >= 0 ) {
						if( cx > 0 && event.ptMouse.x < m_rcNewPos.right - m_iSepWidth ) return;
						if( cx < 0 && event.ptMouse.x > m_rcNewPos.right ) return;
						rc.right += cx;
						if( rc.right - rc.left <= GetMinWidth() ) {
							if( m_rcNewPos.right - m_rcNewPos.left <= GetMinWidth() ) return;
							rc.right = rc.left + GetMinWidth();
						}
						if( rc.right - rc.left >= GetMaxWidth() ) {
							if( m_rcNewPos.right - m_rcNewPos.left >= GetMaxWidth() ) return;
							rc.right = rc.left + GetMaxWidth();
						}
					}
					else {
						if( cx > 0 && event.ptMouse.x < m_rcNewPos.left ) return;
						if( cx < 0 && event.ptMouse.x > m_rcNewPos.left - m_iSepWidth ) return;
						rc.left += cx;
						if( rc.right - rc.left <= GetMinWidth() ) {
							if( m_rcNewPos.right - m_rcNewPos.left <= GetMinWidth() ) return;
							rc.left = rc.right - GetMinWidth();
						}
						if( rc.right - rc.left >= GetMaxWidth() ) {
							if( m_rcNewPos.right - m_rcNewPos.left >= GetMaxWidth() ) return;
							rc.left = rc.right - GetMaxWidth();
						}
					}

					DuiRect rcInvalidate = GetThumbRect(true);
					m_rcNewPos = rc;
					m_FixedSize.cx = m_rcNewPos.right - m_rcNewPos.left;

					if( m_bImmMode ) {
						m_rcItem = m_rcNewPos;
						NeedParentUpdate();
					}
					else {
						rcInvalidate.Join(GetThumbRect(true));
						rcInvalidate.Join(GetThumbRect(false));
						if( m_pManager ) m_pManager->Invalidate(rcInvalidate);
					}
					return;
				}
			}
			if( event.Type == UIEVENT_SETCURSOR )
			{
				RECT rcSeparator = GetThumbRect(false);
				if( IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
					return;
				}
			}
		}
		__super::DoEvent(event);
	}

	RECT HBox::GetThumbRect(bool bUseNew) const
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
			if( m_iSepWidth >= 0 ) return DuiRect(m_rcNewPos.right - m_iSepWidth, m_rcNewPos.top, m_rcNewPos.right, m_rcNewPos.bottom);
			else return DuiRect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.left - m_iSepWidth, m_rcNewPos.bottom);
		}
		else {
			if( m_iSepWidth >= 0 ) return DuiRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
			else return DuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
		}
	}
}

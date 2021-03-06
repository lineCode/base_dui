#include "stdafx.h"

namespace dui
{
	VBox::VBox() : m_iSepHeight(0), m_uButtonState(0), m_bImmMode(false)
	{
		m_ptLastMouse.x = m_ptLastMouse.y = 0;
		::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
	}

	LPCTSTR VBox::GetClass() const
	{
		return DUI_CTR_VBOX;
	}

	LPVOID VBox::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_VBOX) == 0 ) return static_cast<VBox*>(this);
		return __super::GetInterface(pstrName);
	}

	UINT VBox::GetControlFlags() const
	{
		if( IsEnabled() && m_iSepHeight != 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void VBox::SetPos(RECT rc, bool bNeedInvalidate)
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

		int cxNeeded = 0;
		int nAdjustables = 0;
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
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cy == 0 ) {
				nAdjustables++;
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetMargin().top + pControl->GetMargin().bottom;

			sz.cx = MAX(sz.cx, 0);
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			cxNeeded = MAX(cxNeeded, sz.cx + rcMargin.left + rcMargin.right);
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * m_iChildMargin;

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		if( nAdjustables > 0 ) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;

		int iEstimate = 0;
		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			Control* pControl = static_cast<Control*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}

			iEstimate += 1;
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
      cyFixedRemaining = cyFixedRemaining - (rcMargin.top + rcMargin.bottom);
			if (iEstimate > 1) cyFixedRemaining = cyFixedRemaining - m_iChildMargin;
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cy == 0 ) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cy = MAX(0, szRemaining.cy - rcMargin.bottom - cyFixedRemaining);
				} 
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = pControl->GetMaxWidth();
			if( sz.cx == 0 ) sz.cx = szAvailable.cx - rcMargin.left - rcMargin.right;
			if( sz.cx < 0 ) sz.cx = 0;
			if( sz.cx > szControlAvailable.cx ) sz.cx = szControlAvailable.cx;
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
#if 1
			UINT iChildAlign = GetChildAlign(); 
			UINT pos_style = pControl->GetPosStyle();
			if (pos_style & POS_STYLE_BOTTOM)
			{
				assert(pControl == m_items.GetAt(m_items.GetSize() - 1));	//只有最后一个可靠下
				rcMargin.top = szAvailable.cy - iPosY - sz.cy - rcMargin.bottom;
			}

			if (iChildAlign == DT_CENTER || (pos_style & POS_STYLE_CENTER)) {
				int iPosX = (rc.right + rc.left) / 2;
				RECT rcCtrl = { iPosX - sz.cx/2, iPosY + rcMargin.top, iPosX + sz.cx - sz.cx/2, iPosY + sz.cy + rcMargin.top };
				pControl->SetPos(rcCtrl, false);
			}
			else if (iChildAlign == DT_RIGHT || (pos_style & POS_STYLE_RIGHT)) {
				int iPosX = rc.right;
				RECT rcCtrl = { iPosX - rcMargin.right - sz.cx, iPosY + rcMargin.top, iPosX - rcMargin.right, iPosY + sz.cy + rcMargin.top };
				pControl->SetPos(rcCtrl, false);
			}
			else {
				int iPosX = rc.left;
				RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + rcMargin.left + sz.cx, iPosY + sz.cy + rcMargin.top };
				pControl->SetPos(rcCtrl, false);
			}
#else
			UINT iChildAlign = GetChildAlign(); 
			if (iChildAlign == DT_CENTER) {
				int iPosX = (rc.right + rc.left) / 2;
				RECT rcCtrl = { iPosX - sz.cx/2, iPosY + rcMargin.top, iPosX + sz.cx - sz.cx/2, iPosY + sz.cy + rcMargin.top };
				pControl->SetPos(rcCtrl, false);
			}
			else if (iChildAlign == DT_RIGHT) {
				int iPosX = rc.right;
				RECT rcCtrl = { iPosX - rcMargin.right - sz.cx, iPosY + rcMargin.top, iPosX - rcMargin.right, iPosY + sz.cy + rcMargin.top };
				pControl->SetPos(rcCtrl, false);
			}
			else {
				int iPosX = rc.left;
				RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + rcMargin.left + sz.cx, iPosY + sz.cy + rcMargin.top };
				pControl->SetPos(rcCtrl, false);
			}
#endif
			iPosY += sz.cy + m_iChildMargin + rcMargin.top + rcMargin.bottom;
			cyNeeded += sz.cy + rcMargin.top + rcMargin.bottom;
			szRemaining.cy -= sz.cy + m_iChildMargin + rcMargin.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * m_iChildMargin;
	}

	void VBox::DoPostPaint(HDC hDC, const RECT& rcPaint)
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) {
			RECT rcSeparator = GetThumbRect(true);
			Render::DrawColor(hDC, rcSeparator, 0xAA000000);
		}
	}

	void VBox::SetSepHeight(int iHeight)
	{
		m_iSepHeight = iHeight;
	}

	int VBox::GetSepHeight() const
	{
		return m_iSepHeight;
	}

	void VBox::SetSepImmMode(bool bImmediately)
	{
		if( m_bImmMode == bImmediately ) return;
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
			m_pManager->RemovePostPaint(this);
		}

		m_bImmMode = bImmediately;
	}

	bool VBox::IsSepImmMode() const
	{
		return m_bImmMode;
	}

	void VBox::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("sepheight")) == 0 ) SetSepHeight(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("sepimm")) == 0 ) SetSepImmMode(_tcscmp(pstrValue, _T("true")) == 0);
		else __super::SetAttribute(pstrName, pstrValue);
	}

	void VBox::DoEvent(Event& event)
	{
		if( m_iSepHeight != 0 ) {
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
					LONG cy = event.ptMouse.y - m_ptLastMouse.y;
					m_ptLastMouse = event.ptMouse;
					RECT rc = m_rcNewPos;
					if( m_iSepHeight >= 0 ) {
						if( cy > 0 && event.ptMouse.y < m_rcNewPos.bottom + m_iSepHeight ) return;
						if( cy < 0 && event.ptMouse.y > m_rcNewPos.bottom ) return;
						rc.bottom += cy;
						if( rc.bottom - rc.top <= GetMinHeight() ) {
							if( m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight() ) return;
							rc.bottom = rc.top + GetMinHeight();
						}
						if( rc.bottom - rc.top >= GetMaxHeight() ) {
							if( m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight() ) return;
							rc.bottom = rc.top + GetMaxHeight();
						}
					}
					else {
						if( cy > 0 && event.ptMouse.y < m_rcNewPos.top ) return;
						if( cy < 0 && event.ptMouse.y > m_rcNewPos.top + m_iSepHeight ) return;
						rc.top += cy;
						if( rc.bottom - rc.top <= GetMinHeight() ) {
							if( m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight() ) return;
							rc.top = rc.bottom - GetMinHeight();
						}
						if( rc.bottom - rc.top >= GetMaxHeight() ) {
							if( m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight() ) return;
							rc.top = rc.bottom - GetMaxHeight();
						}
					}

					DuiRect rcInvalidate = GetThumbRect(true);
					m_rcNewPos = rc;
					m_FixedSize.cy = m_rcNewPos.bottom - m_rcNewPos.top;

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
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
					return;
				}
			}
		}
		__super::DoEvent(event);
	}

	RECT VBox::GetThumbRect(bool bUseNew) const
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
			if( m_iSepHeight >= 0 ) 
				return DuiRect(m_rcNewPos.left, MAX(m_rcNewPos.bottom - m_iSepHeight, m_rcNewPos.top), 
				m_rcNewPos.right, m_rcNewPos.bottom);
			else 
				return DuiRect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.right, 
				MIN(m_rcNewPos.top - m_iSepHeight, m_rcNewPos.bottom));
		}
		else {
			if( m_iSepHeight >= 0 ) 
				return DuiRect(m_rcItem.left, MAX(m_rcItem.bottom - m_iSepHeight, m_rcItem.top), m_rcItem.right, 
				m_rcItem.bottom);
			else 
				return DuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.right, 
				MIN(m_rcItem.top - m_iSepHeight, m_rcItem.bottom));

		}
	}
}

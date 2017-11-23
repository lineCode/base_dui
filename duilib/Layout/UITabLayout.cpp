#include "stdafx.h"
#include "UITabLayout.h"

#define TABLAYOUT_FADESWITCH_DELTA		25
#define TABLAYOUT_FADESWITCH_TIMERID	12
#define TABLAYOUT_FADESWITCH_ELLAPSE	15

namespace dui
{
	TabLayout::TabLayout() : m_iCurSel(-1), m_uFadeSwitchPos(0), m_uFadeSwitchDelta(0), m_cFadeSwitchLeftId(-1), m_cFadeSwitchRightId(-1), m_bFadeSwitchRightToLeft(false)/*, pOldFadeSwitchItem_(NULL)*/
	{
	}

	LPCTSTR TabLayout::GetClass() const
	{
		return DUI_CTR_TABLAYOUT;
	}

	LPVOID TabLayout::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_TABLAYOUT) == 0 ) return static_cast<TabLayout*>(this);
		return ScrollContainer::GetInterface(pstrName);
	}

	void TabLayout::SetFadeSwitch(bool bFadeSwitch)
	{
		m_uFadeSwitchDelta = bFadeSwitch ? TABLAYOUT_FADESWITCH_DELTA : 0;
	}

	bool TabLayout::GetFadeSwitch()
	{
		return m_uFadeSwitchDelta > 0;
	}

	bool TabLayout::Add(Control* pControl)
	{
		bool ret = ScrollContainer::Add(pControl);
		if( !ret ) return ret;

		if(m_iCurSel == -1 && pControl->IsVisible())
		{
			m_iCurSel = GetItemIndex(pControl);
		}
		else
		{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool TabLayout::AddAt(Control* pControl, int iIndex)
	{
		bool ret = ScrollContainer::AddAt(pControl, iIndex);
		if( !ret ) return ret;

		if(m_iCurSel == -1 && pControl->IsVisible())
		{
			m_iCurSel = GetItemIndex(pControl);
		}
		else if( m_iCurSel != -1 && iIndex <= m_iCurSel )
		{
			m_iCurSel += 1;
		}
		else
		{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool TabLayout::Remove(Control* pControl, bool bDoNotDestroy)
	{
		if( pControl == NULL) return false;

		int index = GetItemIndex(pControl);
		bool ret = ScrollContainer::Remove(pControl, bDoNotDestroy);
		if( !ret ) return false;

		if( m_iCurSel == index)
		{
			if( GetCount() > 0 )
			{
				m_iCurSel=0;
				GetItemAt(m_iCurSel)->SetVisible(true);
			}
			else
				m_iCurSel=-1;
			NeedParentUpdate();
		}
		else if( m_iCurSel > index )
		{
			m_iCurSel -= 1;
		}

		return ret;
	}

	void TabLayout::RemoveAll()
	{
		m_iCurSel = -1;
		ScrollContainer::RemoveAll();
		NeedParentUpdate();
	}

	int TabLayout::GetCurSel() const
	{
		return m_iCurSel;
	}

	bool TabLayout::SelectItem(int iIndex,  bool bTriggerEvent)
	{
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return false;
		if( iIndex == m_iCurSel ) return true;

		int iOldSel = m_iCurSel;
		m_iCurSel = iIndex;
		for( int it = 0; it < m_items.GetSize(); it++ )
		{
			if( it == iIndex ) {
				GetItemAt(it)->SetVisible(true);
				GetItemAt(it)->SetFocus();
			}
			else 
				GetItemAt(it)->SetVisible(false);
		}
		NeedParentUpdate();

		if( m_pManager != NULL ) {
			m_pManager->SetNextTabControl();
			if (bTriggerEvent) m_pManager->SendNotify(this, UIEVENT_TABSELECT, m_iCurSel, iOldSel);
		}
		//fadeswitch
		if (GetFadeSwitch() /*&& iOldSel >= 0*/) {
			m_pManager->SetTimer(this, TABLAYOUT_FADESWITCH_TIMERID, TABLAYOUT_FADESWITCH_ELLAPSE);
			m_uFadeSwitchPos = 255;
			m_bFadeSwitchRightToLeft = m_iCurSel > iOldSel;
			m_cFadeSwitchLeftId = m_bFadeSwitchRightToLeft ? iOldSel : m_iCurSel;
			m_cFadeSwitchRightId = m_bFadeSwitchRightToLeft ? m_iCurSel : iOldSel;

			if (iOldSel >= 0 && iOldSel < m_items.GetSize())
			{
				//pOldFadeSwitchItem_ = GetItemAt(iOldSel);
				GetItemAt(iOldSel)->SetVisible(true);
			}
			
			//printf("TabLayout SetTimer\n");
		}
		return true;
	}

	bool TabLayout::SelectItem(Control* pControl, bool bTriggerEvent)
	{
		int iIndex=GetItemIndex(pControl);
		if (iIndex==-1)
			return false;
		else
			return SelectItem(iIndex, bTriggerEvent);
	}

	void TabLayout::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("selectedid")) == 0 ) SelectItem(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("fadeswitch")) == 0) SetFadeSwitch(_tcscmp(pstrValue, _T("true")) == 0);
		return ScrollContainer::SetAttribute(pstrName, pstrValue);
	}

	void TabLayout::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		rc = m_rcItem;

		// Adjust for padding
		RECT rcPadding = GetPadding();
		rc.left += rcPadding.left;
		rc.top += rcPadding.top;
		rc.right -= rcPadding.right;
		rc.bottom -= rcPadding.bottom;

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			Control* pControl = static_cast<Control*>(m_items[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it);
				continue;
			}

			if( it != m_iCurSel ) continue;

			RECT rcMargin = pControl->GetMargin();
			rc.left += rcMargin.left;
			rc.top += rcMargin.top;
			rc.right -= rcMargin.right;
			rc.bottom -= rcMargin.bottom;

			SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cx == 0 ) {
				sz.cx = MAX(0, szAvailable.cx);
			}
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			if(sz.cy == 0) {
				sz.cy = MAX(0, szAvailable.cy);
			}
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy};
			pControl->SetPos(rcCtrl, false);
		}
	}

	void TabLayout::DoEvent(TEvent& event)
	{
		bool bHandle = false;
		if (event.Type == UIEVENT_TIMER  && event.wParam == TABLAYOUT_FADESWITCH_TIMERID)
		{
			if (m_uFadeSwitchPos > m_uFadeSwitchDelta){
				m_uFadeSwitchPos -= m_uFadeSwitchDelta;
			}
			else {
				m_uFadeSwitchPos = 0;
				m_pManager->KillTimer(this, TABLAYOUT_FADESWITCH_TIMERID);
				//printf("TabLayout KillTimer\n");
			}

			Control *pControlLeft = GetItemAt(m_cFadeSwitchLeftId), *pControlRight = GetItemAt(m_cFadeSwitchRightId);
			RECT rc = GetPos(), rc1 = rc, rc2 = rc;
			int off = (rc.right - rc.left)*(m_uFadeSwitchPos) / 255, offLeft = 0, offRight = 0;

			if (m_bFadeSwitchRightToLeft)
			{
				assert(pControlRight);
				offRight = off;
				rc2.left = rc.left + offRight;
				rc2.right = rc.right + offRight;
				pControlRight->SetPos(rc2);
				if (pControlLeft)
				{
					offLeft = rc.right - off;
					rc1.left = rc.left - offLeft;
					rc1.right = rc.right - offLeft;
					pControlLeft->SetPos(rc1);
				}
			}
			else
			{
				assert(pControlLeft);
				offLeft = off;
				rc1.left = rc.left - offLeft;
				rc1.right = rc.right - offLeft;
				pControlLeft->SetPos(rc1);
				if (pControlRight)
				{
					offRight = rc.right - off;
					rc2.left = rc.left + offRight;
					rc2.right = rc.right + offRight;
					pControlRight->SetPos(rc2);
				}
			}
			
			/*if (m_uFadeSwitchPos < 255 - m_uFadeAlphaDelta)
				m_uFadeAlpha += m_uFadeAlphaDelta;
			else {
				m_uFadeAlpha = 255;
				m_pManager->KillTimer(this, TABLAYOUT_FADESWITCH_TIMERID);
			}*/
			
			Invalidate();
			bHandle = true;
			
		}

		return __super::DoEvent(event);
	}

	bool TabLayout::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
	{
#if 1
		RECT rcTemp = { 0 };
		if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return true;

		CRenderClip clip;
		CRenderClip::GenerateClip(hDC, rcTemp, clip);
		Control::DoPaint(hDC, rcPaint, pStopControl);

		if (m_items.GetSize() > 0) {
			RECT rc = m_rcItem;
			rc.left += m_rcPadding.left;
			rc.top += m_rcPadding.top;
			rc.right -= m_rcPadding.right;
			rc.bottom -= m_rcPadding.bottom;
			if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

			if (!::IntersectRect(&rcTemp, &rcPaint, &rc)) {
				for (int it = 0; it < m_items.GetSize(); it++) {
					Control* pControl = static_cast<Control*>(m_items[it]);
					if (pControl == pStopControl) return false;
					if (!pControl->IsVisible()) continue;
					if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) continue;
					if (pControl->IsFloat()) {
						if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) continue;
						if (!pControl->Paint(hDC, rcPaint, pStopControl)) return false;
					}
				}
			}
			else {
				CRenderClip childClip;
				CRenderClip::GenerateClip(hDC, rcTemp, childClip);
				for (int it = 0; it < m_items.GetSize(); it++) {
					Control* pControl = static_cast<Control*>(m_items[it]);
					if (pControl == pStopControl) return false;
					if (!pControl->IsVisible()) continue;
					if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) continue;
					if (pControl->IsFloat()) {
						if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) continue;
						CRenderClip::UseOldClipBegin(hDC, childClip);
						if (!pControl->Paint(hDC, rcPaint, pStopControl)) return false;
						CRenderClip::UseOldClipEnd(hDC, childClip);
					}
					else {
						if (!::IntersectRect(&rcTemp, &rc, &pControl->GetPos())) continue;
						if (!pControl->Paint(hDC, rcPaint, pStopControl)) return false;
					}
				}
			}
		}

		if (m_pVerticalScrollBar != NULL) {
			if (m_pVerticalScrollBar == pStopControl) return false;
			if (m_pVerticalScrollBar->IsVisible()) {
				if (::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos())) {
					if (!m_pVerticalScrollBar->Paint(hDC, rcPaint, pStopControl)) return false;
				}
			}
		}

		if (m_pHorizontalScrollBar != NULL) {
			if (m_pHorizontalScrollBar == pStopControl) return false;
			if (m_pHorizontalScrollBar->IsVisible()) {
				if (::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos())) {
					if (!m_pHorizontalScrollBar->Paint(hDC, rcPaint, pStopControl)) return false;
				}
			}
		}
		return true;
#else
		return __super::DoPaint(hDC, rcPaint, pStopControl);
#endif
	}
}

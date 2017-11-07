#include "StdAfx.h"

namespace dui
{
	ScrollContainer::ScrollContainer() :
		m_pVerticalScrollBar(NULL),
		m_pHorizontalScrollBar(NULL),
		m_bScrollProcess(false)
	{

	}

	ScrollContainer::~ScrollContainer()
	{
		if (m_pVerticalScrollBar) m_pVerticalScrollBar->Delete();
		if (m_pHorizontalScrollBar) m_pHorizontalScrollBar->Delete();
	}

	LPCTSTR ScrollContainer::GetClass() const
	{
		return DUI_CTR_SCROLLCONTAINER;
	}

	LPVOID ScrollContainer::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_SCROLLCONTAINER) == 0) 
			return static_cast<ScrollContainer*>(this);
		return __super::GetInterface(pstrName);
	}

	void ScrollContainer::SetMouseEnabled(bool bEnabled)
	{
		if (m_pVerticalScrollBar != NULL) m_pVerticalScrollBar->SetMouseEnabled(bEnabled);
		if (m_pHorizontalScrollBar != NULL) m_pHorizontalScrollBar->SetMouseEnabled(bEnabled);

		return __super::SetMouseEnabled(bEnabled);
	}

	void ScrollContainer::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("vscrollbar")) == 0) {
			EnableScrollBar(_tcscmp(pstrValue, _T("true")) == 0, GetHorizontalScrollBar() != NULL);
		}
		else if (_tcscmp(pstrName, _T("vscrollbarstyle")) == 0) {
			EnableScrollBar(true, GetHorizontalScrollBar() != NULL);
			if (GetVerticalScrollBar()) GetVerticalScrollBar()->SetAttributeList(pstrValue);
		}
		else if (_tcscmp(pstrName, _T("hscrollbar")) == 0) {
			EnableScrollBar(GetVerticalScrollBar() != NULL, _tcscmp(pstrValue, _T("true")) == 0);
		}
		else if (_tcscmp(pstrName, _T("hscrollbarstyle")) == 0) {
			EnableScrollBar(GetVerticalScrollBar() != NULL, true);
			if (GetHorizontalScrollBar()) GetHorizontalScrollBar()->SetAttributeList(pstrValue);
		}else
			__super::SetAttribute(pstrName, pstrValue);
	}

	void ScrollContainer::SetManager(CPaintManager* pManager, Control* pParent, bool bInit)
	{
		if (m_pVerticalScrollBar != NULL) m_pVerticalScrollBar->SetManager(pManager, this, bInit);
		if (m_pHorizontalScrollBar != NULL) m_pHorizontalScrollBar->SetManager(pManager, this, bInit);

		__super::SetManager(pManager, pParent, bInit);
	}

	Control* ScrollContainer::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		// Check if this guy is valid
		if ((uFlags & UIFIND_VISIBLE) != 0 && !IsVisible()) return NULL;
		if ((uFlags & UIFIND_ENABLED) != 0 && !IsEnabled()) return NULL;
		if ((uFlags & UIFIND_HITTEST) != 0 && !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData)))) return NULL;
		if ((uFlags & UIFIND_UPDATETEST) != 0 && Proc(this, pData) != NULL) return NULL;

		Control* pResult = NULL;
		if ((uFlags & UIFIND_ME_FIRST) != 0) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) pResult = Proc(this, pData);
		}
		if (pResult == NULL && m_pCover != NULL) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled()) pResult = m_pCover->FindControl(Proc, pData, uFlags);
		}

		if (pResult == NULL && m_pVerticalScrollBar != NULL) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
		}
		if (pResult == NULL && m_pHorizontalScrollBar != NULL) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);
		}

		if (pResult != NULL) return pResult;

		if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled()) {
			RECT rc = m_rcItem;
			rc.left += m_rcPadding.left;
			rc.top += m_rcPadding.top;
			rc.right -= m_rcPadding.right;
			rc.bottom -= m_rcPadding.bottom;

			if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

			if ((uFlags & UIFIND_TOP_FIRST) != 0) {
				for (int it = m_items.GetSize() - 1; it >= 0; it--) {
					pResult = static_cast<Control*>(m_items[it])->FindControl(Proc, pData, uFlags);
					if (pResult != NULL) {
						if ((uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))))
							continue;
						else
							return pResult;
					}
				}
			}
			else {
				for (int it = 0; it < m_items.GetSize(); it++) {
					pResult = static_cast<Control*>(m_items[it])->FindControl(Proc, pData, uFlags);
					if (pResult != NULL) {
						if ((uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))))
							continue;
						else
							return pResult;
					}
				}
			}
		}

		pResult = NULL;
		if (pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) pResult = Proc(this, pData);
		}
		return pResult;
	}

	void ScrollContainer::DoEvent(TEvent& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pParent != NULL) m_pParent->DoEvent(event);
			else Control::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_KEYDOWN)
		{
			if (IsKeyboardEnabled() && IsEnabled()) {
				if (m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() && m_pVerticalScrollBar->IsEnabled())
				{
					switch (event.chKey) {
					case VK_DOWN:
						LineDown();
						return;
					case VK_UP:
						LineUp();
						return;
					case VK_NEXT:
						PageDown();
						return;
					case VK_PRIOR:
						PageUp();
						return;
					case VK_HOME:
						HomeUp();
						return;
					case VK_END:
						EndDown();
						return;
					}
				}
				else if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled())
				{
					switch (event.chKey) {
					case VK_DOWN:
						LineRight();
						return;
					case VK_UP:
						LineLeft();
						return;
					case VK_NEXT:
						PageRight();
						return;
					case VK_PRIOR:
						PageLeft();
						return;
					case VK_HOME:
						HomeLeft();
						return;
					case VK_END:
						EndRight();
						return;
					}
				}
			}
		}
		else if (event.Type == UIEVENT_SCROLLWHEEL)
		{
			if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled())
			{
				RECT rcHorizontalScrollBar = m_pHorizontalScrollBar->GetPos();
				if (::PtInRect(&rcHorizontalScrollBar, event.ptMouse))
				{
					switch (LOWORD(event.wParam)) {
					case SB_LINEUP:
						LineLeft();
						return;
					case SB_LINEDOWN:
						LineRight();
						return;
					}
				}
			}
			if (m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() && m_pVerticalScrollBar->IsEnabled())
			{
				switch (LOWORD(event.wParam)) {
				case SB_LINEUP:
					LineUp();
					return;
				case SB_LINEDOWN:
					LineDown();
					return;
				}
			}
			if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled())
			{
				switch (LOWORD(event.wParam)) {
				case SB_LINEUP:
					LineLeft();
					return;
				case SB_LINEDOWN:
					LineRight();
					return;
				}
			}
		}
		return __super::DoEvent(event);
	}

	bool ScrollContainer::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
	{
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
	}

	void ScrollContainer::SetFloatPos(int iIndex)
	{
		// 因为Control::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
		if (iIndex < 0 || iIndex >= m_items.GetSize()) return;

		Control* pControl = static_cast<Control*>(m_items[iIndex]);

		if (!pControl->IsVisible()) return;
		if (!pControl->IsFloat()) return;

		SIZE szXY = pControl->GetFixedXY();
		SIZE sz = { pControl->GetFixedWidth(), pControl->GetFixedHeight() };
		TPercentInfo rcPercent = pControl->GetFloatPercent();
		LONG width = m_rcItem.right - m_rcItem.left;
		LONG height = m_rcItem.bottom - m_rcItem.top;
		RECT rcCtrl = { 0 };
		rcCtrl.left = (LONG)(width*rcPercent.left) + szXY.cx;
		rcCtrl.top = (LONG)(height*rcPercent.top) + szXY.cy;
		rcCtrl.right = (LONG)(width*rcPercent.right) + szXY.cx + sz.cx;
		rcCtrl.bottom = (LONG)(height*rcPercent.bottom) + szXY.cy + sz.cy;
		pControl->SetPos(rcCtrl, false);
	}

	void ScrollContainer::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
	{
		if (m_pHorizontalScrollBar != NULL) {
			if (m_pVerticalScrollBar == NULL) {
				if (cxRequired > rc.right - rc.left && !m_pHorizontalScrollBar->IsVisible()) {
					m_pHorizontalScrollBar->SetVisible(true);
					m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
					m_pHorizontalScrollBar->SetScrollPos(0);
					m_bScrollProcess = true;
					if (!IsFloat()) SetPos(GetPos());
					else SetPos(GetRelativePos());
					m_bScrollProcess = false;
					return;
				}
				// No scrollbar required
				if (!m_pHorizontalScrollBar->IsVisible()) return;

				// Scroll not needed anymore?
				int cxScroll = cxRequired - (rc.right - rc.left);
				if (cxScroll <= 0 && !m_bScrollProcess) {
					m_pHorizontalScrollBar->SetVisible(false);
					m_pHorizontalScrollBar->SetScrollPos(0);
					m_pHorizontalScrollBar->SetScrollRange(0);
					if (!IsFloat()) SetPos(GetPos());
					else SetPos(GetRelativePos());
				}
				else
				{
					RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight() };
					m_pHorizontalScrollBar->SetPos(rcScrollBarPos, false);

					if (m_pHorizontalScrollBar->GetScrollRange() != cxScroll) {
						int iScrollPos = m_pHorizontalScrollBar->GetScrollPos();
						m_pHorizontalScrollBar->SetScrollRange(::abs(cxScroll));
						if (m_pHorizontalScrollBar->GetScrollRange() == 0) {
							m_pHorizontalScrollBar->SetVisible(false);
							m_pHorizontalScrollBar->SetScrollPos(0);
						}
						if (iScrollPos > m_pHorizontalScrollBar->GetScrollPos()) {
							if (!IsFloat()) SetPos(GetPos(), false);
							else SetPos(GetRelativePos(), false);
						}
					}
				}
				return;
			}
			else {
				bool bNeedSetPos = false;
				if (cxRequired > rc.right - rc.left) {
					if (!m_pHorizontalScrollBar->IsVisible()) {
						m_pHorizontalScrollBar->SetVisible(true);
						m_pHorizontalScrollBar->SetScrollPos(0);
						rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
					}
					RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight() };
					m_pHorizontalScrollBar->SetPos(rcScrollBarPos, false);
					if (m_pHorizontalScrollBar->GetScrollRange() != cxRequired - (rc.right - rc.left)) {
						m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
						bNeedSetPos = true;
					}
				}
				else {
					if (m_pHorizontalScrollBar->IsVisible()) {
						m_pHorizontalScrollBar->SetVisible(false);
						rc.bottom += m_pHorizontalScrollBar->GetFixedHeight();
					}
				}

				if (cyRequired > rc.bottom - rc.top && !m_pVerticalScrollBar->IsVisible()) {
					m_pVerticalScrollBar->SetVisible(true);
					m_pVerticalScrollBar->SetScrollRange(cyRequired - (rc.bottom - rc.top));
					m_pVerticalScrollBar->SetScrollPos(0);
					rc.right -= m_pVerticalScrollBar->GetFixedWidth();
					if (m_pHorizontalScrollBar->IsVisible()) {
						RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight() };
						m_pHorizontalScrollBar->SetPos(rcScrollBarPos, false);
						m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
					}
					m_bScrollProcess = true;
					if (!IsFloat()) SetPos(GetPos());
					else SetPos(GetRelativePos());
					m_bScrollProcess = false;
					return;
				}
				// No scrollbar required
				if (!m_pVerticalScrollBar->IsVisible()) {
					if (bNeedSetPos) {
						if (!IsFloat()) SetPos(GetPos());
						else SetPos(GetRelativePos());
					}
					return;
				}

				// Scroll not needed anymore?
				int cyScroll = cyRequired - (rc.bottom - rc.top);
				if (cyScroll <= 0 && !m_bScrollProcess) {
					m_pVerticalScrollBar->SetVisible(false);
					m_pVerticalScrollBar->SetScrollPos(0);
					m_pVerticalScrollBar->SetScrollRange(0);
					rc.right += m_pVerticalScrollBar->GetFixedWidth();
					if (m_pHorizontalScrollBar->IsVisible()) {
						RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight() };
						m_pHorizontalScrollBar->SetPos(rcScrollBarPos, false);
						m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
					}
					if (!IsFloat()) SetPos(GetPos());
					else SetPos(GetRelativePos());
				}
				else
				{
					RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
					m_pVerticalScrollBar->SetPos(rcScrollBarPos, false);

					if (m_pVerticalScrollBar->GetScrollRange() != cyScroll) {
						int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
						m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll));
						if (m_pVerticalScrollBar->GetScrollRange() == 0) {
							m_pVerticalScrollBar->SetVisible(false);
							m_pVerticalScrollBar->SetScrollPos(0);
						}
						if (iScrollPos > m_pVerticalScrollBar->GetScrollPos() || bNeedSetPos) {
							if (!IsFloat()) SetPos(GetPos(), false);
							else SetPos(GetRelativePos(), false);
						}
					}
				}
			}
		}
		else {
			if (m_pVerticalScrollBar == NULL) return;

			if (cyRequired > rc.bottom - rc.top && !m_pVerticalScrollBar->IsVisible()) {
				m_pVerticalScrollBar->SetVisible(true);
				m_pVerticalScrollBar->SetScrollRange(cyRequired - (rc.bottom - rc.top));
				m_pVerticalScrollBar->SetScrollPos(0);
				m_bScrollProcess = true;
				if (!IsFloat()) SetPos(GetPos());
				else SetPos(GetRelativePos());
				m_bScrollProcess = false;
				return;
			}
			// No scrollbar required
			if (!m_pVerticalScrollBar->IsVisible()) return;

			// Scroll not needed anymore?
			int cyScroll = cyRequired - (rc.bottom - rc.top);
			if (cyScroll <= 0 && !m_bScrollProcess) {
				m_pVerticalScrollBar->SetVisible(false);
				m_pVerticalScrollBar->SetScrollPos(0);
				m_pVerticalScrollBar->SetScrollRange(0);
				if (!IsFloat()) SetPos(GetPos());
				else SetPos(GetRelativePos());
			}
			else
			{
				RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
				m_pVerticalScrollBar->SetPos(rcScrollBarPos, false);

				if (m_pVerticalScrollBar->GetScrollRange() != cyScroll) {
					int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
					m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll));
					if (m_pVerticalScrollBar->GetScrollRange() == 0) {
						m_pVerticalScrollBar->SetVisible(false);
						m_pVerticalScrollBar->SetScrollPos(0);
					}
					if (iScrollPos > m_pVerticalScrollBar->GetScrollPos()) {
						if (!IsFloat()) SetPos(GetPos(), false);
						else SetPos(GetRelativePos(), false);
					}
				}
			}
		}
	}

	SIZE ScrollContainer::GetScrollPos() const
	{
		SIZE sz = { 0, 0 };
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) sz.cy = m_pVerticalScrollBar->GetScrollPos();
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) sz.cx = m_pHorizontalScrollBar->GetScrollPos();
		return sz;
	}

	SIZE ScrollContainer::GetScrollRange() const
	{
		SIZE sz = { 0, 0 };
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) sz.cy = m_pVerticalScrollBar->GetScrollRange();
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) sz.cx = m_pHorizontalScrollBar->GetScrollRange();
		return sz;
	}

	void ScrollContainer::SetScrollPos(SIZE szPos)
	{
		int cx = 0;
		int cy = 0;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
			cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if (cx == 0 && cy == 0) return;

		for (int it2 = 0; it2 < m_items.GetSize(); it2++) {
			Control* pControl = static_cast<Control*>(m_items[it2]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) continue;
			pControl->Move(CDuiSize(-cx, -cy), false);
		}

		Invalidate();
	}

	void ScrollContainer::LineUp()
	{
		int cyLine = SCROLLBAR_LINESIZE;
		if (m_pManager) {
			cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;
			if (m_pVerticalScrollBar && m_pVerticalScrollBar->GetScrollUnit() > 1)
				cyLine = m_pVerticalScrollBar->GetScrollUnit();
		}

		SIZE sz = GetScrollPos();
		sz.cy -= cyLine;
		SetScrollPos(sz);
	}

	void ScrollContainer::LineDown()
	{
		int cyLine = SCROLLBAR_LINESIZE;
		if (m_pManager) {
			cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;
			if (m_pVerticalScrollBar && m_pVerticalScrollBar->GetScrollUnit() > 1)
				cyLine = m_pVerticalScrollBar->GetScrollUnit();
		}

		SIZE sz = GetScrollPos();
		sz.cy += cyLine;
		SetScrollPos(sz);
	}

	void ScrollContainer::PageUp()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcPadding.top - m_rcPadding.bottom;
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
		sz.cy -= iOffset;
		SetScrollPos(sz);
	}

	void ScrollContainer::PageDown()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcPadding.top - m_rcPadding.bottom;
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
		sz.cy += iOffset;
		SetScrollPos(sz);
	}

	void ScrollContainer::HomeUp()
	{
		SIZE sz = GetScrollPos();
		sz.cy = 0;
		SetScrollPos(sz);
	}

	void ScrollContainer::EndDown()
	{
		SIZE sz = GetScrollPos();
		sz.cy = GetScrollRange().cy;
		SetScrollPos(sz);
	}

	void ScrollContainer::LineLeft()
	{
		int cxLine = SCROLLBAR_LINESIZE;
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->GetScrollUnit() > 1)
			cxLine = m_pHorizontalScrollBar->GetScrollUnit();

		SIZE sz = GetScrollPos();
		sz.cx -= cxLine;
		SetScrollPos(sz);
	}

	void ScrollContainer::LineRight()
	{
		int cxLine = SCROLLBAR_LINESIZE;
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->GetScrollUnit() > 1)
			cxLine = m_pHorizontalScrollBar->GetScrollUnit();

		SIZE sz = GetScrollPos();
		sz.cx += cxLine;
		SetScrollPos(sz);
	}

	void ScrollContainer::PageLeft()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcPadding.left - m_rcPadding.right;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
		sz.cx -= iOffset;
		SetScrollPos(sz);
	}

	void ScrollContainer::PageRight()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcPadding.left - m_rcPadding.right;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
		sz.cx += iOffset;
		SetScrollPos(sz);
	}

	void ScrollContainer::HomeLeft()
	{
		SIZE sz = GetScrollPos();
		sz.cx = 0;
		SetScrollPos(sz);
	}

	void ScrollContainer::EndRight()
	{
		SIZE sz = GetScrollPos();
		sz.cx = GetScrollRange().cx;
		SetScrollPos(sz);
	}

	void ScrollContainer::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
	{
		if (bEnableVertical && !m_pVerticalScrollBar) {
			m_pVerticalScrollBar = new ScrollBar;
			m_pVerticalScrollBar->SetScrollRange(0);
			m_pVerticalScrollBar->SetOwner(this);
			m_pVerticalScrollBar->SetManager(m_pManager, NULL, false);
			if (m_pManager) {
				LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("VScrollBar"));
				if (pDefaultAttributes) {
					m_pVerticalScrollBar->SetAttributeList(pDefaultAttributes);
				}
			}
		}
		else if (!bEnableVertical && m_pVerticalScrollBar) {
			m_pVerticalScrollBar->Delete();
			m_pVerticalScrollBar = NULL;
		}

		if (bEnableHorizontal && !m_pHorizontalScrollBar) {
			m_pHorizontalScrollBar = new ScrollBar;
			m_pHorizontalScrollBar->SetScrollRange(0);
			m_pHorizontalScrollBar->SetHorizontal(true);
			m_pHorizontalScrollBar->SetOwner(this);
			m_pHorizontalScrollBar->SetManager(m_pManager, NULL, false);
			if (m_pManager) {
				LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("HScrollBar"));
				if (pDefaultAttributes) {
					m_pHorizontalScrollBar->SetAttributeList(pDefaultAttributes);
				}
			}
		}
		else if (!bEnableHorizontal && m_pHorizontalScrollBar) {
			m_pHorizontalScrollBar->Delete();
			m_pHorizontalScrollBar = NULL;
		}

		NeedUpdate();
	}

	ScrollBar* ScrollContainer::GetVerticalScrollBar() const
	{
		return m_pVerticalScrollBar;
	}

	ScrollBar* ScrollContainer::GetHorizontalScrollBar() const
	{
		return m_pHorizontalScrollBar;
	}

	int ScrollContainer::FindSelectable(int iIndex, bool bForward /*= true*/) const
	{
		// NOTE: This is actually a helper-function for the list/combo/ect controls
		//       that allow them to find the next enabled/available selectable item
		if (GetCount() == 0) return -1;
		iIndex = CLAMP(iIndex, 0, GetCount() - 1);
		if (bForward) {
			for (int i = iIndex; i < GetCount(); i++) {
				if (GetItemAt(i)->GetInterface(DUI_CTR_ILISTITEM) != NULL
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled()) return i;
			}
			return -1;
		}
		else {
			for (int i = iIndex; i >= 0; --i) {
				if (GetItemAt(i)->GetInterface(DUI_CTR_ILISTITEM) != NULL
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled()) return i;
			}
			return FindSelectable(0, true);
		}
	}

	RECT ScrollContainer::GetClientPos() const
	{
		RECT rc = m_rcItem;
		rc.left += m_rcPadding.left;
		rc.top += m_rcPadding.top;
		rc.right -= m_rcPadding.right;
		rc.bottom -= m_rcPadding.bottom;

		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		}
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		}
		return rc;
	}

	void ScrollContainer::SetPos(RECT rc, bool bNeedInvalidate)
	{
#if 1
		Control::SetPos(rc, bNeedInvalidate);
		rc = m_rcItem;
		if (m_items.IsEmpty()) return;

		rc.left += m_rcPadding.left;
		rc.top += m_rcPadding.top;
		rc.right -= m_rcPadding.right;
		rc.bottom -= m_rcPadding.bottom;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		for (int it2 = 0; it2 < m_items.GetSize(); it2++) {
			Control* pControl = static_cast<Control*>(m_items[it2]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) {
				SetFloatPos(it2);
				continue;
			}

			int iControlFixedWidth = pControl->GetFixedWidth();
			int iControlFixedHeight = pControl->GetFixedHeight();
			RECT rcMargin = pControl->GetMargin();
			SIZE sz = { iControlFixedWidth, iControlFixedHeight };

			int iPosX = rc.left;
			int iPosY = rc.top;
			if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
				iPosY -= m_pVerticalScrollBar->GetScrollPos();
			}
			RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + sz.cx + rcMargin.left, iPosY + sz.cy + rcMargin.top };
			pControl->SetPos(rcCtrl, false);
		}
#else
		Control::SetPos(rc, bNeedInvalidate);
		if (m_items.IsEmpty()) return;

		rc = m_rcItem;
		rc.left += m_rcPadding.left;
		rc.top += m_rcPadding.top;
		rc.right -= m_rcPadding.right;
		rc.bottom -= m_rcPadding.bottom;

		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
			rc.top -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom += m_pVerticalScrollBar->GetScrollRange();
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		}
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
			rc.left -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right += m_pHorizontalScrollBar->GetScrollRange();
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		}

		for (int it = 0; it < m_items.GetSize(); it++) {
			Control* pControl = static_cast<Control*>(m_items[it]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) {
				SetFloatPos(it);
			}
			else {
				SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
				RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
				pControl->SetPos(rcCtrl, false);
			}
		}
#endif
	}

	void ScrollContainer::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		Control::Move(szOffset, bNeedInvalidate);
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) m_pVerticalScrollBar->Move(szOffset, false);
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) m_pHorizontalScrollBar->Move(szOffset, false);
		for (int it = 0; it < m_items.GetSize(); it++) {
			Control* pControl = static_cast<Control*>(m_items[it]);
			if (pControl != NULL && pControl->IsVisible()) pControl->Move(szOffset, false);
		}
	}
}
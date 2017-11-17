#include "StdAfx.h"

namespace dui
{

	Container::Container()
		: m_iChildMargin(0),
		m_iChildAlign(DT_LEFT),
		m_iChildVAlign(DT_TOP),
		m_bAutoDestroy(true),
		m_bDelayedDestroy(true),
		m_bMouseChildEnabled(true)
	{
		::ZeroMemory(&m_rcPadding, sizeof(m_rcPadding));
	}

	Container::~Container()
	{
		m_bDelayedDestroy = false;
		RemoveAll();
	}

	LPCTSTR Container::GetClass() const
	{
		return DUI_CTR_SCROLLCONTAINER;
	}

	LPVOID Container::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_ICONTAINER) == 0) return static_cast<IContainer*>(this);
		else if (_tcscmp(pstrName, DUI_CTR_CONTAINER) == 0) return static_cast<Container*>(this);
		return __super::GetInterface(pstrName);
	}

	void Container::SetPos(RECT rc, bool bNeedInvalidate)
	{
		__super::SetPos(rc, bNeedInvalidate);
		rc = m_rcItem;
		if (m_items.IsEmpty()) return;

		rc.left += m_rcPadding.left;
		rc.top += m_rcPadding.top;
		rc.right -= m_rcPadding.right;
		rc.bottom -= m_rcPadding.bottom;

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
			
			RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + sz.cx + rcMargin.left, iPosY + sz.cy + rcMargin.top };
			pControl->SetPos(rcCtrl, false);
		}
	}


	Control* Container::GetItemAt(int iIndex) const
	{
		if (iIndex < 0 || iIndex >= m_items.GetSize()) return NULL;
		return static_cast<Control*>(m_items[iIndex]);
	}

	int Container::GetItemIndex(Control* pControl) const
	{
		for (int it = 0; it < m_items.GetSize(); it++) {
			if (static_cast<Control*>(m_items[it]) == pControl) {
				return it;
			}
		}

		return -1;
	}

	bool Container::SetItemIndex(Control* pControl, int iNewIndex)
	{
		for (int it = 0; it < m_items.GetSize(); it++) {
			if (static_cast<Control*>(m_items[it]) == pControl) {
				NeedUpdate();
				m_items.Remove(it);
				return m_items.InsertAt(iNewIndex, pControl);
			}
		}

		return false;
	}

	bool Container::SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex)
	{
		if (pStartControl == NULL || iCount < 0 || iNewStartIndex < 0) return false;
		int iStartIndex = GetItemIndex(pStartControl);
		if (iStartIndex == iNewStartIndex) return true;
		if (iStartIndex + iCount > GetCount()) return false;
		if (iNewStartIndex + iCount > GetCount()) return false;

		PtrArray pControls(iCount);
		pControls.Resize(iCount);
		::CopyMemory(pControls.GetData(), m_items.GetData() + iStartIndex, iCount * sizeof(LPVOID));
		m_items.Remove(iStartIndex, iCount);

		for (int it3 = 0; it3 < pControls.GetSize(); it3++) {
			if (!pControls.InsertAt(iNewStartIndex + it3, pControls[it3])) return false;
		}

		NeedUpdate();
		return true;
	}

	int Container::GetCount() const
	{
		return m_items.GetSize();
	}

	bool Container::Add(Control* pControl)
	{
		if (pControl == NULL) return false;

		if (m_pManager != NULL) m_pManager->InitControls(pControl, this);
		if (IsVisible()) NeedUpdate();
		else pControl->SetInternVisible(false);
		return m_items.Add(pControl);
	}

	bool Container::AddAt(Control* pControl, int iIndex)
	{
		if (pControl == NULL) return false;

		if (m_pManager != NULL) m_pManager->InitControls(pControl, this);
		if (IsVisible()) NeedUpdate();
		else pControl->SetInternVisible(false);
		return m_items.InsertAt(iIndex, pControl);
	}

	bool Container::Remove(Control* pControl, bool bDoNotDestroy)
	{
		if (pControl == NULL) return false;

		for (int it = 0; it < m_items.GetSize(); it++) {
			if (static_cast<Control*>(m_items[it]) == pControl) {
				NeedUpdate();
				if (!bDoNotDestroy && m_bAutoDestroy) {
					if (m_bDelayedDestroy && m_pManager) m_pManager->AddDelayedCleanup(pControl);
					else pControl->Delete();
				}
				return m_items.Remove(it);
			}
		}
		return false;
	}

	bool Container::RemoveAt(int iIndex, bool bDoNotDestroy)
	{
		Control* pControl = GetItemAt(iIndex);
		if (pControl != NULL) {
			return (pControl, bDoNotDestroy);
		}

		return false;
	}

	void Container::RemoveAll()
	{
		for (int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++) {
			if (m_bDelayedDestroy && m_pManager) m_pManager->AddDelayedCleanup(static_cast<Control*>(m_items[it]));
			else static_cast<Control*>(m_items[it])->Delete();
		}
		m_items.Empty();
		NeedUpdate();
	}

	void Container::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("padding")) == 0) {
			RECT rcPadding = { 0 };
			LPTSTR pstr = NULL;
			rcPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetPadding(rcPadding);
		}
		else if (_tcscmp(pstrName, _T("mousechild")) == 0) SetMouseChildEnabled(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("childmargin")) == 0) SetChildMargin(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("childalign")) == 0) {
			if (_tcscmp(pstrValue, _T("left")) == 0) m_iChildAlign = DT_LEFT;
			else if (_tcscmp(pstrValue, _T("center")) == 0) m_iChildAlign = DT_CENTER;
			else if (_tcscmp(pstrValue, _T("right")) == 0) m_iChildAlign = DT_RIGHT;
		}
		else if (_tcscmp(pstrName, _T("childvalign")) == 0) {
			if (_tcscmp(pstrValue, _T("top")) == 0) m_iChildVAlign = DT_TOP;
			else if (_tcscmp(pstrValue, _T("vcenter")) == 0) m_iChildVAlign = DT_VCENTER;
			else if (_tcscmp(pstrValue, _T("bottom")) == 0) m_iChildVAlign = DT_BOTTOM;
		}
		else __super::SetAttribute(pstrName, pstrValue);
	}

	void Container::SetVisible(bool bVisible)
	{
		if (m_bVisible == bVisible) return;
		Control::SetVisible(bVisible);
		for (int it = 0; it < m_items.GetSize(); it++) {
			static_cast<Control*>(m_items[it])->SetInternVisible(IsVisible());
		}
	}

	// 逻辑上，对于ScrollContainer控件不公开此方法
	// 调用此方法的结果是，内部子控件隐藏，控件本身依然显示，背景等效果存在
	void Container::SetInternVisible(bool bVisible)
	{
		__super::SetInternVisible(bVisible);
		if (m_items.IsEmpty()) return;
		for (int it = 0; it < m_items.GetSize(); it++) {
			// 控制子控件显示状态
			// InternVisible状态应由子控件自己控制
			static_cast<Control*>(m_items[it])->SetInternVisible(IsVisible());
		}
	}

	bool Container::IsAutoDestroy() const
	{
		return m_bAutoDestroy;
	}

	void Container::SetAutoDestroy(bool bAuto)
	{
		m_bAutoDestroy = bAuto;
	}

	bool Container::IsDelayedDestroy() const
	{
		return m_bDelayedDestroy;
	}

	void Container::SetDelayedDestroy(bool bDelayed)
	{
		m_bDelayedDestroy = bDelayed;
	}

	RECT Container::GetPadding() const
	{
		return m_rcPadding;
	}

	void Container::SetPadding(RECT rcPadding)
	{
		m_rcPadding = rcPadding;
		NeedUpdate();
	}

	int Container::GetChildMargin() const
	{
		return m_iChildMargin;
	}

	void Container::SetChildMargin(int iPadding)
	{
		m_iChildMargin = iPadding;
		if (m_iChildMargin < 0) m_iChildMargin = 0;
		NeedUpdate();
	}

	UINT Container::GetChildAlign() const
	{
		return m_iChildAlign;
	}

	void Container::SetChildAlign(UINT iAlign)
	{
		m_iChildAlign = iAlign;
		NeedUpdate();
	}

	UINT Container::GetChildVAlign() const
	{
		return m_iChildVAlign;
	}

	void Container::SetChildVAlign(UINT iVAlign)
	{
		m_iChildVAlign = iVAlign;
		NeedUpdate();
	}

	bool Container::IsMouseChildEnabled() const
	{
		return m_bMouseChildEnabled;
	}

	void Container::SetMouseChildEnabled(bool bEnable)
	{
		m_bMouseChildEnabled = bEnable;
	}

	void Container::DoEvent(TEvent& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pParent != NULL) m_pParent->DoEvent(event);
			else __super::DoEvent(event);
			return;
		}
		if (event.Type == UIEVENT_SETFOCUS)
		{
			m_bFocused = true;
			return;
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			m_bFocused = false;
			return;
		}
		
		__super::DoEvent(event);
	}

	bool Container::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
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

		return true;
	}

	void Container::SetManager(CPaintManager* pManager, Control* pParent, bool bInit)
	{
		for (int it = 0; it < m_items.GetSize(); it++) {
			static_cast<Control*>(m_items[it])->SetManager(pManager, this, bInit);
		}

		__super::SetManager(pManager, pParent, bInit);
	}

	Control* Container::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
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
		if (pResult != NULL) return pResult;

		if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled()) {
			RECT rc = m_rcItem;
			rc.left += m_rcPadding.left;
			rc.top += m_rcPadding.top;
			rc.right -= m_rcPadding.right;
			rc.bottom -= m_rcPadding.bottom;
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

	Control* Container::FindSubControl(LPCTSTR pstrSubControlName)
	{
		Control* pSubControl = NULL;
		pSubControl = static_cast<Control*>(GetManager()->FindSubControlByName(this, pstrSubControlName));
		return pSubControl;
	}

	int Container::FindSelectable(int iIndex, bool bForward /*= true*/) const
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

	void Container::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		__super::Move(szOffset, bNeedInvalidate);
		for (int it = 0; it < m_items.GetSize(); it++) {
			Control* pControl = static_cast<Control*>(m_items[it]);
			if (pControl != NULL && pControl->IsVisible()) pControl->Move(szOffset, false);
		}
	}

	void Container::SetFloatPos(int iIndex)
	{
		// 因为Control::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return;

		Control* pControl = static_cast<Control*>(m_items[iIndex]);

		if( !pControl->IsVisible() ) return;
		if( !pControl->IsFloat() ) return;

		SIZE szXY = pControl->GetFixedXY();
		SIZE sz = {pControl->GetFixedWidth(), pControl->GetFixedHeight()};
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
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	
} // namespace dui

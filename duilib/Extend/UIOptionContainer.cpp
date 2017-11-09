#include "StdAfx.h"

namespace dui{

	OptionContainer::OptionContainer() : m_bSelected(false), m_dwSelectedBkColor(0), m_dwSelectedTextColor(0)
	{
		
	}

	OptionContainer::~OptionContainer()
	{
		if (!m_sGroupName.empty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName.c_str(), this);
	}
	LPCTSTR OptionContainer::GetClass() const
	{
		return DUI_CTR_OPTIONCONTAINER;
	}

	LPVOID OptionContainer::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_OPTIONCONTAINER) == 0) return static_cast<OptionContainer*>(this);
		return __super::GetInterface(pstrName);
	}

	void OptionContainer::SetManager(CPaintManager* pManager, Control* pParent, bool bInit)
	{
		__super::SetManager(pManager, pParent, bInit);
		if (bInit && !m_sGroupName.empty()) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName.c_str(), this);
		}
	}

	LPCTSTR OptionContainer::GetGroup() const
	{
		return m_sGroupName.c_str();
	}

	void OptionContainer::SetGroup(LPCTSTR pStrGroupName)
	{
		if (pStrGroupName == NULL) {
			if (m_sGroupName.empty()) return;
			m_sGroupName.clear();
		}
		else {
			if (m_sGroupName == pStrGroupName) return;
			if (!m_sGroupName.empty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName.c_str(), this);
			m_sGroupName = pStrGroupName;
		}

		if (!m_sGroupName.empty()) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName.c_str(), this);
		}
		else {
			if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName.c_str(), this);
		}

		Selected(m_bSelected);
	}

	bool OptionContainer::IsSelected() const
	{
		return m_bSelected;
	}

	void OptionContainer::Selected(bool bSelected, bool bTriggerEvent)
	{
		if (m_bSelected == bSelected) return;
		m_bSelected = bSelected;
		if (m_bSelected) m_uButtonState |= UISTATE_SELECTED;
		else m_uButtonState &= ~UISTATE_SELECTED;

		if (m_pManager != NULL) {
			if (!m_sGroupName.empty()) {
				if (m_bSelected) {
					PtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName.c_str());
					for (int i = 0; i < aOptionGroup->GetSize(); i++) {
						OptionContainer* pControl = static_cast<OptionContainer*>(aOptionGroup->GetAt(i));
						if (pControl != this) {
							pControl->Selected(false, bTriggerEvent);
						}
					}
					if (bTriggerEvent) m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
				}
			}
			else {
				if (bTriggerEvent) m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}

		Invalidate();
	}

	bool OptionContainer::Activate()
	{
		if (!__super::Activate()) return false;
		if (!m_sGroupName.empty()) Selected(true);
		else Selected(!m_bSelected);

		return true;
	}

	void OptionContainer::SetEnabled(bool bEnable)
	{
		__super::SetEnabled(bEnable);
		if (!IsEnabled()) {
			if (m_bSelected) m_uButtonState = UISTATE_SELECTED;
			else m_uButtonState = 0;
		}
	}

	LPCTSTR OptionContainer::GetSelectedImage()
	{
		return m_diSelected.sDrawString.c_str();
	}

	void OptionContainer::SetSelectedImage(LPCTSTR pStrImage)
	{
		if (m_diSelected.sDrawString == pStrImage && m_diSelected.pImageInfo != NULL) return;
		m_diSelected.Clear();
		m_diSelected.sDrawString = pStrImage;
		Invalidate();
	}

	LPCTSTR OptionContainer::GetSelectedHotImage()
	{
		return m_diSelectedHot.sDrawString.c_str();
	}

	void OptionContainer::SetSelectedHotImage(LPCTSTR pStrImage)
	{
		if (m_diSelectedHot.sDrawString == pStrImage && m_diSelectedHot.pImageInfo != NULL) return;
		m_diSelectedHot.Clear();
		m_diSelectedHot.sDrawString = pStrImage;
		Invalidate();
	}

	void OptionContainer::SetSelectedTextColor(DWORD dwTextColor)
	{
		m_dwSelectedTextColor = dwTextColor;
	}

	DWORD OptionContainer::GetSelectedTextColor()
	{
		if (m_dwSelectedTextColor == 0) m_dwSelectedTextColor = m_pManager->GetDefaultFontColor();
		return m_dwSelectedTextColor;
	}

	void OptionContainer::SetSelectedBkColor(DWORD dwBkColor)
	{
		m_dwSelectedBkColor = dwBkColor;
	}

	DWORD OptionContainer::GetSelectBkColor()
	{
		return m_dwSelectedBkColor;
	}

	SIZE OptionContainer::EstimateSize(SIZE szAvailable)
	{
		if (m_cxyFixed.cy == 0) return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
		return __super::EstimateSize(szAvailable);
	}

	void OptionContainer::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("group")) == 0) SetGroup(pstrValue);
		else if (_tcscmp(pstrName, _T("selected")) == 0) Selected(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("selectedimage")) == 0) SetSelectedImage(pstrValue);
		else if (_tcscmp(pstrName, _T("selectedhotimage")) == 0) SetSelectedHotImage(pstrValue);
		else if (_tcscmp(pstrName, _T("selectedbkcolor")) == 0) {
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedBkColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("selectedtextcolor")) == 0) {
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetSelectedTextColor(clrColor);
		}
		else __super::SetAttribute(pstrName, pstrValue);
	}

	void OptionContainer::PaintStatusImage(HDC hDC)
	{
		if ((m_uButtonState & UISTATE_SELECTED) != 0) {
			if ((m_uButtonState & UISTATE_HOT) != 0)
			{
				if (DrawImage(hDC, m_diSelectedHot)) goto Label_ForeImage;
			}

			if (DrawImage(hDC, m_diSelected)) goto Label_ForeImage;
			else if (m_dwSelectedBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
				goto Label_ForeImage;
			}
		}

		UINT uSavedState = m_uButtonState;
		m_uButtonState &= ~UISTATE_PUSHED;
		__super::PaintStatusImage(hDC);
		m_uButtonState = uSavedState;

	Label_ForeImage:
		DrawImage(hDC, m_diFore);
	}

	void OptionContainer::PaintText(HDC hDC)
	{
		if ((m_uButtonState & UISTATE_SELECTED) != 0)
		{
			DWORD oldTextColor = m_dwTextColor;
			if (m_dwSelectedTextColor != 0) m_dwTextColor = m_dwSelectedTextColor;

			if (m_dwTextColor == 0) m_dwTextColor = m_pManager->GetDefaultFontColor();
			if (m_dwDisabledTextColor == 0) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

			if (m_sText.empty()) return;
			int nLinks = 0;
			RECT rc = m_rcItem;
			rc.left += m_rcTextPadding.left;
			rc.right -= m_rcTextPadding.right;
			rc.top += m_rcTextPadding.top;
			rc.bottom -= m_rcTextPadding.bottom;

			if (m_bShowHtml)
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor, \
				NULL, NULL, nLinks, m_iFont, m_uTextStyle);
			else
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText.c_str(), IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor, \
				m_iFont, m_uTextStyle);

			m_dwTextColor = oldTextColor;
		}
		else
		{
			UINT uSavedState = m_uButtonState;
			m_uButtonState &= ~UISTATE_PUSHED;
			__super::PaintText(hDC);
			m_uButtonState = uSavedState;
		}
	}
}
#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace dui
{
	class DUILIB_API OptionBtn : public Button
	{
	public:
		OptionBtn();
		virtual ~OptionBtn();

		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);

		void SetManager(UIManager* pManager, Control* pParent, bool bInit = true);

		bool Activate();
		void SetEnabled(bool bEnable = true);

		LPCTSTR GetSelectedImage();
		void SetSelectedImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedHotImage();
		void SetSelectedHotImage(LPCTSTR pStrImage);

		void SetSelectedTextColor(DWORD dwTextColor);
		DWORD GetSelectedTextColor();

		void SetSelectedBkColor(DWORD dwBkColor);
		DWORD GetSelectBkColor();

		LPCTSTR GetForeImage();
		void SetForeImage(LPCTSTR pStrImage);

		LPCTSTR GetGroup() const;
		void SetGroup(LPCTSTR pStrGroupName = NULL);
		bool IsSelected() const;
		virtual void Selected(bool bSelected, bool bTriggerEvent=true);

		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

	protected:
		bool			m_bSelected;
		String		m_sGroupName;

		DWORD			m_dwSelectedBkColor;
		DWORD			m_dwSelectedTextColor;

		DrawInfo		m_diSelected;
		DrawInfo		m_diSelectedHot;
	};

} // namespace dui

#endif // __UIOPTION_H__
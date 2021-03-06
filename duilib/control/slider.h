#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#pragma once

namespace dui
{
	class DUILIB_API Slider : public Progress
	{
	public:
		Slider();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetEnabled(bool bEnable = true);

		int GetChangeStep();
		void SetChangeStep(int step);
		void SetThumbSize(SIZE szXY);
		RECT GetThumbRect() const;
		bool IsImmMode() const;
		void SetImmMode(bool bImmMode);
		LPCTSTR GetThumbImage() const;
		void SetThumbImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage() const;
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage() const;
		void SetThumbPushedImage(LPCTSTR pStrImage);

		void DoEvent(Event& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);

	protected:
		SIZE m_szThumb;
		UINT m_uButtonState;
		int m_nStep;
		bool m_bImmMode;

		DrawInfo m_diThumb;
		DrawInfo m_diThumbHot;
		DrawInfo m_diThumbPushed;
	};
}

#endif // __UISLIDER_H__
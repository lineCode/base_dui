#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace dui
{
	class DUILIB_API Button : public Label
	{
	public:
		Button();
		virtual ~Button();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		bool Activate();
		void SetEnabled(bool bEnable = true);
		void DoEvent(Event& event);

		void AttachClick(const EventCallback& callback)
		{
			event_map[UIEVENT_CLICK] += callback;
		}

		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetPushedImage();
		void SetPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetHotForeImage();
		void SetHotForeImage(LPCTSTR pStrImage);

		// ��Ӧ��ť��5��״̬ͼ
		void SetFiveStatusImage(LPCTSTR pStrImage);
		void SetFadeAlphaDelta(BYTE uDelta);
		BYTE GetFadeAlphaDelta();

		void SetHotBkColor(DWORD dwColor);
		DWORD GetHotBkColor() const;
		void SetHotTextColor(DWORD dwColor);
		DWORD GetHotTextColor() const;
		void SetPushedTextColor(DWORD dwColor);
		DWORD GetPushedTextColor() const;
		void SetFocusedTextColor(DWORD dwColor);
		DWORD GetFocusedTextColor() const;
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

	protected:
		enum
		{ 
			FADE_TIMERID = 11,
			FADE_ELLAPSE = 30,
		};
		UINT m_uButtonState;
		DWORD m_dwHotBkColor;
		DWORD m_dwHotTextColor;
		DWORD m_dwPushedTextColor;
		DWORD m_dwFocusedTextColor;
		BYTE m_uFadeAlpha;
		BYTE m_uFadeAlphaDelta;

		DrawInfo m_diNormal;
		DrawInfo m_diHot;
		DrawInfo m_diHotFore;
		DrawInfo m_diPushed;
		DrawInfo m_diPushedFore;
		DrawInfo m_diFocused;
		DrawInfo m_diDisabled;
	};

}	// namespace dui

#endif // __UIBUTTON_H__
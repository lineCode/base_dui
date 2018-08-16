#ifndef __UI_BUTTON_CONTAINER_H__
#define __UI_BUTTON_CONTAINER_H__

#pragma once

namespace dui {

	class DUILIB_API ButtonContainer : public LabelContainer
	{
	public:
		ButtonContainer();
		virtual ~ButtonContainer();

		virtual LPCTSTR GetClass() const override;
		virtual LPVOID GetInterface(LPCTSTR pstrName) override;

		UINT GetControlFlags() const;

		bool Activate();
		void SetEnabled(bool bEnable = true);
		void DoEvent(TEvent& event);
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

		// 对应按钮的5个状态图
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

		TDrawInfo m_diNormal;
		TDrawInfo m_diHot;
		TDrawInfo m_diHotFore;
		TDrawInfo m_diPushed;
		TDrawInfo m_diPushedFore;
		TDrawInfo m_diFocused;
		TDrawInfo m_diDisabled;
	};

}

#endif
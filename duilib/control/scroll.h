#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

#pragma once

namespace dui
{
	class ScrollBox;
	class DUILIB_API ScrollBar : public Control
	{
	public:
		ScrollBar();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		ScrollBox* GetOwner() const;
		void SetOwner(ScrollBox* pOwner);

		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);
		void SetFocus();

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		int GetScrollRange() const;
		void SetScrollRange(int nRange);
		int GetScrollPos() const;
		void SetScrollPos(int nPos, bool bTriggerEvent=true);
		int GetLineSize() const;
		void SetLineSize(int nSize);
        int GetScrollUnit() const;
        void SetScrollUnit(int iUnit);

		bool GetShowButton1();
		void SetShowButton1(bool bShow);
		DWORD GetButton1Color() const;
		void SetButton1Color(DWORD dwColor);
		LPCTSTR GetButton1NormalImage();
		void SetButton1NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1HotImage();
		void SetButton1HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1PushedImage();
		void SetButton1PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1DisabledImage();
		void SetButton1DisabledImage(LPCTSTR pStrImage);

		bool GetShowButton2();
		void SetShowButton2(bool bShow);
		DWORD GetButton2Color() const;
		void SetButton2Color(DWORD dwColor);
		LPCTSTR GetButton2NormalImage();
		void SetButton2NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2HotImage();
		void SetButton2HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2PushedImage();
		void SetButton2PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2DisabledImage();
		void SetButton2DisabledImage(LPCTSTR pStrImage);

		DWORD GetThumbColor() const;
		void SetThumbColor(DWORD dwColor);
		LPCTSTR GetThumbNormalImage();
		void SetThumbNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage();
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage();
		void SetThumbPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbDisabledImage();
		void SetThumbDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetRailNormalImage();
		void SetRailNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetRailHotImage();
		void SetRailHotImage(LPCTSTR pStrImage);
		LPCTSTR GetRailPushedImage();
		void SetRailPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetRailDisabledImage();
		void SetRailDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetBkNormalImage();
		void SetBkNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetBkHotImage();
		void SetBkHotImage(LPCTSTR pStrImage);
		LPCTSTR GetBkPushedImage();
		void SetBkPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetBkDisabledImage();
		void SetBkDisabledImage(LPCTSTR pStrImage);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void DoEvent(Event& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);

		void PaintBk(HDC hDC);
		void PaintButton1(HDC hDC);
		void PaintButton2(HDC hDC);
		void PaintThumb(HDC hDC);
		void PaintRail(HDC hDC);

	protected:

		enum
		{ 
			DEFAULT_SCROLLBAR_SIZE = 16,
			DEFAULT_TIMERID = 10,
		};

		bool m_bHorizontal;
		int m_nRange;
		int m_nScrollPos;
		int m_nLineSize;
        int m_nScrollUnit;
		ScrollBox* m_pOwner;
		POINT ptLastMouse;
		int m_nLastScrollPos;
		int m_nLastScrollOffset;
		int m_nScrollRepeatDelay;

		DrawInfo m_diBkNormal;
		DrawInfo m_diBkHot;
		DrawInfo m_diBkPushed;
		DrawInfo m_diBkDisabled;

		bool m_bShowButton1;
		RECT m_rcButton1;
		UINT m_uButton1State;
		DWORD m_dwButton1Color;
		DrawInfo m_diButton1Normal;
		DrawInfo m_diButton1Hot;
		DrawInfo m_diButton1Pushed;
		DrawInfo m_diButton1Disabled;

		bool m_bShowButton2;
		RECT m_rcButton2;
		UINT m_uButton2State;
		DWORD m_dwButton2Color;
		DrawInfo m_diButton2Normal;
		DrawInfo m_diButton2Hot;
		DrawInfo m_diButton2Pushed;
		DrawInfo m_diButton2Disabled;

		RECT m_rcThumb;
		UINT m_uThumbState;
		DWORD m_dwThumbColor;
		DrawInfo m_diThumbNormal;
		DrawInfo m_diThumbHot;
		DrawInfo m_diThumbPushed;
		DrawInfo m_diThumbDisabled;

		DrawInfo m_diRailNormal;
		DrawInfo m_diRailHot;
		DrawInfo m_diRailPushed;
		DrawInfo m_diRailDisabled;
	};
}

#endif // __UISCROLLBAR_H__
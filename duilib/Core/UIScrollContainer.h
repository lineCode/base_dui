#ifndef __UISCROLL_CONTAINER_H__
#define __UISCROLL_CONTAINER_H__
#pragma once

#include "UIContainer.h"

namespace dui
{
	class ScrollBar;

	class DUILIB_API ScrollContainer : public Container
	{
	public:
		ScrollContainer();
		virtual ~ScrollContainer();

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void SetMouseEnabled(bool bEnable = true) override;

		void SetManager(CPaintManager* pManager, Control* pParent, bool bInit = true);
		Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

		virtual void DoEvent(TEvent& event) override;
		bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);

		virtual int FindSelectable(int iIndex, bool bForward = true) const;
		RECT GetClientPos() const;
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);

		virtual SIZE GetScrollPos() const;
		virtual SIZE GetScrollRange() const;
		virtual void SetScrollPos(SIZE szPos);
		virtual void LineUp();
		virtual void LineDown();
		virtual void PageUp();
		virtual void PageDown();
		virtual void HomeUp();
		virtual void EndDown();
		virtual void LineLeft();
		virtual void LineRight();
		virtual void PageLeft();
		virtual void PageRight();
		virtual void HomeLeft();
		virtual void EndRight();
		virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
		virtual ScrollBar* GetVerticalScrollBar() const;
		virtual ScrollBar* GetHorizontalScrollBar() const;

	protected:
		virtual void SetFloatPos(int iIndex);
		virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

	protected:

		bool m_bScrollProcess; // ∑¿÷πSetPos—≠ª∑µ˜”√

		ScrollBar* m_pVerticalScrollBar;
		ScrollBar* m_pHorizontalScrollBar;

	};
}


#endif
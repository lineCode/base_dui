#ifndef __UISCROLL_CONTAINER_H__
#define __UISCROLL_CONTAINER_H__
#pragma once

namespace dui
{
	class Scroll;

	class DUILIB_API ScrollBox : public Box		//ScrollBox实现VBox的布局方式
	{
	public:
		ScrollBox();
		virtual ~ScrollBox();

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		virtual void DoEvent(Event& event) override;
		virtual void SetMouseEnabled(bool bEnable = true) override;

		RECT GetClientPos() const;
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);
		bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetScrollBarFloat(bool bFloat);

		void SetManager(UIManager* pManager, Control* pParent, bool bInit = true);
		Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

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
		virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

	protected:
		bool m_bScrollProcess;		// 防止SetPos循环调用
		bool m_bScrollBarFloat;		//是否悬浮，true-不占用listitem位置
		ScrollBar* m_pVerticalScrollBar;
		ScrollBar* m_pHorizontalScrollBar;
	};
}


#endif
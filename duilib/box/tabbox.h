#ifndef __UITABLAYOUT_H__
#define __UITABLAYOUT_H__

#pragma once

namespace dui
{
	class DUILIB_API TabBox : public ScrollBox
	{
	public:
		TabBox();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetFadeSwitch(bool bFadeSwitch);
		bool GetFadeSwitch();

		bool Add(Control* pControl);
		bool AddAt(Control* pControl, int iIndex);
		bool Remove(Control* pControl, bool bDoNotDestroy=false);
		void RemoveAll();
		int GetCurSel() const;
		bool SelectItem(int iIndex, bool bTriggerEvent=true);
		bool SelectItem(Control* pControl,  bool bTriggerEvent=true);
		
		void SetPos(RECT rc, bool bNeedInvalidate = true);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		virtual void DoEvent(Event& event) override;

		virtual bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl) override;

	protected:
		int		m_iCurSel;

		BYTE	m_uFadeSwitchPos;
		BYTE	m_uFadeSwitchDelta;
		char	m_cFadeSwitchLeftId;
		char	m_cFadeSwitchRightId;
		bool	m_bFadeSwitchRightToLeft;		//true l<-r; false l->r

		//Control *pOldFadeSwitchItem_;
	};
}
#endif // __UITABLAYOUT_H__

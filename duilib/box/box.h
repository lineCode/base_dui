#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#pragma once

namespace dui {
/////////////////////////////////////////////////////////////////////////////////////
//
class DUILIB_API Box : public Control
{
public:
	Box();
	virtual ~Box();
public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	//interface IContainer
	virtual Control* GetItemAt(int iIndex) const;
	virtual int GetItemIndex(Control* pControl) const;
	virtual bool SetItemIndex(Control* pControl, int iNewIndex);
	virtual bool SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex);
	virtual int GetCount() const;
	virtual bool Add(Control* pControl);
	virtual bool AddAt(Control* pControl, int iIndex);
	virtual bool Remove(Control* pControl, bool bDoNotDestroy = false);
	virtual bool RemoveAt(int iIndex, bool bDoNotDestroy = false);
	virtual void RemoveAll();

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void SetPos(RECT rc, bool bNeedInvalidate = true);
	virtual void SetVisible(bool bVisible = true) override;
	virtual void SetInternVisible(bool bVisible = true) override;
	virtual RECT GetPadding() const;
	virtual void SetPadding(RECT rcPadding); // �����ڱ߾࣬�൱�����ÿͻ���
	virtual int GetChildMargin() const;
	virtual void SetChildMargin(int iPadding);
	virtual UINT GetChildAlign() const;
	virtual void SetChildAlign(UINT iAlign);
	virtual UINT GetChildVAlign() const;
	virtual void SetChildVAlign(UINT iVAlign);
	virtual bool IsAutoDestroy() const;
	virtual void SetAutoDestroy(bool bAuto);
	virtual bool IsDelayedDestroy() const;
	virtual void SetDelayedDestroy(bool bDelayed);
	virtual bool IsMouseChildEnabled() const;
	virtual void SetMouseChildEnabled(bool bEnable = true);

	virtual void DoEvent(Event& event) override;
	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl) override;

	void SetManager(UIManager* pManager, Control* pParent, bool bInit = true);
	Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	Control* FindSubControl(LPCTSTR pstrSubControlName);

	virtual int FindSelectable(int iIndex, bool bForward = true) const;

	void Move(SIZE szOffset, bool bNeedInvalidate);

	virtual void SetFloatPos(int iIndex);

protected:
	PtrArray m_items;
	//PtrArray m_tmp_fadealpha_items;

	RECT m_rcPadding;
	int m_iChildMargin;
	UINT m_iChildAlign;
	UINT m_iChildVAlign;
	bool m_bAutoDestroy;        //�ж���m_bDelayedDestroy֮��,ָʾremove�����Ƿ�box����ɾ����Control
	bool m_bDelayedDestroy;     //�ж���m_bAutoDestroy֮��,ָʾremove�����Ƿ�����Delete()��Control
	bool m_bMouseChildEnabled;
};

/////////////////////////////////////////////////////////////////////////////////////
//


} // namespace dui

#endif // __UICONTAINER_H__

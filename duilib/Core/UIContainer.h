#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#pragma once

namespace dui {
/////////////////////////////////////////////////////////////////////////////////////
//

class IContainer
{
public:
    virtual Control* GetItemAt(int iIndex) const = 0;
    virtual int GetItemIndex(Control* pControl) const  = 0;
    virtual bool SetItemIndex(Control* pControl, int iNewIndex) = 0;
    virtual bool SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex) = 0;
    virtual int GetCount() const = 0;
    virtual bool Add(Control* pControl) = 0;
    virtual bool AddAt(Control* pControl, int iIndex)  = 0;
    virtual bool Remove(Control* pControl, bool bDoNotDestroy=false) = 0;
    virtual bool RemoveAt(int iIndex, bool bDoNotDestroy=false)  = 0;
    virtual void RemoveAll() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
//
class DUILIB_API Container : public Control, public IContainer
{
public:
	Container();
	virtual ~Container();
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
	virtual void SetPadding(RECT rcPadding); // 设置内边距，相当于设置客户区
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

	virtual void DoEvent(TEvent& event) override;
	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl) override;

	void SetManager(CPaintManager* pManager, Control* pParent, bool bInit = true);
	Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	Control* FindSubControl(LPCTSTR pstrSubControlName);

	virtual int FindSelectable(int iIndex, bool bForward = true) const;

	void Move(SIZE szOffset, bool bNeedInvalidate);

	virtual void SetFloatPos(int iIndex);

protected:
	PtrArray m_items;
	RECT m_rcPadding;
	int m_iChildMargin;
	UINT m_iChildAlign;
	UINT m_iChildVAlign;
	bool m_bAutoDestroy;
	bool m_bDelayedDestroy;
	bool m_bMouseChildEnabled;
};

/////////////////////////////////////////////////////////////////////////////////////
//


} // namespace dui

#endif // __UICONTAINER_H__

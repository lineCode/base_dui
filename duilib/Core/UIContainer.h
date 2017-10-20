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
class ScrollBar;

class DUILIB_API Container : public Control, public IContainer
{
public:
    Container();
    virtual ~Container();

public:
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    Control* GetItemAt(int iIndex) const;
    int GetItemIndex(Control* pControl) const;
    bool SetItemIndex(Control* pControl, int iNewIndex);
    bool SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex);
    int GetCount() const;
    bool Add(Control* pControl);
    bool AddAt(Control* pControl, int iIndex);
    bool Remove(Control* pControl, bool bDoNotDestroy=false);
    bool RemoveAt(int iIndex, bool bDoNotDestroy=false);
    void RemoveAll();

    void DoEvent(TEvent& event);
    void SetVisible(bool bVisible = true);
    void SetInternVisible(bool bVisible = true);
    void SetMouseEnabled(bool bEnable = true);

    virtual RECT GetPadding() const;
    virtual void SetPadding(RECT rcPadding); // 设置内边距，相当于设置客户区
    virtual int GetChildPadding() const;
    virtual void SetChildPadding(int iPadding);
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

    virtual int FindSelectable(int iIndex, bool bForward = true) const;

	RECT GetClientPos() const;
	void SetPos(RECT rc, bool bNeedInvalidate = true);
	void Move(SIZE szOffset, bool bNeedInvalidate = true);
    bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void SetManager(CPaintManager* pManager, Control* pParent, bool bInit = true);
    Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	bool SetSubControlText(LPCTSTR pstrSubControlName,LPCTSTR pstrText);
	bool SetSubControlFixedHeight(LPCTSTR pstrSubControlName,int cy);
	bool SetSubControlFixedWdith(LPCTSTR pstrSubControlName,int cx);
	bool SetSubControlUserData(LPCTSTR pstrSubControlName,LPCTSTR pstrText);

	String GetSubControlText(LPCTSTR pstrSubControlName);
	int GetSubControlFixedHeight(LPCTSTR pstrSubControlName);
	int GetSubControlFixedWdith(LPCTSTR pstrSubControlName);
	const String GetSubControlUserData(LPCTSTR pstrSubControlName);
	Control* FindSubControl(LPCTSTR pstrSubControlName);

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
    PtrArray m_items;
    RECT m_rcPadding;
    int m_iChildPadding;
	UINT m_iChildAlign;
	UINT m_iChildVAlign;
    bool m_bAutoDestroy;
    bool m_bDelayedDestroy;
    bool m_bMouseChildEnabled;
    bool m_bScrollProcess; // 防止SetPos循环调用

    ScrollBar* m_pVerticalScrollBar;
    ScrollBar* m_pHorizontalScrollBar;
};

} // namespace dui

#endif // __UICONTAINER_H__

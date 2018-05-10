#ifndef __UILIST_H__
#define __UILIST_H__
#pragma once

namespace dui {


	//class IListCallback
	//{
	//public:
	//	virtual LPCTSTR GetItemText(Control* pList, int iItem, int iSubItem) = 0;
	//};

	class IList
	{
	public:
		virtual int GetCurSel() const = 0;
		virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true) = 0;
		virtual void DoEvent(TEvent& event) = 0;
	};

	class IListItem2
	{
	public:
		virtual int GetIndex() const = 0;
		virtual void SetIndex(int iIndex) = 0;
		virtual int GetDrawIndex() const = 0;
		virtual void SetDrawIndex(int iIndex) = 0;
		virtual IList* GetOwner() = 0;
		virtual void SetOwner(Control* pOwner) = 0;
		virtual bool IsSelected() const = 0;
		virtual bool Select(bool bSelect = true, bool bTriggerEvent = true) = 0;
		//virtual bool IsExpanded() const = 0;
		//virtual bool Expand(bool bExpand = true) = 0;
		//virtual void DrawItemText(HDC hDC, const RECT& rcItem) = 0;
		//------------------add by djj----------------------
		//virtual void SetText(LPCTSTR text) = 0;
		//virtual LPCTSTR GetText() = 0;
	};

class DUILIB_API List : public ScrollContainer, public IList
{
public:
    List();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool GetScrollSelect();
    void SetScrollSelect(bool bScrollSelect);
    int GetCurSel() const;
	virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true) override;

   /* Control* GetItemAt(int iIndex) const;
    int GetItemIndex(Control* pControl) const;*/
    bool SetItemIndex(Control* pControl, int iIndex);
    bool SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex);
   /* int GetCount() const;*/
    bool Add(Control* pControl);
    bool AddAt(Control* pControl, int iIndex);
    bool Remove(Control* pControl, bool bDoNotDestroy=false);
    bool RemoveAt(int iIndex, bool bDoNotDestroy=false);
    void RemoveAll();

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

   /* int GetChildMargin() const;
    void SetChildMargin(int iPadding);*/

   /* UINT GetItemFixedHeight();
    void SetItemFixedHeight(UINT nHeight);
    int GetItemFont(int index);
    void SetItemFont(int index);
    UINT GetItemTextStyle();
    void SetItemTextStyle(UINT uStyle);
    RECT GetItemTextPadding() const;
    void SetItemTextPadding(RECT rc);
    DWORD GetItemTextColor() const;
    void SetItemTextColor(DWORD dwTextColor);
    DWORD GetItemBkColor() const;
    void SetItemBkColor(DWORD dwBkColor);
    LPCTSTR GetItemBkImage() const;
    void SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
    DWORD GetSelectedItemTextColor() const;
    void SetSelectedItemTextColor(DWORD dwTextColor);
    DWORD GetSelectedItemBkColor() const;
    void SetSelectedItemBkColor(DWORD dwBkColor);
    LPCTSTR GetSelectedItemImage() const;
    void SetSelectedItemImage(LPCTSTR pStrImage);
    DWORD GetHotItemTextColor() const;
    void SetHotItemTextColor(DWORD dwTextColor);
    DWORD GetHotItemBkColor() const;
    void SetHotItemBkColor(DWORD dwBkColor);
    LPCTSTR GetHotItemImage() const;
    void SetHotItemImage(LPCTSTR pStrImage);
    DWORD GetDisabledItemTextColor() const;
    void SetDisabledItemTextColor(DWORD dwTextColor);
    DWORD GetDisabledItemBkColor() const;
    void SetDisabledItemBkColor(DWORD dwBkColor);
    LPCTSTR GetDisabledItemImage() const;
    void SetDisabledItemImage(LPCTSTR pStrImage);
    int GetItemHLineSize() const;
    void SetItemHLineSize(int iSize);
    DWORD GetItemHLineColor() const;
    void SetItemHLineColor(DWORD dwLineColor);
    int GetItemVLineSize() const;
    void SetItemVLineSize(int iSize);
    DWORD GetItemVLineColor() const;
    void SetItemVLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);

    void SetMultiExpanding(bool bMultiExpandable); 
    int GetExpandedItem() const;
    bool ExpandItem(int iIndex, bool bExpand = true);*/

	void SetPos(RECT rc, bool bNeedInvalidate = true);
	//void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoEvent(TEvent& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

  /*  IListCallback* GetTextCallback() const;
    void SetTextCallback(IListCallback* pCallback);*/

   /* SIZE GetScrollPos() const;
    SIZE GetScrollRange() const;
    void SetScrollPos(SIZE szPos);
    void LineUp();
    void LineDown();
    void PageUp();
    void PageDown();
    void HomeUp();
    void EndDown();
    void LineLeft();
    void LineRight();
    void PageLeft();
    void PageRight();
    void HomeLeft();
    void EndRight();
    void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
    virtual ScrollBar* GetVerticalScrollBar() const;
    virtual ScrollBar* GetHorizontalScrollBar() const;*/
    bool SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

protected:
    bool m_bScrollSelect;
    int m_iCurSel;
    //int m_iExpandedItem;
   /* IListCallback* m_pCallback;*/
};

class DUILIB_API ListElement : public Label, public IListItem2
{
public:
	ListElement();
	virtual ~ListElement(){ printf("~ListElement()\n"); };

	LPCTSTR GetClass() const;
	UINT GetControlFlags() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetEnabled(bool bEnable = true);

	int GetIndex() const;
	void SetIndex(int iIndex);
	int GetDrawIndex() const;
	void SetDrawIndex(int iIndex);

	virtual IList* GetOwner() override;
	virtual void SetOwner(Control* pOwner) override;
	void SetVisible(bool bVisible = true);

	bool IsSelected() const;
	bool Select(bool bSelect = true, bool bTriggerEvent=true);
	bool IsExpanded() const;
	bool Expand(bool bExpand = true);

	void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
	bool Activate();

	void DoEvent(TEvent& event);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void DrawItemBk(HDC hDC, const RECT& rcItem);

	virtual void DrawItemText(HDC hDC, const RECT& rcItem){};

	//virtual void SetText(LPCTSTR text);
	//virtual LPCTSTR GetText();
protected:
	int m_iIndex;
	int m_iDrawIndex;
	bool m_bSelected;
	UINT m_uButtonState;
	IList* m_pOwner;
};
#if 0
class DUILIB_API ListContainerElement : public Container, public IListItem
{
public:
    ListContainerElement();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    int GetIndex() const;
    void SetIndex(int iIndex);
    int GetDrawIndex() const;
    void SetDrawIndex(int iIndex);

    IList* GetOwner();
    void SetOwner(Control* pOwner);
    void SetVisible(bool bVisible = true);
    void SetEnabled(bool bEnable = true);

    bool IsSelected() const;
    bool Select(bool bSelect = true, bool bTriggerEvent=true);
    bool IsExpandable() const;
    void SetExpandable(bool bExpandable);
    bool IsExpanded() const;
    bool Expand(bool bExpand = true);

    void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
    bool Activate();

    void DoEvent(TEvent& event);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);

    void DrawItemText(HDC hDC, const RECT& rcItem);    
    void DrawItemBk(HDC hDC, const RECT& rcItem);

    //SIZE EstimateSize(SIZE szAvailable);

	void AttachItemClick(const EventCallback& callback)
	{
		OnEvent[UIEVENT_ITEMCLICK] += callback;
	}

protected:
    int m_iIndex;
    int m_iDrawIndex;
    bool m_bSelected;
    bool m_bExpandable;
    bool m_bExpand;
    UINT m_uButtonState;
    IList* m_pOwner;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API ListHBoxElement : public ListContainerElement
{
public:
    ListHBoxElement();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetPos(RECT rc, bool bNeedInvalidate = true);
    bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);
};
#endif
} // namespace dui

#endif // __UILIST_H__

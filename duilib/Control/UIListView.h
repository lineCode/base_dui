#ifndef __UILISTVIEW_H__
#define __UILISTVIEW_H__

#pragma once
#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"

namespace dui {
/////////////////////////////////////////////////////////////////////////////////////
//

typedef int (CALLBACK *PULVCompareFunc)(UINT_PTR, UINT_PTR, UINT_PTR);

#define UILIST_MAX_COLUMNS 64

typedef struct tagListViewInfo
{
    int nColumns;
    RECT rcColumn[UILIST_MAX_COLUMNS];
    UINT uFixedHeight; 
    int nFont;
    UINT uTextStyle;
    RECT rcTextPadding;
    DWORD dwTextColor;
    DWORD dwBkColor;
    TDrawInfo diBk;
    bool bAlternateBk;
    DWORD dwSelectedTextColor;
    DWORD dwSelectedBkColor;
    TDrawInfo diSelected;
    DWORD dwHotTextColor;
    DWORD dwHotBkColor;
    TDrawInfo diHot;
    DWORD dwDisabledTextColor;
    DWORD dwDisabledBkColor;
    TDrawInfo diDisabled;
    int iHLineSize;
    DWORD dwHLineColor;
    int iVLineSize;
    DWORD dwVLineColor;
    bool bShowHtml;
    bool bMultiExpandable;
} ListViewInfo;


/////////////////////////////////////////////////////////////////////////////////////
//

class IListViewCallback
{
public:
    virtual LPCTSTR GetItemText(Control* pList, int iItem, int iSubItem) = 0;
};

class ListHeader;
class IListView
{
public:
	virtual ListViewInfo* GetListInfo() = 0;
	virtual int GetCurSel() const = 0;
	virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true) = 0;
	virtual void DoEvent(TEvent& event) = 0;
	virtual bool ExpandItem(int iIndex, bool bExpand = true) = 0;
	virtual int GetExpandedItem() const = 0;
    virtual ListHeader* GetHeader() const = 0;
    virtual ScrollContainer* GetList() const = 0;
    virtual IListViewCallback* GetTextCallback() const = 0;
    virtual void SetTextCallback(IListViewCallback* pCallback) = 0;
};

class IListItem
{
public:
    virtual int GetIndex() const = 0;
    virtual void SetIndex(int iIndex) = 0;
    virtual int GetDrawIndex() const = 0;
    virtual void SetDrawIndex(int iIndex) = 0;
	virtual IListView* GetOwner() = 0;
    virtual void SetOwner(Control* pOwner) = 0;
    virtual bool IsSelected() const = 0;
    virtual bool Select(bool bSelect = true, bool bTriggerEvent=true) = 0;
    virtual bool IsExpanded() const = 0;
    virtual bool Expand(bool bExpand = true) = 0;
    virtual void DrawItemText(HDC hDC, const RECT& rcItem) = 0;
	//------------------add by djj----------------------
	//virtual void SetText(LPCTSTR text) = 0;
	//virtual LPCTSTR GetText() = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class ListBody;
class ListHeader;

class DUILIB_API ListView : public ScrollContainer, public IListView
{
public:
    ListView();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool GetScrollSelect();
    void SetScrollSelect(bool bScrollSelect);
    int GetCurSel() const;
    bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent=true);

    Control* GetItemAt(int iIndex) const;
    int GetItemIndex(Control* pControl) const;
    bool SetItemIndex(Control* pControl, int iIndex);
    bool SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex);
    int GetCount() const;
    bool Add(Control* pControl);
    bool AddAt(Control* pControl, int iIndex);
    bool Remove(Control* pControl, bool bDoNotDestroy=false);
    bool RemoveAt(int iIndex, bool bDoNotDestroy=false);
    void RemoveAll();

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

    int GetChildMargin() const;
    void SetChildMargin(int iPadding);

    ListHeader* GetHeader() const;  
    ScrollContainer* GetList() const;
    ListViewInfo* GetListInfo();

    UINT GetItemFixedHeight();
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
    bool ExpandItem(int iIndex, bool bExpand = true);

	void SetPos(RECT rc, bool bNeedInvalidate = true);
	void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoEvent(TEvent& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    IListViewCallback* GetTextCallback() const;
    void SetTextCallback(IListViewCallback* pCallback);

    SIZE GetScrollPos() const;
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
    virtual ScrollBar* GetHorizontalScrollBar() const;
    bool SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

protected:
    bool m_bScrollSelect;
    int m_iCurSel;
    int m_iExpandedItem;
    IListViewCallback* m_pCallback;
    ListBody* m_pList;
    ListHeader* m_pHeader;
    ListViewInfo m_ListInfo;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API ListHeader : public HorizontalLayout
{
public:
    ListHeader();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    SIZE EstimateSize(SIZE szAvailable);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API ListHeaderItem : public Control
{
public:
    ListHeaderItem();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    void SetEnabled(bool bEnable = true);

	bool IsDragable() const;
    void SetDragable(bool bDragable);
	DWORD GetSepWidth() const;
    void SetSepWidth(int iWidth);
	DWORD GetTextStyle() const;
    void SetTextStyle(UINT uStyle);
	DWORD GetTextColor() const;
    void SetTextColor(DWORD dwTextColor);
    DWORD GetSepColor() const;
    void SetSepColor(DWORD dwSepColor);
	void SetTextPadding(RECT rc);
	RECT GetTextPadding() const;
    void SetFont(int index);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetSepImage() const;
    void SetSepImage(LPCTSTR pStrImage);

    void DoEvent(TEvent& event);
    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    RECT GetThumbRect() const;

    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

protected:
    POINT ptLastMouse;
    bool m_bDragable;
    UINT m_uButtonState;
    int m_iSepWidth;
    DWORD m_dwTextColor;
    DWORD m_dwSepColor;
    int m_iFont;
    UINT m_uTextStyle;
    bool m_bShowHtml;
	RECT m_rcTextPadding;
    TDrawInfo m_diNormal;
    TDrawInfo m_diHot;
    TDrawInfo m_diPushed;
    TDrawInfo m_diFocused;
    TDrawInfo m_diSep;
};

#if 1

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

	IListView* GetOwner();
    void SetOwner(Control* pOwner);
    void SetVisible(bool bVisible = true);
    void SetEnabled(bool bEnable = true);

    bool IsSelected() const;
    bool Select(bool bSelect = true, bool bTriggerEvent=true);
#if 1
    bool IsExpandable() const;
    void SetExpandable(bool bExpandable);
    bool IsExpanded() const;
    bool Expand(bool bExpand = true);
#endif
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
	IListView* m_pOwner;
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

#endif // __UILISTVIEW_H__

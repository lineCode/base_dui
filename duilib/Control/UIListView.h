#ifndef __UILISTVIEW_H__
#define __UILISTVIEW_H__

#pragma once
#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"

namespace dui {
/////////////////////////////////////////////////////////////////////////////////////
//

#define UILIST_MAX_COLUMNS 64

typedef struct tagListViewInfo
{
    int nColumns;
    RECT rcColumn[UILIST_MAX_COLUMNS];
    UINT uFixedHeight; 
    int nFont;
    UINT uTextStyle;
    RECT rcTextPadding;

	bool bAlternateBk;
    DWORD dwTextColor;
	DWORD dwSelectedTextColor;
	DWORD dwHotTextColor;
	DWORD dwDisabledTextColor;

    DWORD dwBkColor;
	DWORD dwSelectedBkColor;
	DWORD dwHotBkColor;
    DWORD dwDisabledBkColor;
	
    int iHLineSize;
    int iVLineSize;
	DWORD dwHLineColor;
    DWORD dwVLineColor;

	TDrawInfo diBk;
	TDrawInfo diSelected;
	TDrawInfo diHot;
	TDrawInfo diDisabled;
} ListViewInfo;


/////////////////////////////////////////////////////////////////////////////////////
//

class IListViewCallback
{
public:
    virtual LPCTSTR GetItemText(Control* pList, int iItem, int iSubItem) = 0;
};

class ListHeader;
class IListView : public IList
{
public:
	virtual ListViewInfo* GetListInfo() = 0;
	virtual ListHeader* GetHeader() const = 0;
    virtual ScrollContainer* GetList() const = 0;
    virtual IListViewCallback* GetTextCallback() const = 0;
    virtual void SetTextCallback(IListViewCallback* pCallback) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
//
class ListHeader : public HorizontalLayout
{
public:
	ListHeader(){};
	virtual ~ListHeader(){
		printf("~ListHeader\n");
	};
	virtual LPCTSTR GetClass() const{ return DUI_CTR_LISTHEADER; };
	virtual LPVOID GetInterface(LPCTSTR pstrName){
		if (_tcscmp(pstrName, DUI_CTR_LISTHEADER) == 0) return this;
		return HorizontalLayout::GetInterface(pstrName);
	};

	virtual SIZE EstimateSize(SIZE szAvailable);
};
/////////////////////////////////////////////////////////////////////////////////////
//

class ListBody;
class DUILIB_API ListView : public ScrollContainer, public IListView
{

public:
    ListView();
	virtual ~ListView(){
		printf("~ListView\n");
	};
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
 /*   bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);

    void SetMultiExpanding(bool bMultiExpandable); */

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
    //int m_iExpandedItem;
    IListViewCallback* m_pCallback;
    ListBody* m_pList;
    ListHeader* m_pHeader;
    ListViewInfo m_ListInfo;
};


} // namespace dui

#endif // __UILISTVIEW_H__

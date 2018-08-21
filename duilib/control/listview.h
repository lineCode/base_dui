#ifndef __UILISTVIEW_H__
#define __UILISTVIEW_H__

#pragma once

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

	DrawInfo diBk;
	DrawInfo diSelected;
	DrawInfo diHot;
	DrawInfo diDisabled;
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
    virtual ScrollBox* GetList() const = 0;
    virtual IListViewCallback* GetTextCallback() const = 0;
    virtual void SetTextCallback(IListViewCallback* pCallback) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
//
class ListHeader : public HBox
{
public:
	ListHeader(){};
	virtual ~ListHeader(){
		printf("~ListHeader\n");
	};
	virtual LPCTSTR GetClass() const{ return DUI_CTR_LISTHEADER; };
	virtual LPVOID GetInterface(LPCTSTR pstrName){
		if (_tcscmp(pstrName, DUI_CTR_LISTHEADER) == 0) return this;
		return HBox::GetInterface(pstrName);
	};

	virtual SIZE EstimateSize(SIZE szAvailable);
};

/////////////////////////////////////////////////////////////////////////////////////
//
class ListBody : public ScrollBox
{
public:
	ListBody(IListView* pOwner);
	virtual ~ListBody(){
		printf("~ListBody\n");
	};
	virtual bool Add(Control* pControl);
	void SetScrollPos(SIZE szPos);
	void SetPos(RECT rc, bool bNeedInvalidate = true);
	void DoEvent(Event& event);
	bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);
	bool SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData, int& iCurSel);

	//----SingleLine add by djj------------
	class SingleLine : public HBox
	{
	public:
		SingleLine(IListView *pOwner, unsigned index) : m_pOwner(pOwner), m_index(index){};
		virtual ~SingleLine(){
			printf("~SingleLine\n");
		};
		//行为由ListBody全权负责
		/*virtual bool Add(Control* pControl){
		if (GetCount() >= m_column)
		return false;
		return __super::Add(pControl);
		};*/
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);

		virtual SIZE EstimateSize(SIZE szAvailable);
	private:
		IListView	*m_pOwner;
		unsigned	m_index;
	};

protected:
	static int __cdecl ItemComareFunc(void *pvlocale, const void *item1, const void *item2);
	int __cdecl ItemComareFunc(const void *item1, const void *item2);

protected:
	IListView* m_pOwner;
	PULVCompareFunc m_pCompareFunc;
	UINT_PTR m_compareData;
};

/////////////////////////////////////////////////////////////////////////////////////
//
class DUILIB_API ListView : public ScrollBox, public IListView
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

	int GetChildMargin() const{ return m_pList->GetChildMargin(); };
	void SetChildMargin(int iPadding){ m_pList->SetChildMargin(iPadding); };

    ListHeader* GetHeader() const;  
    ScrollBox* GetList() const;
	ListViewInfo* GetListInfo(){ return &m_ListInfo; };

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

	void SetPos(RECT rc, bool bNeedInvalidate = true);
	void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoEvent(Event& event);
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
    virtual ScrollBar* GetVerticalScrollBar() const override;
	virtual ScrollBar* GetHorizontalScrollBar() const override;
    bool SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

protected:
    bool m_bScrollSelect;
    int m_iCurSel;
    IListViewCallback* m_pCallback;
    ListBody* m_pList;
    ListHeader* m_pHeader;
    ListViewInfo m_ListInfo;
};


} // namespace dui

#endif // __UILISTVIEW_H__

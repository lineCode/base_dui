#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace dui {
/////////////////////////////////////////////////////////////////////////////////////
//
#define ComboBody_EQUAL_LIST	1

#ifdef ComboBody_EQUAL_LIST
	typedef List ComboBody;
#endif

class CComboWnd;

class DUILIB_API Combo : public Container
{
    friend class CComboWnd;
public:
    Combo();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoInit();
    UINT GetControlFlags() const;

  /*  String GetText() const;*/
    void SetEnabled(bool bEnable = true);

    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);

    int GetCurSel() const;
	bool GetSelectCloseFlag();
	void SetSelectCloseFlag(bool flag);
    bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent=true);
    bool ExpandItem(int iIndex, bool bExpand = true);
    int GetExpandedItem() const;

    bool SetItemIndex(Control* pControl, int iNewIndex);
    bool SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex);
    bool Add(Control* pControl);
    bool AddAt(Control* pControl, int iIndex);
    bool Remove(Control* pControl, bool bDoNotDestroy=false);
    bool RemoveAt(int iIndex, bool bDoNotDestroy=false);
    void RemoveAll();

    bool Activate();

	String GetDropBoxFile() const{ return m_dropBoxFile; };
	void SetDropBoxFile(String xmlfile){ m_dropBoxFile = xmlfile; };
	String GetDropBoxFolder() const{ return m_dropBoxFolder; };
	void SetDropBoxFolder(String folder){ m_dropBoxFolder = folder; };

	bool GetShowText() const;
	void SetShowText(bool flag);
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage() const;
    void SetDisabledImage(LPCTSTR pStrImage);

    SIZE EstimateSize(SIZE szAvailable);
	void SetPos(RECT rc, bool bNeedInvalidate = true);
	void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoEvent(TEvent& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void AttachSelect(const EventCallback& callback)
	{
		OnEvent[UIEVENT_ITEMSELECT] += callback;
	}
    
    bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);
    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

protected:
    CComboWnd* m_pWindow;

	String m_text;
    int m_iCurSel;
	bool m_bShowText;
	bool m_bSelectCloseFlag;
    RECT m_rcTextPadding;
    String m_sDropBoxAttributes;
    SIZE m_szDropBox;
	String m_dropBoxFile;
	String m_dropBoxFolder;
    UINT m_uButtonState;

	TDrawInfo m_diNormal;
    TDrawInfo m_diHot;
    TDrawInfo m_diPushed;
    TDrawInfo m_diFocused;
    TDrawInfo m_diDisabled;

   // ListViewInfo m_ListInfo;
};

} // namespace dui

#endif // __UICOMBO_H__

#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace dui {
/////////////////////////////////////////////////////////////////////////////////////
//

typedef List ComboBody;

class ComboWnd;

class DUILIB_API Combo : public Box
{
    friend class ComboWnd;
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
    void DoEvent(Event& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void AttachSelect(const EventCallback& callback)
	{
		event_map[UIEVENT_ITEMSELECT] += callback;
	}
    
    bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);
    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

protected:
    ComboWnd* m_pWindow;

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

	DrawInfo m_diNormal;
    DrawInfo m_diHot;
    DrawInfo m_diPushed;
    DrawInfo m_diFocused;
    DrawInfo m_diDisabled;

   // ListViewInfo m_ListInfo;
};

} // namespace dui

#endif // __UICOMBO_H__

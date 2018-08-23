#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once

namespace dui {

/////////////////////////////////////////////////////////////////////////////////////
//
#define POS_STYLE_TOP						0x00000000
#define POS_STYLE_LEFT                     0x00000000
#define POS_STYLE_CENTER                   0x00000001
#define POS_STYLE_RIGHT                    0x00000002
#define POS_STYLE_VCENTER                  0x00000004
#define POS_STYLE_BOTTOM                   0x00000008

typedef Control* (CALLBACK* FINDCONTROLPROC)(Control*, LPVOID);

class DUILIB_API Control
{
public:
    Control();
    virtual void Delete();

protected:
    virtual ~Control();

public:
    virtual String GetName() const;
    virtual void SetName(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual UINT GetControlFlags() const;
	virtual HWND GetNativeWindow() const;

    virtual bool Activate();
    virtual UIManager* GetManager() const;
    virtual void SetManager(UIManager* pManager, Control* pParent, bool bInit = true);
    virtual Control* GetParent() const;
    virtual Control* GetCover() const;
    virtual void SetCover(Control *pControl);

    // �ı����
    virtual String GetText() const;
    virtual void SetText(LPCTSTR pstrText);

    // ͼ�����
    DWORD GetBkColor() const;
    void SetBkColor(DWORD dwBackColor);
    LPCTSTR GetBkImage();
    void SetBkImage(LPCTSTR pStrImage);
	DWORD GetFocusBorderColor() const;
	void SetFocusBorderColor(DWORD dwBorderColor);
    bool IsColorHSL() const;
    void SetColorHSL(bool bColorHSL);
    SIZE GetBorderRound() const;
    void SetBorderRound(SIZE cxyRound);
    bool DrawImage(HDC hDC, DrawInfo& drawInfo);

	//�߿����
	DWORD GetBorderColor() const;
	void SetBorderColor(DWORD dwBorderColor);
	RECT GetBorderSize() const;
	void SetBorderSize(RECT rc);
	void SetBorderSize(int iSize);
	int GetBorderStyle() const;
	void SetBorderStyle(int nStyle);

    // λ�����
    virtual const RECT& GetPos() const;
	virtual RECT GetRelativePos() const; // ���(���ؼ�)λ��
	virtual RECT GetClientPos() const; // �ͻ����򣨳�ȥscrollbar��padding��
	// ֻ�пؼ�Ϊfloat��ʱ���ⲿ����SetPos��Move������Ч�ģ�λ�ò�������Ը��ؼ���λ��
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
	virtual void Move(SIZE szOffset, bool bNeedInvalidate = true);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetX() const;
    virtual int GetY() const;
    virtual RECT GetMargin() const;
    virtual void SetMargin(RECT rcMargin); // ������߾࣬���ϲ㴰�ڻ���
    virtual SIZE GetFixedXY() const;         // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
    virtual void SetFixedXY(SIZE szXY);      // ��floatΪtrueʱ��Ч
	virtual PercentInfo GetFloatPercent() const;
	virtual void SetFloatPercent(PercentInfo piFloatPercent);
    virtual int GetFixedWidth() const;       // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
    virtual void SetFixedWidth(int cx);      // Ԥ��Ĳο�ֵ
    virtual int GetFixedHeight() const;      // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
    virtual void SetFixedHeight(int cy);     // Ԥ��Ĳο�ֵ
    virtual int GetMinWidth() const;
    virtual void SetMinWidth(int cx);
    virtual int GetMaxWidth() const;
    virtual void SetMaxWidth(int cx);
    virtual int GetMinHeight() const;
    virtual void SetMinHeight(int cy);
    virtual int GetMaxHeight() const;
    virtual void SetMaxHeight(int cy);
	virtual LPCTSTR GetForeImage() const;
	void SetForeImage(LPCTSTR pStrImage);

    // �����ʾ
    virtual String GetToolTip() const;
    virtual void SetToolTip(LPCTSTR pstrText);
	virtual void SetToolTipWidth(int nWidth);
	virtual int	  GetToolTipWidth(void);	// ����ToolTip��������

    // ��ݼ�
    virtual TCHAR GetShortcut() const;
    virtual void SetShortcut(TCHAR ch);

    // �˵�
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // �û�����
    virtual const String& GetUserData(); // �������������û�ʹ��
    virtual void SetUserData(LPCTSTR pstrText); // �������������û�ʹ��
    virtual UINT_PTR GetTag() const; // �������������û�ʹ��
    virtual void SetTag(UINT_PTR pTag); // �������������û�ʹ��

    // һЩ��Ҫ������
    virtual bool IsVisible() const;
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true); // �����ڲ����ã���ЩUIӵ�д��ھ������Ҫ��д�˺���
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    virtual bool IsFocused() const;
    virtual void SetFocus();
    virtual bool IsFloat() const;
    virtual void SetFloat(bool bFloat = true);
	// add by djj[20180108]
	void SetPosStyle(UINT style);
	UINT GetPosStyle() const;
	// fade ϵ�� add by djj[20180105]
	virtual void SetFadeAlpha(bool bFade = true);
	virtual bool GetFadeAlpha();

	// �Զ���(δ�����)����
	void AddCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr);
	LPCTSTR GetCustomAttribute(LPCTSTR pstrName) const;
	bool RemoveCustomAttribute(LPCTSTR pstrName);
	void RemoveAllCustomAttribute();

    virtual Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    void Invalidate();
    bool IsUpdateNeeded() const;
    void NeedUpdate();
    void NeedParentUpdate();
    DWORD GetAdjustColor(DWORD dwColor);

    virtual void Init();
    //virtual void DoInit();

    virtual void HandleEvent(Event& event);
    virtual void DoEvent(Event& event);

    virtual String GetAttribute(LPCTSTR pstrName);
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual String GetAttributeList(bool bIgnoreDefault = true);
    virtual void SetAttributeList(LPCTSTR pstrList);

    virtual SIZE EstimateSize(SIZE szAvailable);

	virtual bool Paint(HDC hDC, const RECT& rcPaint, Control* pStopControl=NULL); // ����Ҫ��Ҫ��������
    virtual bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

    virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

	void AttachMouseEnter(const EventCallback& callback)
	{
		m_event_map[UIEVENT_MOUSEENTER] += callback;
	}

	void AttachMouseLeave(const EventCallback& callback)
	{
		m_event_map[UIEVENT_MOUSELEAVE] += callback;
	}

	void AttachMouseHover(const EventCallback& callback)
	{
		m_event_map[UIEVENT_MOUSEHOVER] += callback;
	}

	void AttachButtonDown(const EventCallback& callback)
	{
		m_event_map[UIEVENT_BUTTONDOWN] += callback;
	}

	void AttachButtonUp(const EventCallback& callback)
	{
		m_event_map[UIEVENT_BUTTONUP] += callback;
	}

	void AttachSetFocus(const EventCallback& callback)
	{
		m_event_map[UIEVENT_SETFOCUS] += callback;
	}

	void AttachKillFocus(const EventCallback& callback)
	{
		m_event_map[UIEVENT_KILLFOCUS] += callback;
	}

	void AttachMenu(const EventCallback& callback)
	{
		m_event_map[UIEVENT_CONTEXTMENU] += callback;
	}

	void AttachResize(const EventCallback& callback)
	{
		m_event_map[UIEVENT_RESIZE] += callback;
	}
public:
	EventSource m_cbInit;
	EventSource m_cbDestroy;
	EventSource m_cbNotify;
	EventSource m_cbPaint;
	EventSource m_cbPostPaint;
	EventMap	m_event_map;
protected:
    UIManager* m_pManager;
    Control* m_pParent;
    Control* m_pCover;
    String m_sName;
    bool m_bUpdateNeeded;
    bool m_bMenuUsed;
	bool m_bAsyncNotify;
    RECT m_rcItem;
    RECT m_rcMargin;
    SIZE m_Size;
    SIZE m_FixedSize;	//
    SIZE m_MinSize;
    SIZE m_MaxSize;
    bool m_bVisible;
    bool m_bInternVisible;
    bool m_bEnabled;
    bool m_bMouseEnabled;
	bool m_bKeyboardEnabled ;
    bool m_bFocused;
    bool m_bFloat;
	// fade ϵ�� add by djj[20180105]
	bool m_bFadeAlpha = false;

	UINT m_uPosStyle = POS_STYLE_TOP | POS_STYLE_LEFT;

	PercentInfo m_piFloatPercent;
    bool m_bSetPos; // ��ֹSetPosѭ������

    String m_sText;
    String m_sToolTip;
    TCHAR m_chShortcut;
    String m_sUserData;
    UINT_PTR m_pTag;

    DWORD m_dwBackColor;
	DrawInfo m_diBk;
	DrawInfo m_diFore;
    DWORD m_dwBorderColor;
	DWORD m_dwFocusBorderColor;
    bool m_bColorHSL;
	int m_nBorderStyle;
	int m_nTooltipWidth;
    SIZE m_BorderRoundSize;
    RECT m_rcPaint;
	RECT m_rcBorderSize;
	StringPtrMap m_CustomAttrHash;
};

} // namespace dui

#endif // __UICONTROL_H__

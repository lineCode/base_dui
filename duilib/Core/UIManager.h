#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#pragma once

namespace dui {
/////////////////////////////////////////////////////////////////////////////////////
//

class Control;
class Container;
class IDialogBuilderCallback;
/////////////////////////////////////////////////////////////////////////////////////
//

typedef enum EVENTTYPE_UI
{
    UIEVENT__FIRST = 1,
    UIEVENT__KEYBEGIN,			//2
    UIEVENT_KEYDOWN,
    UIEVENT_KEYUP,
    UIEVENT_CHAR,
    UIEVENT_SYSKEY,
    UIEVENT__KEYEND,			//7
    UIEVENT__MOUSEBEGIN,		//8
    UIEVENT_MOUSEMOVE,
    UIEVENT_MOUSELEAVE,
    UIEVENT_MOUSEENTER,
    UIEVENT_MOUSEHOVER,
    UIEVENT_BUTTONDOWN,
    UIEVENT_BUTTONUP,
    UIEVENT_RBUTTONDOWN,
    UIEVENT_DBLCLICK,
    UIEVENT_CONTEXTMENU,
    UIEVENT_SCROLLWHEEL,
    UIEVENT__MOUSEEND,			//19
    UIEVENT_KILLFOCUS,
    UIEVENT_SETFOCUS,
    UIEVENT_WINDOWSIZE,
    UIEVENT_SETCURSOR,
    UIEVENT_TIMER,
    UIEVENT_NOTIFY,
    UIEVENT_COMMAND,
    UIEVENT__LAST,
};

/////////////////////////////////////////////////////////////////////////////////////
//

// Flags for Control::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_UPDATETEST    0x00000008
#define UIFIND_TOP_FIRST     0x00000010
#define UIFIND_ME_FIRST      0x80000000

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040



/////////////////////////////////////////////////////////////////////////////////////
//

typedef struct DUILIB_API tagTFontInfo
{
    HFONT hFont;
    String sFontName;
    int iSize;
    bool bBold;
    bool bUnderline;
    bool bItalic;
    TEXTMETRIC tm;
} TFontInfo;

typedef struct DUILIB_API tagTImageInfo
{
    HBITMAP hBitmap;
    LPBYTE pBits;
	LPBYTE pSrcBits;
    int nX;
    int nY;
    bool bAlpha;
    bool bUseHSL;
    String sResType;
    DWORD dwMask;
} TImageInfo;

typedef struct DUILIB_API tagTDrawInfo
{
	tagTDrawInfo();
	tagTDrawInfo(LPCTSTR lpsz);
	void Clear();
	String sDrawString;
    String sImageName;
	bool bLoaded;
	const TImageInfo* pImageInfo;
	RECT rcDestOffset;
	RECT rcBmpPart;
	RECT rcScale9;
	BYTE uFade;
	bool bHole;
	bool bTiledX;
	bool bTiledY;
} TDrawInfo;

typedef struct DUILIB_API tagTPercentInfo
{
	double left;
	double top;
	double right;
	double bottom;
} TPercentInfo;

typedef struct DUILIB_API tagTResInfo
{
	DWORD m_dwDefaultDisabledColor;
	DWORD m_dwDefaultFontColor;
	DWORD m_dwDefaultLinkFontColor;
	DWORD m_dwDefaultLinkHoverFontColor;
	DWORD m_dwDefaultSelectedBkColor;
	TFontInfo m_DefaultFontInfo;
	StringPtrMap m_CustomFonts;			//"Font"
	StringPtrMap m_ColorHash;			//"Color"
	StringPtrMap m_ImageHash;
	StringPtrMap m_AttrHash;			//"Default" + "Class"
	StringPtrMap m_MultiLanguageHash;
} TResInfo;

// Structure for notifications from the system
// to the control implementation.
typedef struct DUILIB_API tagTEventUI
{
    int Type;
    Control* pSender;
    DWORD dwTimestamp;
    POINT ptMouse;
    TCHAR chKey;
    WORD wKeyState;
    WPARAM wParam;
    LPARAM lParam;
} TEvent;

// Listener interface
class DUILIB_API INotify
{
public:
    virtual void Notify(TNotify& msg) = 0;
};

// MessageFilter interface
class DUILIB_API IMessageFilter
{
public:
    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

class DUILIB_API ITranslateAccelerator
{
public:
	virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//
typedef Control* (*LPCREATECONTROL)(LPCTSTR pstrType);

class DUILIB_API CPaintManager
{
public:
    CPaintManager();
    ~CPaintManager();

public:
    void Init(HWND hWnd, LPCTSTR pstrName = NULL);
	bool IsUpdateNeeded() const;
    void NeedUpdate();
	void Invalidate();
    void Invalidate(RECT& rcItem);

	LPCTSTR GetThisResPath() const;
	void SetThisResPath(const LPCTSTR path);
	LPCTSTR GetName() const;
    HDC GetPaintDC() const;
	HBITMAP GetPaintOffscreenBitmap();
    HWND GetPaintWindow() const;
    HWND GetTooltipWindow() const;
	int GetTooltipWindowWidth() const;
	void SetTooltipWindowWidth(int iWidth);
	int GetHoverTime() const;
	void SetHoverTime(int iTime);

    POINT GetMousePos() const;
    SIZE GetClientSize() const;
    SIZE GetInitSize();
    void SetInitSize(int cx, int cy);
    RECT& GetSizeBox();
    void SetSizeBox(RECT& rcSizeBox);
    RECT& GetCaptionRect();
    void SetCaptionRect(RECT& rcCaption);
    SIZE GetRoundCorner() const;
    void SetRoundCorner(int cx, int cy);
    SIZE GetMinInfo() const;
    void SetMinInfo(int cx, int cy);
    SIZE GetMaxInfo() const;
    void SetMaxInfo(int cx, int cy);
    bool IsShowUpdateRect() const;
    void SetShowUpdateRect(bool show);
    bool IsNoActivate();
    void SetNoActivate(bool bNoActivate);

	BYTE GetOpacity() const;
	void SetOpacity(BYTE nOpacity);

	bool IsLayered();
	void SetLayered(bool bLayered);
	RECT& GetLayeredPadding();
	void SetLayeredPadding(RECT& rcLayeredPadding);
	BYTE GetLayeredOpacity();
	void SetLayeredOpacity(BYTE nOpacity);
	LPCTSTR GetLayeredImage();
	void SetLayeredImage(LPCTSTR pstrImage);
	CShadowUI* GetShadow();

    static HINSTANCE GetInstance();
    static String GetInstancePath();
    static String GetCurrentPath();
    static HINSTANCE GetResourceDll();
	static const String& GetGlobalResDir();
    static const String& GetResourceZip();
    static bool IsCachedResourceZip();
    static HANDLE GetResourceZipHandle();
    static void SetInstance(HINSTANCE hInst);
    static void SetCurrentPath(LPCTSTR pStrPath);
    static void SetResourceDll(HINSTANCE hInst);
    static void SetGlobalResDir(LPCTSTR pStrPath);
	static void SetResourceZip(LPVOID pVoid, unsigned int len);
    static void SetResourceZip(LPCTSTR pstrZip, bool bCachedResourceZip = false);
	static bool LoadGlobalResource();				//add by djj
    static bool GetHSL(short* H, short* S, short* L);
    static void SetHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
    static void ReloadSkin();
	static CPaintManager* GetPaintManager(LPCTSTR pstrName);
	static PtrArray* GetPaintManagers();
    static bool LoadPlugin(LPCTSTR pstrModuleName);
    static PtrArray* GetPlugins();

	bool IsForceUseSharedRes() const;
	void SetForceUseSharedRes(bool bForce);

    DWORD GetDefaultDisabledColor() const;
    void SetDefaultDisabledColor(DWORD dwColor, bool bShared = false);
    DWORD GetDefaultFontColor() const;
    void SetDefaultFontColor(DWORD dwColor, bool bShared = false);
    DWORD GetDefaultLinkFontColor() const;
    void SetDefaultLinkFontColor(DWORD dwColor, bool bShared = false);
    DWORD GetDefaultLinkHoverFontColor() const;
    void SetDefaultLinkHoverFontColor(DWORD dwColor, bool bShared = false);
    DWORD GetDefaultSelectedBkColor() const;
    void SetDefaultSelectedBkColor(DWORD dwColor, bool bShared = false);
	//------------Font--------------
    TFontInfo* GetDefaultFontInfo();
    void SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared = false);
    DWORD GetCustomFontCount(bool bShared = false) const;
    HFONT AddFont(int id, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared = false);
    HFONT GetFont(int id);
    HFONT GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	int GetFontIndex(HFONT hFont, bool bShared = false);
	int GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared = false);
    void RemoveFont(HFONT hFont, bool bShared = false);
    void RemoveFont(int id, bool bShared = false);
    void RemoveAllFonts(bool bShared = false);
    TFontInfo* GetFontInfo(int id);
    TFontInfo* GetFontInfo(HFONT hFont);
	static HFONT AddSharedFont(int id, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	//------------Color--------------
	void AddColor(LPCTSTR pStrFontName, DWORD dwValue);
	DWORD GetColor(LPCTSTR pStrFontName);
	void RemoveColor(LPCTSTR pStrFontName, bool bShared = false);
	void RemoveAllColors(bool bShared = false);
	static void AddSharedColor(LPCTSTR pStrFontName, DWORD dwValue);
	//------------Image--------------
    const TImageInfo* GetImage(LPCTSTR bitmap);
    const TImageInfo* GetImageEx(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0, bool bUseHSL = false);
    const TImageInfo* AddImage(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0, bool bUseHSL = false, bool bShared = false);
    const TImageInfo* AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha, bool bShared = false);
    void RemoveImage(LPCTSTR bitmap, bool bShared = false);
    void RemoveAllImages(bool bShared = false);
	void ReloadImages();
	static void ReloadSharedImages();
	//------------Class--------------
    void AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList, bool bShared = false);
    LPCTSTR GetDefaultAttributeList(LPCTSTR pStrControlName) const;
    bool RemoveDefaultAttributeList(LPCTSTR pStrControlName, bool bShared = false);
    void RemoveAllDefaultAttributeList(bool bShared = false);
	static void AddSharedDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList);
	//-------------------------------
    void AddWindowCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr);
    LPCTSTR GetWindowCustomAttribute(LPCTSTR pstrName) const;
    bool RemoveWindowCustomAttribute(LPCTSTR pstrName);
    void RemoveAllWindowCustomAttribute();
	//---------------Window----------------
    String GetWindowAttribute(LPCTSTR pstrName);
    void SetWindowAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    String GetWindowAttributeList(bool bIgnoreDefault = true);
    void SetWindowAttributeList(LPCTSTR pstrList);
    bool RemoveWindowAttribute(LPCTSTR pstrName);

	//---------------MultiLanguage----------------
	static void AddMultiLanguageString(int id, LPCTSTR pStrMultiLanguage);
	static LPCTSTR GetMultiLanguageString(int id);
	static bool RemoveMultiLanguageString(int id);
	static void RemoveAllMultiLanguageString();
	static void ProcessMultiLanguageTokens(String& pStrMultiLanguage);

    bool AttachDialog(Control* pControl);
    bool InitControls(Control* pControl, Control* pParent = NULL);
	bool RenameControl(Control* pControl, LPCTSTR pstrName);
    void ReapObjects(Control* pControl);

    bool AddOptionGroup(LPCTSTR pStrGroupName, Control* pControl);
    PtrArray* GetOptionGroup(LPCTSTR pStrGroupName);
    void RemoveOptionGroup(LPCTSTR pStrGroupName, Control* pControl);
    void RemoveAllOptionGroups();

    Control* GetFocus() const;
    void SetFocus(Control* pControl, bool bFocusWnd=true);
    void SetFocusNeeded(Control* pControl);

    bool SetNextTabControl(bool bForward = true);

    bool SetTimer(Control* pControl, UINT nTimerID, UINT uElapse);
    bool KillTimer(Control* pControl, UINT nTimerID);
    void KillTimer(Control* pControl);
    void RemoveAllTimers();

    void SetCapture();
    void ReleaseCapture();
    bool IsCaptured();

	bool IsPainting();
	void SetPainting(bool bIsPainting);

    bool AddNotifier(INotify* pControl);
    bool RemoveNotifier(INotify* pControl);   
    void SendNotify(TNotify& Msg, bool bAsync = false, bool bEnableRepeat = true);
    void SendNotify(Control* pControl, LPCTSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false, bool bEnableRepeat = true);

    bool AddPreMessageFilter(IMessageFilter* pFilter);
    bool RemovePreMessageFilter(IMessageFilter* pFilter);

    bool AddMessageFilter(IMessageFilter* pFilter);
    bool RemoveMessageFilter(IMessageFilter* pFilter);

    int GetPostPaintCount() const;
    bool AddPostPaint(Control* pControl);
    bool RemovePostPaint(Control* pControl);
    bool SetPostPaintIndex(Control* pControl, int iIndex);

	int GetNativeWindowCount() const;
	RECT GetNativeWindowRect(HWND hChildWnd);
	bool AddNativeWindow(Control* pControl, HWND hChildWnd);
	bool RemoveNativeWindow(HWND hChildWnd);

    void AddDelayedCleanup(Control* pControl);
    void AddMouseLeaveNeeded(Control* pControl);
    bool RemoveMouseLeaveNeeded(Control* pControl);

	bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool TranslateAccelerator(LPMSG pMsg);

    Control* GetRoot() const;
    Control* FindControl(POINT pt) const;
    Control* FindControl(LPCTSTR pstrName) const;
    Control* FindSubControlByPoint(Control* pParent, POINT pt) const;
    Control* FindSubControlByName(Control* pParent, LPCTSTR pstrName) const;
    Control* FindSubControlByClass(Control* pParent, LPCTSTR pstrClass, int iIndex = 0);
    PtrArray* FindSubControlsByClass(Control* pParent, LPCTSTR pstrClass);

    static void MessageLoop();
    static bool TranslateMessage(const LPMSG pMsg);
	static void Term();

    bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
    bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	void UsedVirtualWnd(bool bUsed);

	/*static*/ Container* CreateBox(const std::wstring& xmlPath, IDialogBuilderCallback *pCallback = NULL, CPaintManager *pManager = NULL, Control *pParent = NULL);
	/*static Box* CreateBoxWithCache(const std::wstring& xmlPath, CreateControlCallback callback = CreateControlCallback());

	static void FillBox(Box* userDefinedBox, const std::wstring& xmlPath, CreateControlCallback callback = CreateControlCallback());
	static void FillBoxWithCache(Box* userDefinedBox, const std::wstring& xmlPath, CreateControlCallback callback = CreateControlCallback());*/

private:
	PtrArray* GetFoundControls();
    static Control* CALLBACK __FindControlFromNameHash(Control* pThis, LPVOID pData);
    static Control* CALLBACK __FindControlFromCount(Control* pThis, LPVOID pData);
    static Control* CALLBACK __FindControlFromPoint(Control* pThis, LPVOID pData);
    static Control* CALLBACK __FindControlFromTab(Control* pThis, LPVOID pData);
    static Control* CALLBACK __FindControlFromShortcut(Control* pThis, LPVOID pData);
    static Control* CALLBACK __FindControlFromName(Control* pThis, LPVOID pData);
    static Control* CALLBACK __FindControlFromClass(Control* pThis, LPVOID pData);
    static Control* CALLBACK __FindControlsFromClass(Control* pThis, LPVOID pData);
	static Control* CALLBACK __FindControlsFromUpdate(Control* pThis, LPVOID pData);

	static void AdjustSharedImagesHSL();
	void AdjustImagesHSL();
	void PostAsyncNotify();

	
	

private:
	String m_sName;
	String m_pThisResPath;
    HWND m_hWndPaint;
    HDC m_hDcPaint;
    HDC m_hDcOffscreen;
    HDC m_hDcBackground;
    HBITMAP m_hbmpOffscreen;
	COLORREF* m_pOffscreenBits;
    HBITMAP m_hbmpBackground;
	COLORREF* m_pBackgroundBits;
	int m_iTooltipWidth;
    int m_iLastTooltipWidth;
	HWND m_hwndTooltip;
	TOOLINFO m_ToolTip;
	int m_iHoverTime;
    bool m_bNoActivate;
    bool m_bShowUpdateRect;
	CShadowUI m_shadow;
    //
    Control* m_pRoot;
    Control* m_pFocus;
    Control* m_pEventHover;
    Control* m_pEventClick;
    Control* m_pEventKey;
    Control* m_pLastToolTip;
    //
    POINT m_ptLastMousePos;
    SIZE m_szMinWindow;
    SIZE m_szMaxWindow;
    SIZE m_szInitWindowSize;
    RECT m_rcSizeBox;
    SIZE m_szRoundCorner;
    RECT m_rcCaption;
    UINT m_uTimerID;
    bool m_bFirstLayout;
    bool m_bUpdateNeeded;
    bool m_bFocusNeeded;
    bool m_bOffscreenPaint;

	BYTE m_nOpacity;
	bool m_bLayered;
	RECT m_rcLayeredPadding;
	bool m_bLayeredChanged;
	RECT m_rcLayeredUpdate;
	TDrawInfo m_diLayered;

    bool m_bMouseTracking;
    bool m_bMouseCapture;
	bool m_bIsPainting;
	bool m_bUsedVirtualWnd;
	bool m_bAsyncNotifyPosted;

    //
    PtrArray m_aNotifiers;				//save WindowImplBase::INotify* and CMenuWnd::INotify*, add in WindowImplBase::OnCreate, remove in WindowImplBase::OnFinalMessage
    PtrArray m_aTimers;					//TIMERINFO
	PtrArray m_aPreMessageFilters;		//save WindowImplBase::IMessageFilter*, add in WindowImplBase::OnCreate, remove in WindowImplBase::OnFinalMessage
    PtrArray m_aMessageFilters;			//save ActiveX::IMessageFilter* and RichEdit::IMessageFilter*, add in WindowImplBase::OnCreate->CPaintManager::InitControls(pControl)->Container::SetManager()->Control::Init()->RichEdit::DoInit(), remove in ~RichEdit()
    PtrArray m_aPostPaintControls;
	PtrArray m_aNativeWindow;
	PtrArray m_aNativeWindowControl;
    PtrArray m_aDelayedCleanup;
    PtrArray m_aAsyncNotify;
    PtrArray m_aFoundControls;
    PtrArray m_aNeedMouseLeaveNeeded;
	PtrArray m_aTranslateAccelerator;
    StringPtrMap m_mNameHash;			//(key:name, value:pControl)
	StringPtrMap m_mWindowAttrHash;		//
    StringPtrMap m_mOptionGroup;		//

    //
	bool m_bForceUseSharedRes;
	TResInfo m_ResInfo;		//StringPtrMap fonts, images, attrs, MultiLanguages,

    //
	static HINSTANCE m_hResourceInstance;
	static String m_pStrGlobalResDir;
	static String m_pStrResourceZip;
	static HANDLE m_hResourceZip;

	static bool m_bCachedResourceZip;
	static TResInfo m_SharedResInfo;
    static HINSTANCE m_hInstance;
	static bool m_bUseHSL;
    static short m_H;
    static short m_S;
    static short m_L;
    static PtrArray m_aPreMessages;		//save all live CPaintManager*, add in CPaintManager::Init(HWND hWnd, LPCTSTR pstrName), remove in ~CPaintManager()
    static PtrArray m_aPlugins;

public:
	//PtrArray m_aTranslateAccelerator;
};

} // namespace dui

#endif // __UIMANAGER_H__

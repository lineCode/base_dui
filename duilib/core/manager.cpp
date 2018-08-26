#include "stdafx.h"
#include <zmouse.h>
#include <stdlib.h>

DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);

namespace dui {

/////////////////////////////////////////////////////////////////////////////////////
//
//

static UINT MapKeyState()
{
    UINT uState = 0;
    if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
    if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_RBUTTON;
    if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_LBUTTON;
    if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
    if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
    return uState;
}

typedef struct tagFINDTABINFO
{
    Control* pFocus;
    Control* pLast;
    bool bForward;
    bool bNextIsIt;
} FINDTABINFO;

typedef struct tagFINDSHORTCUT
{
    TCHAR ch;
    bool bPickNext;
} FINDSHORTCUT;

typedef struct tagTIMERINFO
{
    Control* pSender;
    UINT nLocalID;
    HWND hWnd;
    UINT uWinTimer;
    bool bKilled;
} TIMERINFO;

/////////////////////////////////////////////////////////////////////////////////////

tagTDrawInfo::tagTDrawInfo()
{
	Clear();
}

tagTDrawInfo::tagTDrawInfo(LPCTSTR lpsz)
{
	Clear();
	sDrawString = lpsz;
}

void tagTDrawInfo::Clear()
{
	sDrawString.clear();
	sImageName.clear();
	::ZeroMemory(&bLoaded, sizeof(tagTDrawInfo) - offsetof(tagTDrawInfo, bLoaded));
	uFade = 255;
}

/////////////////////////////////////////////////////////////////////////////////////
typedef BOOL (__stdcall *PFUNCUPDATELAYEREDWINDOW)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);
PFUNCUPDATELAYEREDWINDOW g_fUpdateLayeredWindow = NULL;

HPEN m_hUpdateRectPen = NULL;

HINSTANCE UIManager::m_hResourceInstance = NULL;
String UIManager::m_sGlobalResDir;
String UIManager::m_sResourceZip;
HANDLE UIManager::m_hResourceZip = NULL;
bool UIManager::m_bCachedResourceZip = true;
ResInfo UIManager::m_SharedResInfo;
HINSTANCE UIManager::m_hInstance = NULL;
bool UIManager::m_bUseHSL = false;
short UIManager::m_H = 180;
short UIManager::m_S = 100;
short UIManager::m_L = 100;
PtrArray UIManager::m_aPreMessages;
PtrArray UIManager::m_aPlugins;

UIManager::UIManager() :
m_hWndPaint(NULL),
m_hDcPaint(NULL),
m_hDcOffscreen(NULL),
m_hDcBackground(NULL),
m_hbmpOffscreen(NULL),
m_pOffscreenBits(NULL),
m_hbmpBackground(NULL),
m_pBackgroundBits(NULL),
m_iTooltipWidth(-1),
m_iLastTooltipWidth(-1),
m_hWndTooltip(NULL),
m_iHoverTime(1000),
m_bNoActivate(false),
m_bShowUpdateRect(false),
m_uTimerID(0x1000),
m_pRoot(NULL),
m_pFocus(NULL),
m_pEventHover(NULL),
m_pEventClick(NULL),
m_pEventKey(NULL),
m_pLastToolTip(NULL),
m_bFirstLayout(true),
m_bFocusNeeded(false),
m_bUpdateNeeded(false),
m_bMouseTracking(false),
m_bMouseCapture(false),
m_bIsPainting(false),
m_bOffscreenPaint(true),
m_bAsyncNotifyPosted(false),
m_bForceUseSharedRes(false),
m_nOpacity(0xFF),
m_bLayered(false),
m_bLayeredChanged(false),
m_pDPI(NULL)
{
	if (m_SharedResInfo.m_DefaultFontInfo.sFontName.empty())
	{
		m_SharedResInfo.m_dwDefaultDisabledColor = 0xFFA7A6AA;
		m_SharedResInfo.m_dwDefaultFontColor = 0xFF000000;
		m_SharedResInfo.m_dwDefaultLinkFontColor = 0xFF0000FF;
		m_SharedResInfo.m_dwDefaultLinkHoverFontColor = 0xFFD3215F;
		m_SharedResInfo.m_dwDefaultSelectedBkColor = 0xFFBAE4FF;

		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		lf.lfCharSet = DEFAULT_CHARSET;
		HFONT hDefaultFont = ::CreateFontIndirect(&lf);
		m_SharedResInfo.m_DefaultFontInfo.hFont = hDefaultFont;
		m_SharedResInfo.m_DefaultFontInfo.sFontName = lf.lfFaceName;
		m_SharedResInfo.m_DefaultFontInfo.iSize = -lf.lfHeight;
		m_SharedResInfo.m_DefaultFontInfo.bBold = (lf.lfWeight >= FW_BOLD);
		m_SharedResInfo.m_DefaultFontInfo.bUnderline = (lf.lfUnderline == TRUE);
		m_SharedResInfo.m_DefaultFontInfo.bItalic = (lf.lfItalic == TRUE);
		::ZeroMemory(&m_SharedResInfo.m_DefaultFontInfo.tm, sizeof(m_SharedResInfo.m_DefaultFontInfo.tm));
	}

	m_ResInfo.m_dwDefaultDisabledColor = m_SharedResInfo.m_dwDefaultDisabledColor;
	m_ResInfo.m_dwDefaultFontColor = m_SharedResInfo.m_dwDefaultFontColor;
	m_ResInfo.m_dwDefaultLinkFontColor = m_SharedResInfo.m_dwDefaultLinkFontColor;
	m_ResInfo.m_dwDefaultLinkHoverFontColor = m_SharedResInfo.m_dwDefaultLinkHoverFontColor;
	m_ResInfo.m_dwDefaultSelectedBkColor = m_SharedResInfo.m_dwDefaultSelectedBkColor;

    if( m_hUpdateRectPen == NULL ) {
        m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
        // Boot Windows Common Controls (for the ToolTip control)
        ::InitCommonControls();
        ::LoadLibrary(_T("msimg32.dll"));
    }

    m_szMinWindow.cx = 0;
    m_szMinWindow.cy = 0;
    m_szMaxWindow.cx = 0;
    m_szMaxWindow.cy = 0;
    m_szInitWindowSize.cx = 0;
    m_szInitWindowSize.cy = 0;
    m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
    ::ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
    ::ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
	::ZeroMemory(&m_rcLayeredPadding, sizeof(m_rcLayeredPadding));
	::ZeroMemory(&m_rcLayeredUpdate, sizeof(m_rcLayeredUpdate));
    m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
}

UIManager::~UIManager()
{
	printf("UIManager::~UIManager()\n");
    // Delete the control-tree structures
    for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) static_cast<Control*>(m_aDelayedCleanup[i])->Delete();
    for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) delete static_cast<Event*>(m_aAsyncNotify[i]);
    m_mNameHash.Resize(0);
    if( m_pRoot != NULL ) m_pRoot->Delete();

    ::DeleteObject(m_ResInfo.m_DefaultFontInfo.hFont);
    RemoveAllFonts();
    RemoveAllImages();
    RemoveAllDefaultAttributeList();
	RemoveAllWindowCustomAttribute();
    RemoveAllOptionGroups();
    RemoveAllTimers();

    // Reset other parts...
    if( m_hWndTooltip != NULL )
	{
		::DestroyWindow(m_hWndTooltip);
		m_hWndTooltip = NULL;
	}
    m_pLastToolTip = NULL;
    if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
    if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
    if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
    if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
    if( m_hDcPaint != NULL ) ::ReleaseDC(m_hWndPaint, m_hDcPaint);
    m_aPreMessages.Remove(m_aPreMessages.Find(this));

	//卸载GDIPlus
	/*Gdiplus::GdiplusShutdown(m_gdiplusToken);
	delete m_pGdiplusStartupInput;*/
	// DPI管理对象
	if (m_pDPI != NULL) {
		delete m_pDPI;
		m_pDPI = NULL;
	}
}

void UIManager::Init(HWND hWnd, LPCTSTR pstrName)
{
	ASSERT(::IsWindow(hWnd));

	m_mNameHash.Resize();
	RemoveAllFonts();
	RemoveAllImages();
	RemoveAllDefaultAttributeList();
	RemoveAllWindowCustomAttribute();
	RemoveAllOptionGroups();
	RemoveAllTimers();

	m_sName.clear();
	if( pstrName != NULL ) m_sName = pstrName;

	if( m_hWndPaint != hWnd ) {
		m_hWndPaint = hWnd;
		m_hDcPaint = ::GetDC(hWnd);
		m_aPreMessages.Add(this);
	}
}

HINSTANCE UIManager::GetInstance()
{
    return m_hInstance;
}

String UIManager::GetInstancePath()
{
    if( m_hInstance == NULL ) return _T('\0');
    
    TCHAR tszModule[MAX_PATH + 1] = { 0 };
    ::GetModuleFileName(m_hInstance, tszModule, MAX_PATH);
    String sInstancePath = tszModule;
    int pos = sInstancePath.rfind(_T('\\'));
    if( pos >= 0 ) sInstancePath = sInstancePath.substr(0, pos + 1);
    return sInstancePath;
}

String UIManager::GetCurrentPath()
{
    TCHAR tszModule[MAX_PATH + 1] = { 0 };
    ::GetCurrentDirectory(MAX_PATH, tszModule);
    return tszModule;
}

HINSTANCE UIManager::GetResourceDll()
{
    if( m_hResourceInstance == NULL ) return m_hInstance;
    return m_hResourceInstance;
}

const String& UIManager::GetGlobalResDir()
{
    return m_sGlobalResDir;
}

const String& UIManager::GetResourceZip()
{
    return m_sResourceZip;
}

bool UIManager::IsCachedResourceZip()
{
    return m_bCachedResourceZip;
}

HANDLE UIManager::GetResourceZipHandle()
{
    return m_hResourceZip;
}

void UIManager::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
	ShadowUI::Initialize(hInst);
}

void UIManager::SetCurrentPath(LPCTSTR pStrPath)
{
    ::SetCurrentDirectory(pStrPath);
}

void UIManager::SetResourceDll(HINSTANCE hInst)
{
    m_hResourceInstance = hInst;
}

void UIManager::SetGlobalResDir(LPCTSTR pStrPath)
{
    m_sGlobalResDir = pStrPath;
    if( m_sGlobalResDir.empty() ) return;
    TCHAR cEnd = m_sGlobalResDir.at(m_sGlobalResDir.length() - 1);
    if( cEnd != _T('\\') && cEnd != _T('/') ) m_sGlobalResDir += _T('\\');

	LoadGlobalResource();
}

void UIManager::SetResourceZip(LPVOID pVoid, unsigned int len)
{
    if( m_sResourceZip == _T("membuffer") ) return;
    if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
        CloseZip((HZIP)m_hResourceZip);
        m_hResourceZip = NULL;
    }
    m_sResourceZip = _T("membuffer");
    if( m_bCachedResourceZip ) 
        m_hResourceZip = (HANDLE)OpenZip(pVoid, len, 3);
}

void UIManager::SetResourceZip(LPCTSTR pStrPath, bool bCachedResourceZip)
{
    if( m_sResourceZip == pStrPath && m_bCachedResourceZip == bCachedResourceZip ) return;
    if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
        CloseZip((HZIP)m_hResourceZip);
        m_hResourceZip = NULL;
    }
    m_sResourceZip = pStrPath;
    m_bCachedResourceZip = bCachedResourceZip;
    if( m_bCachedResourceZip ) {
		String sFile = UIManager::GetGlobalResDir();
        sFile += UIManager::GetResourceZip();
        m_hResourceZip = (HANDLE)OpenZip((void*)sFile.c_str(), 0, 2);
    }
}

bool UIManager::LoadGlobalResource()
{
	String file = _T("global.xml");

	Markup xml;
	if (HIWORD(file.c_str()) != NULL) {
		if (*(file.c_str()) == _T('<')) {
			if (!xml.Load(file.c_str())) return false;
		}
		else {
			if (!xml.LoadFromFile(file.c_str(), nullptr)) return false;
		}
	}
	//-----------------------------------------
	MarkupNode root = xml.GetRoot();
	if (!root.IsValid()) return false;
	LPCTSTR pstrClass = NULL;
	int nAttributes = 0;
	LPCTSTR pstrName = NULL;
	LPCTSTR pstrValue = NULL;
	LPTSTR pstr = NULL;
	for (MarkupNode node = root.GetChild(); node.IsValid(); node = node.GetSibling()) {
		pstrClass = node.GetName();
		if (_tcsicmp(pstrClass, _T("Font")) == 0) {
			nAttributes = node.GetAttributeCount();
			int id = m_SharedResInfo.m_CustomFonts.GetSize();
			LPCTSTR pFontName = NULL;
			int size = 12;
			bool bold = false;
			bool underline = false;
			bool italic = false;
			bool defaultfont = false;
			/*bool shared = false;*/
			for (int i = 0; i < nAttributes; i++) {
				pstrName = node.GetAttributeName(i);
				pstrValue = node.GetAttributeValue(i);
				if (_tcsicmp(pstrName, _T("name")) == 0) {
					pFontName = pstrValue;
				}
				else if (_tcsicmp(pstrName, _T("size")) == 0) {
					size = _tcstol(pstrValue, &pstr, 10);
				}
				else if (_tcsicmp(pstrName, _T("bold")) == 0) {
					bold = (_tcsicmp(pstrValue, _T("true")) == 0);
				}
				else if (_tcsicmp(pstrName, _T("underline")) == 0) {
					underline = (_tcsicmp(pstrValue, _T("true")) == 0);
				}
				else if (_tcsicmp(pstrName, _T("italic")) == 0) {
					italic = (_tcsicmp(pstrValue, _T("true")) == 0);
				}
			}
			if (id >= 0 && pFontName) {
				AddSharedFont(id, pFontName, size, bold, underline, italic);
			}
		}
		else if (_tcsicmp(pstrClass, _T("Color")) == 0)
		{
			nAttributes = node.GetAttributeCount();
			LPCTSTR pColorName = NULL;
			DWORD dwValue = 0;
			for (int i = 0; i < nAttributes; i++) {
				pstrName = node.GetAttributeName(i);
				pstrValue = node.GetAttributeValue(i);
				if (_tcsicmp(pstrName, _T("name")) == 0) {
					pColorName = pstrValue;
				}
				else if (_tcsicmp(pstrName, _T("value")) == 0) {
					if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
					LPTSTR pstr = NULL;
					dwValue = _tcstoul(pstrValue, &pstr, 16);
				}
			}
			if (pColorName)
			{
				AddSharedColor(pColorName, dwValue);
			}	
		}
		else if (_tcsicmp(pstrClass, _T("Class")) == 0) {
			nAttributes = node.GetAttributeCount();
			LPCTSTR pName = NULL;
			LPCTSTR pValue = NULL;
			/*bool shared = false;*/
			for (int i = 0; i < nAttributes; i++) {
				pstrName = node.GetAttributeName(i);
				pstrValue = node.GetAttributeValue(i);
				if (_tcsicmp(pstrName, _T("name")) == 0) {
					pName = pstrValue;
				}
				else if (_tcsicmp(pstrName, _T("value")) == 0) {
					pValue = pstrValue;
				}
			}
			if (pName) {
				AddSharedDefaultAttributeList(pName, pValue);
			}
		}
		else if (_tcsicmp(pstrClass, _T("Default")) == 0) {
			nAttributes = node.GetAttributeCount();
			LPCTSTR pControlName = NULL;
			LPCTSTR pControlValue = NULL;
			bool shared = false;
			for (int i = 0; i < nAttributes; i++) {
				pstrName = node.GetAttributeName(i);
				pstrValue = node.GetAttributeValue(i);
				if (_tcsicmp(pstrName, _T("name")) == 0) {
					pControlName = pstrValue;
				}
				else if (_tcsicmp(pstrName, _T("value")) == 0) {
					pControlValue = pstrValue;
				}
				else if (_tcsicmp(pstrName, _T("shared")) == 0) {
					shared = (_tcsicmp(pstrValue, _T("true")) == 0);
				}
			}
			if (pControlName && pControlValue) {
				AddSharedDefaultAttributeList(pControlName, pControlValue);
			}
		}
	}
	return true;
}

bool UIManager::GetHSL(short* H, short* S, short* L)
{
    *H = m_H;
    *S = m_S;
    *L = m_L;
	return m_bUseHSL;
}

void UIManager::SetHSL(bool bUseHSL, short H, short S, short L)
{
	if( m_bUseHSL || m_bUseHSL != bUseHSL ) {
		m_bUseHSL = bUseHSL;
		if( H == m_H && S == m_S && L == m_L ) return;
		m_H = CLAMP(H, 0, 360);
		m_S = CLAMP(S, 0, 200);
		m_L = CLAMP(L, 0, 200);
		AdjustSharedImagesHSL();
		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
			UIManager* pManager = static_cast<UIManager*>(m_aPreMessages[i]);
			if( pManager != NULL ) pManager->AdjustImagesHSL();
		}
	}
}

void UIManager::ReloadSkin()
{
	ReloadSharedImages();
    for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
        UIManager* pManager = static_cast<UIManager*>(m_aPreMessages[i]);
        pManager->ReloadImages();
    }
}

UIManager* UIManager::GetPaintManager(LPCTSTR pstrName)
{
	if( pstrName == NULL ) return NULL;
	String sName = pstrName;
	if( sName.empty() ) return NULL;
	for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
		UIManager* pManager = static_cast<UIManager*>(m_aPreMessages[i]);
		if( pManager != NULL && sName == pManager->GetName() ) return pManager;
	}
	return NULL;
}

PtrArray* UIManager::GetPaintManagers()
{
	return &m_aPreMessages;
}

bool UIManager::LoadPlugin(LPCTSTR pstrModuleName)
{
    ASSERT( !::IsBadStringPtr(pstrModuleName,-1) || pstrModuleName == NULL );
    if( pstrModuleName == NULL ) return false;
    HMODULE hModule = ::LoadLibrary(pstrModuleName);
    if( hModule != NULL ) {
        LPCREATECONTROL lpCreateControl = (LPCREATECONTROL)::GetProcAddress(hModule, "CreateControl");
        if( lpCreateControl != NULL ) {
            if( m_aPlugins.Find(lpCreateControl) >= 0 ) return true;
            m_aPlugins.Add(lpCreateControl);
            return true;
        }
    }
    return false;
}

PtrArray* UIManager::GetPlugins()
{
    return &m_aPlugins;
}

HWND UIManager::GetPaintWindow() const
{
    return m_hWndPaint;
}

HWND UIManager::GetTooltipWindow() const
{
	return m_hWndTooltip;
}

int UIManager::GetTooltipWindowWidth() const
{
	return m_iTooltipWidth;
}

void UIManager::SetTooltipWindowWidth(int iWidth)
{
	if( m_iTooltipWidth != iWidth ) {
		m_iTooltipWidth = iWidth;
		if( m_hWndTooltip != NULL && m_iTooltipWidth >= 0  ) {
			m_iTooltipWidth = (int)::SendMessage(m_hWndTooltip, TTM_SETMAXTIPWIDTH, 0, m_iTooltipWidth);
		}
	}
}

int UIManager::GetHoverTime() const
{
	return m_iHoverTime;
}

void UIManager::SetHoverTime(int iTime)
{
	m_iHoverTime = iTime;
}

LPCTSTR UIManager::GetThisResPath() const
{
	return m_sThisResPath.c_str();
}

void UIManager::SetThisResPath(const LPCTSTR path)
{
	m_sThisResPath = path;
}

LPCTSTR UIManager::GetName() const
{
	return m_sName.c_str();
}

HDC UIManager::GetPaintDC() const
{
    return m_hDcPaint;
}

HBITMAP UIManager::GetPaintOffscreenBitmap()
{
	return m_hbmpOffscreen;
}

POINT UIManager::GetMousePos() const
{
    return m_ptLastMousePos;
}

SIZE UIManager::GetClientSize() const
{
    RECT rcClient = { 0 };
    ::GetClientRect(m_hWndPaint, &rcClient);
    return DuiSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
}

SIZE UIManager::GetInitSize()
{
    return m_szInitWindowSize;
}

void UIManager::SetInitSize(int cx, int cy)
{
    m_szInitWindowSize.cx = cx;
    m_szInitWindowSize.cy = cy;
    if( m_pRoot == NULL && m_hWndPaint != NULL ) {
        ::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }
}

RECT& UIManager::GetSizeBox()
{
    return m_rcSizeBox;
}

void UIManager::SetSizeBox(RECT& rcSizeBox)
{
    m_rcSizeBox = rcSizeBox;
}

RECT& UIManager::GetCaptionRect()
{
    return m_rcCaption;
}

void UIManager::SetCaptionRect(RECT& rcCaption)
{
    m_rcCaption = rcCaption;
}

SIZE UIManager::GetRoundCorner() const
{
    return m_szRoundCorner;
}

void UIManager::SetRoundCorner(int cx, int cy)
{
    m_szRoundCorner.cx = cx;
    m_szRoundCorner.cy = cy;
}

SIZE UIManager::GetMinInfo() const
{
	return m_szMinWindow;
}

void UIManager::SetMinInfo(int cx, int cy)
{
    ASSERT(cx>=0 && cy>=0);
    m_szMinWindow.cx = cx;
    m_szMinWindow.cy = cy;
}

SIZE UIManager::GetMaxInfo() const
{
    return m_szMaxWindow;
}

void UIManager::SetMaxInfo(int cx, int cy)
{
    ASSERT(cx>=0 && cy>=0);
    m_szMaxWindow.cx = cx;
    m_szMaxWindow.cy = cy;
}

bool UIManager::IsShowUpdateRect() const
{
	return m_bShowUpdateRect;
}

void UIManager::SetShowUpdateRect(bool show)
{
    m_bShowUpdateRect = show;
}

bool UIManager::IsNoActivate()
{
    return m_bNoActivate;
}

void UIManager::SetNoActivate(bool bNoActivate)
{
    m_bNoActivate = bNoActivate;
}

BYTE UIManager::GetOpacity() const
{
	return m_nOpacity;
}

void UIManager::SetOpacity(BYTE nOpacity)
{
	m_nOpacity = nOpacity;
	if( m_hWndPaint != NULL ) {
		typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
		PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes = NULL;

		HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
		if (hUser32)
		{
			fSetLayeredWindowAttributes = 
				(PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
			if( fSetLayeredWindowAttributes == NULL ) return;
		}

		DWORD dwStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
		DWORD dwNewStyle = dwStyle;
		if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
		else dwNewStyle &= ~WS_EX_LAYERED;
		if(dwStyle != dwNewStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
		fSetLayeredWindowAttributes(m_hWndPaint, 0, nOpacity, LWA_ALPHA);

		m_bLayered = false;
		Invalidate();
	}
}

//void UIManager::SetBackgroundTransparent(bool bTrans)
//{
//	m_bAlphaBackground = bTrans;
//}

bool UIManager::IsLayered()
{
	return m_bLayered;
}

void UIManager::SetLayered(bool bLayered)
{
	if( m_hWndPaint != NULL && bLayered != m_bLayered ) {
		UINT uStyle = GetWindowStyle(m_hWndPaint);
		if( (uStyle & WS_CHILD) != 0 ) return;

		if( g_fUpdateLayeredWindow == NULL ) {
			HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
			if (hUser32) {
				g_fUpdateLayeredWindow = 
					(PFUNCUPDATELAYEREDWINDOW)::GetProcAddress(hUser32, "UpdateLayeredWindow");
				if( g_fUpdateLayeredWindow == NULL ) return;
			}
		}
		DWORD dwExStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
		DWORD dwNewExStyle = dwExStyle;
		if( bLayered ) {
			dwNewExStyle |= WS_EX_LAYERED;
			::SetTimer(m_hWndPaint, LAYEREDUPDATE_TIMERID, 10L, NULL);
		}
		else {
			dwNewExStyle &= ~WS_EX_LAYERED;
			::KillTimer(m_hWndPaint, LAYEREDUPDATE_TIMERID);
		}
		if(dwExStyle != dwNewExStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewExStyle);
		m_bLayered = bLayered;
		if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
		Invalidate();
	}
}

RECT& UIManager::GetLayeredPadding()
{
	return m_rcLayeredPadding;
}

void UIManager::SetLayeredPadding(RECT& rcLayeredPadding)
{
	m_rcLayeredPadding = rcLayeredPadding;
	m_bLayeredChanged = true;
	Invalidate();
}

BYTE UIManager::GetLayeredOpacity()
{
	return m_nOpacity;
}

void UIManager::SetLayeredOpacity(BYTE nOpacity)
{
	m_nOpacity = nOpacity;
	m_bLayeredChanged = true;
	Invalidate();
}

LPCTSTR UIManager::GetLayeredImage()
{
	return m_diLayered.sDrawString.c_str();
}

void UIManager::SetLayeredImage(LPCTSTR pstrImage)
{
	m_diLayered.sDrawString = pstrImage;
	RECT rcNull = {0};
	Render::DrawImage(NULL, this, rcNull, rcNull, m_diLayered);
}

ShadowUI* UIManager::GetShadow()
{
	return &m_shadow;
}

bool UIManager::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
{
    for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) 
    {
        bool bHandled = false;
        LRESULT lResult = static_cast<IMessageFilter*>(m_aPreMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
        if( bHandled ) {
            return true;
        }
    }
    switch( uMsg ) {
    case WM_KEYDOWN:
        {
           // Tabbing between controls
           if( wParam == VK_TAB ) {
               if( m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), DUI_CTR_RICHEDIT) != NULL ) {
                   if( static_cast<RichEdit*>(m_pFocus)->IsWantTab() ) return false;
               }
               SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
               return true;
           }
        }
        break;
    case WM_SYSCHAR:
        {
		   if( m_pRoot == NULL ) return false;
           // Handle ALT-shortcut key-combinations
           FINDSHORTCUT fs = { 0 };
           fs.ch = toupper((int)wParam);
           Control* pControl = m_pRoot->FindControl(__FindControlFromShortcut, &fs, UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
           if( pControl != NULL ) {
               pControl->SetFocus();
               pControl->Activate();
               return true;
           }
        }
        break;
    case WM_SYSKEYDOWN:
        {
           if( m_pFocus != NULL ) {
               Event event/* = { UIEVENT__FIRST }*/;
               event.Type = UIEVENT_SYSKEY;
			   event.pSender = m_pFocus;
               event.chKey = (TCHAR)wParam;
               event.ptMouse = m_ptLastMousePos;
               event.wKeyState = MapKeyState();
               event.dwTimestamp = ::GetTickCount();
			   m_pFocus->HandleEvent(event);
           }
        }
        break;
    }
    return false;
}

bool UIManager::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
//#ifdef _DEBUG
//    switch( uMsg ) {
//    case WM_NCPAINT:
//    case WM_NCHITTEST:
//    case WM_SETCURSOR:
//       break;
//    default:
//       DUITRACE(_T("MSG: %-20s (%08ld)"), DUITRACEMSG(uMsg), ::GetTickCount());
//    }
//#endif
    // Not ready yet?
    if( m_hWndPaint == NULL ) return false;
    
    // Cycle through listeners
    for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) 
    {
        bool bHandled = false;
        LRESULT lResult = static_cast<IMessageFilter*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
        if( bHandled ) {
            lRes = lResult;
			switch( uMsg ) {
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONUP:
				{
					POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					m_ptLastMousePos = pt;
				}
				break;
			case WM_CONTEXTMENU:
			case WM_MOUSEWHEEL:
				{
					POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					::ScreenToClient(m_hWndPaint, &pt);
					m_ptLastMousePos = pt;
				}
				break;
			}
            return true;
        }
    }

	if( m_bLayered ) {
		switch( uMsg ) {
		case WM_NCACTIVATE:
			if( !::IsIconic(m_hWndPaint) ) {
				lRes = (wParam == 0) ? TRUE : FALSE;
				return true;
			}
			break;
		case WM_NCCALCSIZE:
		case WM_NCPAINT:
			lRes = 0;
			return true;
		}
	}

    // Custom handling of events
    switch( uMsg ) {
    case WM_APP + 1:
        {
			m_bAsyncNotifyPosted = false;

			Event* pMsg = NULL;
			while( pMsg = static_cast<Event*>(m_aAsyncNotify.GetAt(0)) ) {
				m_aAsyncNotify.Remove(0);
				if( pMsg->pSender != NULL ) {
					if (pMsg->pSender->m_cbNotify) pMsg->pSender->m_cbNotify(pMsg);
				}
				for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
					static_cast<INotify*>(m_aNotifiers[j])->Notify(*pMsg);
				}
				delete pMsg;
			}

            for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) 
                static_cast<Control*>(m_aDelayedCleanup[i])->Delete();
            m_aDelayedCleanup.Empty();
        }
        break;
    case WM_CLOSE:
        {
            // Make sure all matching "closing" events are sent
            Event event/* = { UIEVENT__FIRST }*/;
            event.ptMouse = m_ptLastMousePos;
			event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            if( m_pEventHover != NULL ) {
                event.Type = UIEVENT_MOUSELEAVE;
                event.pSender = m_pEventHover;
				m_pEventHover->HandleEvent(event);
            }
            if( m_pEventClick != NULL ) {
                event.Type = UIEVENT_BUTTONUP;
                event.pSender = m_pEventClick;
				m_pEventClick->HandleEvent(event);
            }

            SetFocus(NULL);

			if( ::GetActiveWindow() == m_hWndPaint ) {
				HWND hwndParent = GetWindowOwner(m_hWndPaint);
                if ((GetWindowStyle(m_hWndPaint) & WS_CHILD) !=0 ) hwndParent = GetParent(m_hWndPaint);
				if( hwndParent != NULL ) ::SetFocus(hwndParent);
			}
			if (m_hWndTooltip != NULL) {
				::DestroyWindow(m_hWndTooltip);
				m_hWndTooltip = NULL;
			}
        }
        break;
    case WM_ERASEBKGND:
        {
            // We'll do the painting here...
            lRes = 1;
        }
        return true;
    case WM_PAINT:
        {
            if( m_pRoot == NULL ) {
                PAINTSTRUCT ps = { 0 };
                ::BeginPaint(m_hWndPaint, &ps);
				Render::DrawColor(m_hDcPaint, ps.rcPaint, 0xFF000000);
                ::EndPaint(m_hWndPaint, &ps);
                return true;
            }

			RECT rcClient = { 0 };
			::GetClientRect(m_hWndPaint, &rcClient);

			RECT rcPaint = { 0 };
			if( m_bLayered ) {
				m_bOffscreenPaint = true;
				rcPaint = m_rcLayeredUpdate;
				if( ::IsRectEmpty(&m_rcLayeredUpdate) ) {
					PAINTSTRUCT ps = { 0 };
					::BeginPaint(m_hWndPaint, &ps);
					::EndPaint(m_hWndPaint, &ps);
					return true;
				}
				if( rcPaint.right > rcClient.right ) rcPaint.right = rcClient.right;
				if( rcPaint.bottom > rcClient.bottom ) rcPaint.bottom = rcClient.bottom;
				::ZeroMemory(&m_rcLayeredUpdate, sizeof(m_rcLayeredUpdate));
			}
			else {
				if( !::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE) ) return true;
			}
			
            // Set focus to first control?
            if( m_bFocusNeeded ) {
                SetNextTabControl();
            }

			SetPainting(true);
            if( m_bUpdateNeeded ) {
                m_bUpdateNeeded = false;
                if( !::IsRectEmpty(&rcClient) ) {
                    if( m_pRoot->IsUpdateNeeded() ) {
						RECT rcRoot = rcClient;
                        if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
                        if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
                        if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
                        if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
                        m_hDcOffscreen = NULL;
                        m_hDcBackground = NULL;
                        m_hbmpOffscreen = NULL;
                        m_hbmpBackground = NULL;
						if( m_bLayered ) {
							rcRoot.left += m_rcLayeredPadding.left;
							rcRoot.top += m_rcLayeredPadding.top;
							rcRoot.right -= m_rcLayeredPadding.right;
							rcRoot.bottom -= m_rcLayeredPadding.bottom;
						}
						m_pRoot->SetPos(rcRoot, true);
                    }
                    else {
						Control* pControl = NULL;
						m_aFoundControls.Empty();
						m_pRoot->FindControl(__FindControlsFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST | UIFIND_UPDATETEST);
						for( int it = 0; it < m_aFoundControls.GetSize(); it++ ) {
							pControl = static_cast<Control*>(m_aFoundControls[it]);
							if( !pControl->IsFloat() ) pControl->SetPos(pControl->GetPos(), true);
							else pControl->SetPos(pControl->GetRelativePos(), true);
						}
                    }
                    // We'll want to notify the window when it is first initialized
                    // with the correct layout. The window form would take the time
                    // to submit swipes/animations.
					if( m_bFirstLayout ) {
						m_bFirstLayout = false;
						SendNotify(m_pRoot, UIEVENT_WINDOWINIT, 0, 0, false);
						if( m_bLayered && m_bLayeredChanged ) {
							Invalidate();
							SetPainting(false);
							return true;
						}
					}
                }
            }
			else if( m_bLayered && m_bLayeredChanged ) {
				RECT rcRoot = rcClient;
				if( m_pOffscreenBits ) ::ZeroMemory(m_pOffscreenBits, (rcRoot.right - rcRoot.left) 
					* (rcRoot.bottom - rcRoot.top) * 4);
				rcRoot.left += m_rcLayeredPadding.left;
				rcRoot.top += m_rcLayeredPadding.top;
				rcRoot.right -= m_rcLayeredPadding.right;
				rcRoot.bottom -= m_rcLayeredPadding.bottom;
				m_pRoot->SetPos(rcRoot, true);
			}
            //
            // Render screen
            //
            // Prepare offscreen bitmap?
            if( m_bOffscreenPaint && m_hbmpOffscreen == NULL )
            {
                m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
				if( m_bLayered ) m_hbmpOffscreen = Render::CreateARGB32Bitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, &m_pOffscreenBits); 
                else m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
                ASSERT(m_hDcOffscreen);
                ASSERT(m_hbmpOffscreen);
            }
            // Begin Windows paint
            PAINTSTRUCT ps = { 0 };
            ::BeginPaint(m_hWndPaint, &ps);
            if( m_bOffscreenPaint )
            {
                HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
                int iSaveDC = ::SaveDC(m_hDcOffscreen);
				if (m_bLayered && m_diLayered.pImageInfo == NULL) {
					COLORREF* pOffscreenBits = NULL;
					for( LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y ) {
						for( LONG x = rcPaint.left; x < rcPaint.right; ++x ) {
							pOffscreenBits = m_pOffscreenBits + y*(rcClient.right - rcClient.left) + x;
							*pOffscreenBits = 0;
						}
					}
				}
                m_pRoot->Paint(m_hDcOffscreen, rcPaint, NULL);

				if( m_bLayered ) {
					for( int i = 0; i < m_aNativeWindow.GetSize(); ) {
						HWND hChildWnd = static_cast<HWND>(m_aNativeWindow[i]);
						if (!::IsWindow(hChildWnd)) {
							m_aNativeWindow.Remove(i);
							m_aNativeWindowControl.Remove(i);
							continue;
						}
						++i;
						if (!::IsWindowVisible(hChildWnd)) continue;
						RECT rcChildWnd = GetNativeWindowRect(hChildWnd);
						RECT rcTemp = { 0 };
						if( !::IntersectRect(&rcTemp, &rcPaint, &rcChildWnd) ) continue;

						COLORREF* pChildBitmapBits = NULL;
						HDC hChildMemDC = ::CreateCompatibleDC(m_hDcOffscreen);
						HBITMAP hChildBitmap = Render::CreateARGB32Bitmap(hChildMemDC, rcChildWnd.right-rcChildWnd.left, rcChildWnd.bottom-rcChildWnd.top, &pChildBitmapBits); 
						::ZeroMemory(pChildBitmapBits, (rcChildWnd.right - rcChildWnd.left)*(rcChildWnd.bottom - rcChildWnd.top)*4);
						HBITMAP hOldChildBitmap = (HBITMAP) ::SelectObject(hChildMemDC, hChildBitmap);
						::SendMessage(hChildWnd, WM_PRINT, (WPARAM)hChildMemDC,(LPARAM)(PRF_CHECKVISIBLE|PRF_CHILDREN|PRF_CLIENT|PRF_OWNED));
						COLORREF* pChildBitmapBit;
						for( LONG y = 0; y < rcChildWnd.bottom-rcChildWnd.top; y++ ) {
							for( LONG x = 0; x < rcChildWnd.right-rcChildWnd.left; x++ ) {
								pChildBitmapBit = pChildBitmapBits+y*(rcChildWnd.right-rcChildWnd.left) + x;
								if (*pChildBitmapBit != 0x00000000) *pChildBitmapBit |= 0xff000000;
							}
						}
						::BitBlt(m_hDcOffscreen, rcChildWnd.left, rcChildWnd.top, rcChildWnd.right - rcChildWnd.left,
							rcChildWnd.bottom - rcChildWnd.top, hChildMemDC, 0, 0, SRCCOPY);
						::SelectObject(hChildMemDC, hOldChildBitmap);
						::DeleteObject(hChildBitmap);
						::DeleteDC(hChildMemDC);
					}
				}

                for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
                    Control* pPostPaintControl = static_cast<Control*>(m_aPostPaintControls[i]);
                    pPostPaintControl->DoPostPaint(m_hDcOffscreen, rcPaint);
                }
                ::RestoreDC(m_hDcOffscreen, iSaveDC);
                if( m_bLayered ) {
                    RECT rcWnd = { 0 };
                    ::GetWindowRect(m_hWndPaint, &rcWnd);
                    DWORD dwWidth = rcClient.right - rcClient.left;
                    DWORD dwHeight = rcClient.bottom - rcClient.top;
                    RECT rcLayeredClient = rcClient;
                    rcLayeredClient.left += m_rcLayeredPadding.left;
                    rcLayeredClient.top += m_rcLayeredPadding.top;
                    rcLayeredClient.right -= m_rcLayeredPadding.right;
                    rcLayeredClient.bottom -= m_rcLayeredPadding.bottom;

					COLORREF* pOffscreenBits = m_pOffscreenBits;
					COLORREF* pBackgroundBits = m_pBackgroundBits;
					BYTE A = 0;
					BYTE R = 0;
					BYTE G = 0;
					BYTE B = 0;
					if (m_diLayered.pImageInfo != NULL) {
						if( m_hbmpBackground == NULL) {
							m_hDcBackground = ::CreateCompatibleDC(m_hDcPaint);
							m_hbmpBackground = Render::CreateARGB32Bitmap(m_hDcPaint, dwWidth, dwHeight, &m_pBackgroundBits); 
							ASSERT(m_hDcBackground);
							ASSERT(m_hbmpBackground);
							::ZeroMemory(m_pBackgroundBits, dwWidth * dwHeight * 4);
							::SelectObject(m_hDcBackground, m_hbmpBackground);
							RenderClip clip;
							RenderClip::GenerateClip(m_hDcBackground, rcLayeredClient, clip);
							Render::DrawImage(m_hDcBackground, this, rcLayeredClient, rcLayeredClient, m_diLayered);
						}
						else if( m_bLayeredChanged ) {
							::ZeroMemory(m_pBackgroundBits, dwWidth * dwHeight * 4);
							RenderClip clip;
							RenderClip::GenerateClip(m_hDcBackground, rcLayeredClient, clip);
							Render::DrawImage(m_hDcBackground, this, rcLayeredClient, rcLayeredClient, m_diLayered);
						}
						if( m_diLayered.pImageInfo->bAlpha ) {
							for( LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y ) {
								for( LONG x = rcPaint.left; x < rcPaint.right; ++x ) {
									pOffscreenBits = m_pOffscreenBits + y * dwWidth + x;
									pBackgroundBits = m_pBackgroundBits + y * dwWidth + x;
									A = (BYTE)((*pBackgroundBits) >> 24);
									R = (BYTE)((*pOffscreenBits) >> 16) * A / 255;
									G = (BYTE)((*pOffscreenBits) >> 8) * A / 255;
									B = (BYTE)(*pOffscreenBits) * A / 255;
									*pOffscreenBits = RGB(B, G, R) + ((DWORD)A << 24);
								}
							}
						}
					}
                    BLENDFUNCTION bf = { AC_SRC_OVER, 0, m_nOpacity, AC_SRC_ALPHA };
                    POINT ptPos   = { rcWnd.left, rcWnd.top };
                    SIZE sizeWnd  = { dwWidth, dwHeight };
                    POINT ptSrc   = { 0, 0 };
                    g_fUpdateLayeredWindow(m_hWndPaint, m_hDcPaint, &ptPos, &sizeWnd, m_hDcOffscreen, &ptSrc, 0, &bf, ULW_ALPHA);
                }
                else 
                    ::BitBlt(m_hDcPaint, rcPaint.left, rcPaint.top, rcPaint.right - rcPaint.left,
                        rcPaint.bottom - rcPaint.top, m_hDcOffscreen, rcPaint.left, rcPaint.top, SRCCOPY);
                ::SelectObject(m_hDcOffscreen, hOldBitmap);

                if( m_bShowUpdateRect && !m_bLayered ) {
                    HPEN hOldPen = (HPEN)::SelectObject(m_hDcPaint, m_hUpdateRectPen);
                    ::SelectObject(m_hDcPaint, ::GetStockObject(HOLLOW_BRUSH));
                    ::Rectangle(m_hDcPaint, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
                    ::SelectObject(m_hDcPaint, hOldPen);
                }
            }
            else
            {
                // A standard paint job
                int iSaveDC = ::SaveDC(m_hDcPaint);
                m_pRoot->Paint(m_hDcPaint, rcPaint, NULL);
                ::RestoreDC(m_hDcPaint, iSaveDC);
            }
            // All Done!
            ::EndPaint(m_hWndPaint, &ps);

			SetPainting(false);
			m_bLayeredChanged = false;
			if( m_bUpdateNeeded ) Invalidate();
        }
        return true;
    case WM_PRINTCLIENT:
        {
            if( m_pRoot == NULL ) break;
            RECT rcClient;
            ::GetClientRect(m_hWndPaint, &rcClient);
            HDC hDC = (HDC) wParam;
            int save = ::SaveDC(hDC);
            m_pRoot->Paint(hDC, rcClient, NULL);
            // Check for traversing children. The crux is that WM_PRINT will assume
            // that the DC is positioned at frame coordinates and will paint the child
            // control at the wrong position. We'll simulate the entire thing instead.
            if( (lParam & PRF_CHILDREN) != 0 ) {
                HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
                while( hWndChild != NULL ) {
                    RECT rcPos = { 0 };
                    ::GetWindowRect(hWndChild, &rcPos);
                    ::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
                    ::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
                    // NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
                    //       since the latter will not print the nonclient correctly for
                    //       EDIT controls.
                    ::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
                    hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
                }
            }
            ::RestoreDC(hDC, save);
        }
        break;
    case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
            if( m_szMinWindow.cx > 0 ) lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
            if( m_szMinWindow.cy > 0 ) lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
            if( m_szMaxWindow.cx > 0 ) lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
            if( m_szMaxWindow.cy > 0 ) lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
        }
        break;
    case WM_SIZE:
        {
            if( m_pFocus != NULL ) {
                Event event/* = { UIEVENT__FIRST }*/;
                event.Type = UIEVENT_WINDOWSIZE;
                event.pSender = m_pFocus;
                event.wParam = wParam;
                event.lParam = lParam;
                event.dwTimestamp = ::GetTickCount();
                event.ptMouse = m_ptLastMousePos;
                event.wKeyState = MapKeyState();
				m_pFocus->HandleEvent(event);
            }
            if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
			if( m_bLayered ) Invalidate();
        }
        return true;
    case WM_TIMER:
        {
			if( LOWORD(wParam) == LAYEREDUPDATE_TIMERID ) {
				if( m_bLayered && !::IsRectEmpty(&m_rcLayeredUpdate) ) {
					LRESULT lRes = 0;
					if( !::IsIconic(m_hWndPaint) ) MessageHandler(WM_PAINT, 0, 0L, lRes);
					break;
				}
			}
            for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
                const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
                if( pTimer->hWnd == m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) && pTimer->bKilled == false) {
                    Event event/* = { UIEVENT__FIRST }*/;
                    event.Type = UIEVENT_TIMER;
                    event.pSender = pTimer->pSender;
                    event.dwTimestamp = ::GetTickCount();
                    event.ptMouse = m_ptLastMousePos;
                    event.wKeyState = MapKeyState();
                    event.wParam = pTimer->nLocalID;
                    event.lParam = lParam;
					pTimer->pSender->HandleEvent(event);
                    break;
                }
            }
        }
        break;
    case WM_MOUSEACTIVATE:
        {
            if (m_bNoActivate) {
                lRes = MA_NOACTIVATE;
                return true;
            }
        }
        break;
    case WM_MOUSEHOVER:
        {
			printf("UIManager::MessageHandler() WM_MOUSEHOVER m_bMouseTracking = false;\n");
            if (m_pRoot == NULL) break;
            m_bMouseTracking = false;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            Control* pHover = FindControl(pt);
            if (pHover == NULL) break;
            // Generate mouse hover event
            if (m_pEventHover != NULL) {
                Event event/* = { UIEVENT__FIRST }*/;
                event.Type = UIEVENT_MOUSEHOVER;
                event.pSender = m_pEventHover;
                event.wParam = wParam;
                event.lParam = lParam;
                event.dwTimestamp = ::GetTickCount();
                event.ptMouse = pt;
                event.wKeyState = MapKeyState();
				m_pEventHover->HandleEvent(event);
            }
            // Create tooltip information
            String sToolTip = pHover->GetToolTip();
            if (sToolTip.empty()) return true;
            ProcessMultiLanguageTokens(sToolTip);
            ::ZeroMemory(&m_ToolTipInfo, sizeof(TOOLINFO));
            m_ToolTipInfo.cbSize = sizeof(TOOLINFO);
            m_ToolTipInfo.uFlags = TTF_IDISHWND;
            m_ToolTipInfo.hwnd = m_hWndPaint;
            m_ToolTipInfo.uId = (UINT_PTR)m_hWndPaint;
            m_ToolTipInfo.hinst = m_hInstance;
			m_ToolTipInfo.lpszText = const_cast<LPTSTR>((LPCTSTR)sToolTip.c_str());
            m_ToolTipInfo.rect = pHover->GetPos();
            if (m_hWndTooltip == NULL) {
                m_hWndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
                ::SendMessage(m_hWndTooltip, TTM_ADDTOOL, 0, (LPARAM)&m_ToolTipInfo);
                ::SendMessage(m_hWndTooltip, TTM_SETMAXTIPWIDTH, 0, pHover->GetToolTipWidth());
                ::SendMessage(m_hWndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolTipInfo);
                ::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_ToolTipInfo);

            }
            // by jiangdong 2016-8-6 修改tooltip 悬停时候 闪烁bug
            if (m_pLastToolTip == NULL) {
                m_pLastToolTip = pHover;
            }
            else{
                if (m_pLastToolTip == pHover){
                    if (m_iLastTooltipWidth != pHover->GetToolTipWidth()){
                        ::SendMessage(m_hWndTooltip, TTM_SETMAXTIPWIDTH, 0, pHover->GetToolTipWidth());
                        m_iLastTooltipWidth = pHover->GetToolTipWidth();

                    }
                    ::SendMessage(m_hWndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolTipInfo);
                    ::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_ToolTipInfo);
                }
                else{
                    ::SendMessage(m_hWndTooltip, TTM_SETMAXTIPWIDTH, 0, pHover->GetToolTipWidth());
                    ::SendMessage(m_hWndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolTipInfo);
                    ::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_ToolTipInfo);
                }
            }
            //修改在ListElement 有提示 子项无提示下无法跟随移动！（按理说不应该移动的）
            ::SendMessage(m_hWndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)(DWORD)MAKELONG(pt.x, pt.y));
    }
        return true;
    case WM_MOUSELEAVE:
        {
			printf("UIManager::MessageHandler() WM_MOUSELEAVE\n");
            if( m_pRoot == NULL ) break;
            if( m_hWndTooltip != NULL ) ::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTipInfo);
            if( m_bMouseTracking ) {
                POINT pt = { 0 };
                RECT rcWnd = { 0 };
                ::GetCursorPos(&pt);
                ::GetWindowRect(m_hWndPaint, &rcWnd);
                if( !::IsIconic(m_hWndPaint) && ::GetActiveWindow() == m_hWndPaint && ::PtInRect(&rcWnd, pt) ) {
                    if( ::SendMessage(m_hWndPaint, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y)) == HTCLIENT ) {
                        ::ScreenToClient(m_hWndPaint, &pt);
                        ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
                    }
                    else 
                        ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)-1);
                }
                else 
                    ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)-1);
            }
            m_bMouseTracking = false;
			printf("UIManager::MessageHandler() WM_MOUSELEAVE m_bMouseTracking = false\n");
        }
        break;
    case WM_MOUSEMOVE:
        {
            if( m_pRoot == NULL ) break;
            // Start tracking this entire window again...
            if( !m_bMouseTracking ) {
                TRACKMOUSEEVENT tme = { 0 };
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_HOVER | TME_LEAVE;
                tme.hwndTrack = m_hWndPaint;
                tme.dwHoverTime = m_hWndTooltip == NULL ? m_iHoverTime : (DWORD) ::SendMessage(m_hWndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
                _TrackMouseEvent(&tme);
                m_bMouseTracking = true;
				printf("WM_MOUSEMOVE _TrackMouseEvent(&tme)\n");
            }
            // Generate the appropriate mouse messages
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            Control* pNewHover = FindControl(pt);
            if( pNewHover != NULL && pNewHover->GetManager() != this ) break;
            Event event = { /*UIEVENT__FIRST*/ };
            event.ptMouse = pt;
            event.wParam = wParam;
            event.lParam = lParam;
            event.dwTimestamp = ::GetTickCount();
            event.wKeyState = MapKeyState();
            if( !IsCaptured() ) {
                //pNewHover = FindControl(pt);
                if( pNewHover != NULL && pNewHover->GetManager() != this ) break;
                if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
                    event.Type = UIEVENT_MOUSELEAVE;
                    event.pSender = m_pEventHover;

                    PtrArray aNeedMouseLeaveNeeded(m_aNeedMouseLeaveNeeded.GetSize());
                    aNeedMouseLeaveNeeded.Resize(m_aNeedMouseLeaveNeeded.GetSize());
					::CopyMemory(aNeedMouseLeaveNeeded.GetData(), m_aNeedMouseLeaveNeeded.GetData(), m_aNeedMouseLeaveNeeded.GetSize() * sizeof(LPVOID));
                    for( int i = 0; i < aNeedMouseLeaveNeeded.GetSize(); i++ ) {
						static_cast<Control*>(aNeedMouseLeaveNeeded[i])->HandleEvent(event);
                    }

					m_pEventHover->HandleEvent(event);
                    m_pEventHover = NULL;
                    if( m_hWndTooltip != NULL ) ::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTipInfo);
                }
                if( pNewHover != m_pEventHover && pNewHover != NULL ) {
                    event.Type = UIEVENT_MOUSEENTER;
                    event.pSender = pNewHover;
					pNewHover->HandleEvent(event);
                    m_pEventHover = pNewHover;
                }
            }
            if( m_pEventClick != NULL ) {
                event.Type = UIEVENT_MOUSEMOVE;
                event.pSender = m_pEventClick;
				m_pEventClick->HandleEvent(event);
            }
            else if( pNewHover != NULL ) {
                event.Type = UIEVENT_MOUSEMOVE;
                event.pSender = pNewHover;
				pNewHover->HandleEvent(event);
            }
        }
        break;
    case WM_LBUTTONDOWN:
        {
            // We alway set focus back to our app (this helps
            // when Win32 child windows are placed on the dialog
            // and we need to remove them on focus change).
            if (!m_bNoActivate) ::SetFocus(m_hWndPaint);
            if( m_pRoot == NULL ) break;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            Control* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            m_pEventClick = pControl;
            pControl->SetFocus();
            SetCapture();
            Event event/* = { UIEVENT__FIRST }*/;
            event.Type = UIEVENT_BUTTONDOWN;
            event.pSender = pControl;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
			pControl->HandleEvent(event);
        }
        break;
    case WM_LBUTTONDBLCLK:
        {
            if (!m_bNoActivate) ::SetFocus(m_hWndPaint);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            Control* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            SetCapture();
            Event event/* = { UIEVENT__FIRST }*/;
            event.Type = UIEVENT_DBLCLICK;
            event.pSender = pControl;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
			pControl->HandleEvent(event);
            m_pEventClick = pControl;
        }
        break;
    case WM_LBUTTONUP:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            if( m_pEventClick == NULL ) break;
            ReleaseCapture();
            Event event/* = { UIEVENT__FIRST }*/;
            event.Type = UIEVENT_BUTTONUP;
            event.pSender = m_pEventClick;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
			// By daviyang35 at 2015-6-5 16:10:13
			// 在Click事件中弹出了模态对话框，退出阶段窗口实例可能已经删除
			// this成员属性赋值将会导致heap错误
			// this成员函数调用将会导致野指针异常
			// 使用栈上的成员来调用响应，提前清空成员
			// 当阻塞的模态窗口返回时，回栈阶段不访问任何类实例方法或属性
			// 将不会触发异常
			Control* pClick = m_pEventClick;
			m_pEventClick = NULL;
			pClick->HandleEvent(event);
        }
        break;
    case WM_RBUTTONDOWN:
        {
            if (!m_bNoActivate) ::SetFocus(m_hWndPaint);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            Control* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            pControl->SetFocus();
            Event event/* = { UIEVENT__FIRST }*/;
            event.Type = UIEVENT_RBUTTONDOWN;
            event.pSender = pControl;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
			pControl->HandleEvent(event);
            m_pEventClick = pControl;
        }
        break;
    case WM_CONTEXTMENU:
        {
            if( m_pRoot == NULL ) break;
            if( IsCaptured() ) break;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);
            m_ptLastMousePos = pt;
            if( m_pEventClick == NULL ) break;
            Event event/* = { UIEVENT__FIRST }*/;
            event.Type = UIEVENT_CONTEXTMENU;
            event.pSender = m_pEventClick;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.lParam = (LPARAM)m_pEventClick;
            event.dwTimestamp = ::GetTickCount();
			m_pEventClick->HandleEvent(event);
            m_pEventClick = NULL;
        }
        break;
    case WM_MOUSEWHEEL:
        {
            if( m_pRoot == NULL ) break;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);
            m_ptLastMousePos = pt;
            Control* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            int zDelta = (int) (short) HIWORD(wParam);
            Event event/* = { UIEVENT__FIRST }*/;
            event.Type = UIEVENT_SCROLLWHEEL;
            event.pSender = pControl;
            event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
            event.lParam = lParam;
			event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
			pControl->HandleEvent(event);

            // Let's make sure that the scroll item below the cursor is the same as before...
            ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
        }
        break;
    case WM_CHAR:
        {
            if( m_pRoot == NULL ) break;
            if( m_pFocus == NULL ) break;
            Event event/* = { UIEVENT__FIRST }*/;
            event.Type = UIEVENT_CHAR;
            event.pSender = m_pFocus;
            event.wParam = wParam;
            event.lParam = lParam;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
			m_pFocus->HandleEvent(event);
        }
        break;
    case WM_KEYDOWN:
        {
            if( m_pRoot == NULL ) break;
            if( m_pFocus == NULL ) break;
            Event event/* = { UIEVENT__FIRST }*/;
            event.Type = UIEVENT_KEYDOWN;
            event.pSender = m_pFocus;
            event.wParam = wParam;
            event.lParam = lParam;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
			m_pFocus->HandleEvent(event);
            m_pEventKey = m_pFocus;
        }
        break;
    case WM_KEYUP:
        {
            if( m_pRoot == NULL ) break;
            if( m_pEventKey == NULL ) break;
            Event event/* = { UIEVENT__FIRST }*/;
            event.Type = UIEVENT_KEYUP;
            event.pSender = m_pEventKey;
            event.wParam = wParam;
            event.lParam = lParam;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
			m_pEventKey->HandleEvent(event);
            m_pEventKey = NULL;
        }
        break;
    case WM_SETCURSOR:
        {
            if( m_pRoot == NULL ) break;
            if( LOWORD(lParam) != HTCLIENT ) break;
            if( m_bMouseCapture ) return true;

            POINT pt = { 0 };
            ::GetCursorPos(&pt);
            ::ScreenToClient(m_hWndPaint, &pt);
            Control* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
            Event event/* = { UIEVENT__FIRST }*/;
            event.Type = UIEVENT_SETCURSOR;
            event.pSender = pControl;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
			pControl->HandleEvent(event);
        }
        return true;
	case WM_KILLFOCUS:
		{
			if( wParam != NULL ) {
				HWND hWnd = ::GetFocus();
				HWND hParentWnd = NULL;
				while( hParentWnd = ::GetParent(hWnd) ) {
					if( m_hWndPaint == hParentWnd ) {
						for( int i = 0; i < m_aNativeWindow.GetSize(); i++ ) {
							if( static_cast<HWND>(m_aNativeWindow[i]) == hWnd ) {
								if( static_cast<Control*>(m_aNativeWindowControl[i]) != m_pFocus ) {
									SetFocus(static_cast<Control*>(m_aNativeWindowControl[i]), false);
								}
								break;
							}
						}
						break;
					}
					hWnd = hParentWnd;
				}
			}
		}
		break;
    case WM_NOTIFY:
        {
            LPNMHDR lpNMHDR = (LPNMHDR) lParam;
            if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
            return true;
        }
        break;
    case WM_COMMAND:
        {
            if( lParam == 0 ) break;
            HWND hWndChild = (HWND) lParam;
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            return true;
        }
        break;
    case WM_CTLCOLOREDIT:
	case WM_CTLCOLORSTATIC:
        {
			// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
			// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
            if( lParam == 0 ) break;
            HWND hWndChild = (HWND) lParam;
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

bool UIManager::IsUpdateNeeded() const
{
	return m_bUpdateNeeded;
}

void UIManager::NeedUpdate()
{
    m_bUpdateNeeded = true;
}

void UIManager::Invalidate()
{
	if( !m_bLayered ) ::InvalidateRect(m_hWndPaint, NULL, FALSE);
	else {
		RECT rcClient = { 0 };
		::GetClientRect(m_hWndPaint, &rcClient);
		::UnionRect(&m_rcLayeredUpdate, &m_rcLayeredUpdate, &rcClient);
	}
}

void UIManager::Invalidate(RECT& rcItem)
{
	if( rcItem.left < 0 ) rcItem.left = 0;
	if( rcItem .top < 0 ) rcItem.top = 0;
	if( rcItem.right < rcItem.left ) rcItem.right = rcItem.left;
	if( rcItem.bottom < rcItem.top ) rcItem.bottom = rcItem.top;
	if( !m_bLayered ) ::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
	else ::UnionRect(&m_rcLayeredUpdate, &m_rcLayeredUpdate, &rcItem);
}

bool UIManager::AttachDialog(Control* pControl)
{
    ASSERT(::IsWindow(m_hWndPaint));
    // Reset any previous attachment
    SetFocus(NULL);
    m_pEventKey = NULL;
    m_pEventHover = NULL;
    m_pEventClick = NULL;
    m_pLastToolTip = NULL;
    // Remove the existing control-tree. We might have gotten inside this function as
    // a result of an event fired or similar, so we cannot just delete the objects and
    // pull the internal memory of the calling code. We'll delay the cleanup.
    if( m_pRoot != NULL ) {
        for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) static_cast<Control*>(m_aDelayedCleanup[i])->Delete();
        m_aDelayedCleanup.Empty();
        for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) delete static_cast<Event*>(m_aAsyncNotify[i]);
        m_aAsyncNotify.Empty();
        m_mNameHash.Resize(0);
        m_aPostPaintControls.Empty();
		m_aNativeWindow.Empty();
        AddDelayedCleanup(m_pRoot);
    }
    // Set the dialog root element
    m_pRoot = pControl;
    // Go ahead...
    m_bUpdateNeeded = true;
    m_bFirstLayout = true;
    m_bFocusNeeded = true;
    // Initiate all control

	m_shadow.Create(this);

    return InitControls(pControl);
}

bool UIManager::InitControls(Control* pControl, Control* pParent /*= NULL*/)
{
    ASSERT(pControl);
    if( pControl == NULL ) return false;
    pControl->SetManager(this, pParent != NULL ? pParent : pControl->GetParent(), true);
    pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
    return true;
}

bool UIManager::RenameControl(Control* pControl, LPCTSTR pstrName)
{
	ASSERT(pControl);
	if( pControl == NULL || pControl->GetManager() != this || pstrName == NULL || *pstrName == _T('\0')) return false;
	if (pControl->GetName() == pstrName) return true;
	if (NULL != FindControl(pstrName)) return false;
	m_mNameHash.Remove(pControl->GetName().c_str());
	bool bResult = m_mNameHash.Insert(pstrName, pControl);
	if (bResult) pControl->SetName(pstrName);
	return bResult;
}

void UIManager::ReapObjects(Control* pControl)
{
    if( pControl == NULL ) return;
    if( pControl == m_pEventKey ) m_pEventKey = NULL;
    if( pControl == m_pEventHover ) m_pEventHover = NULL;
    if( pControl == m_pEventClick ) m_pEventClick = NULL;
    if( pControl == m_pFocus ) m_pFocus = NULL;
    KillTimer(pControl);
    const String& sName = pControl->GetName();
    if( !sName.empty() ) {
		if (pControl == FindControl(sName.c_str())) m_mNameHash.Remove(sName.c_str());
    }
    for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) {
        Event* pMsg = static_cast<Event*>(m_aAsyncNotify[i]);
        if( pMsg->pSender == pControl ) pMsg->pSender = NULL;
    }    
}

bool UIManager::AddOptionGroup(LPCTSTR pStrGroupName, Control* pControl)
{
    if (pControl == NULL || pStrGroupName == NULL) return false;

    LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
    if( lp ) {
        PtrArray* aOptionGroup = static_cast<PtrArray*>(lp);
        for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
            if( static_cast<Control*>(aOptionGroup->GetAt(i)) == pControl ) {
                return false;
            }
        }
        aOptionGroup->Add(pControl);
    }
    else {
        PtrArray* aOptionGroup = new PtrArray(6);
        aOptionGroup->Add(pControl);
        m_mOptionGroup.Insert(pStrGroupName, aOptionGroup);
    }
    return true;
}

PtrArray* UIManager::GetOptionGroup(LPCTSTR pStrGroupName)
{
    LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
    if( lp ) return static_cast<PtrArray*>(lp);
    return NULL;
}

void UIManager::RemoveOptionGroup(LPCTSTR pStrGroupName, Control* pControl)
{
	LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
	if( lp ) {
		PtrArray* aOptionGroup = static_cast<PtrArray*>(lp);
		if( aOptionGroup == NULL ) return;
		for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
			if( static_cast<Control*>(aOptionGroup->GetAt(i)) == pControl ) {
				aOptionGroup->Remove(i);
				break;
			}
		}
		if (aOptionGroup->IsEmpty()) {
			delete aOptionGroup;
			m_mOptionGroup.Remove(pStrGroupName);
		}
	}
}

void UIManager::RemoveAllOptionGroups()
{
	PtrArray* aOptionGroup;
	for( int i = 0; i< m_mOptionGroup.GetSize(); i++ ) {
		if(LPCTSTR key = m_mOptionGroup.GetAt(i)) {
			aOptionGroup = static_cast<PtrArray*>(m_mOptionGroup.Find(key));
			delete aOptionGroup;
		}
	}
	m_mOptionGroup.RemoveAll();
}

void UIManager::MessageLoop()
{
    MSG msg = { 0 };
    while( ::GetMessage(&msg, NULL, 0, 0) ) {
        if( !UIManager::TranslateMessage(&msg) ) {
            ::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			//try{
   //         ::DispatchMessage(&msg);
			//} catch(...) {
			//	DUITRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);
			//	#ifdef _DEBUG
			//	throw "UIManager::MessageLoop";
			//	#endif
			//}
        }
    }
}

void UIManager::Term()
{
    if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
        CloseZip((HZIP)m_hResourceZip);
        m_hResourceZip = NULL;
    }
}

Control* UIManager::GetFocus() const
{
    return m_pFocus;
}

void UIManager::SetFocus(Control* pControl, bool bFocusWnd)
{
    // Paint manager window has focus?
    HWND hFocusWnd = ::GetFocus();
    if( bFocusWnd && hFocusWnd != m_hWndPaint && pControl != m_pFocus && !m_bNoActivate) ::SetFocus(m_hWndPaint);
    // Already has focus?
    if( pControl == m_pFocus ) return;
    // Remove focus from old control
    if( m_pFocus != NULL ) 
    {
        Event event/* = { UIEVENT__FIRST }*/;
        event.Type = UIEVENT_KILLFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
		m_pFocus->HandleEvent(event);
		SendNotify(m_pFocus, UIEVENT_KILLFOCUS);
        m_pFocus = NULL;
    }
    if( pControl == NULL ) return;
    // Set focus to new control
    if( pControl != NULL 
        && pControl->GetManager() == this 
        && pControl->IsVisible() 
        && pControl->IsEnabled() ) 
    {
        m_pFocus = pControl;
        Event event/* = { UIEVENT__FIRST }*/;
        event.Type = UIEVENT_SETFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
		m_pFocus->HandleEvent(event);
		SendNotify(m_pFocus, UIEVENT_SETFOCUS);
    }
}

void UIManager::SetFocusNeeded(Control* pControl)
{
    if (!m_bNoActivate) ::SetFocus(m_hWndPaint);
    if( pControl == NULL ) return;
    if( m_pFocus != NULL ) {
        Event event/* = { UIEVENT__FIRST }*/;
        event.Type = UIEVENT_KILLFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
		m_pFocus->HandleEvent(event);
		SendNotify(m_pFocus, UIEVENT_KILLFOCUS);
        m_pFocus = NULL;
    }
    FINDTABINFO info = { 0 };
    info.pFocus = pControl;
    info.bForward = false;
    m_pFocus = m_pRoot->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
    m_bFocusNeeded = true;
    if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
}

bool UIManager::SetTimer(Control* pControl, UINT nTimerID, UINT uElapse)
{
    ASSERT(pControl!=NULL);
    ASSERT(uElapse>0);
    for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
        if( pTimer->pSender == pControl
            && pTimer->hWnd == m_hWndPaint
            && pTimer->nLocalID == nTimerID ) {
            if( pTimer->bKilled == true ) {
                if( ::SetTimer(m_hWndPaint, pTimer->uWinTimer, uElapse, NULL) ) {
                    pTimer->bKilled = false;
                    return true;
                }
                return false;
            }
            return false;
        }
    }

    m_uTimerID = (++m_uTimerID) % 0xFF;
    if( !::SetTimer(m_hWndPaint, m_uTimerID, uElapse, NULL) ) return FALSE;
    TIMERINFO* pTimer = new TIMERINFO;
    if( pTimer == NULL ) return FALSE;
    pTimer->hWnd = m_hWndPaint;
    pTimer->pSender = pControl;
    pTimer->nLocalID = nTimerID;
    pTimer->uWinTimer = m_uTimerID;
    pTimer->bKilled = false;
    return m_aTimers.Add(pTimer);
}

bool UIManager::KillTimer(Control* pControl, UINT nTimerID)
{
    ASSERT(pControl!=NULL);
    for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
        if( pTimer->pSender == pControl
            && pTimer->hWnd == m_hWndPaint
            && pTimer->nLocalID == nTimerID )
        {
            if( pTimer->bKilled == false ) {
                if( ::IsWindow(m_hWndPaint) ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
                pTimer->bKilled = true;
                return true;
            }
        }
    }
    return false;
}

void UIManager::KillTimer(Control* pControl)
{
    ASSERT(pControl!=NULL);
    int count = m_aTimers.GetSize();
    for( int i = 0, j = 0; i < count; i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i - j]);
        if( pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint ) {
            if( pTimer->bKilled == false ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
            delete pTimer;
            m_aTimers.Remove(i - j);
            j++;
        }
    }
}

void UIManager::RemoveAllTimers()
{
    for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
        if( pTimer->hWnd == m_hWndPaint ) {
            if( pTimer->bKilled == false ) {
                if( ::IsWindow(m_hWndPaint) ) ::KillTimer(m_hWndPaint, pTimer->uWinTimer);
            }
            delete pTimer;
        }
    }

    m_aTimers.Empty();
}

void UIManager::SetCapture()
{
    ::SetCapture(m_hWndPaint);
    m_bMouseCapture = true;
}

void UIManager::ReleaseCapture()
{
    ::ReleaseCapture();
    m_bMouseCapture = false;
}

bool UIManager::IsCaptured()
{
    return m_bMouseCapture;
}


bool UIManager::IsPainting()
{
	return m_bIsPainting;
}

void UIManager::SetPainting(bool bIsPainting)
{
	m_bIsPainting = bIsPainting;
}

bool UIManager::SetNextTabControl(bool bForward)
{
    // If we're in the process of restructuring the layout we can delay the
    // focus calulation until the next repaint.
    if( m_bUpdateNeeded && bForward ) {
        m_bFocusNeeded = true;
        ::InvalidateRect(m_hWndPaint, NULL, FALSE);
        return true;
    }
    // Find next/previous tabbable control
    FINDTABINFO info1 = { 0 };
    info1.pFocus = m_pFocus;
    info1.bForward = bForward;
    Control* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
    if( pControl == NULL ) {  
        if( bForward ) {
            // Wrap around
            FINDTABINFO info2 = { 0 };
            info2.pFocus = bForward ? NULL : info1.pLast;
            info2.bForward = bForward;
            pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
        }
        else {
            pControl = info1.pLast;
        }
    }
    if( pControl != NULL ) SetFocus(pControl);
    m_bFocusNeeded = false;
    return true;
}

bool UIManager::AddNotifier(INotify* pNotifier)
{
    if (pNotifier == NULL) return false;

    ASSERT(m_aNotifiers.Find(pNotifier)<0);
    return m_aNotifiers.Add(pNotifier);
}

bool UIManager::RemoveNotifier(INotify* pNotifier)
{
    for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
        if( static_cast<INotify*>(m_aNotifiers[i]) == pNotifier ) {
            return m_aNotifiers.Remove(i);
        }
    }
    return false;
}

bool UIManager::AddPreMessageFilter(IMessageFilter* pFilter)
{
    if (pFilter == NULL) return false;

    ASSERT(m_aPreMessageFilters.Find(pFilter)<0);
    return m_aPreMessageFilters.Add(pFilter);
}

bool UIManager::RemovePreMessageFilter(IMessageFilter* pFilter)
{
    for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) {
        if( static_cast<IMessageFilter*>(m_aPreMessageFilters[i]) == pFilter ) {
            return m_aPreMessageFilters.Remove(i);
        }
    }
    return false;
}

bool UIManager::AddMessageFilter(IMessageFilter* pFilter)
{
    if (pFilter == NULL) return false;

    ASSERT(m_aMessageFilters.Find(pFilter)<0);
    return m_aMessageFilters.Add(pFilter);
}

bool UIManager::RemoveMessageFilter(IMessageFilter* pFilter)
{
    for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
        if( static_cast<IMessageFilter*>(m_aMessageFilters[i]) == pFilter ) {
            return m_aMessageFilters.Remove(i);
        }
    }
    return false;
}

int UIManager::GetPostPaintCount() const
{
    return m_aPostPaintControls.GetSize();
}

bool UIManager::AddPostPaint(Control* pControl)
{
    if (pControl == NULL) return false;

    ASSERT(m_aPostPaintControls.Find(pControl) < 0);
    return m_aPostPaintControls.Add(pControl);
}

bool UIManager::RemovePostPaint(Control* pControl)
{
    for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
        if( static_cast<Control*>(m_aPostPaintControls[i]) == pControl ) {
            return m_aPostPaintControls.Remove(i);
        }
    }
    return false;
}

bool UIManager::SetPostPaintIndex(Control* pControl, int iIndex)
{
    if (pControl == NULL) return false;

    RemovePostPaint(pControl);
    return m_aPostPaintControls.InsertAt(iIndex, pControl);
}

int UIManager::GetNativeWindowCount() const
{
	return m_aNativeWindow.GetSize();
}

RECT UIManager::GetNativeWindowRect(HWND hChildWnd)
{
	RECT rcChildWnd;
	::GetWindowRect(hChildWnd, &rcChildWnd);
	::ScreenToClient(m_hWndPaint, (LPPOINT)(&rcChildWnd));
	::ScreenToClient(m_hWndPaint, (LPPOINT)(&rcChildWnd)+1);
	return rcChildWnd;
}

bool UIManager::AddNativeWindow(Control* pControl, HWND hChildWnd)
{
    if (pControl == NULL || hChildWnd == NULL) return false;

	RECT rcChildWnd = GetNativeWindowRect(hChildWnd);
	Invalidate(rcChildWnd);

	if (m_aNativeWindow.Find(hChildWnd) >= 0) return false;
	if (m_aNativeWindow.Add(hChildWnd)) {
		m_aNativeWindowControl.Add(pControl);
		return true;
	}
	return false;
}

bool UIManager::RemoveNativeWindow(HWND hChildWnd)
{
	for( int i = 0; i < m_aNativeWindow.GetSize(); i++ ) {
		if( static_cast<HWND>(m_aNativeWindow[i]) == hChildWnd ) {
			if( m_aNativeWindow.Remove(i) ) {
				m_aNativeWindowControl.Remove(i);
				return true;
			}
			return false;
		}
	}
	return false;
}

void UIManager::AddDelayedCleanup(Control* pControl)
{
    if (pControl == NULL) return;
    pControl->SetManager(this, NULL, false);
    m_aDelayedCleanup.Add(pControl);
	PostAsyncNotify();
}

void UIManager::AddMouseLeaveNeeded(Control* pControl)
{
    if (pControl == NULL) return;
    for( int i = 0; i < m_aNeedMouseLeaveNeeded.GetSize(); i++ ) {
        if( static_cast<Control*>(m_aNeedMouseLeaveNeeded[i]) == pControl ) {
            return;
        }
    }
    m_aNeedMouseLeaveNeeded.Add(pControl);
}

bool UIManager::RemoveMouseLeaveNeeded(Control* pControl)
{
    if (pControl == NULL) return false;
    for( int i = 0; i < m_aNeedMouseLeaveNeeded.GetSize(); i++ ) {
        if( static_cast<Control*>(m_aNeedMouseLeaveNeeded[i]) == pControl ) {
            return m_aNeedMouseLeaveNeeded.Remove(i);
        }
    }
    return false;
}

void UIManager::SendNotify(Control* pControl, EVENTTYPE_UI type, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/, bool bEnableRepeat /*= true*/)
{
	Event Msg;
	Msg.pSender = pControl;
	Msg.Type = type;
	Msg.wParam = wParam;
	Msg.lParam = lParam;
	SendNotify(Msg, bAsync, bEnableRepeat);
}

void UIManager::SendNotify(Event& Msg, bool bAsync /*= false*/, bool bEnableRepeat /*= true*/)
{
    Msg.ptMouse = m_ptLastMousePos;
    Msg.dwTimestamp = ::GetTickCount();

    if( !bAsync ) {
        // Send to all listeners
        if( Msg.pSender != NULL ) {
			if (Msg.pSender->m_cbNotify) Msg.pSender->m_cbNotify(&Msg);
        }
        for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
            static_cast<INotify*>(m_aNotifiers[i])->Notify(Msg);
        }
    }
    else {
		if( !bEnableRepeat ) {
			for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) {
				Event* pMsg = static_cast<Event*>(m_aAsyncNotify[i]);
				if( pMsg->pSender == Msg.pSender && pMsg->Type == Msg.Type) {
					pMsg->wParam = Msg.wParam;
					pMsg->lParam = Msg.lParam;
					pMsg->ptMouse = Msg.ptMouse;
					pMsg->dwTimestamp = Msg.dwTimestamp;
					return;
				}
			}
		}

		Event *pMsg = new Event;
		pMsg->pSender = Msg.pSender;
		pMsg->Type = Msg.Type;
		pMsg->wParam = Msg.wParam;
		pMsg->lParam = Msg.lParam;
		pMsg->ptMouse = Msg.ptMouse;
		pMsg->dwTimestamp = Msg.dwTimestamp;
		m_aAsyncNotify.Add(pMsg);

		PostAsyncNotify();
    }
}

bool UIManager::IsForceUseSharedRes() const
{
	return m_bForceUseSharedRes;
}

void UIManager::SetForceUseSharedRes(bool bForce)
{
	m_bForceUseSharedRes = bForce;
}

DWORD UIManager::GetDefaultDisabledColor() const
{
    return m_ResInfo.m_dwDefaultDisabledColor;
}

void UIManager::SetDefaultDisabledColor(DWORD dwColor, bool bShared)
{
	if (bShared)
	{
		if (m_ResInfo.m_dwDefaultDisabledColor == m_SharedResInfo.m_dwDefaultDisabledColor)
			m_ResInfo.m_dwDefaultDisabledColor = dwColor;
		m_SharedResInfo.m_dwDefaultDisabledColor = dwColor;
	}
	else
	{
		m_ResInfo.m_dwDefaultDisabledColor = dwColor;
	}
}

DWORD UIManager::GetDefaultFontColor() const
{
	return m_ResInfo.m_dwDefaultFontColor;
}

void UIManager::SetDefaultFontColor(DWORD dwColor, bool bShared)
{
	if (bShared)
	{
		if (m_ResInfo.m_dwDefaultFontColor == m_SharedResInfo.m_dwDefaultFontColor)
			m_ResInfo.m_dwDefaultFontColor = dwColor;
		m_SharedResInfo.m_dwDefaultFontColor = dwColor;
	}
	else
	{
		m_ResInfo.m_dwDefaultFontColor = dwColor;
	}
}

DWORD UIManager::GetDefaultLinkFontColor() const
{
    return m_ResInfo.m_dwDefaultLinkFontColor;
}

void UIManager::SetDefaultLinkFontColor(DWORD dwColor, bool bShared)
{
	if (bShared)
	{
		if (m_ResInfo.m_dwDefaultLinkFontColor == m_SharedResInfo.m_dwDefaultLinkFontColor)
			m_ResInfo.m_dwDefaultLinkFontColor = dwColor;
		m_SharedResInfo.m_dwDefaultLinkFontColor = dwColor;
	}
	else
	{
		m_ResInfo.m_dwDefaultLinkFontColor = dwColor;
	}
}

DWORD UIManager::GetDefaultLinkHoverFontColor() const
{
    return m_ResInfo.m_dwDefaultLinkHoverFontColor;
}

void UIManager::SetDefaultLinkHoverFontColor(DWORD dwColor, bool bShared)
{
	if (bShared)
	{
		if (m_ResInfo.m_dwDefaultLinkHoverFontColor == m_SharedResInfo.m_dwDefaultLinkHoverFontColor)
			m_ResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
		m_SharedResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
	}
	else
	{
		m_ResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
	}
}

DWORD UIManager::GetDefaultSelectedBkColor() const
{
    return m_ResInfo.m_dwDefaultSelectedBkColor;
}

void UIManager::SetDefaultSelectedBkColor(DWORD dwColor, bool bShared)
{
	if (bShared)
	{
		if (m_ResInfo.m_dwDefaultSelectedBkColor == m_SharedResInfo.m_dwDefaultSelectedBkColor)
			m_ResInfo.m_dwDefaultSelectedBkColor = dwColor;
		m_SharedResInfo.m_dwDefaultSelectedBkColor = dwColor;
	}
	else
	{
		m_ResInfo.m_dwDefaultSelectedBkColor = dwColor;
	}
}

FontInfo* UIManager::GetDefaultFontInfo()
{
	if (m_ResInfo.m_DefaultFontInfo.sFontName.empty())
	{
		if( m_SharedResInfo.m_DefaultFontInfo.tm.tmHeight == 0 ) 
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, m_SharedResInfo.m_DefaultFontInfo.hFont);
			::GetTextMetrics(m_hDcPaint, &m_SharedResInfo.m_DefaultFontInfo.tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		return &m_SharedResInfo.m_DefaultFontInfo;
	}
	else
	{
		if( m_ResInfo.m_DefaultFontInfo.tm.tmHeight == 0 ) 
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, m_ResInfo.m_DefaultFontInfo.hFont);
			::GetTextMetrics(m_hDcPaint, &m_ResInfo.m_DefaultFontInfo.tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		return &m_ResInfo.m_DefaultFontInfo;
	}
}

void UIManager::SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared)
{
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    _tcsncpy(lf.lfFaceName, pStrFontName, LF_FACESIZE);
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = -nSize;
    if( bBold ) lf.lfWeight += FW_BOLD;
    if( bUnderline ) lf.lfUnderline = TRUE;
    if( bItalic ) lf.lfItalic = TRUE;
    HFONT hFont = ::CreateFontIndirect(&lf);
    if( hFont == NULL ) return;

	if (bShared)
	{
		::DeleteObject(m_SharedResInfo.m_DefaultFontInfo.hFont);
		m_SharedResInfo.m_DefaultFontInfo.hFont = hFont;
		m_SharedResInfo.m_DefaultFontInfo.sFontName = pStrFontName;
		m_SharedResInfo.m_DefaultFontInfo.iSize = nSize;
		m_SharedResInfo.m_DefaultFontInfo.bBold = bBold;
		m_SharedResInfo.m_DefaultFontInfo.bUnderline = bUnderline;
		m_SharedResInfo.m_DefaultFontInfo.bItalic = bItalic;
		::ZeroMemory(&m_SharedResInfo.m_DefaultFontInfo.tm, sizeof(m_SharedResInfo.m_DefaultFontInfo.tm));
		if( m_hDcPaint ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
			::GetTextMetrics(m_hDcPaint, &m_SharedResInfo.m_DefaultFontInfo.tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
	}
	else
	{
		::DeleteObject(m_ResInfo.m_DefaultFontInfo.hFont);
		m_ResInfo.m_DefaultFontInfo.hFont = hFont;
		m_ResInfo.m_DefaultFontInfo.sFontName = pStrFontName;
		m_ResInfo.m_DefaultFontInfo.iSize = nSize;
		m_ResInfo.m_DefaultFontInfo.bBold = bBold;
		m_ResInfo.m_DefaultFontInfo.bUnderline = bUnderline;
		m_ResInfo.m_DefaultFontInfo.bItalic = bItalic;
		::ZeroMemory(&m_ResInfo.m_DefaultFontInfo.tm, sizeof(m_ResInfo.m_DefaultFontInfo.tm));
		if( m_hDcPaint ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
			::GetTextMetrics(m_hDcPaint, &m_ResInfo.m_DefaultFontInfo.tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
	}
}

DWORD UIManager::GetCustomFontCount(bool bShared) const
{
	if (bShared)
		return m_SharedResInfo.m_CustomFonts.GetSize();
	else
		return m_ResInfo.m_CustomFonts.GetSize();
}

HFONT UIManager::AddFont(int id, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared)
{
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    _tcsncpy(lf.lfFaceName, pStrFontName, LF_FACESIZE);
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = -nSize;
    if( bBold ) lf.lfWeight += FW_BOLD;
    if( bUnderline ) lf.lfUnderline = TRUE;
    if( bItalic ) lf.lfItalic = TRUE;
    HFONT hFont = ::CreateFontIndirect(&lf);
    if( hFont == NULL ) return NULL;

    FontInfo* pFontInfo = new FontInfo;
    if( !pFontInfo ) return false;
    ::ZeroMemory(pFontInfo, sizeof(FontInfo));
    pFontInfo->hFont = hFont;
    pFontInfo->sFontName = pStrFontName;
    pFontInfo->iSize = nSize;
    pFontInfo->bBold = bBold;
    pFontInfo->bUnderline = bUnderline;
    pFontInfo->bItalic = bItalic;
    if( m_hDcPaint ) {
        HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);
	if (bShared || m_bForceUseSharedRes)
	{
		FontInfo* pOldFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
		if (pOldFontInfo)
		{
			::DeleteObject(pOldFontInfo->hFont);
			delete pOldFontInfo;
            m_SharedResInfo.m_CustomFonts.Remove(idBuffer);
		}

		if( !m_SharedResInfo.m_CustomFonts.Insert(idBuffer, pFontInfo) ) 
		{
			::DeleteObject(hFont);
			delete pFontInfo;
			return NULL;
		}
	}
	else
	{
		FontInfo* pOldFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
		if (pOldFontInfo)
		{
			::DeleteObject(pOldFontInfo->hFont);
			delete pOldFontInfo;
            m_ResInfo.m_CustomFonts.Remove(idBuffer);
		}

		if( !m_ResInfo.m_CustomFonts.Insert(idBuffer, pFontInfo) ) 
		{
			::DeleteObject(hFont);
			delete pFontInfo;
			return NULL;
		}
	}

    return hFont;
}

HFONT UIManager::AddSharedFont(int id, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcsncpy(lf.lfFaceName, pStrFontName, LF_FACESIZE);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if (bBold) lf.lfWeight += FW_BOLD;
	if (bUnderline) lf.lfUnderline = TRUE;
	if (bItalic) lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);
	if (hFont == NULL) return NULL;

	FontInfo* pFontInfo = new FontInfo;
	if (!pFontInfo) return false;
	::ZeroMemory(pFontInfo, sizeof(FontInfo));
	pFontInfo->hFont = hFont;
	pFontInfo->sFontName = pStrFontName;
	pFontInfo->iSize = nSize;
	pFontInfo->bBold = bBold;
	pFontInfo->bUnderline = bUnderline;
	pFontInfo->bItalic = bItalic;
	if (HDC hdc = ::GetDC(NULL)) {
		HFONT hOldFont = (HFONT) ::SelectObject(hdc, hFont);
		::GetTextMetrics(hdc, &pFontInfo->tm);
		::SelectObject(hdc, hOldFont);
		::ReleaseDC(NULL, hdc);
	}
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);
	//if (bShared || m_bForceUseSharedRes)
	{
		FontInfo* pOldFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
		if (pOldFontInfo)
		{
			::DeleteObject(pOldFontInfo->hFont);
			delete pOldFontInfo;
			m_SharedResInfo.m_CustomFonts.Remove(idBuffer);
		}

		if (!m_SharedResInfo.m_CustomFonts.Insert(idBuffer, pFontInfo))
		{
			::DeleteObject(hFont);
			delete pFontInfo;
			return NULL;
		}
	}

	return hFont;
}

HFONT UIManager::GetFont(int id)
{
	if (id < 0) return GetDefaultFontInfo()->hFont;
		
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);
	FontInfo* pFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
	if( !pFontInfo ) pFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
	if (!pFontInfo) return GetDefaultFontInfo()->hFont;
	return pFontInfo->hFont;
}

HFONT UIManager::GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
    FontInfo* pFontInfo = NULL;
	for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) {
		if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
			pFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(key));
			if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return pFontInfo->hFont;
		}
	}
	for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) {
		if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
			pFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key));
			if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return pFontInfo->hFont;
		}
	}

	return NULL;
}

int UIManager::GetFontIndex(HFONT hFont, bool bShared)
{
	FontInfo* pFontInfo = NULL;
	if (bShared)
	{
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->hFont == hFont) return _ttoi(key);
			}
		}
	}
	else
	{
		for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->hFont == hFont) return _ttoi(key);
			}
		}
	}

	return -1;
}

int UIManager::GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared)
{
	FontInfo* pFontInfo = NULL;
	if (bShared)
	{
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
					pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
					return _ttoi(key);
			}
		}
	}
	else
	{
		for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
					pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
					return _ttoi(key);
			}
		}
	}

	return -1;
}

void UIManager::RemoveFont(HFONT hFont, bool bShared)
{
    FontInfo* pFontInfo = NULL;
	if (bShared)
	{
		for( int i = 0; i < m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) 
			{
				pFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->hFont == hFont) 
				{
					::DeleteObject(pFontInfo->hFont);
					delete pFontInfo;
					m_SharedResInfo.m_CustomFonts.Remove(key);
					return;
				}
			}
		}
	}
	else
	{
		for( int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) 
			{
				pFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->hFont == hFont) 
				{
					::DeleteObject(pFontInfo->hFont);
					delete pFontInfo;
					m_ResInfo.m_CustomFonts.Remove(key);
					return;
				}
			}
		}
	}
}

void UIManager::RemoveFont(int id, bool bShared)
{
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);

	FontInfo* pFontInfo = NULL;
	if (bShared)
	{
		pFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
		if (pFontInfo)
		{
			::DeleteObject(pFontInfo->hFont);
			delete pFontInfo;
			m_SharedResInfo.m_CustomFonts.Remove(idBuffer);
		}
	}
	else
	{
		pFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
		if (pFontInfo)
		{
			::DeleteObject(pFontInfo->hFont);
			delete pFontInfo;
			m_ResInfo.m_CustomFonts.Remove(idBuffer);
		}
	}
}

void UIManager::RemoveAllFonts(bool bShared)
{
	FontInfo* pFontInfo;
	if (bShared)
	{
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key, false));
				if (pFontInfo) {
					::DeleteObject(pFontInfo->hFont);
					delete pFontInfo;
				}
			}
		}
		m_SharedResInfo.m_CustomFonts.RemoveAll();
	}
	else
	{
		for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(key, false));
				if (pFontInfo) {
					::DeleteObject(pFontInfo->hFont);
					delete pFontInfo;
				}
			}
		}
		m_ResInfo.m_CustomFonts.RemoveAll();
	}
}

FontInfo* UIManager::GetFontInfo(int id)
{
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);
	FontInfo* pFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
	if (!pFontInfo) pFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
	if (!pFontInfo) pFontInfo = GetDefaultFontInfo();
    if (pFontInfo->tm.tmHeight == 0) 
	{
        HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
    return pFontInfo;
}

FontInfo* UIManager::GetFontInfo(HFONT hFont)
{
	FontInfo* pFontInfo = NULL;
	for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) 
	{
		if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) 
		{
			pFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(key));
			if (pFontInfo && pFontInfo->hFont == hFont) break;
		}
	}
	if (!pFontInfo)
	{
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) 
			{
				pFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->hFont == hFont) break;
			}
		}
	}
	if (!pFontInfo) pFontInfo = GetDefaultFontInfo();
	if( pFontInfo->tm.tmHeight == 0 ) {
		HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
		::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
		::SelectObject(m_hDcPaint, hOldFont);
	}
	return pFontInfo;
}
//--------------------------Color---------------------------
void UIManager::AddColor(LPCTSTR pStrFontName, DWORD dwValue)
{
	LPVOID pData = static_cast<LPVOID>(m_ResInfo.m_ColorHash.Find(pStrFontName));
	if (pData)
	{
		m_ResInfo.m_CustomFonts.Remove(pStrFontName);
	}

	if (!m_ResInfo.m_CustomFonts.Insert(pStrFontName, (LPVOID)dwValue))
	{
		return ;
	}
}

DWORD UIManager::GetColor(LPCTSTR pStrFontName)
{
	if (!pStrFontName || !_tcscmp(pStrFontName, _T("")))
		return 0;

	LPVOID pData = static_cast<LPVOID>(m_ResInfo.m_ColorHash.Find(pStrFontName));
	if (!pData) pData = static_cast<LPVOID>(m_SharedResInfo.m_ColorHash.Find(pStrFontName));
	if (!pData)
	{
#ifdef _DEBUG
		if (pStrFontName[0] != '#')
			_tprintf(_T("UIManager::GetColor(%s) empty\n"), pStrFontName);
#endif	
		return 0;
	}
	return (DWORD)pData;
}

void UIManager::RemoveColor(LPCTSTR pStrFontName, bool bShared /*= false*/)
{
}

void UIManager::RemoveAllColors(bool bShared/* = false*/)
{
}

/*static*/ 
void UIManager::AddSharedColor(LPCTSTR pStrFontName, DWORD dwValue)
{
	LPVOID pData = static_cast<LPVOID>(m_SharedResInfo.m_ColorHash.Find(pStrFontName));
	if (pData)
	{
		m_SharedResInfo.m_ColorHash.Remove(pStrFontName);
	}

	if (!m_SharedResInfo.m_ColorHash.Insert(pStrFontName, (LPVOID)dwValue))
	{
		return;
	}
}
//--------------------------Image---------------------------
const ImageInfo* UIManager::GetImage(LPCTSTR bitmap)
{
    ImageInfo* data = static_cast<ImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
    if( !data ) data = static_cast<ImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
    return data;
}

const ImageInfo* UIManager::GetImageEx(LPCTSTR bitmap, LPCTSTR type, DWORD mask, bool bUseHSL)
{
    const ImageInfo* data = GetImage(bitmap);
    if( !data ) {
        if( AddImage(bitmap, type, mask, bUseHSL, false) ) {
			if (m_bForceUseSharedRes) data = static_cast<ImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
			else data = static_cast<ImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap)); 
        }
    }

    return data;
}

const ImageInfo* UIManager::AddImage(LPCTSTR bitmap, LPCTSTR type, DWORD mask, bool bUseHSL, bool bShared)
{
	if( bitmap == NULL || bitmap[0] == _T('\0') ) return NULL;

    ImageInfo* data = NULL;
    if( type != NULL ) {
        if( isdigit(*bitmap) ) {
            LPTSTR pstr = NULL;
            int iIndex = _tcstol(bitmap, &pstr, 10);
            data = Render::LoadImage(iIndex, this, type, mask);
        }
        else {
			data = Render::LoadImage(bitmap, this, type, mask);
        }
    }
    else {
		data = Render::LoadImage(bitmap, this, NULL, mask);
    }

	if( data == NULL ) return NULL;
	data->bUseHSL = bUseHSL;
	if( type != NULL ) data->sResType = type;
	data->dwMask = mask;
	if( data->bUseHSL ) {
		data->pSrcBits = new BYTE[data->nX * data->nY * 4];
		::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
	}
	else data->pSrcBits = NULL;
	if( m_bUseHSL ) Render::AdjustImage(true, data, m_H, m_S, m_L);
	if (data)
	{
		if (bShared || m_bForceUseSharedRes)
		{
            ImageInfo* pOldImageInfo = static_cast<ImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
            if (pOldImageInfo)
            {
                Render::FreeImage(pOldImageInfo);
                m_SharedResInfo.m_ImageHash.Remove(bitmap);
            }

			if( !m_SharedResInfo.m_ImageHash.Insert(bitmap, data) ) {
				Render::FreeImage(data);
				data = NULL;
			}
		}
		else
		{
            ImageInfo* pOldImageInfo = static_cast<ImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
            if (pOldImageInfo)
            {
                Render::FreeImage(pOldImageInfo);
                m_ResInfo.m_ImageHash.Remove(bitmap);
            }

			if( !m_ResInfo.m_ImageHash.Insert(bitmap, data) ) {
				Render::FreeImage(data);
				data = NULL;
			}
		}
	}

    return data;
}

const ImageInfo* UIManager::AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha, bool bShared)
{
	// 因无法确定外部HBITMAP格式，不能使用hsl调整
	if( bitmap == NULL || bitmap[0] == _T('\0') ) return NULL;
    if( hBitmap == NULL || iWidth <= 0 || iHeight <= 0 ) return NULL;

	ImageInfo* data = new ImageInfo;
	data->hBitmap = hBitmap;
	data->pBits = NULL;
	data->nX = iWidth;
	data->nY = iHeight;
	data->bAlpha = bAlpha;
	data->bUseHSL = false;
	data->pSrcBits = NULL;
	//data->sResType = _T("");
	data->dwMask = 0;

	if (bShared || m_bForceUseSharedRes)
	{
		if( !m_SharedResInfo.m_ImageHash.Insert(bitmap, data) ) {
			Render::FreeImage(data);
			data = NULL;
		}
	}
	else
	{
		if( !m_SharedResInfo.m_ImageHash.Insert(bitmap, data) ) {
			Render::FreeImage(data);
			data = NULL;
		}
	}

    return data;
}

void UIManager::RemoveImage(LPCTSTR bitmap, bool bShared)
{
	ImageInfo* data = NULL;
	if (bShared) 
	{
		data = static_cast<ImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
		if (data)
		{
			Render::FreeImage(data) ;
			m_SharedResInfo.m_ImageHash.Remove(bitmap);
		}
	}
	else
	{
		data = static_cast<ImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
		if (data)
		{
			Render::FreeImage(data) ;
			m_ResInfo.m_ImageHash.Remove(bitmap);
		}
	}
}

void UIManager::RemoveAllImages(bool bShared)
{
	if (bShared)
	{
		ImageInfo* data;
		for( int i = 0; i< m_SharedResInfo.m_ImageHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_SharedResInfo.m_ImageHash.GetAt(i)) {
				data = static_cast<ImageInfo*>(m_SharedResInfo.m_ImageHash.Find(key, false));
				if (data) {
					Render::FreeImage(data);
				}
			}
		}
		m_SharedResInfo.m_ImageHash.RemoveAll();
	}
	else
	{
		ImageInfo* data;
		for( int i = 0; i< m_ResInfo.m_ImageHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_ResInfo.m_ImageHash.GetAt(i)) {
				data = static_cast<ImageInfo*>(m_ResInfo.m_ImageHash.Find(key, false));
				if (data) {
					Render::FreeImage(data);
				}
			}
		}
		m_ResInfo.m_ImageHash.RemoveAll();
	}
}

void UIManager::AdjustSharedImagesHSL()
{
	ImageInfo* data;
	for( int i = 0; i< m_SharedResInfo.m_ImageHash.GetSize(); i++ ) {
		if(LPCTSTR key = m_SharedResInfo.m_ImageHash.GetAt(i)) {
			data = static_cast<ImageInfo*>(m_SharedResInfo.m_ImageHash.Find(key));
			if( data && data->bUseHSL ) {
				Render::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
			}
		}
	}
}

void UIManager::AdjustImagesHSL()
{
	ImageInfo* data;
	for( int i = 0; i< m_ResInfo.m_ImageHash.GetSize(); i++ ) {
		if(LPCTSTR key = m_ResInfo.m_ImageHash.GetAt(i)) {
			data = static_cast<ImageInfo*>(m_ResInfo.m_ImageHash.Find(key));
			if( data && data->bUseHSL ) {
				Render::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
			}
		}
	}
	Invalidate();
}

void UIManager::PostAsyncNotify()
{
	if (!m_bAsyncNotifyPosted) {
		::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
		m_bAsyncNotifyPosted = true;
	}
}

void UIManager::ReloadSharedImages()
{
	ImageInfo* data;
	ImageInfo* pNewData;
	for( int i = 0; i< m_SharedResInfo.m_ImageHash.GetSize(); i++ ) {
		if(LPCTSTR bitmap = m_SharedResInfo.m_ImageHash.GetAt(i)) {
			data = static_cast<ImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
			if( data != NULL ) {
				if( !data->sResType.empty() ) {
					if( isdigit(*bitmap) ) {
						LPTSTR pstr = NULL;
						int iIndex = _tcstol(bitmap, &pstr, 10);
						pNewData = Render::LoadImage(iIndex, NULL, data->sResType.c_str(), data->dwMask);
					}
					else {
						pNewData = Render::LoadImage(bitmap, NULL, data->sResType.c_str(), data->dwMask);
					}
				}
				else {
					pNewData = Render::LoadImage(bitmap, NULL, NULL, data->dwMask);
				}
				if( pNewData == NULL ) continue;

				Render::FreeImage(data, false);
				data->hBitmap = pNewData->hBitmap;
				data->pBits = pNewData->pBits;
				data->nX = pNewData->nX;
				data->nY = pNewData->nY;
				data->bAlpha = pNewData->bAlpha;
				data->pSrcBits = NULL;
				if( data->bUseHSL ) {
					data->pSrcBits = new BYTE[data->nX * data->nY * 4];
					::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
				}
				else data->pSrcBits = NULL;
				if( m_bUseHSL ) Render::AdjustImage(true, data, m_H, m_S, m_L);

				delete pNewData;
			}
		}
	}
}

void UIManager::ReloadImages()
{
	ImageInfo* data;
	ImageInfo* pNewData;
	for( int i = 0; i< m_ResInfo.m_ImageHash.GetSize(); i++ ) {
		if(LPCTSTR bitmap = m_ResInfo.m_ImageHash.GetAt(i)) {
			data = static_cast<ImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
			if( data != NULL ) {
				if( !data->sResType.empty() ) {
					if( isdigit(*bitmap) ) {
						LPTSTR pstr = NULL;
						int iIndex = _tcstol(bitmap, &pstr, 10);
						pNewData = Render::LoadImage(iIndex, this, data->sResType.c_str(), data->dwMask);
					}
					else {
						pNewData = Render::LoadImage(bitmap, this, data->sResType.c_str(), data->dwMask);
					}
				}
				else {
					pNewData = Render::LoadImage(bitmap, this, NULL, data->dwMask);
				}
				if( pNewData == NULL ) continue;

				Render::FreeImage(data, false);
				data->hBitmap = pNewData->hBitmap;
				data->pBits = pNewData->pBits;
				data->nX = pNewData->nX;
				data->nY = pNewData->nY;
				data->bAlpha = pNewData->bAlpha;
				data->pSrcBits = NULL;
				if( data->bUseHSL ) {
					data->pSrcBits = new BYTE[data->nX * data->nY * 4];
					::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
				}
				else data->pSrcBits = NULL;
				if( m_bUseHSL ) Render::AdjustImage(true, data, m_H, m_S, m_L);

				delete pNewData;
			}
		}
	}
    if( m_pRoot ) m_pRoot->Invalidate();
}

void UIManager::AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList, bool bShared)
{
	if (bShared || m_bForceUseSharedRes)
	{
		String* pDefaultAttr = new String(pStrControlAttrList);
		if (pDefaultAttr != NULL)
		{
			String* pOldDefaultAttr = static_cast<String*>(m_SharedResInfo.m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
			if (pOldDefaultAttr) delete pOldDefaultAttr;
		}
	}
	else
	{
		String* pDefaultAttr = new String(pStrControlAttrList);
		if (pDefaultAttr != NULL)
		{
			String* pOldDefaultAttr = static_cast<String*>(m_ResInfo.m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
			if (pOldDefaultAttr) delete pOldDefaultAttr;
		}
	}
}

//static
void UIManager::AddSharedDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList)
{
	String* pDefaultAttr = new String(pStrControlAttrList);
	if (pDefaultAttr != NULL)
	{
		String* pOldDefaultAttr = static_cast<String*>(m_SharedResInfo.m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
		if (pOldDefaultAttr) delete pOldDefaultAttr;
	}
}

LPCTSTR UIManager::GetDefaultAttributeList(LPCTSTR pStrControlName) const
{
    String* pDefaultAttr = static_cast<String*>(m_ResInfo.m_AttrHash.Find(pStrControlName));
	if( !pDefaultAttr ) pDefaultAttr = static_cast<String*>(m_SharedResInfo.m_AttrHash.Find(pStrControlName));
	if (pDefaultAttr) return pDefaultAttr->c_str();
	return NULL;
}

bool UIManager::RemoveDefaultAttributeList(LPCTSTR pStrControlName, bool bShared)
{
	if (bShared)
	{
		String* pDefaultAttr = static_cast<String*>(m_SharedResInfo.m_AttrHash.Find(pStrControlName));
		if( !pDefaultAttr ) return false;

		delete pDefaultAttr;
		return m_SharedResInfo.m_AttrHash.Remove(pStrControlName);
	}
	else
	{
		String* pDefaultAttr = static_cast<String*>(m_ResInfo.m_AttrHash.Find(pStrControlName));
		if( !pDefaultAttr ) return false;

		delete pDefaultAttr;
		return m_ResInfo.m_AttrHash.Remove(pStrControlName);
	}
}

void UIManager::RemoveAllDefaultAttributeList(bool bShared)
{
	if (bShared)
	{
		String* pDefaultAttr;
		for( int i = 0; i< m_SharedResInfo.m_AttrHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_SharedResInfo.m_AttrHash.GetAt(i)) {
				pDefaultAttr = static_cast<String*>(m_SharedResInfo.m_AttrHash.Find(key));
				if (pDefaultAttr) delete pDefaultAttr;
			}
		}
		m_SharedResInfo.m_AttrHash.RemoveAll();
	}
	else
	{
		String* pDefaultAttr;
		for( int i = 0; i< m_ResInfo.m_AttrHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_ResInfo.m_AttrHash.GetAt(i)) {
				pDefaultAttr = static_cast<String*>(m_ResInfo.m_AttrHash.Find(key));
				if (pDefaultAttr) delete pDefaultAttr;
			}
		}
		m_ResInfo.m_AttrHash.RemoveAll();
	}
}

void UIManager::AddWindowCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr)
{
	if( pstrName == NULL || pstrName[0] == _T('\0') || pstrAttr == NULL || pstrAttr[0] == _T('\0') ) return;
	String* pCostomAttr = new String(pstrAttr);
	if (pCostomAttr != NULL) {
		if (m_mWindowAttrHash.Find(pstrName) == NULL)
			m_mWindowAttrHash.Set(pstrName, (LPVOID)pCostomAttr);
		else
			delete pCostomAttr;
	}
}

LPCTSTR UIManager::GetWindowCustomAttribute(LPCTSTR pstrName) const
{
	if( pstrName == NULL || pstrName[0] == _T('\0') ) return NULL;
	String* pCostomAttr = static_cast<String*>(m_mWindowAttrHash.Find(pstrName));
	if (pCostomAttr) return pCostomAttr->c_str();
	return NULL;
}

bool UIManager::RemoveWindowCustomAttribute(LPCTSTR pstrName)
{
	if( pstrName == NULL || pstrName[0] == _T('\0') ) return NULL;
	String* pCostomAttr = static_cast<String*>(m_mWindowAttrHash.Find(pstrName));
	if( !pCostomAttr ) return false;

	delete pCostomAttr;
	return m_mWindowAttrHash.Remove(pstrName);
}

void UIManager::RemoveAllWindowCustomAttribute()
{
	String* pCostomAttr;
	for( int i = 0; i< m_mWindowAttrHash.GetSize(); i++ ) {
		if(LPCTSTR key = m_mWindowAttrHash.GetAt(i)) {
			pCostomAttr = static_cast<String*>(m_mWindowAttrHash.Find(key));
			delete pCostomAttr;
		}
	}
	m_mWindowAttrHash.Resize();
}

String UIManager::GetWindowAttribute(LPCTSTR pstrName)
{
    return _T("");
}

void UIManager::SetWindowAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcsicmp(pstrName, _T("class")) == 0) {
		LPCTSTR pValue = GetDefaultAttributeList(pstrValue);
		if (pValue){
			SetWindowAttributeList(pValue);
		}
	}
    else if( _tcsicmp(pstrName, _T("size")) == 0 ) {
        LPTSTR pstr = NULL;
        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		SetInitSize(GetDPIObj()->Scale(cx),GetDPIObj()->Scale(cy));
    } 
    else if( _tcsicmp(pstrName, _T("sizebox")) == 0 ) {
        RECT rcSizeBox = { 0 };
        LPTSTR pstr = NULL;
        rcSizeBox.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetSizeBox(rcSizeBox);
    }
    else if( _tcsicmp(pstrName, _T("caption")) == 0 ) {
        RECT rcCaption = { 0 };
        LPTSTR pstr = NULL;
        rcCaption.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetCaptionRect(rcCaption);
    }
    else if( _tcsicmp(pstrName, _T("roundcorner")) == 0 ) {
        LPTSTR pstr = NULL;
        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
        SetRoundCorner(cx, cy);
    } 
    else if( _tcsicmp(pstrName, _T("mininfo")) == 0 ) {
        LPTSTR pstr = NULL;
        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
        SetMinInfo(cx, cy);
    }
    else if( _tcsicmp(pstrName, _T("maxinfo")) == 0 ) {
        LPTSTR pstr = NULL;
        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
        SetMaxInfo(cx, cy);
    }
    else if( _tcsicmp(pstrName, _T("showdirty")) == 0 ) {
        SetShowUpdateRect(_tcsicmp(pstrValue, _T("true")) == 0);
    } 
    else if( _tcscmp(pstrName, _T("noactivate")) == 0 ) {
        SetNoActivate(_tcsicmp(pstrValue, _T("true")) == 0);
    }
	else if( _tcsicmp(pstrName, _T("opacity")) == 0 ) {
		SetOpacity(_ttoi(pstrValue));
	} 
	else if( _tcscmp(pstrName, _T("layeredopacity")) == 0 ) {
		SetLayeredOpacity(_ttoi(pstrValue));
    } 
	else if (_tcscmp(pstrName, _T("layered")) == 0) {
		SetLayered(_tcscmp(pstrValue, _T("true")) == 0);
	}
    else if( _tcscmp(pstrName, _T("layeredimage")) == 0 ) {
        SetLayered(true);
        SetLayeredImage(pstrValue);
    } 
	//UIShadow
	else if (_tcscmp(pstrName, _T("showshadow")) == 0) {
		GetShadow()->ShowShadow(_tcscmp(pstrValue, _T("true")) == 0);
	}
	else if (_tcscmp(pstrName, _T("shadowimage")) == 0) {
		GetShadow()->SetImage(pstrValue);
	}
	else if (_tcscmp(pstrName, _T("shadowcolor")) == 0) {
		DWORD clrColor = GetColor(pstrName);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		GetShadow()->SetColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("shadowsize")) == 0) {
		GetShadow()->SetSize(_ttoi(pstrValue));
	}
	else if (_tcscmp(pstrName, _T("shadowcorner")) == 0) {
		RECT rcCorner = { 0 };
		LPTSTR pstr = NULL;
		rcCorner.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
		rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
		rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
		rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
		GetShadow()->SetShadowCorner(rcCorner);
	}
	else if( _tcscmp(pstrName, _T("shadowsharpness")) == 0 ) {
		GetShadow()->SetSharpness(_ttoi(pstrValue));
	}
	else if( _tcscmp(pstrName, _T("shadowdarkness")) == 0 ) {
		GetShadow()->SetDarkness(_ttoi(pstrValue));
	}
	else if( _tcscmp(pstrName, _T("shadowposition")) == 0 ) {
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		GetShadow()->SetPosition(cx, cy);
	}
	else
	{
		wprintf(_T("UIManager::SetAttribute attribute:%s not found\n"), pstrName);
		AddWindowCustomAttribute(pstrName, pstrValue);
	}
}

String UIManager::GetWindowAttributeList(bool bIgnoreDefault)
{
    return _T("");
}

void UIManager::SetWindowAttributeList(LPCTSTR pstrList)
{
    String sItem;
    String sValue;
    while( *pstrList != _T('\0') ) {
		sItem.clear();
		sValue.clear();
        while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sItem += *pstrList++;
            }
        }
        ASSERT( *pstrList == _T('=') );
        if( *pstrList++ != _T('=') ) return;
        ASSERT( *pstrList == _T('\"') );
        if( *pstrList++ != _T('\"') ) return;
        while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sValue += *pstrList++;
            }
        }
        ASSERT( *pstrList == _T('\"') );
        if( *pstrList++ != _T('\"') ) return;
		SetWindowAttribute(sItem.c_str(), sValue.c_str());
        if( *pstrList++ != _T(' ') ) return;
    }
}

bool UIManager::RemoveWindowAttribute(LPCTSTR pstrName)
{
    return false;
}

void UIManager::AddMultiLanguageString(int id, LPCTSTR pStrMultiLanguage)
{
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);

	String* pMultiLanguage = new String(pStrMultiLanguage);
	if (pMultiLanguage != NULL)
	{
		String* pOldMultiLanguage = static_cast<String*>(m_SharedResInfo.m_MultiLanguageHash.Set(idBuffer, (LPVOID)pMultiLanguage));
		if (pOldMultiLanguage) delete pOldMultiLanguage;
	}
}

LPCTSTR UIManager::GetMultiLanguageString(int id)
{
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);

	String* pMultiLanguage = static_cast<String*>(m_SharedResInfo.m_MultiLanguageHash.Find(idBuffer));
	if (pMultiLanguage) return pMultiLanguage->c_str();
	return NULL;
}

bool UIManager::RemoveMultiLanguageString(int id)
{
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);

	String* pMultiLanguage = static_cast<String*>(m_SharedResInfo.m_MultiLanguageHash.Find(idBuffer));
	if( !pMultiLanguage ) return false;

	delete pMultiLanguage;
	return m_SharedResInfo.m_MultiLanguageHash.Remove(idBuffer);
}

void UIManager::RemoveAllMultiLanguageString()
{
	String* pMultiLanguage;
	for( int i = 0; i< m_SharedResInfo.m_MultiLanguageHash.GetSize(); i++ ) {
		if(LPCTSTR key = m_SharedResInfo.m_MultiLanguageHash.GetAt(i)) {
			pMultiLanguage = static_cast<String*>(m_SharedResInfo.m_MultiLanguageHash.Find(key));
			if (pMultiLanguage) delete pMultiLanguage;
		}
	}
	m_SharedResInfo.m_MultiLanguageHash.RemoveAll();
}

void UIManager::ProcessMultiLanguageTokens(String& pStrMultiLanguage)
{
#if MODE_COMMENT
	// Replace string-tokens: %{nnn}, nnn=int
	int iPos = pStrMultiLanguage.find(_T('%'));
	while( iPos >= 0 ) {
		if( pStrMultiLanguage.at(iPos + 1) == _T('{') ) {
			int iEndPos = iPos + 2;
			while (isdigit(pStrMultiLanguage.at(iEndPos))) iEndPos++;
			if (pStrMultiLanguage.at(iEndPos) == '}') {
				LPCTSTR pStrTemp = UIManager::GetMultiLanguageString((UINT)_ttoi(pStrMultiLanguage.c_str() + iPos + 2));
                if (pStrTemp)
#if 0
					nbase::StringReplaceAll(pStrMultiLanguage.substr(iPos, iEndPos - iPos + 1), pStrTemp, pStrMultiLanguage);
#else
				    pStrMultiLanguage.Replace(pStrMultiLanguage.substr(iPos, iEndPos - iPos + 1), pStrTemp);
#endif
			}
		}
		iPos = pStrMultiLanguage.find(_T('%'), iPos + 1);
	}
#endif
}

Control* UIManager::GetRoot() const
{
    ASSERT(m_pRoot);
    return m_pRoot;
}

Control* UIManager::FindControl(POINT pt) const
{
    ASSERT(m_pRoot);
    return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

Control* UIManager::FindControl(LPCTSTR pstrName) const
{
    ASSERT(m_pRoot);
    return static_cast<Control*>(m_mNameHash.Find(pstrName));
}

Control* UIManager::FindSubControlByPoint(Control* pParent, POINT pt) const
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    return pParent->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

Control* UIManager::FindSubControlByName(Control* pParent, LPCTSTR pstrName) const
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    return pParent->FindControl(__FindControlFromName, (LPVOID)pstrName, UIFIND_ALL);
}

Control* UIManager::FindSubControlByClass(Control* pParent, LPCTSTR pstrClass, int iIndex)
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    m_aFoundControls.Resize(iIndex + 1);
    return pParent->FindControl(__FindControlFromClass, (LPVOID)pstrClass, UIFIND_ALL);
}

PtrArray* UIManager::FindSubControlsByClass(Control* pParent, LPCTSTR pstrClass)
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    m_aFoundControls.Empty();
    pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, UIFIND_ALL);
    return &m_aFoundControls;
}

PtrArray* UIManager::GetFoundControls()
{
    return &m_aFoundControls;
}

Control* CALLBACK UIManager::__FindControlFromNameHash(Control* pThis, LPVOID pData)
{
    UIManager* pManager = static_cast<UIManager*>(pData);
    const String& sName = pThis->GetName();
    if( sName.empty() ) return NULL;
    // Add this control to the hash list
	pManager->m_mNameHash.Set(sName.c_str(), pThis);
    return NULL; // Attempt to add all controls
}

Control* CALLBACK UIManager::__FindControlFromCount(Control* /*pThis*/, LPVOID pData)
{
    int* pnCount = static_cast<int*>(pData);
    (*pnCount)++;
    return NULL;  // Count all controls
}

Control* CALLBACK UIManager::__FindControlFromPoint(Control* pThis, LPVOID pData)
{
    LPPOINT pPoint = static_cast<LPPOINT>(pData);
    return ::PtInRect(&pThis->GetPos(), *pPoint) ? pThis : NULL;
}

Control* CALLBACK UIManager::__FindControlFromTab(Control* pThis, LPVOID pData)
{
    FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
    if( pInfo->pFocus == pThis ) {
        if( pInfo->bForward ) pInfo->bNextIsIt = true;
        return pInfo->bForward ? NULL : pInfo->pLast;
    }
    if( (pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0 ) return NULL;
    pInfo->pLast = pThis;
    if( pInfo->bNextIsIt ) return pThis;
    if( pInfo->pFocus == NULL ) return pThis;
    return NULL;  // Examine all controls
}

Control* CALLBACK UIManager::__FindControlFromShortcut(Control* pThis, LPVOID pData)
{
    if( !pThis->IsVisible() ) return NULL; 
    FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
    if( pFS->ch == toupper(pThis->GetShortcut()) ) pFS->bPickNext = true;
    if( _tcsstr(pThis->GetClass(), DUI_CTR_LABEL) != NULL ) return NULL;   // Labels never get focus!
    return pFS->bPickNext ? pThis : NULL;
}

Control* CALLBACK UIManager::__FindControlFromName(Control* pThis, LPVOID pData)
{
    LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
    const String& sName = pThis->GetName();
    if( sName.empty() ) return NULL;
	return (_tcsicmp(sName.c_str(), pstrName) == 0) ? pThis : NULL;
}

Control* CALLBACK UIManager::__FindControlFromClass(Control* pThis, LPVOID pData)
{
    LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
    LPCTSTR pType = pThis->GetClass();
    PtrArray* pFoundControls = pThis->GetManager()->GetFoundControls();
    if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) {
        int iIndex = -1;
        while( pFoundControls->GetAt(++iIndex) != NULL ) ;
        if( iIndex < pFoundControls->GetSize() ) pFoundControls->SetAt(iIndex, pThis);
    }
    if( pFoundControls->GetAt(pFoundControls->GetSize() - 1) != NULL ) return pThis; 
    return NULL;
}

Control* CALLBACK UIManager::__FindControlsFromClass(Control* pThis, LPVOID pData)
{
    LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
    LPCTSTR pType = pThis->GetClass();
    if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) 
        pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
    return NULL;
}

Control* CALLBACK UIManager::__FindControlsFromUpdate(Control* pThis, LPVOID pData)
{
	if( pThis->IsUpdateNeeded() ) {
		pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
		return pThis;
	}
	return NULL;
}

bool UIManager::TranslateAccelerator(LPMSG pMsg)
{
	for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++)
	{
		LRESULT lResult = static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator[i])->TranslateAccelerator(pMsg);
		if( lResult == S_OK ) return true;
	}
	return false;
}

bool UIManager::TranslateMessage(const LPMSG pMsg)
{
	// Pretranslate Message takes care of system-wide messages, such as
	// tabbing and shortcut key-combos. We'll look for all messages for
	// each window and any child control attached.
	UINT uStyle = GetWindowStyle(pMsg->hwnd);
	UINT uChildRes = uStyle & WS_CHILD;	
	LRESULT lRes = 0;
	if (uChildRes != 0)
	{
		HWND hWndParent = ::GetParent(pMsg->hwnd);
		//code by redrain 2014.12.3,解决edit和webbrowser按tab无法切换焦点的bug
		//		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) 
		for( int i = m_aPreMessages.GetSize() - 1; i >= 0 ; --i ) 
		{
			UIManager* pT = static_cast<UIManager*>(m_aPreMessages[i]);        
			HWND hTempParent = hWndParent;
			while(hTempParent)
			{

				if(pMsg->hwnd == pT->GetPaintWindow() || hTempParent == pT->GetPaintWindow())
				{
					if (pT->TranslateAccelerator(pMsg))
						return true;

					pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes);
					// 					if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
					// 						return true;
					// 
					// 					return false;  
				}
				hTempParent = GetParent(hTempParent);
			}

		}
	}
	else
	{
		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) 
		{
			int size = m_aPreMessages.GetSize();
			UIManager* pT = static_cast<UIManager*>(m_aPreMessages[i]);
			if(pMsg->hwnd == pT->GetPaintWindow())
			{
				if (pT->TranslateAccelerator(pMsg))
					return true;

				if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
					return true;

				return false;
			}
		}
	}
	return false;
}

bool UIManager::AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
{
	ASSERT(m_aTranslateAccelerator.Find(pTranslateAccelerator) < 0);
	return m_aTranslateAccelerator.Add(pTranslateAccelerator);
}

bool UIManager::RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
{
	for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++)
	{
		if (static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator[i]) == pTranslateAccelerator)
		{
			return m_aTranslateAccelerator.Remove(i);
		}
	}
	return false;
}

Box* UIManager::CreateBox(const std::wstring& xmlPath, IDialogBuilderCallback *pCallback/* = NULL*/, UIManager *pManager/* = NULL*/, Control *pParent/* = NULL*/)
{
	CDialogBuilder builder;

	Box* pBox = NULL;
	
	Control *pRoot = builder.Create(xmlPath.c_str(), (UINT)0, pCallback, pManager, pParent);

	pBox = dynamic_cast<Box*>(pRoot);
	ASSERT(pBox);
	if (pBox == NULL)
	{
		MessageBox(NULL, _T("加载资源文件失败"), _T("Duilib"), MB_OK | MB_ICONERROR);
		ExitProcess(1);
		return 0;
	}

	return pBox;
}

bool UIManager::FillBox(Box* pFilledContainer, const std::wstring& xmlPath, IDialogBuilderCallback *pCallback/* = NULL*/, UIManager *pManager/* = NULL*/, Control *pParent/* = NULL*/)
{
	if (!pFilledContainer)
	{
		return false;
	}

	CDialogBuilder builder;
	Control *pRoot = builder.Create(xmlPath.c_str(), (UINT)0, pCallback, pManager, pParent, pFilledContainer);
	ASSERT(pFilledContainer == pRoot);
	if (pFilledContainer != pRoot)
	{
		MessageBox(NULL, _T("加载资源文件失败"), _T("Duilib"), MB_OK | MB_ICONERROR);
		ExitProcess(1);
		return false;
	}

	return true;
}

DPI * UIManager::GetDPIObj()
{
	if (m_pDPI == NULL) {
		m_pDPI = new DPI;
	}
	return m_pDPI;
}

void UIManager::SetDPI(int iDPI)
{
	int scale1 = GetDPIObj()->GetScale();
	GetDPIObj()->SetScale(iDPI);
	int scale2 = GetDPIObj()->GetScale();
	ResetDPIAssets();
	RECT rcWnd = { 0 };
	::GetWindowRect(GetPaintWindow(), &rcWnd);
	RECT*  prcNewWindow = &rcWnd;
	if (!::IsZoomed(GetPaintWindow())) {
		RECT rc = rcWnd;
		rc.right = rcWnd.left + (rcWnd.right - rcWnd.left) * scale2 / scale1;
		rc.bottom = rcWnd.top + (rcWnd.bottom - rcWnd.top) * scale2 / scale1;
		prcNewWindow = &rc;
	}
	SetWindowPos(GetPaintWindow(), NULL, prcNewWindow->left, prcNewWindow->top, prcNewWindow->right - prcNewWindow->left, prcNewWindow->bottom - prcNewWindow->top, SWP_NOZORDER | SWP_NOACTIVATE);
	if (GetRoot() != NULL) GetRoot()->NeedUpdate();
	::PostMessage(GetPaintWindow(), WM_USER_SET_DPI, 0, 0);
}

void UIManager::SetAllDPI(int iDPI)
{
	for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
		UIManager* pManager = static_cast<UIManager*>(m_aPreMessages[i]);
		pManager->SetDPI(iDPI);
	}
}

void UIManager::ResetDPIAssets()
{
	//RemoveAllDrawInfos();		//comment by djj 20171121
	RemoveAllImages();

	for (int it = 0; it < m_ResInfo.m_CustomFonts.GetSize(); it++) {
		FontInfo* pFontInfo = static_cast<FontInfo*>(m_ResInfo.m_CustomFonts.Find(m_ResInfo.m_CustomFonts[it]));
		RebuildFont(pFontInfo);
	}
	RebuildFont(&m_ResInfo.m_DefaultFontInfo);

	for (int it = 0; it < m_SharedResInfo.m_CustomFonts.GetSize(); it++) {
		FontInfo* pFontInfo = static_cast<FontInfo*>(m_SharedResInfo.m_CustomFonts.Find(m_SharedResInfo.m_CustomFonts[it]));
		RebuildFont(pFontInfo);
	}
	RebuildFont(&m_SharedResInfo.m_DefaultFontInfo);

	PtrArray *richEditList = FindSubControlsByClass(GetRoot(), _T("RichEditUI"));
	for (int i = 0; i < richEditList->GetSize(); i++)
	{
		RichEdit* pT = static_cast<RichEdit*>((*richEditList)[i]);
		pT->SetFont(pT->GetFont());

	}
}

void UIManager::RebuildFont(FontInfo * pFontInfo)
{
	::DeleteObject(pFontInfo->hFont);
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcsncpy(lf.lfFaceName, pFontInfo->sFontName.c_str(), LF_FACESIZE);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -GetDPIObj()->Scale(pFontInfo->iSize);
	lf.lfQuality = CLEARTYPE_QUALITY;
	if (pFontInfo->bBold) lf.lfWeight += FW_BOLD;
	if (pFontInfo->bUnderline) lf.lfUnderline = TRUE;
	if (pFontInfo->bItalic) lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);
	pFontInfo->hFont = hFont;
	::ZeroMemory(&(pFontInfo->tm), sizeof(pFontInfo->tm));
	if (m_hDcPaint) {
		HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
		::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
		::SelectObject(m_hDcPaint, hOldFont);
	}
}

} // namespace dui

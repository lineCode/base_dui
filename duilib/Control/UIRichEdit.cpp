#include "stdafx.h"
#include "Help/image.h"
#include "Help/file.h"
#ifdef _USEIMM
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#endif
// These constants are for backward compatibility. They are the 
// sizes used for initialization and reset in RichEdit 1.0

namespace dui {

const LONG cInitTextMax = (32 * 1024) - 1;

EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    {0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
    return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
    return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

HRESULT InitDefaultCharFormat(RichEdit* re, CHARFORMAT2W* pcf, HFONT hfont) 
{
    memset(pcf, 0, sizeof(CHARFORMAT2W));
    LOGFONT lf;
    if( !hfont )
        hfont = re->GetManager()->GetFont(re->GetFont());
    ::GetObject(hfont, sizeof(LOGFONT), &lf);

    DWORD dwColor = re->GetTextColor();
    pcf->cbSize = sizeof(CHARFORMAT2W);
    pcf->crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
    LONG yPixPerInch = GetDeviceCaps(re->GetManager()->GetPaintDC(), LOGPIXELSY);
    pcf->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    pcf->yOffset = 0;
    pcf->dwEffects = 0;
    pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
    if(lf.lfWeight >= FW_BOLD)
        pcf->dwEffects |= CFE_BOLD;
    if(lf.lfItalic)
        pcf->dwEffects |= CFE_ITALIC;
    if(lf.lfUnderline)
        pcf->dwEffects |= CFE_UNDERLINE;
    pcf->bCharSet = lf.lfCharSet;
    pcf->bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
    _tcscpy(pcf->szFaceName, lf.lfFaceName);
#else
    //need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pcf->szFaceName, LF_FACESIZE) ;
#endif

    return S_OK;
}

HRESULT InitDefaultParaFormat(RichEdit* re, PARAFORMAT2* ppf) 
{	
    memset(ppf, 0, sizeof(PARAFORMAT2));
    ppf->cbSize = sizeof(PARAFORMAT2);
    ppf->dwMask = PFM_ALL;
    ppf->wAlignment = PFA_LEFT;
    ppf->cTabCount = 1;
    ppf->rgxTabs[0] = lDefaultTab;

    return S_OK;
}

HRESULT CreateHost(RichEdit *re, const CREATESTRUCT *pcs, CTxtWinHost **pptec)
{
    HRESULT hr = E_FAIL;
    //GdiSetBatchLimit(1);

    CTxtWinHost *phost = new CTxtWinHost();
    if(phost)
    {
        if (phost->Init(re, pcs))
        {
            *pptec = phost;
            hr = S_OK;
        }
    }

    if (FAILED(hr))
    {
        delete phost;
    }

    return TRUE;
}

CTxtWinHost::CTxtWinHost() : m_re(NULL)
{
    ::ZeroMemory(&cRefs, sizeof(CTxtWinHost) - offsetof(CTxtWinHost, cRefs));
    cchTextMost = cInitTextMax;
    laccelpos = -1;
}

CTxtWinHost::~CTxtWinHost()
{
    pserv->OnTxInPlaceDeactivate();
    pserv->Release();
}

////////////////////// Create/Init/Destruct Commands ///////////////////////

BOOL CTxtWinHost::Init(RichEdit *re, const CREATESTRUCT *pcs)
{
	IUnknown *pUnk = NULL;
    HRESULT hr;

    m_re = re;
    // Initialize Reference count
    cRefs = 1;

    // Create and cache CHARFORMAT for this control
    if(FAILED(InitDefaultCharFormat(re, &cf, NULL)))
        goto err;

    // Create and cache PARAFORMAT for this control
    if(FAILED(InitDefaultParaFormat(re, &pf)))
        goto err;

    // edit controls created without a window are multiline by default
    // so that paragraph formats can be
    dwStyle = ES_MULTILINE;

    // edit controls are rich by default
    fRich = re->IsRich();

    cchTextMost = re->GetLimitText();

    if (pcs )
    {
        dwStyle = pcs->style;

        if ( !(dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)) )
        {
            fWordWrap = TRUE;
        }
    }

    if( !(dwStyle & ES_LEFT) )
    {
        if(dwStyle & ES_CENTER)
            pf.wAlignment = PFA_CENTER;
        else if(dwStyle & ES_RIGHT)
            pf.wAlignment = PFA_RIGHT;
    }

    fInplaceActive = TRUE;

    // Create Text Services component
    //if(FAILED(CreateTextServices(NULL, this, &pUnk)))
    //    goto err;

	PCreateTextServices TextServicesProc = NULL;
	HMODULE hmod = LoadLibrary(_T("msftedit.dll"));
	if (hmod)
	{
		TextServicesProc = (PCreateTextServices)GetProcAddress(hmod,"CreateTextServices");
	}

	if (TextServicesProc)
	{
		HRESULT hr = TextServicesProc(NULL, this, &pUnk);
	}

    hr = pUnk->QueryInterface(IID_ITextServices,(void **)&pserv);

    // Whether the previous call succeeded or failed we are done
    // with the private interface.
    pUnk->Release();

    if(FAILED(hr))
    {
        goto err;
    }

    // Set window text
    if(pcs && pcs->lpszName)
    {
#ifdef _UNICODE		
        if(FAILED(pserv->TxSetText((TCHAR *)pcs->lpszName)))
            goto err;
#else
        size_t iLen = _tcslen(pcs->lpszName);
        LPWSTR lpText = new WCHAR[iLen + 1];
        ::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
        ::MultiByteToWideChar(CP_ACP, 0, pcs->lpszName, -1, (LPWSTR)lpText, iLen) ;
        if(FAILED(pserv->TxSetText((LPWSTR)lpText))) {
            delete[] lpText;
            goto err;
        }
        delete[] lpText;
#endif
    }

    return TRUE;

err:
    return FALSE;
}

/////////////////////////////////  IUnknown ////////////////////////////////


HRESULT CTxtWinHost::QueryInterface(REFIID riid, void **ppvObject)
{
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) 
        || IsEqualIID(riid, IID_ITextHost)) 
    {
        AddRef();
        *ppvObject = (ITextHost *) this;
        hr = S_OK;
    }

    return hr;
}

ULONG CTxtWinHost::AddRef(void)
{
    return ++cRefs;
}

ULONG CTxtWinHost::Release(void)
{
    ULONG c_Refs = --cRefs;

    if (c_Refs == 0)
    {
        delete this;
    }

    return c_Refs;
}

/////////////////////////////////  Far East Support  //////////////////////////////////////

HIMC CTxtWinHost::TxImmGetContext(void)
{
    return NULL;
}

void CTxtWinHost::TxImmReleaseContext(HIMC himc)
{
    //::ImmReleaseContext( hwnd, himc );
}

//////////////////////////// ITextHost Interface  ////////////////////////////

HDC CTxtWinHost::TxGetDC()
{
    return m_re->GetManager()->GetPaintDC();
}

int CTxtWinHost::TxReleaseDC(HDC hdc)
{
    return 1;
}

BOOL CTxtWinHost::TxShowScrollBar(INT fnBar, BOOL fShow)
{
    ScrollBar* pVerticalScrollBar = m_re->GetVerticalScrollBar();
    ScrollBar* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if( fnBar == SB_VERT && pVerticalScrollBar ) {
        pVerticalScrollBar->SetVisible(fShow == TRUE);
    }
    else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
        pHorizontalScrollBar->SetVisible(fShow == TRUE);
    }
    else if( fnBar == SB_BOTH ) {
        if( pVerticalScrollBar ) pVerticalScrollBar->SetVisible(fShow == TRUE);
        if( pHorizontalScrollBar ) pHorizontalScrollBar->SetVisible(fShow == TRUE);
    }
    return TRUE;
}

BOOL CTxtWinHost::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
{
    if( fuSBFlags == SB_VERT ) {
        m_re->EnableScrollBar(true, m_re->GetHorizontalScrollBar() != NULL);
        m_re->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    }
    else if( fuSBFlags == SB_HORZ ) {
        m_re->EnableScrollBar(m_re->GetVerticalScrollBar() != NULL, true);
        m_re->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    }
    else if( fuSBFlags == SB_BOTH ) {
        m_re->EnableScrollBar(true, true);
        m_re->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
        m_re->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    }
    return TRUE;
}

BOOL CTxtWinHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
    ScrollBar* pVerticalScrollBar = m_re->GetVerticalScrollBar();
    ScrollBar* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if( fnBar == SB_VERT && pVerticalScrollBar ) {
        if( nMaxPos - nMinPos - rcClient.bottom + rcClient.top <= 0 ) {
            pVerticalScrollBar->SetVisible(false);
        }
        else {
            pVerticalScrollBar->SetVisible(true);
            pVerticalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.bottom + rcClient.top);
        }
    }
    else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
        if( nMaxPos - nMinPos - rcClient.right + rcClient.left <= 0 ) {
            pHorizontalScrollBar->SetVisible(false);
        }
        else {
            pHorizontalScrollBar->SetVisible(true);
            pHorizontalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.right + rcClient.left);
        }   
    }
    return TRUE;
}

BOOL CTxtWinHost::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)
{
    ScrollBar* pVerticalScrollBar = m_re->GetVerticalScrollBar();
    ScrollBar* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if( fnBar == SB_VERT && pVerticalScrollBar ) {
        pVerticalScrollBar->SetScrollPos(nPos);
    }
    else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
        pHorizontalScrollBar->SetScrollPos(nPos);
    }
    return TRUE;
}

void CTxtWinHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
    if( prc == NULL ) {
        m_re->GetManager()->Invalidate(rcClient);
        return;
    }
    RECT rc = *prc;
    m_re->GetManager()->Invalidate(rc);
}

void CTxtWinHost::TxViewChange(BOOL fUpdate) 
{
    if( m_re->OnTxViewChanged() ) m_re->Invalidate();
}

BOOL CTxtWinHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
    iCaretWidth = xWidth;
    iCaretHeight = yHeight;
    return ::CreateCaret(m_re->GetManager()->GetPaintWindow(), hbmp, xWidth, yHeight);
}

BOOL CTxtWinHost::TxShowCaret(BOOL fShow)
{
	fShowCaret = fShow;
    if(fShow)
        return ::ShowCaret(m_re->GetManager()->GetPaintWindow());
    else
        return ::HideCaret(m_re->GetManager()->GetPaintWindow());
}

BOOL CTxtWinHost::TxSetCaretPos(INT x, INT y)
{
	POINT ptCaret = { 0 };
	::GetCaretPos(&ptCaret);
	RECT rcCaret = { ptCaret.x, ptCaret.y, ptCaret.x + iCaretLastWidth, ptCaret.y + iCaretLastHeight };
	if( m_re->GetManager()->IsLayered() ) m_re->GetManager()->Invalidate(rcCaret);
	else if( fNeedFreshCaret == TRUE ) {
		m_re->GetManager()->Invalidate(rcCaret);
		fNeedFreshCaret = FALSE;
	}
	rcCaret.left = x;
	rcCaret.top = y;
	rcCaret.right = x + iCaretWidth;
	rcCaret.bottom = y + iCaretHeight;
	if( m_re->GetManager()->IsLayered() ) m_re->GetManager()->Invalidate(rcCaret);
	iCaretLastWidth = iCaretWidth;
	iCaretLastHeight = iCaretHeight;
	return ::SetCaretPos(x, y);
}

BOOL CTxtWinHost::TxSetTimer(UINT idTimer, UINT uTimeout)
{
    fTimer = TRUE;
    return m_re->GetManager()->SetTimer(m_re, idTimer, uTimeout) == TRUE;
}

void CTxtWinHost::TxKillTimer(UINT idTimer)
{
    m_re->GetManager()->KillTimer(m_re, idTimer);
    fTimer = FALSE;
}

void CTxtWinHost::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	LPCRECT lprcClip,	HRGN hrgnUpdate, LPRECT lprcUpdate,	UINT fuScroll)	
{
    return;
}

void CTxtWinHost::TxSetCapture(BOOL fCapture)
{
    if (fCapture) m_re->GetManager()->SetCapture();
    else m_re->GetManager()->ReleaseCapture();
    fCaptured = fCapture;
}

void CTxtWinHost::TxSetFocus()
{
    m_re->SetFocus();
}

void CTxtWinHost::TxSetCursor(HCURSOR hcur,	BOOL fText)
{
    ::SetCursor(hcur);
}

BOOL CTxtWinHost::TxScreenToClient(LPPOINT lppt)
{
    return ::ScreenToClient(m_re->GetManager()->GetPaintWindow(), lppt);	
}

BOOL CTxtWinHost::TxClientToScreen(LPPOINT lppt)
{
    return ::ClientToScreen(m_re->GetManager()->GetPaintWindow(), lppt);
}

HRESULT CTxtWinHost::TxActivate(LONG *plOldState)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxDeactivate(LONG lNewState)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxGetClientRect(LPRECT prc)
{
    *prc = rcClient;
    GetControlRect(prc);
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetViewInset(LPRECT prc)
{
    prc->left = prc->right = prc->top = prc->bottom = 0;
    return NOERROR;	
}

HRESULT CTxtWinHost::TxGetCharFormat(const CHARFORMATW **ppCF)
{
    *ppCF = &cf;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetParaFormat(const PARAFORMAT **ppPF)
{
    *ppPF = &pf;
    return NOERROR;
}

COLORREF CTxtWinHost::TxGetSysColor(int nIndex) 
{
    return ::GetSysColor(nIndex);
}

HRESULT CTxtWinHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
    *pstyle = !fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetMaxLength(DWORD *pLength)
{
    *pLength = cchTextMost;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetScrollBars(DWORD *pdwScrollBar)
{
    *pdwScrollBar =  dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
        ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

    return NOERROR;
}

HRESULT CTxtWinHost::TxGetPasswordChar(TCHAR *pch)
{
#ifdef _UNICODE
    *pch = chPasswordChar;
#else
    ::WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetAcceleratorPos(LONG *pcp)
{
    *pcp = laccelpos;
    return S_OK;
} 										   

HRESULT CTxtWinHost::OnTxCharFormatChange(const CHARFORMATW *pcf)
{
    return S_OK;
}

HRESULT CTxtWinHost::OnTxParaFormatChange(const PARAFORMAT *ppf)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
{
    DWORD dwProperties = 0;

    if (fRich)
    {
        dwProperties = TXTBIT_RICHTEXT;
    }

    if (dwStyle & ES_MULTILINE)
    {
        dwProperties |= TXTBIT_MULTILINE;
    }

    if (dwStyle & ES_READONLY)
    {
        dwProperties |= TXTBIT_READONLY;
    }

    if (dwStyle & ES_PASSWORD)
    {
        dwProperties |= TXTBIT_USEPASSWORD;
    }

    if (!(dwStyle & ES_NOHIDESEL))
    {
        dwProperties |= TXTBIT_HIDESELECTION;
    }

    if (fEnableAutoWordSel)
    {
        dwProperties |= TXTBIT_AUTOWORDSEL;
    }

    if (fWordWrap)
    {
        dwProperties |= TXTBIT_WORDWRAP;
    }

    if (fAllowBeep)
    {
        dwProperties |= TXTBIT_ALLOWBEEP;
    }

    if (fSaveSelection)
    {
        dwProperties |= TXTBIT_SAVESELECTION;
    }

    *pdwBits = dwProperties & dwMask; 
    return NOERROR;
}


HRESULT CTxtWinHost::TxNotify(DWORD iNotify, void *pv)
{
    if( iNotify == EN_REQUESTRESIZE ) {
        RECT rc;
        REQRESIZE *preqsz = (REQRESIZE *)pv;
        GetControlRect(&rc);
        rc.bottom = rc.top + preqsz->rc.bottom;
        rc.right  = rc.left + preqsz->rc.right;
        SetClientRect(&rc);
        return S_OK;
    }
    m_re->OnTxNotify(iNotify, pv);
    return S_OK;
}

HRESULT CTxtWinHost::TxGetExtent(LPSIZEL lpExtent)
{
    *lpExtent = sizelExtent;
    return S_OK;
}

HRESULT	CTxtWinHost::TxGetSelectionBarWidth (LONG *plSelBarWidth)
{
    *plSelBarWidth = lSelBarWidth;
    return S_OK;
}

void CTxtWinHost::SetWordWrap(BOOL fWordWrap)
{
    fWordWrap = fWordWrap;
    pserv->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
}

BOOL CTxtWinHost::IsReadOnly()
{
    return (dwStyle & ES_READONLY) != 0;
}

void CTxtWinHost::SetReadOnly(BOOL fReadOnly)
{
    if (fReadOnly)
    {
        dwStyle |= ES_READONLY;
    }
    else
    {
        dwStyle &= ~ES_READONLY;
    }

    pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, 
        fReadOnly ? TXTBIT_READONLY : 0);
}

void CTxtWinHost::SetFont(HFONT hFont) 
{
    if( hFont == NULL ) return;
    LOGFONT lf;
    ::GetObject(hFont, sizeof(LOGFONT), &lf);
    LONG yPixPerInch = ::GetDeviceCaps(m_re->GetManager()->GetPaintDC(), LOGPIXELSY);
    cf.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    if(lf.lfWeight >= FW_BOLD) cf.dwEffects |= CFE_BOLD;
	else cf.dwEffects &= ~CFE_BOLD;
    if(lf.lfItalic) cf.dwEffects |= CFE_ITALIC;
	else cf.dwEffects &= ~CFE_ITALIC;
    if(lf.lfUnderline) cf.dwEffects |= CFE_UNDERLINE;
	else cf.dwEffects &= ~CFE_UNDERLINE;
    cf.bCharSet = lf.lfCharSet;
    cf.bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
    _tcscpy(cf.szFaceName, lf.lfFaceName);
#else
    //need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, cf.szFaceName, LF_FACESIZE) ;
#endif

    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
        TXTBIT_CHARFORMATCHANGE);
}

void CTxtWinHost::SetColor(DWORD dwColor)
{
    cf.crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
        TXTBIT_CHARFORMATCHANGE);
}

SIZEL* CTxtWinHost::GetExtent() 
{
    return &sizelExtent;
}

void CTxtWinHost::SetExtent(SIZEL *psizelExtent) 
{ 
    sizelExtent = *psizelExtent; 
    pserv->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
}

void CTxtWinHost::LimitText(LONG nChars)
{
    cchTextMost = nChars;
    if( cchTextMost <= 0 ) cchTextMost = cInitTextMax;
    pserv->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
}

BOOL CTxtWinHost::IsCaptured()
{
    return fCaptured;
}

BOOL CTxtWinHost::IsShowCaret()
{
	return fShowCaret;
}

void CTxtWinHost::NeedFreshCaret()
{
	fNeedFreshCaret = TRUE;
}

INT CTxtWinHost::GetCaretWidth()
{
	return iCaretWidth;
}

INT CTxtWinHost::GetCaretHeight()
{
	return iCaretHeight;
}

BOOL CTxtWinHost::GetAllowBeep()
{
    return fAllowBeep;
}

void CTxtWinHost::SetAllowBeep(BOOL fAllowBeep)
{
    fAllowBeep = fAllowBeep;

    pserv->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, 
        fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
}

WORD CTxtWinHost::GetDefaultAlign()
{
    return pf.wAlignment;
}

void CTxtWinHost::SetDefaultAlign(WORD wNewAlign)
{
    pf.wAlignment = wNewAlign;

    // Notify control of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL CTxtWinHost::GetRichTextFlag()
{
    return fRich;
}

void CTxtWinHost::SetRichTextFlag(BOOL fNew)
{
    fRich = fNew;

    pserv->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
        fNew ? TXTBIT_RICHTEXT : 0);
}

LONG CTxtWinHost::GetDefaultLeftIndent()
{
    return pf.dxOffset;
}

void CTxtWinHost::SetDefaultLeftIndent(LONG lNewIndent)
{
    pf.dxOffset = lNewIndent;

    pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

void CTxtWinHost::SetClientRect(RECT *prc) 
{
    rcClient = *prc;

    LONG xPerInch = ::GetDeviceCaps(m_re->GetManager()->GetPaintDC(), LOGPIXELSX); 
    LONG yPerInch =	::GetDeviceCaps(m_re->GetManager()->GetPaintDC(), LOGPIXELSY); 
    sizelExtent.cx = DXtoHimetricX(rcClient.right - rcClient.left, xPerInch);
    sizelExtent.cy = DYtoHimetricY(rcClient.bottom - rcClient.top, yPerInch);

    pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
}

BOOL CTxtWinHost::SetSaveSelection(BOOL f_SaveSelection)
{
    BOOL fResult = f_SaveSelection;

    fSaveSelection = f_SaveSelection;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, 
        fSaveSelection ? TXTBIT_SAVESELECTION : 0);

    return fResult;		
}

HRESULT	CTxtWinHost::OnTxInPlaceDeactivate()
{
    HRESULT hr = pserv->OnTxInPlaceDeactivate();

    if (SUCCEEDED(hr))
    {
        fInplaceActive = FALSE;
    }

    return hr;
}

HRESULT	CTxtWinHost::OnTxInPlaceActivate(LPCRECT prcClient)
{
    fInplaceActive = TRUE;

    HRESULT hr = pserv->OnTxInPlaceActivate(prcClient);

    if (FAILED(hr))
    {
        fInplaceActive = FALSE;
    }

    return hr;
}

BOOL CTxtWinHost::DoSetCursor(RECT *prc, POINT *pt)
{
    RECT rc = prc ? *prc : rcClient;

    // Is this in our rectangle?
    if (PtInRect(&rc, *pt))
    {
        RECT *prcClient = (!fInplaceActive || prc) ? &rc : NULL;
        pserv->OnTxSetCursor(DVASPECT_CONTENT,	-1, NULL, NULL,  m_re->GetManager()->GetPaintDC(),
            NULL, prcClient, pt->x, pt->y);

        return TRUE;
    }

    return FALSE;
}

void CTxtWinHost::GetControlRect(LPRECT prc)
{
    prc->top = rcClient.top;
    prc->bottom = rcClient.bottom;
    prc->left = rcClient.left;
    prc->right = rcClient.right;
}

void CTxtWinHost::SetTransparent(BOOL f_Transparent)
{
    fTransparent = f_Transparent;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
}

LONG CTxtWinHost::SetAccelPos(LONG l_accelpos)
{
    LONG laccelposOld = l_accelpos;

    laccelpos = l_accelpos;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

    return laccelposOld;
}

WCHAR CTxtWinHost::SetPasswordChar(WCHAR ch_PasswordChar)
{
    WCHAR chOldPasswordChar = chPasswordChar;

    chPasswordChar = ch_PasswordChar;

    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
        (chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

    return chOldPasswordChar;
}

void CTxtWinHost::SetDisabled(BOOL fOn)
{
    cf.dwMask	 |= CFM_COLOR | CFM_DISABLED;
    cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

    if( !fOn )
    {
        cf.dwEffects &= ~CFE_DISABLED;
    }

    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
        TXTBIT_CHARFORMATCHANGE);
}

LONG CTxtWinHost::SetSelBarWidth(LONG l_SelBarWidth)
{
    LONG lOldSelBarWidth = lSelBarWidth;

    lSelBarWidth = l_SelBarWidth;

    if (lSelBarWidth)
    {
        dwStyle |= ES_SELECTIONBAR;
    }
    else
    {
        dwStyle &= (~ES_SELECTIONBAR);
    }

    pserv->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

    return lOldSelBarWidth;
}

BOOL CTxtWinHost::GetTimerState()
{
    return fTimer;
}

void CTxtWinHost::SetCharFormat(CHARFORMAT2W &c)
{
    cf = c;
}

void CTxtWinHost::SetParaFormat(PARAFORMAT2 &p)
{
    pf = p;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

RichEdit::RichEdit() : m_pTwh(NULL), m_bVScrollBarFixing(false), m_bWantTab(true), m_bWantReturn(true), 
    m_bWantCtrlReturn(true), m_bTransparent(true), m_bRich(true), m_bReadOnly(false), m_bWordWrap(false), m_dwTextColor(0), m_iFont(-1), 
	m_iLimitText(cInitTextMax), m_lTwhStyle(ES_MULTILINE), m_bDrawCaret(true), m_bInited(false)
{
	::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
}

RichEdit::~RichEdit()
{
    if( m_pTwh ) {
		m_pTwh->GetTextServices()->TxSendMessage(EM_SETOLECALLBACK, 0, 0, 0);

		if (m_pOleCallback)
		{
			delete m_pOleCallback;
			m_pOleCallback = nullptr;
		}

        m_pTwh->Release();
        m_pManager->RemoveMessageFilter(this);
    }
}

LPCTSTR RichEdit::GetClass() const
{
    return DUI_CTR_RICHEDIT;
}

LPVOID RichEdit::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_RICHEDIT) == 0 ) return static_cast<RichEdit*>(this);
    return ScrollContainer::GetInterface(pstrName);
}

UINT RichEdit::GetControlFlags() const
{
    if( !IsEnabled() ) return Control::GetControlFlags();

    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

bool RichEdit::IsWantTab()
{
    return m_bWantTab;
}

void RichEdit::SetWantTab(bool bWantTab)
{
    m_bWantTab = bWantTab;
}

bool RichEdit::IsWantReturn()
{
    return m_bWantReturn;
}

void RichEdit::SetWantReturn(bool bWantReturn)
{
    m_bWantReturn = bWantReturn;
}

bool RichEdit::IsWantCtrlReturn()
{
    return m_bWantCtrlReturn;
}

void RichEdit::SetWantCtrlReturn(bool bWantCtrlReturn)
{
    m_bWantCtrlReturn = bWantCtrlReturn;
}

bool RichEdit::IsTransparent()
{
    return m_bTransparent;
}

void RichEdit::SetTransparent(bool bTransparent)
{
    m_bTransparent = bTransparent;
    if( m_pTwh ) m_pTwh->SetTransparent(bTransparent);
}

bool RichEdit::IsRich()
{
    return m_bRich;
}

void RichEdit::SetRich(bool bRich)
{
    m_bRich = bRich;
    if( m_pTwh ) m_pTwh->SetRichTextFlag(bRich);
}

bool RichEdit::IsReadOnly()
{
    return m_bReadOnly;
}

void RichEdit::SetReadOnly(bool bReadOnly)
{
    m_bReadOnly = bReadOnly;
    if( m_pTwh ) m_pTwh->SetReadOnly(bReadOnly);
}

bool RichEdit::IsWordWrap()
{
    return m_bWordWrap;
}

void RichEdit::SetWordWrap(bool bWordWrap)
{
    m_bWordWrap = bWordWrap;
    if( m_pTwh ) m_pTwh->SetWordWrap(bWordWrap);
}

int RichEdit::GetFont()
{
    return m_iFont;
}

void RichEdit::SetFont(int index)
{
    m_iFont = index;
    if( m_pTwh ) {
        m_pTwh->SetFont(GetManager()->GetFont(m_iFont));
    }
}

void RichEdit::SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
    if( m_pTwh ) {
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
        m_pTwh->SetFont(hFont);
        ::DeleteObject(hFont);
    }
}

LONG RichEdit::GetWinStyle()
{
    return m_lTwhStyle;
}

void RichEdit::SetWinStyle(LONG lStyle)
{
    m_lTwhStyle = lStyle;
}

DWORD RichEdit::GetTextColor()
{
    return m_dwTextColor;
}

void RichEdit::SetTextColor(DWORD dwTextColor)
{
    m_dwTextColor = dwTextColor;
    if( m_pTwh ) {
        m_pTwh->SetColor(dwTextColor);
    }
}

int RichEdit::GetLimitText()
{
    return m_iLimitText;
}

void RichEdit::SetLimitText(int iChars)
{
    m_iLimitText = iChars;
    if( m_pTwh ) {
        m_pTwh->LimitText(m_iLimitText);
    }
}

long RichEdit::GetTextLength(DWORD dwFlags) const
{
    GETTEXTLENGTHEX textLenEx;
    textLenEx.flags = dwFlags;
#ifdef _UNICODE
    textLenEx.codepage = 1200;
#else
    textLenEx.codepage = CP_ACP;
#endif
    LRESULT lResult;
    TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
    return (long)lResult;
}

String RichEdit::GetText() const
{
    long lLen = GetTextLength(GTL_DEFAULT);
    LPTSTR lpText = NULL;
    GETTEXTEX gt;
    gt.flags = GT_DEFAULT;
#ifdef _UNICODE
    gt.cb = sizeof(TCHAR) * (lLen + 1) ;
    gt.codepage = 1200;
    lpText = new TCHAR[lLen + 1];
    ::ZeroMemory(lpText, (lLen + 1) * sizeof(TCHAR));
#else
    gt.cb = sizeof(TCHAR) * lLen * 2 + 1;
    gt.codepage = CP_ACP;
    lpText = new TCHAR[lLen * 2 + 1];
    ::ZeroMemory(lpText, (lLen * 2 + 1) * sizeof(TCHAR));
#endif
    gt.lpDefaultChar = NULL;
    gt.lpUsedDefChar = NULL;
    TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)lpText, 0);
    String sText(lpText);
    delete[] lpText;
    return sText;
}

void RichEdit::SetText(LPCTSTR pstrText)
{
    m_sText = pstrText;
    if( !m_pTwh ) return;
    SetSel(0, -1);
    ReplaceSel(pstrText, FALSE);
}

bool RichEdit::IsModify() const
{ 
    if( !m_pTwh ) return false;
    LRESULT lResult;
    TxSendMessage(EM_GETMODIFY, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

void RichEdit::SetModify(bool bModified) const
{ 
    TxSendMessage(EM_SETMODIFY, bModified, 0, 0);
}

void RichEdit::GetSel(CHARRANGE &cr) const
{ 
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
}

void RichEdit::GetSel(long& nStartChar, long& nEndChar) const
{
    CHARRANGE cr;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
    nStartChar = cr.cpMin;
    nEndChar = cr.cpMax;
}

int RichEdit::SetSel(CHARRANGE &cr)
{ 
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
    return (int)lResult;
}

int RichEdit::SetSel(long nStartChar, long nEndChar)
{
    CHARRANGE cr;
    cr.cpMin = nStartChar;
    cr.cpMax = nEndChar;
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
    return (int)lResult;
}

void RichEdit::ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo)
{
#ifdef _UNICODE		
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
#else
    int iLen = _tcslen(lpszNewText);
    LPWSTR lpText = new WCHAR[iLen + 1];
    ::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
    ::MultiByteToWideChar(CP_ACP, 0, lpszNewText, -1, (LPWSTR)lpText, iLen) ;
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpText, 0); 
    delete[] lpText;
#endif
}

void RichEdit::ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo)
{
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
}

String RichEdit::GetSelText() const
{
    if( !m_pTwh ) return String();
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
    LPWSTR lpText = NULL;
    lpText = new WCHAR[cr.cpMax - cr.cpMin + 1];
    ::ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(WCHAR));
    TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpText, 0);
    String sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

int RichEdit::SetSelAll()
{
    return SetSel(0, -1);
}

int RichEdit::SetSelNone()
{
    return SetSel(-1, 0);
}

bool RichEdit::GetZoom(int& nNum, int& nDen) const
{
    LRESULT lResult;
    TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool RichEdit::SetZoom(int nNum, int nDen)
{
    if (nNum < 0 || nNum > 64) return false;
    if (nDen < 0 || nDen > 64) return false;
    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, nNum, nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool RichEdit::SetZoomOff()
{
    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

WORD RichEdit::GetSelectionType() const
{
    LRESULT lResult;
    TxSendMessage(EM_SELECTIONTYPE, 0, 0, &lResult);
    return (WORD)lResult;
}

bool RichEdit::GetAutoURLDetect() const
{
    LRESULT lResult;
    TxSendMessage(EM_GETAUTOURLDETECT, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

bool RichEdit::SetAutoURLDetect(bool bAutoDetect)
{
    LRESULT lResult;
    TxSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0, &lResult);
    return (BOOL)lResult == FALSE;
}

DWORD RichEdit::GetEventMask() const
{
    LRESULT lResult;
    TxSendMessage(EM_GETEVENTMASK, 0, 0, &lResult);
    return (DWORD)lResult;
}

DWORD RichEdit::SetEventMask(DWORD dwEventMask)
{
    LRESULT lResult;
    TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask, &lResult);
    return (DWORD)lResult;
}

String RichEdit::GetTextRange(long nStartChar, long nEndChar) const
{
    TEXTRANGEW tr = { 0 };
    tr.chrg.cpMin = nStartChar;
    tr.chrg.cpMax = nEndChar;
    LPWSTR lpText = NULL;
    lpText = new WCHAR[nEndChar - nStartChar + 1];
    ::ZeroMemory(lpText, (nEndChar - nStartChar + 1) * sizeof(WCHAR));
    tr.lpstrText = lpText;
    TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, 0);
    String sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

void RichEdit::HideSelection(bool bHide, bool bChangeStyle)
{
    TxSendMessage(EM_HIDESELECTION, bHide, bChangeStyle, 0);
}

void RichEdit::ScrollCaret()
{
    TxSendMessage(EM_SCROLLCARET, 0, 0, 0);
}

int RichEdit::InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo)
{
    int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

int RichEdit::AppendText(LPCTSTR lpstrText, bool bCanUndo)
{
    int nRet = SetSel(-1, -1);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

DWORD RichEdit::GetDefaultCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

bool RichEdit::SetDefaultCharFormat(CHARFORMAT2 &cf)
{
    if( !m_pTwh ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    if( (BOOL)lResult == TRUE ) {
        CHARFORMAT2W cfw;
        cfw.cbSize = sizeof(CHARFORMAT2W);
        TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cfw, 0);
        m_pTwh->SetCharFormat(cfw);
        return true;
    }
    return false;
}

DWORD RichEdit::GetSelectionCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

bool RichEdit::SetSelectionCharFormat(CHARFORMAT2 &cf)
{
    if( !m_pTwh ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);
    return (BOOL)lResult == TRUE;
}

bool RichEdit::SetWordCharFormat(CHARFORMAT2 &cf)
{
    if( !m_pTwh ) return false;
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION|SCF_WORD, (LPARAM)&cf, &lResult); 
    return (BOOL)lResult == TRUE;
}

DWORD RichEdit::GetParaFormat(PARAFORMAT2 &pf) const
{
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    return (DWORD)lResult;
}

bool RichEdit::SetParaFormat(PARAFORMAT2 &pf)
{
    if( !m_pTwh ) return false;
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    if( (BOOL)lResult == TRUE ) {
        m_pTwh->SetParaFormat(pf);
        return true;
    }
    return false;
}

bool RichEdit::Redo()
{ 
    if( !m_pTwh ) return false;
    LRESULT lResult;
    TxSendMessage(EM_REDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE; 
}

bool RichEdit::Undo()
{ 
    if( !m_pTwh ) return false;
    LRESULT lResult;
    TxSendMessage(EM_UNDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE; 
}

void RichEdit::Clear()
{ 
    TxSendMessage(WM_CLEAR, 0, 0, 0); 
}

void RichEdit::Copy()
{ 
    TxSendMessage(WM_COPY, 0, 0, 0); 
}

void RichEdit::Cut()
{ 
    TxSendMessage(WM_CUT, 0, 0, 0); 
}

void RichEdit::Paste()
{ 
    TxSendMessage(WM_PASTE, 0, 0, 0); 
}

int RichEdit::GetLineCount() const
{ 
    if( !m_pTwh ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_GETLINECOUNT, 0, 0, &lResult);
    return (int)lResult; 
}

String RichEdit::GetLine(int nIndex, int nMaxLength) const
{
    LPWSTR lpText = NULL;
    lpText = new WCHAR[nMaxLength + 1];
    ::ZeroMemory(lpText, (nMaxLength + 1) * sizeof(WCHAR));
    *(LPWORD)lpText = (WORD)nMaxLength;
    TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpText, 0);
    String sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

int RichEdit::LineIndex(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINEINDEX, nLine, 0, &lResult);
    return (int)lResult;
}

int RichEdit::LineLength(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINELENGTH, nLine, 0, &lResult);
    return (int)lResult;
}

bool RichEdit::LineScroll(int nLines, int nChars)
{
    LRESULT lResult;
    TxSendMessage(EM_LINESCROLL, nChars, nLines, &lResult);
    return (BOOL)lResult == TRUE;
}

CDuiPoint RichEdit::GetCharPos(long lChar) const
{ 
    CDuiPoint pt; 
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar, 0); 
    return pt;
}

long RichEdit::LineFromChar(long nIndex) const
{ 
    if( !m_pTwh ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lResult);
    return (long)lResult;
}

CDuiPoint RichEdit::PosFromChar(UINT nChar) const
{ 
    POINTL pt; 
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar, 0); 
    return CDuiPoint(pt.x, pt.y); 
}

int RichEdit::CharFromPos(CDuiPoint pt) const
{ 
    POINTL ptl = {pt.x, pt.y}; 
    if( !m_pTwh ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl, &lResult);
    return (int)lResult; 
}

void RichEdit::EmptyUndoBuffer()
{ 
    TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, 0); 
}

UINT RichEdit::SetUndoLimit(UINT nLimit)
{ 
    if( !m_pTwh ) return 0;
    LRESULT lResult;
    TxSendMessage(EM_SETUNDOLIMIT, (WPARAM) nLimit, 0, &lResult);
    return (UINT)lResult; 
}

long RichEdit::StreamIn(int nFormat, EDITSTREAM &es)
{ 
    if( !m_pTwh ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult;
}

long RichEdit::StreamOut(int nFormat, EDITSTREAM &es)
{ 
    if( !m_pTwh ) return 0L;
    LRESULT lResult;
    TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult; 
}

RECT RichEdit::GetTextPadding() const
{
	return m_rcTextPadding;
}

void RichEdit::SetTextPadding(RECT rc)
{
	m_rcTextPadding = rc;
	Invalidate();
}

void RichEdit::SetPromptMode(bool bPrompt)
{
	m_bAllowPrompt = bPrompt;
}
bool RichEdit::GetPromptMode()
{
	return m_bAllowPrompt;
}
void RichEdit::SetPromptText(const String& strText)
{
	m_sPromptText = strText;
}
String RichEdit::GetPromptText() const
{
	return m_sPromptText;
}
void RichEdit::SetPromptColor(DWORD color)
{
	m_dwPromptColor = color;
}
DWORD RichEdit::GetPromptColor() const
{
	return m_dwPromptColor;
}

void RichEdit::DoInit()
{
	if(m_bInited)
		return ;

    CREATESTRUCT cs;
    cs.style = m_lTwhStyle;
    cs.x = 0;
    cs.y = 0;
    cs.cy = 0;
    cs.cx = 0;
    cs.lpszName = m_sText.c_str();
    CreateHost(this, &cs, &m_pTwh);
    if( m_pTwh ) {
        if( m_bTransparent ) m_pTwh->SetTransparent(TRUE);
        LRESULT lResult;
        m_pTwh->GetTextServices()->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lResult);
        m_pTwh->OnTxInPlaceActivate(NULL);
        m_pManager->AddMessageFilter(this);
		if( m_pManager->IsLayered() ) 
			m_pManager->SetTimer(this, DEFAULT_TIMERID, ::GetCaretBlinkTime());
    }

	if (!m_pOleCallback)
		m_pOleCallback = new IRichEditOleCallbackEx(this, InsertCustomItemErrorCallback());
	m_pOleCallback->SetCustomMode(true);
	m_pTwh->GetTextServices()->TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)m_pOleCallback, 0);
	
	m_bInited= true;
}

HRESULT RichEdit::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const
{
    if( m_pTwh ) {
        if( msg == WM_KEYDOWN && TCHAR(wparam) == VK_RETURN ) {
            if( !m_bWantReturn || (::GetKeyState(VK_CONTROL) < 0 && !m_bWantCtrlReturn) ) {
				if (m_pManager != NULL) m_pManager->SendNotify((Control*)this, UIEVENT_RETURN);
                return S_OK;
            }
        }
        return m_pTwh->GetTextServices()->TxSendMessage(msg, wparam, lparam, plresult);
    }
    return S_FALSE;
}

IDropTarget* RichEdit::GetTxDropTarget()
{
    IDropTarget *pdt = NULL;
    if( m_pTwh->GetTextServices()->TxGetDropTarget(&pdt) == NOERROR ) return pdt;
    return NULL;
}

bool RichEdit::OnTxViewChanged()
{
    return true;
}

bool RichEdit::SetDropAcceptFile(bool bAccept) 
{
	LRESULT lResult;
	TxSendMessage(EM_SETEVENTMASK, 0,ENM_DROPFILES|ENM_LINK, // ENM_CHANGE| ENM_CORRECTTEXT | ENM_DRAGDROPDONE | ENM_DROPFILES | ENM_IMECHANGE | ENM_LINK | ENM_OBJECTPOSITIONS | ENM_PROTECTED | ENM_REQUESTRESIZE | ENM_SCROLL | ENM_SELCHANGE | ENM_UPDATE,
		&lResult);
	return (BOOL)lResult == FALSE;
}

void RichEdit::OnTxNotify(DWORD iNotify, void *pv)
{
	switch(iNotify)
	{ 
	case EN_CHANGE:	
		GetManager()->SendNotify(this, UIEVENT_TEXTCHANGE);
		break;
	case EN_DROPFILES:   
	case EN_MSGFILTER:   
	case EN_OLEOPFAILED:   
	case EN_PROTECTED:   
	case EN_SAVECLIPBOARD:   
	case EN_SELCHANGE:   
	case EN_STOPNOUNDO:   
	case EN_LINK:   
	case EN_OBJECTPOSITIONS:   
	case EN_DRAGDROPDONE:   
		{
			if(pv)                        // Fill out NMHDR portion of pv   
			{   
				LONG nId =  GetWindowLong(this->GetManager()->GetPaintWindow(), GWL_ID);   
				NMHDR  *phdr = (NMHDR *)pv;   
				phdr->hwndFrom = this->GetManager()->GetPaintWindow();   
				phdr->idFrom = nId;   
				phdr->code = iNotify;  

				if(SendMessage(this->GetManager()->GetPaintWindow(), WM_NOTIFY, (WPARAM) nId, (LPARAM) pv))   
				{   
					//hr = S_FALSE;   
				}   
			}    
		}
		break;
	}
}

// ���з�rich��ʽ��richedit��һ��������bug�������һ���ǿ���ʱ��LineDown��SetScrollPos�޷����������
// ����iPos����Ϊ���������bug
void RichEdit::SetScrollPos(SIZE szPos)
{
    int cx = 0;
    int cy = 0;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
        m_pVerticalScrollBar->SetScrollPos(szPos.cy);
        cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
    }
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
        int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
        m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
        cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
    }
    if( cy != 0 ) {
        int iPos = 0;
        if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
            iPos = m_pVerticalScrollBar->GetScrollPos();
        WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pVerticalScrollBar->GetScrollPos());
        TxSendMessage(WM_VSCROLL, wParam, 0L, 0);
        if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
            if( cy > 0 && m_pVerticalScrollBar->GetScrollPos() <= iPos )
                m_pVerticalScrollBar->SetScrollPos(iPos);
        }
    }
    if( cx != 0 ) {
        WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pHorizontalScrollBar->GetScrollPos());
        TxSendMessage(WM_HSCROLL, wParam, 0L, 0);
    }
}

void RichEdit::LineUp()
{
    TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);
}

void RichEdit::LineDown()
{
    int iPos = 0;
    if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
        iPos = m_pVerticalScrollBar->GetScrollPos();
    TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L, 0);
    if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        if( m_pVerticalScrollBar->GetScrollPos() <= iPos )
            m_pVerticalScrollBar->SetScrollPos(m_pVerticalScrollBar->GetScrollRange());
    }
}

void RichEdit::PageUp()
{
    TxSendMessage(WM_VSCROLL, SB_PAGEUP, 0L, 0);
}

void RichEdit::PageDown()
{
    TxSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L, 0);
}

void RichEdit::HomeUp()
{
    TxSendMessage(WM_VSCROLL, SB_TOP, 0L, 0);
}

void RichEdit::EndDown()
{
    TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, 0);
}

void RichEdit::LineLeft()
{
    TxSendMessage(WM_HSCROLL, SB_LINELEFT, 0L, 0);
}

void RichEdit::LineRight()
{
    TxSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L, 0);
}

void RichEdit::PageLeft()
{
    TxSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L, 0);
}

void RichEdit::PageRight()
{
    TxSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L, 0);
}

void RichEdit::HomeLeft()
{
    TxSendMessage(WM_HSCROLL, SB_LEFT, 0L, 0);
}

void RichEdit::EndRight()
{
    TxSendMessage(WM_HSCROLL, SB_RIGHT, 0L, 0);
}

void RichEdit::DoEvent(TEvent& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else Control::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
    {
        if( m_pTwh && m_pTwh->DoSetCursor(NULL, &event.ptMouse) ) {
            return;
        }
    }
    else if( event.Type == UIEVENT_WINDOWSIZE ) {
        if( m_pTwh ) m_pTwh->NeedFreshCaret();
    }
    else if( event.Type == UIEVENT_SETFOCUS ) {
        if( m_pTwh ) {
            m_pTwh->OnTxInPlaceActivate(NULL);
            m_pTwh->GetTextServices()->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
        }
		m_bFocused = true;
		Invalidate();
		return;
    }
    if( event.Type == UIEVENT_KILLFOCUS )  {
        if( m_pTwh ) {
            m_pTwh->OnTxInPlaceActivate(NULL);
            m_pTwh->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
        }
		m_bFocused = false;
		Invalidate();
		return;
    }
	else if( event.Type == UIEVENT_TIMER ) {
		if( event.wParam == DEFAULT_TIMERID ) {
			if( m_pTwh && m_pManager->IsLayered() && IsFocused() ) {
				if (::GetFocus() != m_pManager->GetPaintWindow()) return;
				m_bDrawCaret = !m_bDrawCaret;
				POINT ptCaret;
				::GetCaretPos(&ptCaret);
				RECT rcCaret = { ptCaret.x, ptCaret.y, ptCaret.x + m_pTwh->GetCaretWidth(), 
					ptCaret.y + m_pTwh->GetCaretHeight() };
				RECT rcTemp = rcCaret;
				if( !::IntersectRect(&rcCaret, &rcTemp, &m_rcItem) ) return;
				Control* pParent = this;
				RECT rcParent;
				while( pParent = pParent->GetParent() ) {
					rcTemp = rcCaret;
					rcParent = pParent->GetPos();
					if( !::IntersectRect(&rcCaret, &rcTemp, &rcParent) ) {
						return;
					}
				}                    
				m_pManager->Invalidate(rcCaret);
			}
			return;
		}
		if( m_pTwh ) {
			m_pTwh->GetTextServices()->TxSendMessage(WM_TIMER, event.wParam, event.lParam, 0);
		} 
		return;
	}
    if( event.Type == UIEVENT_SCROLLWHEEL ) {
        if( (event.wKeyState & MK_CONTROL) != 0  ) {
            return;
        }
    }
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK ) 
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE ) 
    {
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP ) 
    {
        return;
    }
    if( event.Type > UIEVENT__KEYBEGIN && event.Type < UIEVENT__KEYEND )
    {
        return;
    }
    ScrollContainer::DoEvent(event);
}

SIZE RichEdit::EstimateSize(SIZE szAvailable)
{
    //return CDuiSize(m_rcItem); // ���ַ�ʽ�ڵ�һ�����ô�С֮��ʹ�С������
    return ScrollContainer::EstimateSize(szAvailable);
}

void RichEdit::SetPos(RECT rc, bool bNeedInvalidate)
{
    Control::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

	RECT rcPadding = GetPadding();
	rc.left += rcPadding.left;
	rc.top += rcPadding.top;
	rc.right -= rcPadding.right;
	rc.bottom -= rcPadding.bottom;

    RECT rcScrollView = rc;

    bool bVScrollBarVisiable = false;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        bVScrollBarVisiable = true;
        rc.top -= m_pVerticalScrollBar->GetScrollPos();
        rc.bottom -= m_pVerticalScrollBar->GetScrollPos();
        rc.bottom += m_pVerticalScrollBar->GetScrollRange();
        rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        rcScrollView.right -= m_pVerticalScrollBar->GetFixedWidth();
    }
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
        rc.left -= m_pHorizontalScrollBar->GetScrollPos();
        rc.right -= m_pHorizontalScrollBar->GetScrollPos();
        rc.right += m_pHorizontalScrollBar->GetScrollRange();
        rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
        rcScrollView.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
    }

    if( m_pTwh != NULL ) {
		RECT rcScrollTextView = rcScrollView;
		rcScrollTextView.left += m_rcTextPadding.left;
		rcScrollTextView.right -= m_rcTextPadding.right;
		rcScrollTextView.top += m_rcTextPadding.top;
		rcScrollTextView.bottom -= m_rcTextPadding.bottom;
		RECT rcText = rc;
		rcText.left += m_rcTextPadding.left;
		rcText.right -= m_rcTextPadding.right;
		rcText.top += m_rcTextPadding.top;
		rcText.bottom -= m_rcTextPadding.bottom;
        m_pTwh->SetClientRect(&rcScrollTextView);
        if( bVScrollBarVisiable && (!m_pVerticalScrollBar->IsVisible() || m_bVScrollBarFixing) ) {
            LONG lWidth = rcText.right - rcText.left + m_pVerticalScrollBar->GetFixedWidth();
            LONG lHeight = 0;
            SIZEL szExtent = { -1, -1 };
            m_pTwh->GetTextServices()->TxGetNaturalSize(
                DVASPECT_CONTENT, 
                GetManager()->GetPaintDC(), 
                NULL,
                NULL,
                TXTNS_FITTOCONTENT,
                &szExtent,
                &lWidth,
                &lHeight);
            if( lHeight > rcText.bottom - rcText.top ) {
                m_pVerticalScrollBar->SetVisible(true);
                m_pVerticalScrollBar->SetScrollPos(0);
                m_bVScrollBarFixing = true;
            }
            else {
                if( m_bVScrollBarFixing ) {
                    m_pVerticalScrollBar->SetVisible(false);
                    m_bVScrollBarFixing = false;
                }
            }
        }
    }

    if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
        RECT rcScrollBarPos = { rcScrollView.right, rcScrollView.top, 
            rcScrollView.right + m_pVerticalScrollBar->GetFixedWidth(), rcScrollView.bottom};
        m_pVerticalScrollBar->SetPos(rcScrollBarPos, false);
    }
    if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
        RECT rcScrollBarPos = { rcScrollView.left, rcScrollView.bottom, rcScrollView.right, 
            rcScrollView.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
        m_pHorizontalScrollBar->SetPos(rcScrollBarPos, false);
    }

    for( int it = 0; it < m_items.GetSize(); it++ ) {
        Control* pControl = static_cast<Control*>(m_items[it]);
        if( !pControl->IsVisible() ) continue;
        if( pControl->IsFloat() ) {
            SetFloatPos(it);
        }
        else {
			SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
			pControl->SetPos(rcCtrl, false);
		}
    }
}

void RichEdit::Move(SIZE szOffset, bool bNeedInvalidate)
{
	ScrollContainer::Move(szOffset, bNeedInvalidate);
	if( m_pTwh != NULL ) {
		RECT rc = m_rcItem;
		rc.left += m_rcPadding.left;
		rc.top += m_rcPadding.top;
		rc.right -= m_rcPadding.right;
		rc.bottom -= m_rcPadding.bottom;

		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		m_pTwh->SetClientRect(&rc);
	}
}

bool RichEdit::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
    RECT rcTemp = { 0 };
    if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return true;

    CRenderClip clip;
    CRenderClip::GenerateClip(hDC, rcTemp, clip);
    Control::DoPaint(hDC, rcPaint, pStopControl);

    if( m_pTwh ) {
        RECT rc;
        m_pTwh->GetControlRect(&rc);
        // Remember wparam is actually the hdc and lparam is the update
        // rect because this message has been preprocessed by the window.
        m_pTwh->GetTextServices()->TxDraw(
            DVASPECT_CONTENT,  		// Draw Aspect
            /*-1*/0,				// Lindex
            NULL,					// Info for drawing optimazation
            NULL,					// target device information
            hDC,			        // Draw device HDC
            NULL, 				   	// Target device HDC
            (RECTL*)&rc,			// Bounding client rectangle
            NULL, 		            // Clipping rectangle for metafiles
            (RECT*)&rcPaint,		// Update rectangle
            NULL, 	   				// Call back function
            NULL,					// Call back parameter
            0);				        // What view of the object
        if( m_bVScrollBarFixing ) {
            LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->GetFixedWidth();
            LONG lHeight = 0;
            SIZEL szExtent = { -1, -1 };
            m_pTwh->GetTextServices()->TxGetNaturalSize(
                DVASPECT_CONTENT, 
                GetManager()->GetPaintDC(), 
                NULL,
                NULL,
                TXTNS_FITTOCONTENT,
                &szExtent,
                &lWidth,
                &lHeight);
            if( lHeight <= rc.bottom - rc.top ) {
                NeedUpdate();
            }
        }
    }

    if( m_items.GetSize() > 0 ) {
        RECT rc = m_rcItem;
        rc.left += m_rcPadding.left;
        rc.top += m_rcPadding.top;
        rc.right -= m_rcPadding.right;
        rc.bottom -= m_rcPadding.bottom;
        if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

        if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                Control* pControl = static_cast<Control*>(m_items[it]);
				if( pControl == pStopControl ) return false;
                if( !pControl->IsVisible() ) continue;
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
                }
            }
        }
        else {
            CRenderClip childClip;
            CRenderClip::GenerateClip(hDC, rcTemp, childClip);
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                Control* pControl = static_cast<Control*>(m_items[it]);
				if( pControl == pStopControl ) return false;
                if( !pControl->IsVisible() ) continue;
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    CRenderClip::UseOldClipBegin(hDC, childClip);
                    if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
                    CRenderClip::UseOldClipEnd(hDC, childClip);
                }
                else {
                    if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
                    if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
                }
            }
        }
    }

	if( m_pTwh && m_pTwh->IsShowCaret() && m_pManager->IsLayered() && IsFocused() && m_bDrawCaret ) {
		POINT ptCaret;
		::GetCaretPos(&ptCaret);
		if( ::PtInRect(&m_rcItem, ptCaret) ) {
			RECT rcCaret = { ptCaret.x, ptCaret.y, ptCaret.x, ptCaret.y + m_pTwh->GetCaretHeight() };
			CRenderEngine::DrawLine(hDC, rcCaret, m_pTwh->GetCaretWidth(), 0xFF000000);
		}
	}

    if( m_pVerticalScrollBar != NULL ) {
        if( m_pVerticalScrollBar == pStopControl ) return false;
        if (m_pVerticalScrollBar->IsVisible()) {
            if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
                if( !m_pVerticalScrollBar->Paint(hDC, rcPaint, pStopControl) ) return false;
            }
        }
    }

    if( m_pHorizontalScrollBar != NULL ) {
        if( m_pHorizontalScrollBar == pStopControl ) return false;
        if (m_pHorizontalScrollBar->IsVisible()) {
            if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
                if( !m_pHorizontalScrollBar->Paint(hDC, rcPaint, pStopControl) ) return false;
            }
        }
    }
    return true;
}

void RichEdit::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("vscrollbar")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_DISABLENOSCROLL | WS_VSCROLL;
    }
    else if( _tcscmp(pstrName, _T("autovscroll")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_AUTOVSCROLL;
    }
    else if( _tcscmp(pstrName, _T("hscrollbar")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_DISABLENOSCROLL | WS_HSCROLL;
    }
	else  if (_tcscmp(pstrName, _T("autohscroll")) == 0) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_AUTOHSCROLL;
    }
    else if( _tcscmp(pstrName, _T("wanttab")) == 0 ) {
        SetWantTab(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcscmp(pstrName, _T("wantreturn")) == 0 ) {
        SetWantReturn(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcscmp(pstrName, _T("wantctrlreturn")) == 0 ) {
        SetWantCtrlReturn(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcscmp(pstrName, _T("transparent")) == 0 ) {
        SetTransparent(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcscmp(pstrName, _T("rich")) == 0 ) {
        SetRich(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if( _tcscmp(pstrName, _T("multiline")) == 0 ) {
        if( _tcscmp(pstrValue, _T("false")) == 0 ) m_lTwhStyle &= ~ES_MULTILINE;
    }
    else if( _tcscmp(pstrName, _T("readonly")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) { m_lTwhStyle |= ES_READONLY; m_bReadOnly = true; }
    }
    else if( _tcscmp(pstrName, _T("password")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_PASSWORD;
    }
    else if( _tcscmp(pstrName, _T("texthalign")) == 0 ) {
        if( _tcsstr(pstrValue, _T("left")) != NULL ) {
            m_lTwhStyle &= ~(ES_CENTER | ES_RIGHT);
            m_lTwhStyle |= ES_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) {
            m_lTwhStyle &= ~(ES_LEFT | ES_RIGHT);
            m_lTwhStyle |= ES_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL ) {
            m_lTwhStyle &= ~(ES_LEFT | ES_CENTER);
            m_lTwhStyle |= ES_RIGHT;
        }
    }
    else if( _tcscmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
	else if (_tcscmp(pstrName, _T("textcolor")) == 0 || _tcscmp(pstrName, _T("normaltextcolor")) == 0) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetTextColor(clrColor);
    }
	else if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
		RECT rcTextPadding = { 0 };
		LPTSTR pstr = NULL;
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetTextPadding(rcTextPadding);
	}
	//-------prompt-----
	else if (_tcscmp(pstrName, _T("promptmode")) == 0)
	{
		SetPromptMode(_tcscmp(pstrValue, _T("true")) == 0);
	}
	else if (_tcscmp(pstrName, _T("prompttext")) == 0)
	{
		SetPromptText(pstrName);
	}
	else if (_tcscmp(pstrName, _T("promptcolor")) == 0)
	{
		while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
		SetPromptColor(clrColor);
	}
    else ScrollContainer::SetAttribute(pstrName, pstrValue);
}

LRESULT RichEdit::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if( !IsVisible() || !IsEnabled() ) return 0;
    if( !IsMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
    if( uMsg == WM_MOUSEWHEEL && (LOWORD(wParam) & MK_CONTROL) == 0 ) return 0;

    bool bWasHandled = true;
    if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR ) {
        if( !m_pTwh->IsCaptured() ) {
            switch (uMsg) {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                {
                    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    Control* pHover = GetManager()->FindControl(pt);
                    if(pHover != this) {
                        bWasHandled = false;
                        return 0;
                    }
                }
                break;
            }
        }
        // Mouse message only go when captured or inside rect
        DWORD dwHitResult = m_pTwh->IsCaptured() ? HITRESULT_HIT : HITRESULT_OUTSIDE;
        if( dwHitResult == HITRESULT_OUTSIDE ) {
            RECT rc;
            m_pTwh->GetControlRect(&rc);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            if( uMsg == WM_SETCURSOR ) {
                ::GetCursorPos(&pt);
                ::ScreenToClient(GetManager()->GetPaintWindow(), &pt);
            }
            else if( uMsg == WM_MOUSEWHEEL ) ::ScreenToClient(GetManager()->GetPaintWindow(), &pt);
            if( ::PtInRect(&rc, pt) && !GetManager()->IsCaptured() ) dwHitResult = HITRESULT_HIT;
        }
        if( dwHitResult != HITRESULT_HIT ) return 0;
        if( uMsg == WM_SETCURSOR ) bWasHandled = false;
        else if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_RBUTTONDOWN ) {
			if (!GetManager()->IsNoActivate()) ::SetFocus(GetManager()->GetPaintWindow());
            SetFocus();
        }
    }
#ifdef _UNICODE
    else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
#else
    else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR ) {
#endif
        if( !IsFocused() ) return 0;
    }
#ifdef _USEIMM
	else if( uMsg == WM_IME_STARTCOMPOSITION ) {
		if( IsFocused() ) {
			POINT ptCaret;
			::GetCaretPos(&ptCaret);
			HIMC hMic = ::ImmGetContext(GetManager()->GetPaintWindow());
			COMPOSITIONFORM cpf;
			cpf.dwStyle = CFS_FORCE_POSITION;
			cpf.ptCurrentPos.x = ptCaret.x + m_pTwh->GetCaretWidth();
			cpf.ptCurrentPos.y = ptCaret.y;
			::ImmSetCompositionWindow(hMic, &cpf);

			HFONT hFont = GetManager()->GetFont(m_iFont);
			LOGFONT lf;
			::GetObject(hFont, sizeof(LOGFONT), &lf);
			::ImmSetCompositionFont(hMic, &lf);

			::ImmReleaseContext(GetManager()->GetPaintWindow(), hMic);
		}
		bWasHandled = false;
		return 0;
	}
#endif
    else if( uMsg == WM_CONTEXTMENU ) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ::ScreenToClient(GetManager()->GetPaintWindow(), &pt);
        Control* pHover = GetManager()->FindControl(pt);
        if(pHover != this) {
            bWasHandled = false;
            return 0;
        }
    }
    else
    {
        switch( uMsg ) {
        case WM_HELP:
            bWasHandled = false;
            break;
        default:
            return 0;
        }
    }
    LRESULT lResult = 0;
    HRESULT Hr = TxSendMessage(uMsg, wParam, lParam, &lResult);
    if( Hr == S_OK ) bHandled = bWasHandled;
    else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR )
        bHandled = bWasHandled;
    else if( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) {
        if( m_pTwh->IsCaptured() ) bHandled = bWasHandled;
    }
    return lResult;
}

//--------------------------------------------------------------------------------------------------

bool RichEdit::InsertEmoji(const std::wstring &file_name, const std::wstring &tag, int size)
{
	int emoji_size = size;
	GetManager()->GetDPIObj()->Scale(emoji_size);
	return InsertCustomItem(InsertCustomItemErrorCallback(), true, file_name, tag, RE_OLE_TYPE_FACE, 0, true, emoji_size, 0);
}

bool RichEdit::InsertImage(InsertCustomItemErrorCallback callback, bool total_count_limit, const std::wstring& file, const std::wstring& file_tag/* = L""*/, bool loading/* = false*/, LONG cp/* = REO_CP_SELECTION*/)
{
	return InsertCustomItem(callback, total_count_limit, loading ? L"" : file, file_tag.empty() ? file : file_tag, loading ? RE_OLE_TYPE_IMAGELOADING : RE_OLE_TYPE_IMAGE, 0, false, MAX_CUSTOM_ITEM_W, MAX_CUSTOM_ITEM_H, cp);
}

bool RichEdit::InsertFile(InsertCustomItemErrorCallback callback, const std::wstring& file)
{
	if (Re_FindFile(file))
	{
		return false;
	}
	return  InsertCustomItem(callback, true, L"", file, RE_OLE_TYPE_FILE, 0, false, MAX_CUSTOM_FILE_W, MAX_CUSTOM_ITEM_H);
}

void RichEdit::InsertHBitmap(InsertCustomItemErrorCallback callback, HBITMAP hbitmap, bool custom)
{
	ITextServices *text_service = m_pTwh->GetTextServices();
	if (!text_service || hbitmap == NULL)
		return;

	String tmp_image_path = CPaintManager::GetInstancePath() + L"tmp\\re\\";
	if (!duihelp::FilePathIsExist(tmp_image_path.c_str(), true))
	{
		duihelp::CreateDir(tmp_image_path.c_str());
	}
	String guid = L"23s15d87t353qw2e";
	String file_path = tmp_image_path + guid + _T(".bmp");

	if(duihelp::BaseSaveBitmap(hbitmap, file_path))
		InsertImage(callback, false, file_path);
}


} // namespace dui

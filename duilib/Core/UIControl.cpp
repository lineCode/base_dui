#include "StdAfx.h"

namespace dui {

Control::Control() : 
m_pManager(NULL), 
m_pParent(NULL), 
m_pCover(NULL),
m_bUpdateNeeded(true),
m_bMenuUsed(false),
m_bAsyncNotify(false),
m_bVisible(true), 
m_bInternVisible(true),
m_bFocused(false),
m_bEnabled(true),
m_bMouseEnabled(true),
m_bKeyboardEnabled(true),
m_bFloat(false),
m_bSetPos(false),
m_chShortcut('\0'),
m_pTag(NULL),
m_dwBackColor(0),
m_dwBackColor2(0),
m_dwBackColor3(0),
m_dwBorderColor(0),
m_dwFocusBorderColor(0),
m_bColorHSL(false),
m_nBorderStyle(PS_SOLID),
m_nTooltipWidth(300)
{
    m_cXY.cx = m_cXY.cy = 0;
    m_cxyFixed.cx = m_cxyFixed.cy = 0;
    m_cxyMin.cx = m_cxyMin.cy = 0;
    m_cxyMax.cx = m_cxyMax.cy = 9999;
    m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

    ::ZeroMemory(&m_rcMargin, sizeof(RECT));
    ::ZeroMemory(&m_rcItem, sizeof(RECT));
    ::ZeroMemory(&m_rcPaint, sizeof(RECT));
	::ZeroMemory(&m_rcBorderSize,sizeof(RECT));
	m_piFloatPercent.left = m_piFloatPercent.top = m_piFloatPercent.right = m_piFloatPercent.bottom = 0.0f;
}

Control::~Control()
{
    if( m_pCover != NULL ) {
        m_pCover->Delete();
        m_pCover = NULL;
    }

	RemoveAllCustomAttribute();
	if (OnDestroy)
	{
		TEvent event;
		event.pSender = this;
		OnDestroy(&event);
	}
    if( m_pManager != NULL ) m_pManager->ReapObjects(this);
}

void Control::Delete()
{
    if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
    delete this;
}

String Control::GetName() const
{
    return m_sName;
}

void Control::SetName(LPCTSTR pstrName)
{
	if (m_sName != pstrName) {
		m_sName = pstrName;
		if (m_pManager != NULL) m_pManager->RenameControl(this, pstrName);
	}
}

LPVOID Control::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_CONTROL) == 0 ) return this;
    return NULL;
}

LPCTSTR Control::GetClass() const
{
    return DUI_CTR_CONTROL;
}

UINT Control::GetControlFlags() const
{
    return 0;
}

HWND Control::GetNativeWindow() const
{
	return NULL;
}

bool Control::Activate()
{
    if( !IsVisible() ) return false;
    if( !IsEnabled() ) return false;
    return true;
}

CPaintManager* Control::GetManager() const
{
    return m_pManager;
}

void Control::SetManager(CPaintManager* pManager, Control* pParent, bool bInit)
{
    if( m_pCover != NULL ) m_pCover->SetManager(pManager, this, bInit);
    m_pManager = pManager;
    m_pParent = pParent;
    if( bInit && m_pParent ) Init();
}

Control* Control::GetParent() const
{
    return m_pParent;
}

Control* Control::GetCover() const
{
    return m_pCover;
}

void Control::SetCover(Control *pControl)
{
    if( m_pCover == pControl ) return;
    if( m_pCover != NULL ) m_pCover->Delete();
    m_pCover = pControl;
    if( m_pCover != NULL ) {
        m_pManager->InitControls(m_pCover, this);
        if( IsVisible() ) NeedUpdate();
        else pControl->SetInternVisible(false);
    }
}

String Control::GetText() const
{
    return m_sText;
}

void Control::SetText(LPCTSTR pstrText)
{
    if( m_sText == pstrText ) return;

    m_sText = pstrText;
    Invalidate();
}

DWORD Control::GetBkColor() const
{
    return m_dwBackColor;
}

void Control::SetBkColor(DWORD dwBackColor)
{
    if( m_dwBackColor == dwBackColor ) return;

    m_dwBackColor = dwBackColor;
    Invalidate();
}

DWORD Control::GetBkColor2() const
{
    return m_dwBackColor2;
}

void Control::SetBkColor2(DWORD dwBackColor)
{
    if( m_dwBackColor2 == dwBackColor ) return;

    m_dwBackColor2 = dwBackColor;
    Invalidate();
}

DWORD Control::GetBkColor3() const
{
    return m_dwBackColor3;
}

void Control::SetBkColor3(DWORD dwBackColor)
{
    if( m_dwBackColor3 == dwBackColor ) return;

    m_dwBackColor3 = dwBackColor;
    Invalidate();
}

LPCTSTR Control::GetBkImage()
{
    return m_diBk.sDrawString.c_str();
}

void Control::SetBkImage(LPCTSTR pStrImage)
{
	if( m_diBk.sDrawString == pStrImage && m_diBk.pImageInfo != NULL ) return;
	m_diBk.Clear();
	m_diBk.sDrawString = pStrImage;
	DrawImage(NULL, m_diBk);
	if( m_bFloat && m_cxyFixed.cx == 0 && m_cxyFixed.cy == 0 && m_diBk.pImageInfo ) {
		m_cxyFixed.cx = m_diBk.pImageInfo->nX;
		m_cxyFixed.cy = m_diBk.pImageInfo->nY;
	}
	Invalidate();
}

DWORD Control::GetBorderColor() const
{
    return m_dwBorderColor;
}

void Control::SetBorderColor(DWORD dwBorderColor)
{
    if( m_dwBorderColor == dwBorderColor ) return;

    m_dwBorderColor = dwBorderColor;
    Invalidate();
}

DWORD Control::GetFocusBorderColor() const
{
    return m_dwFocusBorderColor;
}

void Control::SetFocusBorderColor(DWORD dwBorderColor)
{
    if( m_dwFocusBorderColor == dwBorderColor ) return;

    m_dwFocusBorderColor = dwBorderColor;
    Invalidate();
}

bool Control::IsColorHSL() const
{
    return m_bColorHSL;
}

void Control::SetColorHSL(bool bColorHSL)
{
    if( m_bColorHSL == bColorHSL ) return;

    m_bColorHSL = bColorHSL;
    Invalidate();
}

RECT Control::GetBorderSize() const
{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_rcBorderSize);
    return m_rcBorderSize;
}

void Control::SetBorderSize( RECT rc )
{
	m_rcBorderSize = rc;
	Invalidate();
}

void Control::SetBorderSize(int iSize)
{
	m_rcBorderSize.left = m_rcBorderSize.top = m_rcBorderSize.right = m_rcBorderSize.bottom = iSize;
	Invalidate();
}

SIZE Control::GetBorderRound() const
{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_cxyBorderRound);
    return m_cxyBorderRound;
}

void Control::SetBorderRound(SIZE cxyRound)
{
    m_cxyBorderRound = cxyRound;
    Invalidate();
}

bool Control::DrawImage(HDC hDC, TDrawInfo& drawInfo)
{
	return CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, drawInfo);
}

const RECT& Control::GetPos() const
{
    return m_rcItem;
}

RECT Control::GetRelativePos() const
{
	Control* pParent = GetParent();
	if( pParent != NULL ) {
		RECT rcParentPos = pParent->GetPos();
		CDuiRect rcRelativePos(m_rcItem);
		rcRelativePos.Offset(-rcParentPos.left, -rcParentPos.top);
		return rcRelativePos;
	}
	else {
		return CDuiRect(0, 0, 0, 0);
	}
}

RECT Control::GetClientPos() const 
{
	return m_rcItem;
}

void Control::SetPos(RECT rc, bool bNeedInvalidate)
{
    if( rc.right < rc.left ) rc.right = rc.left;
    if( rc.bottom < rc.top ) rc.bottom = rc.top;

    CDuiRect invalidateRc = m_rcItem;
    if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = rc;

	if( m_bFloat ) {
		Control* pParent = GetParent();
		if( pParent != NULL ) {
			RECT rcParentPos = pParent->GetPos();
			RECT rcCtrl = {rcParentPos.left + rc.left, rcParentPos.top + rc.top, 
				rcParentPos.left + rc.right, rcParentPos.top + rc.bottom};
			m_rcItem = rcCtrl;

			LONG width = rcParentPos.right - rcParentPos.left;
			LONG height = rcParentPos.bottom - rcParentPos.top;
			RECT rcPercent = {(LONG)(width*m_piFloatPercent.left), (LONG)(height*m_piFloatPercent.top),
				(LONG)(width*m_piFloatPercent.right), (LONG)(height*m_piFloatPercent.bottom)};
			m_cXY.cx = rc.left - rcPercent.left;
			m_cXY.cy = rc.top - rcPercent.top;
			m_cxyFixed.cx = rc.right - rcPercent.right - m_cXY.cx;
			m_cxyFixed.cy = rc.bottom - rcPercent.bottom - m_cXY.cy;
		}
	}
	else {
		m_rcItem = rc;
	}
    if( m_pManager == NULL ) return;

    if( !m_bSetPos ) {
        m_bSetPos = true;
		if (event_map.find(UIEVENT_RESIZE) != event_map.cend())
		{
			TEvent event;
			event.pSender = this;
			event_map.find(UIEVENT_RESIZE)->second(&event);
		}
        m_bSetPos = false;
    }
    
    m_bUpdateNeeded = false;

	if( bNeedInvalidate && IsVisible() ) {
		invalidateRc.Join(m_rcItem);
		Control* pParent = this;
		RECT rcTemp;
		RECT rcParent;
		while( pParent = pParent->GetParent() ) {
			if( !pParent->IsVisible() ) return;
			rcTemp = invalidateRc;
			rcParent = pParent->GetPos();
			if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) return;
		}
		m_pManager->Invalidate(invalidateRc);
	}

    if( m_pCover != NULL && m_pCover->IsVisible() ) {
        if( m_pCover->IsFloat() ) {
            SIZE szXY = m_pCover->GetFixedXY();
            SIZE sz = {m_pCover->GetFixedWidth(), m_pCover->GetFixedHeight()};
            TPercentInfo rcPercent = m_pCover->GetFloatPercent();
            LONG width = m_rcItem.right - m_rcItem.left;
            LONG height = m_rcItem.bottom - m_rcItem.top;
            RECT rcCtrl = { 0 };
            rcCtrl.left = (LONG)(width*rcPercent.left) + szXY.cx;
            rcCtrl.top = (LONG)(height*rcPercent.top) + szXY.cy;
            rcCtrl.right = (LONG)(width*rcPercent.right) + szXY.cx + sz.cx;
            rcCtrl.bottom = (LONG)(height*rcPercent.bottom) + szXY.cy + sz.cy;
            m_pCover->SetPos(rcCtrl, false);
        }
        else {
            SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
            if( sz.cx < m_pCover->GetMinWidth() ) sz.cx = m_pCover->GetMinWidth();
            if( sz.cx > m_pCover->GetMaxWidth() ) sz.cx = m_pCover->GetMaxWidth();
            if( sz.cy < m_pCover->GetMinHeight() ) sz.cy = m_pCover->GetMinHeight();
            if( sz.cy > m_pCover->GetMaxHeight() ) sz.cy = m_pCover->GetMaxHeight();
            RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
            m_pCover->SetPos(rcCtrl, false);
        }
    }
}

void Control::Move(SIZE szOffset, bool bNeedInvalidate)
{
	CDuiRect invalidateRc = m_rcItem;
	m_rcItem.left += szOffset.cx;
	m_rcItem.top += szOffset.cy;
	m_rcItem.right += szOffset.cx;
	m_rcItem.bottom += szOffset.cy;

	if( bNeedInvalidate && m_pManager == NULL && IsVisible() ) {
		invalidateRc.Join(m_rcItem);
		Control* pParent = this;
		RECT rcTemp;
		RECT rcParent;
		while( pParent = pParent->GetParent() ) {
			if( !pParent->IsVisible() ) return;
			rcTemp = invalidateRc;
			rcParent = pParent->GetPos();
			if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) return;
		}
		m_pManager->Invalidate(invalidateRc);
	}

    if( m_pCover != NULL && m_pCover->IsVisible() ) m_pCover->Move(szOffset, false);
}

int Control::GetWidth() const
{
    return m_rcItem.right - m_rcItem.left;
}

int Control::GetHeight() const
{
    return m_rcItem.bottom - m_rcItem.top;
}

int Control::GetX() const
{
    return m_rcItem.left;
}

int Control::GetY() const
{
    return m_rcItem.top;
}

RECT Control::GetMargin() const
{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_rcMargin);
    return m_rcMargin;
}

void Control::SetMargin(RECT rcMargin)
{
    m_rcMargin = rcMargin;
    NeedParentUpdate();
}

SIZE Control::GetFixedXY() const
{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_cXY);
    return m_cXY;
}

void Control::SetFixedXY(SIZE szXY)
{
    m_cXY.cx = szXY.cx;
    m_cXY.cy = szXY.cy;
    NeedParentUpdate();
}

TPercentInfo Control::GetFloatPercent() const
{
	return m_piFloatPercent;
}

void Control::SetFloatPercent(TPercentInfo piFloatPercent)
{
	m_piFloatPercent = piFloatPercent;
	NeedParentUpdate();
}

int Control::GetFixedWidth() const
{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_cxyFixed.cx);
    return m_cxyFixed.cx;
}

void Control::SetFixedWidth(int cx)
{
    if( cx < 0 ) return; 
    m_cxyFixed.cx = cx;
    NeedParentUpdate();
}

int Control::GetFixedHeight() const
{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_cxyFixed.cy);
    return m_cxyFixed.cy;
}

void Control::SetFixedHeight(int cy)
{
    if( cy < 0 ) return; 
    m_cxyFixed.cy = cy;
    NeedParentUpdate();
}
//min
int Control::GetMinWidth() const{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_cxyMin.cx);
    return m_cxyMin.cx;
}
void Control::SetMinWidth(int cx){
    if( m_cxyMin.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMin.cx = cx;
    NeedParentUpdate();
}
int Control::GetMinHeight() const{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_cxyMin.cy);
	return m_cxyMin.cy;
}
void Control::SetMinHeight(int cy){
	if (m_cxyMin.cy == cy) return;

	if (cy < 0) return;
	m_cxyMin.cy = cy;
	NeedParentUpdate();
}
//max
int Control::GetMaxWidth() const{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_cxyMax.cx);
	//if (m_cxyMax.cx < m_cxyMin.cx) return m_cxyMin.cx;	//comment by djj20171123
    return m_cxyMax.cx;
}
void Control::SetMaxWidth(int cx){
    if( m_cxyMax.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMax.cx = cx;
    NeedParentUpdate();
}
int Control::GetMaxHeight() const{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_cxyMax.cy);
	//if (m_cxyMax.cy < m_cxyMin.cy) return m_cxyMin.cy;	//comment by djj20171123
	return m_cxyMax.cy;
}
void Control::SetMaxHeight(int cy){
	if (m_cxyMax.cy == cy) return;

	if (cy < 0) return;
	m_cxyMax.cy = cy;
	NeedParentUpdate();
}

String Control::GetToolTip() const
{
    return m_sToolTip;
}

void Control::SetToolTip(LPCTSTR pstrText)
{
	m_sToolTip = pstrText;
}

void Control::SetToolTipWidth( int nWidth )
{
	m_nTooltipWidth=nWidth;
}

int Control::GetToolTipWidth( void )
{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_nTooltipWidth);
	return m_nTooltipWidth;
}

TCHAR Control::GetShortcut() const
{
    return m_chShortcut;
}

void Control::SetShortcut(TCHAR ch)
{
    m_chShortcut = ch;
}

bool Control::IsContextMenuUsed() const
{
    return m_bMenuUsed;
}

void Control::SetContextMenuUsed(bool bMenuUsed)
{
    m_bMenuUsed = bMenuUsed;
}

const String& Control::GetUserData()
{
    return m_sUserData;
}

void Control::SetUserData(LPCTSTR pstrText)
{
    m_sUserData = pstrText;
}

UINT_PTR Control::GetTag() const
{
    return m_pTag;
}

void Control::SetTag(UINT_PTR pTag)
{
    m_pTag = pTag;
}

bool Control::IsVisible() const
{
    return m_bVisible && m_bInternVisible;
}

void Control::SetVisible(bool bVisible)
{
    if( m_bVisible == bVisible ) return;

    bool v = IsVisible();
    m_bVisible = bVisible;
    if( m_bFocused ) m_bFocused = false;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}
    if( IsVisible() != v ) {
        NeedParentUpdate();
    }

    if( m_pCover != NULL ) m_pCover->SetInternVisible(IsVisible());
}

void Control::SetInternVisible(bool bVisible)
{
    m_bInternVisible = bVisible;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}

    if( m_pCover != NULL ) m_pCover->SetInternVisible(IsVisible());
}

bool Control::IsEnabled() const
{
    return m_bEnabled;
}

void Control::SetEnabled(bool bEnabled)
{
    if( m_bEnabled == bEnabled ) return;

    m_bEnabled = bEnabled;
    Invalidate();
}

bool Control::IsMouseEnabled() const
{
    return m_bMouseEnabled;
}

void Control::SetMouseEnabled(bool bEnabled)
{
    m_bMouseEnabled = bEnabled;
}

bool Control::IsKeyboardEnabled() const
{
	return m_bKeyboardEnabled ;
}
void Control::SetKeyboardEnabled(bool bEnabled)
{
	m_bKeyboardEnabled = bEnabled ; 
}

bool Control::IsFocused() const
{
    return m_bFocused;
}

void Control::SetFocus()
{
    if( m_pManager != NULL ) m_pManager->SetFocus(this, false);
}

bool Control::IsFloat() const
{
    return m_bFloat;
}

void Control::SetFloat(bool bFloat)
{
    if( m_bFloat == bFloat ) return;

    m_bFloat = bFloat;
    NeedParentUpdate();
}

// add by djj[20180108]
void Control::SetPosStyle(UINT style)
{
	m_uPosStyle = style;
}

UINT Control::GetPosStyle() const
{
	return m_uPosStyle;
}

// ---------fade 系列 add by djj[20180105]---
void Control::SetFadeAlpha(bool bFade/* = true*/){
	if (m_bFadeAlpha == bFade) return;

	m_bFadeAlpha = bFade;
	NeedParentUpdate();
}

bool Control::GetFadeAlpha(){ 
	return m_bFadeAlpha; 
}
// ----------end fade 系列------------
void Control::AddCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr)
{
	if( pstrName == NULL || pstrName[0] == _T('\0') || pstrAttr == NULL || pstrAttr[0] == _T('\0') ) return;
	String* pCostomAttr = new String(pstrAttr);
	if (pCostomAttr != NULL) {
		if (m_mCustomAttrHash.Find(pstrName) == NULL)
			m_mCustomAttrHash.Set(pstrName, (LPVOID)pCostomAttr);
		else
			delete pCostomAttr;
	}
}

LPCTSTR Control::GetCustomAttribute(LPCTSTR pstrName) const
{
	if( pstrName == NULL || pstrName[0] == _T('\0') ) return NULL;
	String* pCostomAttr = static_cast<String*>(m_mCustomAttrHash.Find(pstrName));
	if (pCostomAttr) return pCostomAttr->c_str();
	return NULL;
}

bool Control::RemoveCustomAttribute(LPCTSTR pstrName)
{
	if( pstrName == NULL || pstrName[0] == _T('\0') ) return NULL;
	String* pCostomAttr = static_cast<String*>(m_mCustomAttrHash.Find(pstrName));
	if( !pCostomAttr ) return false;

	delete pCostomAttr;
	return m_mCustomAttrHash.Remove(pstrName);
}

void Control::RemoveAllCustomAttribute()
{
	String* pCostomAttr;
	for( int i = 0; i< m_mCustomAttrHash.GetSize(); i++ ) {
		if(LPCTSTR key = m_mCustomAttrHash.GetAt(i)) {
			pCostomAttr = static_cast<String*>(m_mCustomAttrHash.Find(key));
			delete pCostomAttr;
		}
	}
	m_mCustomAttrHash.Resize();
}

Control* Control::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
    if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
	if( (uFlags & UIFIND_UPDATETEST) != 0 && Proc(this, pData) != NULL ) return NULL;
    
    Control* pResult = NULL;
    if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
        if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
    }
    if( pResult == NULL && m_pCover != NULL ) {
        /*if( (uFlags & UIFIND_HITTEST) == 0 || true)*/ pResult = m_pCover->FindControl(Proc, pData, uFlags);
    }
    if( pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0 ) {
        if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
    }
    return pResult;
}

void Control::Invalidate()
{
    if( !IsVisible() ) return;

    RECT invalidateRc = m_rcItem;

    Control* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while( pParent = pParent->GetParent() )
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
        {
            return;
        }
    }

    if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
}

bool Control::IsUpdateNeeded() const
{
    return m_bUpdateNeeded;
}

void Control::NeedUpdate()
{
    if( !IsVisible() ) return;
    m_bUpdateNeeded = true;
    Invalidate();

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

void Control::NeedParentUpdate()
{
    if( GetParent() ) {
        GetParent()->NeedUpdate();
        GetParent()->Invalidate();
    }
    else {
        NeedUpdate();
    }

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

DWORD Control::GetAdjustColor(DWORD dwColor)
{
    if( !m_bColorHSL ) return dwColor;
    short H, S, L;
    CPaintManager::GetHSL(&H, &S, &L);
    return CRenderEngine::AdjustColor(dwColor, H, S, L);
}

void Control::Init()
{
    DoInit();
#if 1
	
#else
    if( OnInit ) 
		OnInit(this);
#endif
}

void Control::DoInit()
{

}

void Control::Event(TEvent& event)
{
	auto it = event_map.find(event.Type);
	if (it == event_map.cend() || it->second(&event))
	{
		DoEvent(event);
	}
}

void Control::DoEvent(TEvent& event)
{
    if( event.Type == UIEVENT_SETCURSOR )
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        return;
    }
    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        m_bFocused = true;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        m_bFocused = false;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_TIMER )
    {
		m_pManager->SendNotify(this, UIEVENT_TIMER, event.wParam, event.lParam);
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        if( IsContextMenuUsed() ) {
			m_pManager->SendNotify(this, UIEVENT_CONTEXTMENU, event.wParam, event.lParam);
            return;
        }
    }
    if( m_pParent != NULL ) m_pParent->DoEvent(event);
}

LPCTSTR Control::GetForeImage() const
{
	return m_diFore.sDrawString.c_str();
}

void Control::SetForeImage( LPCTSTR pStrImage )
{
	if( m_diFore.sDrawString == pStrImage && m_diFore.pImageInfo != NULL ) return;
	m_diFore.Clear();
	m_diFore.sDrawString = pStrImage;
	Invalidate();
}

String Control::GetAttribute(LPCTSTR pstrName)
{
    return _T("");
}

void Control::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("class")) == 0){
		LPCTSTR pValue = m_pManager->GetDefaultAttributeList(pstrValue);
		if (pValue){
			SetAttributeList(pValue);
		}
	}
    else if( _tcscmp(pstrName, _T("pos")) == 0 ) {
        RECT rcPos = { 0 };
        LPTSTR pstr = NULL;
        rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SIZE szXY = {rcPos.left, rcPos.top};
        SetFixedXY(szXY);
		//ASSERT(rcPos.right - rcPos.left >= 0);
		//ASSERT(rcPos.bottom - rcPos.top >= 0);
        SetFixedWidth(rcPos.right - rcPos.left);
        SetFixedHeight(rcPos.bottom - rcPos.top);
    }
    else if( _tcscmp(pstrName, _T("margin")) == 0 ) {
        RECT rcMargin = { 0 };
        LPTSTR pstr = NULL;
		rcMargin.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcMargin.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcMargin.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcMargin.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetMargin(rcMargin);
    }
    else if( _tcscmp(pstrName, _T("bkcolor")) == 0 || _tcscmp(pstrName, _T("bkcolor1")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bkcolor2")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetBkColor2(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bkcolor3")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetBkColor3(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bordercolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetBorderColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("focusbordercolor")) == 0 ) {
		DWORD clrColor = m_pManager->GetColor(pstrValue);
		if (clrColor == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			clrColor = _tcstoul(pstrValue, &pstr, 16);
		}
        SetFocusBorderColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("colorhsl")) == 0 ) SetColorHSL(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("bordersize")) == 0 ) {
		String nValue = pstrValue;
		if (nValue.find(',') < 0)
		{
			SetBorderSize(_ttoi(pstrValue));
		}
		else
		{
			RECT rcBorder = { 0 };
			LPTSTR pstr = NULL;
			rcBorder.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcBorder.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcBorder.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcBorder.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetBorderSize(rcBorder);
		}
	}
	else if( _tcscmp(pstrName, _T("borderstyle")) == 0 ) SetBorderStyle(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("borderround")) == 0 ) {
        SIZE cxyRound = { 0 };
        LPTSTR pstr = NULL;
        cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);     
        SetBorderRound(cxyRound);
    }
    else if( _tcscmp(pstrName, _T("bkimage")) == 0 ) SetBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("width")) == 0 ) SetFixedWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("height")) == 0 ) SetFixedHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("minwidth")) == 0 ) SetMinWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("minheight")) == 0 ) SetMinHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("maxwidth")) == 0 ) SetMaxWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("maxheight")) == 0 ) SetMaxHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("name")) == 0 ) SetName(pstrValue);
    else if( _tcscmp(pstrName, _T("text")) == 0 ) SetText(pstrValue);
    else if( _tcscmp(pstrName, _T("tooltip")) == 0 ) SetToolTip(pstrValue);
    else if( _tcscmp(pstrName, _T("userdata")) == 0 ) SetUserData(pstrValue);
    else if( _tcscmp(pstrName, _T("tag")) == 0 ) SetTag(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("enabled")) == 0 ) SetEnabled(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("mouse")) == 0 ) SetMouseEnabled(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("keyboard")) == 0 ) SetKeyboardEnabled(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("visible")) == 0 ) SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("float")) == 0 ) {
		String nValue = pstrValue;
		if (nValue.find(',') < 0) {
			SetFloat(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else {
			TPercentInfo piFloatPercent = { 0 };
			LPTSTR pstr = NULL;
			piFloatPercent.left = _tcstod(pstrValue, &pstr);  ASSERT(pstr);
			piFloatPercent.top = _tcstod(pstr + 1, &pstr);    ASSERT(pstr);
			piFloatPercent.right = _tcstod(pstr + 1, &pstr);  ASSERT(pstr);
			piFloatPercent.bottom = _tcstod(pstr + 1, &pstr); ASSERT(pstr);
			SetFloatPercent(piFloatPercent);
			SetFloat(true);
		}
	}
    else if( _tcscmp(pstrName, _T("shortcut")) == 0 ) SetShortcut(pstrValue[0]);
    else if( _tcscmp(pstrName, _T("menu")) == 0 ) SetContextMenuUsed(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
	//------------------------------add by djj[20180108]---------------------------
	else if(_tcscmp(pstrName, _T("halign")) == 0) {
		if (_tcsstr(pstrValue, _T("left")) != NULL) {
			m_uPosStyle &= ~(POS_STYLE_CENTER | POS_STYLE_RIGHT);
			m_uPosStyle |= POS_STYLE_LEFT;
		}
		if (_tcsstr(pstrValue, _T("center")) != NULL) {
			m_uPosStyle &= ~(POS_STYLE_LEFT | POS_STYLE_RIGHT);
			m_uPosStyle |= POS_STYLE_CENTER;
		}
		if (_tcsstr(pstrValue, _T("right")) != NULL) {
			m_uPosStyle &= ~(POS_STYLE_LEFT | POS_STYLE_CENTER);
			m_uPosStyle |= POS_STYLE_RIGHT;
		}
	}
	else if (_tcscmp(pstrName, _T("valign")) == 0)
	{
		if (_tcsstr(pstrValue, _T("top")) != NULL) {
			m_uPosStyle &= ~(POS_STYLE_BOTTOM | POS_STYLE_VCENTER);
			m_uPosStyle |= POS_STYLE_TOP;
		}
		if (_tcsstr(pstrValue, _T("center")) != NULL) {
			m_uPosStyle &= ~(POS_STYLE_TOP | POS_STYLE_BOTTOM);
			m_uPosStyle |= POS_STYLE_VCENTER;
		}
		if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
			m_uPosStyle &= ~(POS_STYLE_TOP | POS_STYLE_VCENTER);
			m_uPosStyle |= POS_STYLE_BOTTOM;
		}
	}
	//--------------------------------------fade系列 add by djj[20180105]----------------------------------------
	else if (_tcscmp(pstrName, _T("fadealpha")) == 0) SetFadeAlpha(_tcscmp(pstrValue, _T("true")) == 0);
	else {
		wprintf(_T("Control::SetAttribute control:%s attribute:%s not found\n"), GetName().c_str(), pstrName);
		AddCustomAttribute(pstrName, pstrValue);
	}
}

String Control::GetAttributeList(bool bIgnoreDefault)
{
	return _T("");
}

void Control::SetAttributeList(LPCTSTR pstrList)
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
		SetAttribute(sItem.c_str(), sValue.c_str());
        if( *pstrList++ != _T(' ') ) return;
    }
}

SIZE Control::EstimateSize(SIZE szAvailable)
{
	if (m_pManager != NULL) return m_pManager->GetDPIObj()->Scale(m_cxyFixed);
    return m_cxyFixed;
}

bool Control::Paint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
	if (pStopControl == this) return false;
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return true;
	if( OnPaint ) {
		TEvent event;
		event.pSender = this;
		if (!OnPaint(&event)) return true;
	}
	if (!DoPaint(hDC, rcPaint, pStopControl)) return false;
    if( m_pCover != NULL ) return m_pCover->Paint(hDC, rcPaint);
    return true;
}

bool Control::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
	if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) return true;

	SIZE cxyBorderRound;
	RECT rcBorderSize;
	if (m_pManager) {
		cxyBorderRound = GetManager()->GetDPIObj()->Scale(m_cxyBorderRound);
		rcBorderSize = GetManager()->GetDPIObj()->Scale(m_rcBorderSize);
	}
	else {
		cxyBorderRound = m_cxyBorderRound;
		rcBorderSize = m_rcBorderSize;
	}
    // 绘制循序：背景颜色->背景图->状态图->文本->边框
	if (cxyBorderRound.cx > 0 || cxyBorderRound.cy > 0) {
        CRenderClip roundClip;
		CRenderClip::GenerateRoundClip(hDC, m_rcPaint, m_rcItem, cxyBorderRound.cx, cxyBorderRound.cy, roundClip);
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
    else {
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
    return true;
}

void Control::PaintBkColor(HDC hDC)
{
    if( m_dwBackColor != 0 ) {
        if( m_dwBackColor2 != 0 ) {
            if( m_dwBackColor3 != 0 ) {
                RECT rc = m_rcItem;
                rc.bottom = (rc.bottom + rc.top) / 2;
                CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2), true, 8);
                rc.top = rc.bottom;
                rc.bottom = m_rcItem.bottom;
                CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor2), GetAdjustColor(m_dwBackColor3), true, 8);
            }
            else 
                CRenderEngine::DrawGradient(hDC, m_rcItem, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2), true, 16);
        }
        else if( m_dwBackColor >= 0xFF000000 ) CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwBackColor));
        else CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_dwBackColor));
    }
}

void Control::PaintBkImage(HDC hDC)
{
	DrawImage(hDC, m_diBk);
}

void Control::PaintStatusImage(HDC hDC)
{
    return;
}

void Control::PaintText(HDC hDC)
{
    return;
}

void Control::PaintBorder(HDC hDC)
{
	//int nBorderSize;
	SIZE cxyBorderRound;
	RECT rcBorderSize;
	if (m_pManager) {
		//nBorderSize = GetManager()->GetDPIObj()->Scale(m_nBorderSize);
		cxyBorderRound = GetManager()->GetDPIObj()->Scale(m_cxyBorderRound);
		rcBorderSize = GetManager()->GetDPIObj()->Scale(m_rcBorderSize);
	}
	else {
		//nBorderSize = m_nBorderSize;
		cxyBorderRound = m_cxyBorderRound;
		rcBorderSize = m_rcBorderSize;
	}

	if(rcBorderSize.left > 0 && (m_dwBorderColor != 0 || m_dwFocusBorderColor != 0)) {
		if( cxyBorderRound.cx > 0 || cxyBorderRound.cy > 0 )//画圆角边框
		{
			if (IsFocused() && m_dwFocusBorderColor != 0)
				CRenderEngine::DrawRoundRect(hDC, m_rcItem, rcBorderSize.left, cxyBorderRound.cx, cxyBorderRound.cy, GetAdjustColor(m_dwFocusBorderColor), m_nBorderStyle);
			else
				CRenderEngine::DrawRoundRect(hDC, m_rcItem, rcBorderSize.left, cxyBorderRound.cx, cxyBorderRound.cy, GetAdjustColor(m_dwBorderColor), m_nBorderStyle);
		}
		else {
			if (rcBorderSize.right == rcBorderSize.left && rcBorderSize.top == rcBorderSize.left && rcBorderSize.bottom == rcBorderSize.left) {
				if (IsFocused() && m_dwFocusBorderColor != 0)
					CRenderEngine::DrawRect(hDC, m_rcItem, rcBorderSize.left, GetAdjustColor(m_dwFocusBorderColor), m_nBorderStyle);
				else
					CRenderEngine::DrawRect(hDC, m_rcItem, rcBorderSize.left, GetAdjustColor(m_dwBorderColor), m_nBorderStyle);
			}
			else {
				RECT rcBorder;
				if(rcBorderSize.left > 0){
					rcBorder		= m_rcItem;
                    rcBorder.left  += rcBorderSize.left / 2;
					rcBorder.right	= rcBorder.left;
					if (IsFocused() && m_dwFocusBorderColor != 0)
						CRenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.left,GetAdjustColor(m_dwFocusBorderColor),m_nBorderStyle);
					else
						CRenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.left,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(rcBorderSize.top > 0) {
					rcBorder		= m_rcItem;
                    rcBorder.top   += rcBorderSize.top / 2;
					rcBorder.bottom	= rcBorder.top;
                    rcBorder.left  += rcBorderSize.left;
                    rcBorder.right -= rcBorderSize.right;
					if (IsFocused() && m_dwFocusBorderColor != 0)
						CRenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.top,GetAdjustColor(m_dwFocusBorderColor),m_nBorderStyle);
					else
						CRenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.top,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(rcBorderSize.right > 0) {
					rcBorder		= m_rcItem;
					rcBorder.left	= m_rcItem.right - rcBorderSize.right / 2;
                    rcBorder.right  = rcBorder.left;
					if (IsFocused() && m_dwFocusBorderColor != 0)
						CRenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.right,GetAdjustColor(m_dwFocusBorderColor),m_nBorderStyle);
					else
						CRenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.right,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(rcBorderSize.bottom > 0) {
					rcBorder		= m_rcItem;
					rcBorder.top	= m_rcItem.bottom - rcBorderSize.bottom / 2;
                    rcBorder.bottom = rcBorder.top;
                    rcBorder.left  += rcBorderSize.left;
                    rcBorder.right -= rcBorderSize.right;
					if (IsFocused() && m_dwFocusBorderColor != 0)
						CRenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.bottom,GetAdjustColor(m_dwFocusBorderColor),m_nBorderStyle);
					else
						CRenderEngine::DrawLine(hDC,rcBorder,rcBorderSize.bottom,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
			}
		}
	}
}

void Control::DoPostPaint(HDC hDC, const RECT& rcPaint)
{
	if (OnPostPaint)
	{
		TEvent event;
		event.pSender = this;
		OnPostPaint(&event);
	}
}

int Control::GetBorderStyle() const
{
	return m_nBorderStyle;
}

void Control::SetBorderStyle( int nStyle )
{
	m_nBorderStyle = nStyle;
	Invalidate();
}

} // namespace dui

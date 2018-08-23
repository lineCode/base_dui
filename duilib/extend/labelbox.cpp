#include "stdafx.h"

namespace dui{

	LabelBox::LabelBox() :
		m_pWideText(0),
		m_uTextStyle(DT_VCENTER | DT_SINGLELINE),
		m_dwTextColor(0),
		m_dwDisabledTextColor(0),
		m_iFont(-1),
		m_bShowHtml(false),
		m_bNeedEstimateSize(true),
		m_EnableEffect(false),
		m_bEnableLuminous(false),
		m_fLuminousFuzzy(3),
		m_gdiplusToken(0),
		m_dwTextColor1(-1),
		m_dwTextShadowColorA(0xff000000),
		m_dwTextShadowColorB(-1),
		m_GradientAngle(0),
		m_EnabledStroke(false),
		m_dwStrokeColor(0),
		m_EnabledShadow(false),
		m_GradientLength(0)
	{
		m_ShadowOffset.X = 0.0f;
		m_ShadowOffset.Y = 0.0f;
		m_ShadowOffset.Width = 0.0f;
		m_ShadowOffset.Height = 0.0f;

		m_cxyFixedLast.cx = m_cxyFixedLast.cy = 0;
		m_szAvailableLast.cx = m_szAvailableLast.cy = 0;
		::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));

#ifdef _USE_GDIPLUS
		GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
#endif
	}

	LabelBox::~LabelBox()
	{
#ifdef _UNICODE
		if (m_pWideText && m_pWideText != m_sText.c_str()) delete[] m_pWideText;
#else
		if (m_pWideText) delete[] m_pWideText;
#endif

#ifdef _USE_GDIPLUS
		GdiplusShutdown(m_gdiplusToken);
#endif
	}
	LPCTSTR LabelBox::GetClass() const
	{
		return DUI_CTR_LABELBOX;
	}

	LPVOID LabelBox::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_LABELBOX) == 0) return static_cast<LabelBox*>(this);
		return __super::GetInterface(pstrName);
	}

	void LabelBox::SetFixedWidth(int cx)
	{
		m_bNeedEstimateSize = true;
		Control::SetFixedWidth(cx);
	}

	void LabelBox::SetFixedHeight(int cy)
	{
		m_bNeedEstimateSize = true;
		Control::SetFixedHeight(cy);
	}

	void LabelBox::SetText(LPCTSTR pstrText)
	{
		Control::SetText(pstrText);
		m_bNeedEstimateSize = true;
		if (m_EnableEffect) {
#ifdef _UNICODE
			m_pWideText = (LPWSTR)m_sText.c_str();
#else 
			int iLen = _tcslen(pstrText);
			if (m_pWideText) delete[] m_pWideText;
			m_pWideText = new WCHAR[iLen + 1];
			::ZeroMemory(m_pWideText, (iLen + 1) * sizeof(WCHAR));
			::MultiByteToWideChar(CP_ACP, 0, pstrText, -1, (LPWSTR)m_pWideText, iLen);
#endif
		}
	}

	void LabelBox::SetTextStyle(UINT uStyle)
	{
		m_uTextStyle = uStyle;
		m_bNeedEstimateSize = true;
		Invalidate();
	}

	UINT LabelBox::GetTextStyle() const
	{
		return m_uTextStyle;
	}

	bool LabelBox::IsMultiLine()
	{
		return (m_uTextStyle & DT_SINGLELINE) == 0;
	}

	void LabelBox::SetMultiLine(bool bMultiLine)
	{
		if (bMultiLine)	{
			m_uTextStyle &= ~DT_SINGLELINE;
			m_uTextStyle |= DT_WORDBREAK;
		}
		else m_uTextStyle |= DT_SINGLELINE;
		m_bNeedEstimateSize = true;
	}

	void LabelBox::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
		Invalidate();
	}

	DWORD LabelBox::GetTextColor() const
	{
		return m_dwTextColor;
	}

	void LabelBox::SetDisabledTextColor(DWORD dwTextColor)
	{
		m_dwDisabledTextColor = dwTextColor;
		Invalidate();
	}

	DWORD LabelBox::GetDisabledTextColor() const
	{
		return m_dwDisabledTextColor;
	}

	void LabelBox::SetFont(int index)
	{
		m_iFont = index;
		m_bNeedEstimateSize = true;
		Invalidate();
	}

	int LabelBox::GetFont() const
	{
		return m_iFont;
	}

	RECT LabelBox::GetTextPadding() const
	{
		if (m_pManager) return m_pManager->GetDPIObj()->Scale(m_rcTextPadding);
		return m_rcTextPadding;
	}

	void LabelBox::SetTextPadding(RECT rc)
	{
		m_rcTextPadding = rc;
		m_bNeedEstimateSize = true;
		Invalidate();
	}

	bool LabelBox::IsShowHtml()
	{
		return m_bShowHtml;
	}

	void LabelBox::SetShowHtml(bool bShowHtml)
	{
		if (m_bShowHtml == bShowHtml) return;

		m_bShowHtml = bShowHtml;
		m_bNeedEstimateSize = true;
		Invalidate();
	}

	SIZE LabelBox::EstimateSize(SIZE szAvailable)
	{
		if (m_FixedSize.cx > 0 && m_FixedSize.cy > 0) return Control::EstimateSize(szAvailable);

		if ((m_uTextStyle & DT_SINGLELINE) == 0 &&
			(szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy)) {
			m_bNeedEstimateSize = true;
		}

		if (m_bNeedEstimateSize) {
			m_bNeedEstimateSize = false;
			m_szAvailableLast = szAvailable;
			m_cxyFixedLast = m_FixedSize;
			if ((m_uTextStyle & DT_SINGLELINE) != 0) {
				if (m_cxyFixedLast.cy == 0) {
					m_cxyFixedLast.cy = m_pManager->GetFontInfo(m_iFont)->tm.tmHeight + 8;
					m_cxyFixedLast.cy += m_rcTextPadding.top + m_rcTextPadding.bottom;
				}
				if (m_cxyFixedLast.cx == 0) {
					RECT rcText = { 0, 0, 9999, m_cxyFixedLast.cy };
					if (m_bShowHtml) {
						int nLinks = 0;
						Render::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), 0, NULL, NULL, nLinks, m_iFont, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
					}
					else {
						Render::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), 0, m_iFont, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
					}
					m_cxyFixedLast.cx = rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right;
				}
			}
			else {
				if (m_cxyFixedLast.cx == 0) {
					m_cxyFixedLast.cx = szAvailable.cx;
				}
				RECT rcText = { 0, 0, m_cxyFixedLast.cx, 9999 };
				rcText.left += m_rcTextPadding.left;
				rcText.right -= m_rcTextPadding.right;
				if (m_bShowHtml) {
					int nLinks = 0;
					Render::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), 0, NULL, NULL, nLinks, m_iFont, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
				}
				else {
					Render::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), 0, m_iFont, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
				}
				m_cxyFixedLast.cy = rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom;
			}
		}
		if (m_pManager) return m_pManager->GetDPIObj()->Scale(m_cxyFixedLast);
		return m_cxyFixedLast;
	}

	void LabelBox::DoEvent(Event& event)
	{
		if (event.Type == UIEVENT_SETFOCUS)
		{
			m_bFocused = true;
			return;
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			m_bFocused = false;
			return;
		}
		__super::DoEvent(event);
	}

	void LabelBox::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("texthalign")) == 0) {
			if (_tcsstr(pstrValue, _T("left")) != NULL) {
				m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_uTextStyle |= DT_LEFT;
			}
			if (_tcsstr(pstrValue, _T("center")) != NULL) {
				m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_uTextStyle |= DT_CENTER;
			}
			if (_tcsstr(pstrValue, _T("right")) != NULL) {
				m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_uTextStyle |= DT_RIGHT;
			}
		}
		else if (_tcscmp(pstrName, _T("textvalign")) == 0)
		{
			if (_tcsstr(pstrValue, _T("top")) != NULL) {
				m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
				m_uTextStyle |= DT_TOP;
			}
			if (_tcsstr(pstrValue, _T("vcenter")) != NULL) {
				m_uTextStyle &= ~(DT_TOP | DT_BOTTOM);
				m_uTextStyle |= DT_VCENTER;
			}
			if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
				m_uTextStyle &= ~(DT_TOP | DT_VCENTER);
				m_uTextStyle |= DT_BOTTOM;
			}
		}
		else if (_tcscmp(pstrName, _T("endellipsis")) == 0) {
			if (_tcscmp(pstrValue, _T("true")) == 0) m_uTextStyle |= DT_END_ELLIPSIS;
			else m_uTextStyle &= ~DT_END_ELLIPSIS;
		}
		else if (_tcscmp(pstrName, _T("font")) == 0) SetFont(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("textcolor")) == 0 || _tcscmp(pstrName, _T("normaltextcolor")) == 0) {
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetTextColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("disabledtextcolor")) == 0) {
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetDisabledTextColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("textpadding")) == 0) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetTextPadding(rcTextPadding);
		}
		else if (_tcscmp(pstrName, _T("multiline")) == 0) SetMultiLine(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("showhtml")) == 0) SetShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("enabledeffect")) == 0) SetEnabledEffect(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("enabledluminous")) == 0) SetEnabledLuminous(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("luminousfuzzy")) == 0) SetLuminousFuzzy((float)_tstof(pstrValue));
		else if (_tcscmp(pstrName, _T("gradientangle")) == 0) SetGradientAngle(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("enabledstroke")) == 0) SetEnabledStroke(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("enabledshadow")) == 0) SetEnabledShadow(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("gradientlength")) == 0) SetGradientLength(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("shadowoffset")) == 0){
			LPTSTR pstr = NULL;
			int offsetx = _tcstol(pstrValue, &pstr, 10);	ASSERT(pstr);
			int offsety = _tcstol(pstr + 1, &pstr, 10);		ASSERT(pstr);
			SetShadowOffset(offsetx, offsety);
		}
		else if (_tcscmp(pstrName, _T("textcolor1")) == 0) {
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetTextColor1(clrColor);
		}
		else if (_tcscmp(pstrName, _T("textshadowcolora")) == 0) {
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetTextShadowColorA(clrColor);
		}
		else if (_tcscmp(pstrName, _T("textshadowcolorb")) == 0) {
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetTextShadowColorB(clrColor);
		}
		else if (_tcscmp(pstrName, _T("strokecolor")) == 0) {
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetStrokeColor(clrColor);
		}
		else __super::SetAttribute(pstrName, pstrValue);
	}

	void LabelBox::PaintText(HDC hDC)
	{
		if (m_dwTextColor == 0) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if (m_dwDisabledTextColor == 0) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		RECT rc = m_rcItem;
		RECT rcTextPadding = m_rcTextPadding;
		GetManager()->GetDPIObj()->Scale(&rcTextPadding);
		rc.left += rcTextPadding.left;
		rc.right -= rcTextPadding.right;
		rc.top += rcTextPadding.top;
		rc.bottom -= rcTextPadding.bottom;

		if (!GetEnabledEffect())
		{
			if (m_sText.empty()) return;
			int nLinks = 0;
			if (IsEnabled()) {
				if (m_bShowHtml)
					Render::DrawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), m_dwTextColor, \
					NULL, NULL, nLinks, m_iFont, m_uTextStyle);
				else
					Render::DrawText(hDC, m_pManager, rc, m_sText.c_str(), m_dwTextColor, \
					m_iFont, m_uTextStyle);
			}
			else {
				if (m_bShowHtml)
					Render::DrawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), m_dwDisabledTextColor, \
					NULL, NULL, nLinks, m_iFont, m_uTextStyle);
				else
					Render::DrawText(hDC, m_pManager, rc, m_sText.c_str(), m_dwDisabledTextColor, \
					m_iFont, m_uTextStyle);
			}
		}
		else
		{
#ifdef _USE_GDIPLUS
			Font	nFont(hDC, m_pManager->GetFont(GetFont()));
			Graphics nGraphics(hDC);
			nGraphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

			StringFormat format;
			StringAlignment sa = StringAlignment::StringAlignmentNear;
			if ((m_uTextStyle & DT_VCENTER) != 0) sa = StringAlignment::StringAlignmentCenter;
			else if ((m_uTextStyle & DT_BOTTOM) != 0) sa = StringAlignment::StringAlignmentFar;
			format.SetLineAlignment((StringAlignment)sa);
			sa = StringAlignment::StringAlignmentNear;
			if ((m_uTextStyle & DT_CENTER) != 0) sa = StringAlignment::StringAlignmentCenter;
			else if ((m_uTextStyle & DT_RIGHT) != 0) sa = StringAlignment::StringAlignmentFar;
			format.SetAlignment((StringAlignment)sa);

			RectF nRc((float)rc.left, (float)rc.top, (float)rc.right - rc.left, (float)rc.bottom - rc.top);
			RectF nShadowRc = nRc;
			nShadowRc.X += m_ShadowOffset.X;
			nShadowRc.Y += m_ShadowOffset.Y;

			int nGradientLength = GetGradientLength();

			if (nGradientLength == 0)
				nGradientLength = (rc.bottom - rc.top);

			LinearGradientBrush nLineGrBrushA(Point(GetGradientAngle(), 0), Point(0, nGradientLength), ARGB2Color(GetTextShadowColorA()), ARGB2Color(GetTextShadowColorB() == -1 ? GetTextShadowColorA() : GetTextShadowColorB()));
			LinearGradientBrush nLineGrBrushB(Point(GetGradientAngle(), 0), Point(0, nGradientLength), ARGB2Color(GetTextColor()), ARGB2Color(GetTextColor1() == -1 ? GetTextColor() : GetTextColor1()));

			if (GetEnabledLuminous())
			{
				// from http://bbs.csdn.net/topics/390346428
				int iFuzzyWidth = (int)(nRc.Width / GetLuminousFuzzy());
				if (iFuzzyWidth < 1) iFuzzyWidth = 1;
				int iFuzzyHeight = (int)(nRc.Height / GetLuminousFuzzy());
				if (iFuzzyHeight < 1) iFuzzyHeight = 1;
				RectF nTextRc(0.0f, 0.0f, nRc.Width, nRc.Height);

				Bitmap Bit1((INT)nRc.Width, (INT)nRc.Height);
				Graphics g1(&Bit1);
				g1.SetSmoothingMode(SmoothingModeAntiAlias);
				g1.SetTextRenderingHint(TextRenderingHintAntiAlias);
				g1.SetCompositingQuality(CompositingQualityAssumeLinear);
				Bitmap Bit2(iFuzzyWidth, iFuzzyHeight);
				Graphics g2(&Bit2);
				g2.SetInterpolationMode(InterpolationModeHighQualityBicubic);
				g2.SetPixelOffsetMode(PixelOffsetModeNone);

				FontFamily ftFamily;
				nFont.GetFamily(&ftFamily);
				int iLen = wcslen(m_pWideText);
				g1.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushB);

				g2.DrawImage(&Bit1, 0, 0, (int)iFuzzyWidth, (int)iFuzzyHeight);
				g1.Clear(Color(0));
				g1.DrawImage(&Bit2, (int)m_ShadowOffset.X, (int)m_ShadowOffset.Y, (int)nRc.Width, (int)nRc.Height);
				g1.SetTextRenderingHint(TextRenderingHintAntiAlias);

				nGraphics.DrawImage(&Bit1, nRc.X, nRc.Y);
			}

			if (GetEnabledStroke() && GetStrokeColor() > 0)
			{
				LinearGradientBrush nLineGrBrushStroke(Point(GetGradientAngle(), 0), Point(0, rc.bottom - rc.top + 2), ARGB2Color(GetStrokeColor()), ARGB2Color(GetStrokeColor()));
#ifdef _UNICODE
				nRc.Offset(-1, 0);
				nGraphics.DrawString(m_sText.c_str(), m_sText.length(), &nFont, nRc, &format, &nLineGrBrushStroke);
				nRc.Offset(2, 0);
				nGraphics.DrawString(m_sText.c_str(), m_sText.length(), &nFont, nRc, &format, &nLineGrBrushStroke);
				nRc.Offset(-1, -1);
				nGraphics.DrawString(m_sText.c_str(), m_sText.length(), &nFont, nRc, &format, &nLineGrBrushStroke);
				nRc.Offset(0, 2);
				nGraphics.DrawString(m_sText.c_str(), m_sText.length(), &nFont, nRc, &format, &nLineGrBrushStroke);
				nRc.Offset(0, -1);
#else
				int iLen = wcslen(m_pWideText);
				nRc.Offset(-1, 0);
				nGraphics.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushStroke);
				nRc.Offset(2, 0);
				nGraphics.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushStroke);
				nRc.Offset(-1, -1);
				nGraphics.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushStroke);
				nRc.Offset(0, 2);
				nGraphics.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushStroke);
				nRc.Offset(0, -1);
#endif	
			}
#ifdef _UNICODE
			if (GetEnabledShadow() && (GetTextShadowColorA() > 0 || GetTextShadowColorB() > 0))
				nGraphics.DrawString(m_sText.c_str(), m_sText.length(), &nFont, nShadowRc, &format, &nLineGrBrushA);

			nGraphics.DrawString(m_sText.c_str(), m_sText.length(), &nFont, nRc, &format, &nLineGrBrushB);
#else
			int iLen = wcslen(m_pWideText);
			if (GetEnabledShadow() && (GetTextShadowColorA() > 0 || GetTextShadowColorB() > 0))
				nGraphics.DrawString(m_pWideText, iLen, &nFont, nShadowRc, &format, &nLineGrBrushA);

			nGraphics.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushB);
#endif
#endif
		}
	}

	void LabelBox::SetShadowOffset(int _offset, int _angle)
	{
		if (_angle > 180 || _angle < -180) return;

		RECT rc = m_rcItem;
		if (_angle >= 0 && _angle <= 180) rc.top -= _offset;
		else if (_angle > -180 && _angle < 0) rc.top += _offset;

		if (_angle > -90 && _angle <= 90) rc.left -= _offset;
		else if (_angle > 90 || _angle < -90) rc.left += _offset;

		m_ShadowOffset.X = (float)rc.top;
		m_ShadowOffset.Y = (float)rc.left;
		Invalidate();
	}

	RectF LabelBox::GetShadowOffset()
	{
		return m_ShadowOffset;
	}

	void LabelBox::SetEnabledEffect(bool _EnabledEffect)
	{
		m_EnableEffect = _EnabledEffect;
		if (m_EnableEffect) {
#ifdef _UNICODE
			m_pWideText = (LPWSTR)m_sText.c_str();
#else 
			int iLen = m_sText.GetLength();
			if (m_pWideText) delete[] m_pWideText;
			m_pWideText = new WCHAR[iLen + 1];
			::ZeroMemory(m_pWideText, (iLen + 1) * sizeof(WCHAR));
			::MultiByteToWideChar(CP_ACP, 0, m_sText.c_str(), -1, (LPWSTR)m_pWideText, iLen);
#endif
		}
		Invalidate();
	}

	bool LabelBox::GetEnabledEffect()
	{
		return m_EnableEffect;
	}

	void LabelBox::SetEnabledLuminous(bool bEnableLuminous)
	{
		m_bEnableLuminous = bEnableLuminous;
		Invalidate();
	}

	bool LabelBox::GetEnabledLuminous()
	{
		return m_bEnableLuminous;
	}

	void LabelBox::SetLuminousFuzzy(float fFuzzy)
	{
		if (fFuzzy < 0.0001f) return;
		m_fLuminousFuzzy = fFuzzy;
		Invalidate();
	}

	float LabelBox::GetLuminousFuzzy()
	{
		return m_fLuminousFuzzy;
	}

	void LabelBox::SetTextColor1(DWORD _TextColor1)
	{
		m_dwTextColor1 = _TextColor1;
		Invalidate();
	}

	DWORD LabelBox::GetTextColor1()
	{
		return m_dwTextColor1;
	}

	void LabelBox::SetTextShadowColorA(DWORD _TextShadowColorA)
	{
		m_dwTextShadowColorA = _TextShadowColorA;
		Invalidate();
	}

	DWORD LabelBox::GetTextShadowColorA()
	{
		return m_dwTextShadowColorA;
	}

	void LabelBox::SetTextShadowColorB(DWORD _TextShadowColorB)
	{
		m_dwTextShadowColorB = _TextShadowColorB;
		Invalidate();
	}

	DWORD LabelBox::GetTextShadowColorB()
	{
		return m_dwTextShadowColorB;
	}

	void LabelBox::SetGradientAngle(int _SetGradientAngle)
	{
		m_GradientAngle = _SetGradientAngle;
		Invalidate();
	}

	int LabelBox::GetGradientAngle()
	{
		return m_GradientAngle;
	}

	void LabelBox::SetEnabledStroke(bool _EnabledStroke)
	{
		m_EnabledStroke = _EnabledStroke;
		Invalidate();
	}

	bool LabelBox::GetEnabledStroke()
	{
		return m_EnabledStroke;
	}

	void LabelBox::SetStrokeColor(DWORD _StrokeColor)
	{
		m_dwStrokeColor = _StrokeColor;
		Invalidate();
	}

	DWORD LabelBox::GetStrokeColor()
	{
		return m_dwStrokeColor;
	}

	void LabelBox::SetEnabledShadow(bool _EnabledShadowe)
	{
		m_EnabledShadow = _EnabledShadowe;
		Invalidate();
	}

	bool LabelBox::GetEnabledShadow()
	{
		return m_EnabledShadow;
	}

	void LabelBox::SetGradientLength(int _GradientLength)
	{
		m_GradientLength = _GradientLength;
		Invalidate();
	}

	int LabelBox::GetGradientLength()
	{
		return m_GradientLength;
	}

}

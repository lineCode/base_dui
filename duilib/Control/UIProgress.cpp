#include "stdafx.h"
#include "UIProgress.h"

namespace dui
{
	Progress::Progress() : m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		SetFixedHeight(12);
	}

	LPCTSTR Progress::GetClass() const
	{
		return DUI_CTR_PROGRESS;
	}

	LPVOID Progress::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_PROGRESS) == 0 ) return static_cast<Progress*>(this);
		return Label::GetInterface(pstrName);
	}

	bool Progress::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void Progress::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		Invalidate();
	}

	int Progress::GetMinValue() const
	{
		return m_nMin;
	}

	void Progress::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int Progress::GetMaxValue() const
	{
		return m_nMax;
	}

	void Progress::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int Progress::GetValue() const
	{
		return m_nValue;
	}

	void Progress::SetValue(int nValue)
	{
		m_nValue = nValue;
		if (m_nValue > m_nMax) m_nValue = m_nMax;
		if (m_nValue < m_nMin) m_nValue = m_nMin;
		Invalidate();
	}

	void Progress::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("hor")) == 0 ) SetHorizontal(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("min")) == 0 ) SetMinValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("max")) == 0 ) SetMaxValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("value")) == 0 ) SetValue(_ttoi(pstrValue));
		else Label::SetAttribute(pstrName, pstrValue);
	}

	void Progress::PaintStatusImage(HDC hDC)
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = {0};
		RECT rcItem = m_pManager->GetDPIObj()->Scale(m_rcItem);
		if( m_bHorizontal ) {
			rc.right = (m_nValue - m_nMin) * (rcItem.right - rcItem.left) / (m_nMax - m_nMin);
			rc.bottom = rcItem.bottom - rcItem.top;
		}
		else {
			rc.top = (rcItem.bottom - rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
			rc.right = rcItem.right - rcItem.left;
			rc.bottom = rcItem.bottom - rcItem.top;
		}
		m_diFore.rcDestOffset = rc;
		if( DrawImage(hDC, m_diFore) ) return;
	}
}

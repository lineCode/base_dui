#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__

#pragma once

namespace dui
{
	class DUILIB_API Progress : public Label
	{
	public:
		Progress();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		int GetMinValue() const;
		void SetMinValue(int nMin);
		int GetMaxValue() const;
		void SetMaxValue(int nMax);
		int GetValue() const;
		void SetValue(int nValue);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);

	protected:
		bool m_bHorizontal;
		int m_nMax;
		int m_nMin;
		int m_nValue;
	};

} // namespace dui

#endif // __UIPROGRESS_H__

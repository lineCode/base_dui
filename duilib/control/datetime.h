#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

namespace dui
{
	class CDateTimeWnd;

	/// 时间选择控件
	class DUILIB_API DateTime : public Label
	{
		friend class CDateTimeWnd;
	public:
		DateTime();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;
		HWND GetNativeWindow() const;

		SYSTEMTIME& GetTime();
		void SetTime(SYSTEMTIME* pst);

		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;

		void UpdateText();

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void Move(SIZE szOffset, bool bNeedInvalidate = true);

		void DoEvent(Event& event);

		//djj:format string.[20170421]
		String GetFormat();								
		void SetFormat(LPCTSTR format);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);	//djj:[20170421]

	protected:
		SYSTEMTIME m_sysTime;
		int        m_nDTUpdateFlag;
		bool       m_bReadOnly;
		String	m_sFormat;

		CDateTimeWnd* m_pWindow;
	};
}
#endif // __UIDATETIME_H__
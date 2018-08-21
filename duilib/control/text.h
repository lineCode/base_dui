#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace dui
{
	class DUILIB_API CTextUI : public Label
	{
	public:
		CTextUI();
		~CTextUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		String* GetLinkContent(int iIndex);

		void DoEvent(Event& event);

		void PaintText(HDC hDC);

	protected:
		enum { MAX_LINK = 8 };
		int m_nLinks;
		RECT m_rcLinks[MAX_LINK];
		String m_sLinks[MAX_LINK];
		int m_nHoverLink;
	};

} // namespace dui

#endif //__UITEXT_H__
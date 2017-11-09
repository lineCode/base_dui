#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace dui
{
	class DUILIB_API ChildLayout : public ScrollContainer
	{
	public:
		ChildLayout();

		void Init();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetChildLayoutXML(String pXML);
		String GetChildLayoutXML();
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual LPCTSTR GetClass() const;

	private:
		String m_pstrXMLFile;
	};
} // namespace dui
#endif // __UICHILDLAYOUT_H__

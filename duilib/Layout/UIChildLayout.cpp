#include "stdafx.h"
#include "UIChildLayout.h"

namespace dui
{
	ChildLayout::ChildLayout()
	{

	}

	void ChildLayout::Init()
	{
		if (!m_pstrXMLFile.empty())
		{
			CDialogBuilder builder;
			ScrollContainer* pChildWindow = static_cast<ScrollContainer*>(builder.Create(m_pstrXMLFile.c_str(), (UINT)0, NULL, m_pManager));
			if (pChildWindow)
			{
				this->Add(pChildWindow);
			}
			else
			{
				this->RemoveAll();
			}
		}
	}

	void ChildLayout::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if( _tcscmp(pstrName, _T("xmlfile")) == 0 )
			SetChildLayoutXML(pstrValue);
		else
			ScrollContainer::SetAttribute(pstrName,pstrValue);
	}

	void ChildLayout::SetChildLayoutXML( String pXML )
	{
		m_pstrXMLFile=pXML;
	}

	String ChildLayout::GetChildLayoutXML()
	{
		return m_pstrXMLFile;
	}

	LPVOID ChildLayout::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_CHILDLAYOUT) == 0 ) return static_cast<ChildLayout*>(this);
		return Control::GetInterface(pstrName);
	}

	LPCTSTR ChildLayout::GetClass() const
	{
		return DUI_CTR_CHILDLAYOUT;
	}
} // namespace dui

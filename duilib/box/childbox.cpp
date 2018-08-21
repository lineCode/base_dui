#include "stdafx.h"

namespace dui
{
	ChildBox::ChildBox()
	{

	}

	void ChildBox::Init()
	{
		if (!m_pstrXMLFile.empty())
		{
			CDialogBuilder builder;
			ScrollBox* pChildWindow = static_cast<ScrollBox*>(builder.Create(m_pstrXMLFile.c_str(), (UINT)0, NULL, m_pManager));
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

	void ChildBox::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if( _tcscmp(pstrName, _T("xmlfile")) == 0 )
			SetChildLayoutXML(pstrValue);
		else
			ScrollBox::SetAttribute(pstrName,pstrValue);
	}

	void ChildBox::SetChildLayoutXML( String pXML )
	{
		m_pstrXMLFile=pXML;
	}

	String ChildBox::GetChildLayoutXML()
	{
		return m_pstrXMLFile;
	}

	LPVOID ChildBox::GetInterface( LPCTSTR pstrName )
	{
		if (_tcscmp(pstrName, DUI_CTR_CHILDBOX) == 0) return static_cast<ChildBox*>(this);
		return Control::GetInterface(pstrName);
	}

	LPCTSTR ChildBox::GetClass() const
	{
		return DUI_CTR_CHILDBOX;
	}
} // namespace dui

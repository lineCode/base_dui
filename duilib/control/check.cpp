#include "stdafx.h"

namespace dui
{
	LPCTSTR CheckBtn::GetClass() const
	{
		return DUI_CTR_CHECKBOX;
	}

	LPVOID CheckBtn::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_CHECKBOX) == 0 ) return static_cast<CheckBtn*>(this);
		return OptionBtn::GetInterface(pstrName);
	}

	void CheckBtn::SetCheck(bool bCheck, bool bTriggerEvent)
	{
		Selected(bCheck, bTriggerEvent);
	}

	bool  CheckBtn::GetCheck() const
	{
		return IsSelected();
	}
}

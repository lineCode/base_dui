#include "stdafx.h"
#include "login_form.h"

using namespace DuiLib;

const LPCTSTR LoginForm::kClassName	= L"LoginForm";

LoginForm::LoginForm()
{
}

LoginForm::~LoginForm()
{

}

CDuiString LoginForm::GetSkinFolder()
{
	return L"";
}

CDuiString LoginForm::GetSkinFile()
{
	return L"login\\login.xml";
}

//ui::UILIB_RESOURCETYPE LoginForm::GetResourceType() const
//{
//	return ui::UILIB_FILE;
//}
//
//std::wstring LoginForm::GetZIPFileName() const
//{
//	return L"login.zip";
//}

LPCTSTR LoginForm::GetWindowClassName() const 
{
	return kClassName;
}

//std::wstring LoginForm::GetWindowId() const
//{
//	return kClassName;
//}

UINT LoginForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void LoginForm::InitWindow()
{
	m_pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list")));
	CListHeaderUI* pListHead = m_pList->GetHeader();
	CContainerUI* pListBody = m_pList->GetList();
	TListInfoUI* pListInfo = m_pList->GetListInfo();
	CListHeaderItemUI *pListHeadItem1 = static_cast<CListHeaderItemUI*>(m_PaintManager.FindControl(_T("list_head_label1")));

	m_pList->SetItemHLineSize(1);
	m_pList->SetItemHLineColor(0xffdddddd);
	m_pList->SetItemVLineSize(1);
	m_pList->SetItemVLineColor(0xffdddddd);

	int sepw = pListHead->GetSepWidth();
	sepw = pListHeadItem1->GetSepWidth();
	bool bval = pListHeadItem1->IsDragable();

	/*DWORD color = pListHeadItem1->GetSepColor();
	color = 0xff222222;
	pListHeadItem1->SetSepWidth(1);
	pListHeadItem1->SetSepColor(color);*/

	pListInfo = m_pList->GetListInfo();

	int i = 0;
	i++;
	
}

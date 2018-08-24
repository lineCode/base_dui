#include "stdafx.h"
#include "db_treeitem.h"
//#include "module/dragdrop/drag_drop.h"

namespace nim_comp
{
	DBTreeItem::DBTreeItem()
	{
		pt_.x = 0;
		pt_.y = 0;
	}

	DBTreeItem::~DBTreeItem()
	{

	}

	void DBTreeItem::Init(DBNode node)
	{
		db_node_ = node;

		btn_expand_ = (dui::Button*)FindSubControl(L"btn_expand");
		label_type_ = (dui::Label*)FindSubControl(L"label_type");
		label_text_ = (dui::Label*)FindSubControl(L"label_text");

		std::wstring text_w;
		nbase::win32::MBCSToUnicode(node.text, text_w);
		label_text_->SetText(text_w.c_str());

		int fixw = 9 + 2 + 5 + 5 + 16 + 16 + strlen(node.text.c_str()) * 8;
		SetFixedWidth(fixw > 240 ? fixw : 240);

		this->AttachItemClick(std::bind(&DBTreeItem::OnClick, this, std::placeholders::_1));

		if (db_node_.type == DBNODETYPE_DB)
		{
			btn_expand_->SetVisible(true);
			label_type_->SetBkImage(L"file='db_conned.png'");
		}
		else if (db_node_.type == DBNODETYPE_TABLE_F || db_node_.type == DBNODETYPE_TABLE_ITEM)
		{
			if (db_node_.type == DBNODETYPE_TABLE_F)
			{
				btn_expand_->SetVisible(true);
			}
			label_type_->SetBkImage(L"file='table.png'");
		}
		else if (db_node_.type == DBNODETYPE_VIEW_F || db_node_.type == DBNODETYPE_VIEW_ITEM)
		{
			if (db_node_.type == DBNODETYPE_VIEW_F)
			{
				btn_expand_->SetVisible(true);
			}
			label_type_->SetBkImage(L"file='view.png'");
		}
		else if (db_node_.type == DBNODETYPE_FUNC_F || db_node_.type == DBNODETYPE_FUNC_ITEM)
		{
			if (db_node_.type == DBNODETYPE_FUNC_F)
			{
				btn_expand_->SetVisible(true);
			}
			label_type_->SetBkImage(L"file='func.png'");
		}
		else if (db_node_.type == DBNODETYPE_PROC_F || db_node_.type == DBNODETYPE_PROC_ITEM)
		{
			if (db_node_.type == DBNODETYPE_PROC_F)
			{
				btn_expand_->SetVisible(true);
			}
			label_type_->SetBkImage(L"file='proc.png'");
		}
	}

	//void DBTreeItem::HandleMessage(ui::EventArgs& msg)
	//{
	//	if (msg.Type == ui::kEventMouseButtonDown)
	//	{
	//		pt_ = msg.ptMouse;
	//	}
	//	else if (msg.Type == ui::kEventMouseMove && msg.wParam == 1)	//Êó±ê×ó¼ü°´ÏÂ
	//	{
	//		if (pt_.x != msg.ptMouse.x || pt_.y != msg.ptMouse.y)
	//		{
	//			OnDragBegin();
	//			SetState(ui::kControlStateNormal);
	//		}
	//	}
	//	return __super::HandleMessage(msg);
	//}

	bool DBTreeItem::OnClick(dui::Event* msg)
	{
		if (GetChildCount() > 0)
		{
			SetNodeExpand(!GetNodeExpand());
			if (GetNodeExpand())
			{
				btn_expand_->SetBkImage(L"file='btn_expended.png'");
			}
			else
			{
				btn_expand_->SetBkImage(L"file='btn_unexpended.png'");
			}
		}
		return false;
	}

	/*void DBTreeItem::OnDragBegin()
	{
		IDropSource *source = new SdkDropSource;
		IDataObject *dataobject = new SdkDataObject;
		HGLOBAL hG = NULL;

		std::wstring str = label_text_->GetText();
		int  strBytes = str.length() * sizeof(wchar_t);
		wchar_t *temp = new wchar_t[str.length() + 1];
		ZeroMemory(temp, strBytes + sizeof(wchar_t));
		memcpy(temp, str.c_str(), strBytes);
		hG = GlobalAlloc(GMEM_DDESHARE, strBytes + sizeof(wchar_t));
		void* pBuffer = GlobalLock(hG);
		{
			memcpy(pBuffer, temp, strBytes + sizeof(wchar_t));
			GlobalUnlock(hG);
		}
		delete[] temp;

		FORMATETC fmt;
		fmt.cfFormat = CF_UNICODETEXT;
		fmt.dwAspect = DVASPECT_CONTENT;
		fmt.lindex = -1;
		fmt.ptd = NULL;
		fmt.tymed = TYMED_HGLOBAL;

		STGMEDIUM stg;
		stg.tymed = TYMED_HGLOBAL;
		stg.hGlobal = hG;
		stg.pUnkForRelease = NULL;

		dataobject->SetData(&fmt, &stg, FALSE);

		DWORD dwEff = DROPEFFECT_NONE;
		LRESULT ret = ::DoDragDrop(dataobject, source, DROPEFFECT_COPY, &dwEff);
		printf("DBTreeItem::OnDragBegin\n");
		return;
	}*/
}
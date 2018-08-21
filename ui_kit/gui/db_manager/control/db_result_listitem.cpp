#include "stdafx.h"
#include "db_result_listitem.h"

namespace nim_comp
{
	DBResultListItem::DBResultListItem(DB_LISTITEM_TYPE t) : type_(t), column_num_(0), width_(0), height_(DB_LIST_ROW_HEIGHT)
	{
		memset(columns_, 0, sizeof(columns_));
		SetFixedHeight(height_);
	}

	DBResultListItem::~DBResultListItem()
	{
		/*for (int i = column_num_-1; i >= 0; i--)
		{
			if (columns_[i])
			{
				delete columns_[i];
				columns_[i] = NULL;
			}
		}*/
	}

	void DBResultListItem::InitCtrl(DB_LISTITEM_TYPE t)
	{
		//this->AttachClick(nbase::Bind(&DBResultListItem::OnClicked, this, std::placeholders::_1));

		if (t == DB_LISTITEM_NORMAL)
		{
			//this->AttachMenu(nbase::Bind(&SessionItem::OnSessionItemMenu, this, std::placeholders::_1));
		}
		else if (t == DB_LISTITEM_HEADER)
		{

		}
	}

	bool DBResultListItem::AddColumn(std::wstring text)
	{
		if (column_num_ >= DB_LIST_MAX_COLUMN)
		{
			return false;
		}
		if (type_ == DB_LISTITEM_NORMAL)
		{
			dui::Label *label = new dui::Label;
			//label->SetBkImage(L"file='item.png' corner='2,2,2,2'");
			//label->SetBkColor(L"bk_db_result_header");

			//int w = DB_LIST_COL_WIDTH;
			label->SetFixedWidth(DB_LIST_COL_WIDTH);
			label->SetFixedHeight(DB_LIST_ROW_HEIGHT);
			label->SetMargin(dui::DuiRect(width_, 0, 0, 0));

			label->SetTextStyle(DT_LEFT | DT_VCENTER);
			label->SetText(text.c_str());

			if (Add(label) == false)
			{
				delete label;
				return false;
			}

			columns_[column_num_++] = label;
			width_ += DB_LIST_COL_WIDTH;
		}
		else if (type_ == DB_LISTITEM_HEADER)
		{
			dui::Label *label = new dui::Label;
			//label->SetBkImage(L"file='header.png' corner='2,2,2,2'");
			//label->SetBkColor(L"bk_db_result_header");

			//int w = DB_LIST_COL_WIDTH;
			label->SetFixedWidth(DB_LIST_COL_WIDTH);
			label->SetFixedHeight(DB_LIST_ROW_HEIGHT);
			label->SetMargin(dui::DuiRect(width_, 0, 0, 0));

			label->SetTextStyle(DT_LEFT | DT_VCENTER);
			label->SetText(text.c_str());

			if (Add(label) == false)
			{
				delete label;
				return false;
			}

			columns_[column_num_++] = label;
			width_ += DB_LIST_COL_WIDTH;
		}
		else
			return false;
		return true;
	}
}
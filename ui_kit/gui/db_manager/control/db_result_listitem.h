#pragma once

namespace nim_comp
{
#define DB_LIST_MAX_COLUMN	128
#define DB_LIST_ROW_HEIGHT	26
#define DB_LIST_COL_WIDTH	118
	class DBResultListItem : public dui::ListContainerElement
	{
	public:
		typedef enum DBResultListItem_MODE
		{
			DB_LISTITEM_NORMAL = 0,
			DB_LISTITEM_HEADER
		}DB_LISTITEM_TYPE;

		DBResultListItem(DB_LISTITEM_TYPE t);
		virtual ~DBResultListItem();

		void InitCtrl(DB_LISTITEM_TYPE t = DB_LISTITEM_NORMAL);

		bool AddColumn(std::wstring text);
	private:
		DB_LISTITEM_TYPE	type_;
		int		width_;
		int		height_;

		int column_num_;
		dui::Label	*columns_[DB_LIST_MAX_COLUMN];
		
	};
}


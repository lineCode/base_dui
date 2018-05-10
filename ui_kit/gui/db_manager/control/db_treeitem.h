#pragma once

#include "../treestruct/db_treestruct.h"

namespace nim_comp
{
	
	class DBTreeItem : public dui::TreeNode
	{
	public:
		DBTreeItem();
		virtual ~DBTreeItem();
		void Init(DBNode node);

	protected:
		//virtual void HandleMessage(ui::EventArgs& msg) override;

	private:
		//bool OnClick(dui::EventArgs* msg);
		void OnDragBegin();
	private:
		DBNode db_node_;

		dui::Button *btn_expand_;
		dui::Label *label_type_;
		dui::Label *label_text_;

		POINT pt_;
	};
}
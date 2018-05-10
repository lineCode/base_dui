#ifndef DB_TREE_STRUCT_H_
#define DB_TREE_STRUCT_H_

namespace nim_comp
{
	enum DBNodeType
	{
		DBNODETYPE_ROOT = 0,
		DBNODETYPE_DB,			//下拉节点
		DBNODETYPE_TABLE_F,
		DBNODETYPE_VIEW_F,			
		DBNODETYPE_FUNC_F,
		DBNODETYPE_PROC_F,
		DBNODETYPE_TABLE_ITEM,			//具体节点
		DBNODETYPE_VIEW_ITEM,
		DBNODETYPE_FUNC_ITEM,
		DBNODETYPE_PROC_ITEM,
	};

	struct DBNode
	{
		std::string text;
		DBNodeType type;
		DBNode()
		{
			type = DBNODETYPE_ROOT;
		}
	};

	struct DBTreeNode
	{
		std::string id_;
		DBNode date_;
		DBTreeNode *next_;
		DBTreeNode *child1_;		//first child

		DBTreeNode(DBNode date)
		{
			id_ = "0";
			date_ = date;
			next_ = NULL;
			child1_ = NULL;
		}
		~DBTreeNode()
		{
			DBTreeNode *child = child1_, *child_del;
			while (child)
			{
				child_del = child;
				child = child->next_;
				delete child_del;
			}
		}
	};

	class DBTreeStruct
	{	
	public:		
		DBTreeStruct(DBTreeNode *root);
		~DBTreeStruct();

		DBTreeNode *GetRoot(){
			return root_;
		};

	public:
		//bool Insert(TemplateTreeNode<T> node);
		bool Insert(DBTreeNode *node, DBTreeNode *fnode);

	private:
		DBTreeNode *root_;

		//DBTreeNode *db_node_;
	};
}


#endif


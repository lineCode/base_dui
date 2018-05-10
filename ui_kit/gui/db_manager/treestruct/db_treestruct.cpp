#include "stdafx.h"
#include "db_treestruct.h"

namespace nim_comp
{
	
	DBTreeStruct::DBTreeStruct(DBTreeNode *root) :root_(root)
	{

	}

	DBTreeStruct::~DBTreeStruct()
	{
		if (root_)
		{
			delete root_;
			root_ = NULL;
		}
	}

	bool DBTreeStruct::Insert(DBTreeNode *node, DBTreeNode *fnode)
	{
		if (root_ == nullptr)
		{
			return false;
		}
		if (fnode == nullptr)
		{
			fnode = root_;
		}
		if (fnode->child1_)
		{
			DBTreeNode *n = fnode->child1_;
			while (n)
			{
				if (n->next_ == NULL)
				{
					n->next_ = node;
					break;
				}
				n = n->next_;
			}
		}
		else
			fnode->child1_ = node;
		return true;
	}
}
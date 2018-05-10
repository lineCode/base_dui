#ifndef TREE_STRUCT_H_
#define TREE_STRUCT_H_

#include <vector>


	/*------why????????
	没定义 用不了
	*/
namespace nim_comp
{
	struct DBNode
	{
		std::string text;
		int type;		//0-root;1-db;2-table
	};

	template <typename T>
	struct TemplateTreeNode
	{
		std::string id_;
		T date_;
		TemplateTreeNode<T> *next_;
		TemplateTreeNode<T> *child1_;		//first child

		TemplateTreeNode(T date)
		{
			id_ = "0";
			date_ = date;
			next_ = NULL;
			child1_ = NULL;
		}
	};

	template <typename T>
	class TemplateTreeView
	{	
	public:		
		TemplateTreeView(TemplateTreeNode<T> *root);
		~TemplateTreeView();

	public:
		//bool Insert(TemplateTreeNode<T> node);
		bool Insert(TemplateTreeNode<T> *node, TemplateTreeNode<T> *fnode);

	private:
		TemplateTreeNode<T> *root_;
	};
}


#endif


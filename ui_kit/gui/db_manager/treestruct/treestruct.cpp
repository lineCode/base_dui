#include "treestruct.h"

namespace nim_comp
{
	template <typename T>
	TemplateTreeView<T>::TemplateTreeView(TemplateTreeNode<T> *root) :root_(root)
	{

	}

	template <typename T>
	TemplateTreeView<T>::~TemplateTreeView()
	{

	}

	template <typename T>
	bool TemplateTreeView<T>::Insert(TemplateTreeNode<T> *node, TemplateTreeNode<T> *fnode)
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
			fnode->child1_->next_ = node;
		}
		else
			fnode->child1_ = node;
		return true;
	}
}
#ifndef __UITREEVIEW_H__
#define __UITREEVIEW_H__

#include <vector>
using namespace std;

#pragma once

namespace dui
{
	class Tree;
	class DUILIB_API TreeNode : public ListContainerElement
	{
	public:
		TreeNode(TreeNode* parent_node = NULL);
		~TreeNode(void);

	public:
		LPCTSTR GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void	DoEvent(TEvent& event);
		void	Invalidate();
		bool	Select(bool bSelect = true, bool bTriggerEvent = true);

		void SetNodeExpand(bool bExpanded);
		bool GetNodeExpand();
		void SetParentNode(TreeNode* parent_node);
		TreeNode* GetParentNode();
		/*void	SetVisibleTag(bool visible);
		bool	GetVisibleTag();*/
		void	SetTreeView(Tree* tree);
		Tree* GetTreeView();
		PtrArray GetTreeNodes();
		long	GetTreeLevel() const;

		long	GetChildCount();
		bool	HasChild() const;
		TreeNode* GetChildNode(int iIndex);

		bool	AddChildNode(TreeNode* node);
		bool	RemoveChildNode(TreeNode* node);

		int		GetIndexFromTree();
		int		GetIndexFromNode();
	protected:
		bool	Add(Control* pControl);
		bool	AddAt(Control* pControl, int iIndex);
		TreeNode* GetLastNode();
		TreeNode* CalLocation(TreeNode* node);
	protected:
		long	m_iTreeLavel;
		//bool	m_bIsVisable;		//internal visible?
		bool	m_bNodeExpanded;	//ÊÇ·ñÕ¹¿ª

		Tree*			m_pTreeView;
		TreeNode*		m_pParentTreeNode;
		PtrArray		m_aTreeNodes;
	};

	class DUILIB_API Tree : public List, public INotify
	{
	public:
		Tree(void);
		~Tree(void);

	public:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID	GetInterface(LPCTSTR pstrName);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void Notify(TEvent& msg);

		//void SetNodeExpand(TreeNode* node, bool bExpanded);

		bool AddChildNode(TreeNode* pNode);
        bool AddChildNodeAt(TreeNode* pNode, int iIndex);
		bool AddChildNodeAt(TreeNode* pNode, TreeNode* pIndexNode);
		bool RemoveChildNode(TreeNode* pNode);

		virtual void RemoveAll();
	protected:
		virtual bool Add(Control* pControl);
		virtual bool AddAt(Control* pControl, int iIndex);
		virtual bool Remove(Control* pControl, bool bDoNotDestroy = false);
		virtual bool RemoveAt(int iIndex, bool bDoNotDestroy = false);
		

	protected:
		TreeNode *m_pVirtualRoot;

	};
}


#endif // __UITREEVIEW_H__

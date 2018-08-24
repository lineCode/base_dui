#ifndef __UITREEVIEW_H__
#define __UITREEVIEW_H__
#pragma once

#include <vector>
using namespace std;

namespace dui
{
	class Tree;
	class DUILIB_API TreeNode : public ListContainerElement
	{
	public:
		TreeNode();
		~TreeNode(void);

	public:
		LPCTSTR GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void	DoEvent(Event& event);
		void	Invalidate();
		bool	Select(bool bSelect = true, bool bTriggerEvent = true);

		void	SetNodeExpand(bool bExpanded);
		bool	GetNodeExpand();
		void	SetDepth(long depth){ m_iDepth = depth; };
		long	GetDepth(){ return m_iDepth; };
		void	SetParentNode(TreeNode* parent_node);
		TreeNode* GetParentNode();
		/*void	SetVisibleTag(bool visible);
		bool	GetVisibleTag();*/
		void	SetTreeView(Tree* tree);
		Tree*	GetTreeView();
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
		long	m_iDepth = 0;
		//bool	m_bIsVisable;		//internal visible?
		bool	m_bExpanded;	//是否展开

		Tree*			m_pOwnerTree;
		TreeNode*		m_pParentNode;
		PtrArray		m_ChildNodes;
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
		virtual void Notify(Event& msg);

		//void SetNodeExpand(TreeNode* node, bool bExpanded);

		bool AddChildNode(TreeNode* pNode);
        bool AddChildNodeAt(TreeNode* pNode, int iIndex);
		bool AddChildNodeAt(TreeNode* pNode, TreeNode* pIndexNode);
		bool RemoveChildNode(TreeNode* pNode);
		virtual void RemoveAll();

		void SetIndent(int indent){ m_iIndent = indent; };
		int GetIndent(){ return m_iIndent; };

	protected:	//不允许外部调用
		virtual bool Add(Control* pControl) override;
		virtual bool AddAt(Control* pControl, int iIndex) override;
		virtual bool Remove(Control* pControl, bool bDoNotDestroy = false) override;
		virtual bool RemoveAt(int iIndex, bool bDoNotDestroy = false) override;
		
	protected:
	private:
		int m_iIndent = 0;
	};
}


#endif // __UITREEVIEW_H__

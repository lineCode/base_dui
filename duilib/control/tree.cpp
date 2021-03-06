#include "stdafx.h"

#pragma warning( disable: 4251 )
namespace dui
{
	TreeNode::TreeNode()
	{
		//m_bIsVisable = true;
		m_bExpanded = true;
		m_pOwnerTree = NULL;
		m_pParentNode = NULL;

		this->SetFixedHeight(18);
		this->SetFixedWidth(250);
	}
	
	TreeNode::~TreeNode( void )
	{

	}

	LPCTSTR TreeNode::GetClass() const
	{
		return DUI_CTR_TREENODE;
	}

	LPVOID TreeNode::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_TREENODE) == 0 ) return static_cast<TreeNode*>(this);
		return __super::GetInterface(pstrName);
	}

	void TreeNode::DoEvent( Event& event )
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else __super::DoEvent(event);
			return;
		}

		__super::DoEvent(event);

		if( event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {
				m_pManager->SendNotify(this, UIEVENT_DBLCLICK);
				Invalidate();
			}
			return;
		}
	}

	void TreeNode::Invalidate()
	{
		if( !IsVisible() )
			return;

		if( GetParent() ) {
			ScrollBox* pParentContainer = static_cast<ScrollBox*>(GetParent()->GetInterface(DUI_CTR_SCROLLBOX));
			if( pParentContainer ) {
				RECT rc = pParentContainer->GetPos();
				RECT rcPadding = pParentContainer->GetPadding();
				rc.left += rcPadding.left;
				rc.top += rcPadding.top;
				rc.right -= rcPadding.right;
				rc.bottom -= rcPadding.bottom;
				ScrollBar* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if (pVerticalScrollBar && pVerticalScrollBar->IsVisible() && !pParentContainer->GetScrollBarFloat()) rc.right -= pVerticalScrollBar->GetFixedWidth();
				ScrollBar* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() && !pParentContainer->GetScrollBarFloat()) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = m_rcItem;
				if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
					return;

				Control* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while( pParent = pParent->GetParent() )
				{
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
						return;
				}

				if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
			}
			else {
				__super::Invalidate();
			}
		}
		else {
			__super::Invalidate();
		}
	}
	
	bool TreeNode::Select( bool bSelect /*= true*/,  bool bTriggerEvent)
	{
		bool nRet = ListContainerElement::Select(bSelect, bTriggerEvent);
		return nRet;
	}

	void TreeNode::SetNodeExpand(bool bExpanded)
	{
		m_bExpanded = bExpanded;

		int nCount = GetChildCount();
		for (int iIndex = 0; iIndex < nCount; iIndex++)
		{
			TreeNode* pItem = GetChildNode(iIndex);
			pItem->SetVisible(bExpanded);
			pItem->SetNodeExpand(bExpanded);
		}
	}

	bool TreeNode::GetNodeExpand()
	{
		return m_bExpanded;
	}

	bool TreeNode::Add(Control* pControl)
	{
	/*	if (_tcsicmp(pControl->GetClass(), DUI_CTR_TREENODE) == 0)
			return AddChildNode((TreeNode*)pControl);*/

		return __super::Add(pControl);
	}

	bool TreeNode::AddAt( Control* pControl, int iIndex )
	{
		if(NULL == static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE)))
			return false;

		TreeNode* pIndexNode = static_cast<TreeNode*>(m_ChildNodes.GetAt(iIndex));
		if(!pIndexNode){
			if(!m_ChildNodes.Add(pControl))
				return false;
		}
		else if(pIndexNode && !m_ChildNodes.InsertAt(iIndex,pControl))
			return false;

		if(!pIndexNode && m_pOwnerTree && m_pOwnerTree->GetItemAt(GetIndexFromTree()+1))
			pIndexNode = static_cast<TreeNode*>(m_pOwnerTree->GetItemAt(GetIndexFromTree()+1)->GetInterface(DUI_CTR_TREENODE));

		pControl = CalLocation((TreeNode*)pControl);

		if(m_pOwnerTree && pIndexNode)
			return m_pOwnerTree->AddChildNodeAt((TreeNode*)pControl,pIndexNode);
		else 
			return m_pOwnerTree->AddChildNode((TreeNode*)pControl);

		return true;
	}

	/*void TreeNode::SetVisibleTag( bool visible )
	{
		m_bIsVisable = visible;
	}

	bool TreeNode::GetVisibleTag()
	{
		return m_bIsVisable;
	}*/

	bool TreeNode::HasChild() const
	{
		return !m_ChildNodes.IsEmpty();
	}
	
	bool TreeNode::AddChildNode(TreeNode* node)
	{
		if (!node)
			return false;

		if (_tcsicmp(node->GetClass(), DUI_CTR_TREENODE) != 0)
			return false;

		node = CalLocation(node);

		bool nRet = true;

		if(m_pOwnerTree){
			TreeNode* pNode = static_cast<TreeNode*>(m_ChildNodes.GetAt(m_ChildNodes.GetSize()-1));
			if(!pNode || !pNode->GetLastNode())
				nRet = (m_pOwnerTree->AddChildNodeAt(node, GetIndexFromTree() + 1) >= 0);
			else nRet = (m_pOwnerTree->AddChildNodeAt(node, pNode->GetLastNode()->GetIndexFromTree() + 1) >= 0);
		}

		if(nRet)
			m_ChildNodes.Add(node);

		return nRet;
	}

	bool TreeNode::RemoveChildNode(TreeNode* node)
	{
		int iIndex = m_ChildNodes.Find(node);
		TreeNode* pNode = static_cast<TreeNode*>(m_ChildNodes.GetAt(iIndex));
		if (pNode && pNode == node)
		{
			while(pNode->HasChild())
				pNode->RemoveChildNode(static_cast<TreeNode*>(pNode->m_ChildNodes.GetAt(0)));

			m_ChildNodes.Remove(iIndex);

			if(m_pOwnerTree)
				m_pOwnerTree->RemoveChildNode(node);

			return true;
		}
		return false;
	}

	void TreeNode::SetParentNode( TreeNode* parent_node )
	{
		m_pParentNode = parent_node;
	}

	TreeNode* TreeNode::GetParentNode()
	{
		return m_pParentNode;
	}

	long TreeNode::GetChildCount()
	{
		return m_ChildNodes.GetSize();
	}

	void TreeNode::SetTreeView( Tree* tree )
	{
		m_pOwnerTree = tree;
	}

	Tree* TreeNode::GetTreeView()
	{
		return m_pOwnerTree;
	}

	void TreeNode::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		ListContainerElement::SetAttribute(pstrName,pstrValue);
	}

	PtrArray TreeNode::GetTreeNodes()
	{
		return m_ChildNodes;
	}

	TreeNode* TreeNode::GetChildNode( int iIndex )
	{
		return static_cast<TreeNode*>(m_ChildNodes.GetAt(iIndex));
	}

	int TreeNode::GetIndexFromTree()
	{
		if(!m_pOwnerTree)
			return -1;

		for(int iIndex = 0;iIndex < m_pOwnerTree->GetCount();iIndex++){
			if(this == m_pOwnerTree->GetItemAt(iIndex))
				return iIndex;
		}

		return -1;
	}
	
	//************************************
	// 函数名称: GetIndexFromNode
	// 返回类型: int
	// 函数说明: 取得相对于兄弟节点的当前索引
	//************************************
	int TreeNode::GetIndexFromNode()
	{
		if(!GetParentNode() && !m_pOwnerTree)
			return -1;

		if(!GetParentNode() && m_pOwnerTree)
			return GetIndexFromTree();

		return GetParentNode()->GetTreeNodes().Find(this);
	}

	TreeNode* TreeNode::GetLastNode( )
	{
		if(!HasChild())
			return this;

		TreeNode* nRetNode = NULL;

		for(int iIndex = 0;iIndex < GetTreeNodes().GetSize();iIndex++){
			TreeNode* pNode = static_cast<TreeNode*>(GetTreeNodes().GetAt(iIndex));
			if(!pNode)
				continue;

			if(pNode->HasChild())
				nRetNode = pNode->GetLastNode();
			else 
				nRetNode = pNode;
		}
		
		return nRetNode;
	}
	
	TreeNode* TreeNode::CalLocation(TreeNode* node)
	{
		node->SetParentNode(this);
		node->SetTreeView(m_pOwnerTree);
		node->SetDepth(m_iDepth + 1);
#if 1	//缩进是这么处理还是重载SetPos中处理？asked by djj
		if (m_pOwnerTree->GetIndent() > 0)
		{
			node->SetFixedWidth(node->GetFixedWidth() + node->GetDepth() * m_pOwnerTree->GetIndent());
			RECT rcPadding = node->GetPadding();
			rcPadding.left += (node->GetDepth() * m_pOwnerTree->GetIndent());
			node->SetPadding(rcPadding);
		}
#endif
		return node;
	}

	/*****************************************************************************/
	/***********************************Tree**********************************/
	/*****************************************************************************/
	Tree::Tree(void) 
	{
	}
	
	Tree::~Tree( void )
	{
	}

	LPCTSTR Tree::GetClass() const
	{
		return DUI_CTR_TREE;
	}

	LPVOID Tree::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_TREE) == 0 ) return static_cast<Tree*>(this);
		return __super::GetInterface(pstrName);
	}

	bool Tree::Add(Control* pControl)
	{
		if (!pControl) return false;

        TreeNode* pTreeNode = static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE));
        if (pTreeNode == NULL) return false;

#if 0
		return m_pVirtualRoot->AddChildNode(pTreeNode);
#else
		__super::Add(pTreeNode);

		if(pTreeNode->GetChildCount() > 0)
		{
			int nCount = pTreeNode->GetChildCount();
			for(int iIndex = 0;iIndex < nCount;iIndex++)
			{
				TreeNode* pNode = pTreeNode->GetChildNode(iIndex);
				if(pNode) Add(pNode);
			}
		}

		pTreeNode->SetTreeView(this);
		return true;
#endif
	}

    bool Tree::AddAt(Control* pControl, int iIndex)
    {
        if (!pControl) return false;

        TreeNode* pTreeNode = static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE));
        if (pTreeNode == NULL) return false;
        return (AddAt(pTreeNode, iIndex) >= 0);
    }

	bool Tree::Remove(Control* pControl, bool bDoNotDestroy)
	{
        if (!pControl) return false;

        TreeNode* pTreeNode = static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE));
        if (pTreeNode == NULL) return __super::Remove(pControl, bDoNotDestroy);

		if(pTreeNode->GetChildCount() > 0)
		{
			int nCount = pTreeNode->GetChildCount();
			for(int iIndex = 0;iIndex < nCount;iIndex++)
			{
				TreeNode* pNode = pTreeNode->GetChildNode(iIndex);
				if(pNode){
					pTreeNode->Remove(pNode, true);
				}
			}
		}
		return __super::Remove(pControl, bDoNotDestroy);
	}

	bool Tree::RemoveAt( int iIndex, bool bDoNotDestroy )
	{
        Control* pControl = GetItemAt(iIndex);
        if (pControl == NULL) return false;

        TreeNode* pTreeNode = static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE));
		if (pTreeNode == NULL) return __super::Remove(pControl, bDoNotDestroy);

		return Remove(pTreeNode);
	}

	void Tree::RemoveAll()
	{
		__super::RemoveAll();
	}

	bool Tree::AddChildNode(TreeNode* pNode)
	{
		return Add(pNode);
	}

	bool Tree::AddChildNodeAt(TreeNode* pNode, int iIndex)
    {
		if (!pNode) return false;

        TreeNode* pTreeNode = static_cast<TreeNode*>(pNode->GetInterface(DUI_CTR_TREENODE));
		if (pTreeNode == NULL) return false;

        TreeNode* pParent = static_cast<TreeNode*>(GetItemAt(iIndex - 1));
		if (!pParent) return false;
#if 0
        pTreeNode->OnNotify += MakeDelegate(this,&Tree::OnDBClickItem);
        pTreeNode->GetFolderButton()->OnNotify += MakeDelegate(this,&Tree::OnFolderChanged);
        pTreeNode->GetCheckBox()->OnNotify += MakeDelegate(this,&Tree::OnCheckBoxChanged);
#endif

		__super::AddAt(pTreeNode, iIndex);

        if(pTreeNode->GetChildCount() > 0)
        {
            int nCount = pTreeNode->GetChildCount();
            for(int iIndex = 0;iIndex < nCount;iIndex++)
            {
                TreeNode* pNode = pTreeNode->GetChildNode(iIndex);
                if(pNode)
                    return AddAt(pNode,iIndex+1);
            }
        }

		return true;
    }

	bool Tree::AddChildNodeAt(TreeNode* pNode, TreeNode* pIndexNode)
	{
		if (!pIndexNode && !pNode)
			return false;

		int nItemIndex = -1;

		for(int iIndex = 0;iIndex < GetCount();iIndex++){
			if(pIndexNode == GetItemAt(iIndex)){
				nItemIndex = iIndex;
				break;
			}
		}

		if(nItemIndex == -1)
			return false;

		return (AddAt(pNode, nItemIndex) >= 0);
	}

	bool Tree::RemoveChildNode(TreeNode* pNode)
	{
		return Remove(pNode);
	}

	void Tree::Notify( Event& msg )
	{
		
	}

	void Tree::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if (_tcscmp(pstrName, _T("indent")) == 0) 
			SetIndent(_ttoi(pstrValue));
		else
			__super::SetAttribute(pstrName, pstrValue);
	}

}
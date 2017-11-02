#include "StdAfx.h"
#include "UITreeView.h"

#pragma warning( disable: 4251 )
namespace dui
{
	//************************************
	// ��������: TreeNode
	// ��������: 
	// ������Ϣ: TreeNode * _ParentNode
	// ����˵��: 
	//************************************
	TreeNode::TreeNode( TreeNode* _ParentNode /*= NULL*/ )
	{
		m_dwItemTextColor		= 0x00000000;
		m_dwItemHotTextColor	= 0;
		m_dwSelItemTextColor	= 0;
		m_dwSelItemHotTextColor	= 0;

		pTreeView		= NULL;
		m_bIsVisable	= true;
		m_bIsCheckBox	= false;
		pParentTreeNode	= NULL;

		pHoriz			= new HorizontalLayout();
		pFolderButton	= new CheckBtn();
		pDottedLine		= new Label();
		pCheckBox		= new CheckBtn();
		pItemButton		= new OptionBtn();

		this->SetFixedHeight(18);
		this->SetFixedWidth(250);
		pFolderButton->SetFixedWidth(GetFixedHeight());
		pDottedLine->SetFixedWidth(2);
		pCheckBox->SetFixedWidth(GetFixedHeight());
		pItemButton->SetAttribute(_T("halign"),_T("left"));

		pDottedLine->SetVisible(false);
		pCheckBox->SetVisible(false);
		pItemButton->SetMouseEnabled(false);

		if(_ParentNode)
		{
			if (_tcsicmp(_ParentNode->GetClass(), DUI_CTR_TREENODE) != 0)
				return;

			pDottedLine->SetVisible(_ParentNode->IsVisible());
			pDottedLine->SetFixedWidth(_ParentNode->GetDottedLine()->GetFixedWidth()+16);
			this->SetParentNode(_ParentNode);
		}

		pHoriz->Add(pDottedLine);
		pHoriz->Add(pFolderButton);
		pHoriz->Add(pCheckBox);
		pHoriz->Add(pItemButton);
		Add(pHoriz);
	}
	
	//************************************
	// ��������: ~TreeNode
	// ��������: 
	// ������Ϣ: void
	// ����˵��: 
	//************************************
	TreeNode::~TreeNode( void )
	{

	}

	//************************************
	// ��������: GetClass
	// ��������: LPCTSTR
	// ����˵��: 
	//************************************
	LPCTSTR TreeNode::GetClass() const
	{
		return DUI_CTR_TREENODE;
	}

	//************************************
	// ��������: GetInterface
	// ��������: LPVOID
	// ������Ϣ: LPCTSTR pstrName
	// ����˵��: 
	//************************************
	LPVOID TreeNode::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_TREENODE) == 0 ) return static_cast<TreeNode*>(this);
		return ListContainerElement::GetInterface(pstrName);
	}
	
	//************************************
	// ��������: DoEvent
	// ��������: void
	// ������Ϣ: TEvent & event
	// ����˵��:
	//************************************
	void TreeNode::DoEvent( TEvent& event )
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else Container::DoEvent(event);
			return;
		}

		ListContainerElement::DoEvent(event);

		if( event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMDBCLICK);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled()) {
				if(m_bSelected && GetSelItemHotTextColor())
					pItemButton->SetTextColor(GetSelItemHotTextColor());
				else
					pItemButton->SetTextColor(GetItemHotTextColor());
			}
			else 
				pItemButton->SetTextColor(pItemButton->GetDisabledTextColor());

			//return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled()) {
				if(m_bSelected && GetSelItemTextColor())
					pItemButton->SetTextColor(GetSelItemTextColor());
				else if(!m_bSelected)
					pItemButton->SetTextColor(GetItemTextColor());
			}
			else 
				pItemButton->SetTextColor(pItemButton->GetDisabledTextColor());

			//return;
		}
	}

	//************************************
	// ��������: Invalidate
	// ��������: void
	// ����˵��: 
	//************************************
	void TreeNode::Invalidate()
	{
		if( !IsVisible() )
			return;

		if( GetParent() ) {
			Container* pParentContainer = static_cast<Container*>(GetParent()->GetInterface(DUI_CTR_CONTAINER));
			if( pParentContainer ) {
				RECT rc = pParentContainer->GetPos();
				RECT rcPadding = pParentContainer->GetPadding();
				rc.left += rcPadding.left;
				rc.top += rcPadding.top;
				rc.right -= rcPadding.right;
				rc.bottom -= rcPadding.bottom;
				ScrollBar* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
				ScrollBar* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

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
				Container::Invalidate();
			}
		}
		else {
			Container::Invalidate();
		}
	}
	
	//************************************
	// ��������: Select
	// ��������: bool
	// ������Ϣ: bool bSelect
	// ����˵��: 
	//************************************
	bool TreeNode::Select( bool bSelect /*= true*/,  bool bTriggerEvent)
	{
		bool nRet = ListContainerElement::Select(bSelect, bTriggerEvent);
		if(m_bSelected)
			pItemButton->SetTextColor(GetSelItemTextColor());
		else 
			pItemButton->SetTextColor(GetItemTextColor());

		return nRet;
	}

	//************************************
	// ��������: Add
	// ��������: bool
	// ������Ϣ: Control * _pTreeNodeUI
	// ����˵��: ͨ���ڵ������ӽڵ�
	//************************************
	bool TreeNode::Add( Control* _pTreeNodeUI )
	{
		if (_tcsicmp(_pTreeNodeUI->GetClass(), DUI_CTR_TREENODE) == 0)
			return AddChildNode((TreeNode*)_pTreeNodeUI);

		return ListContainerElement::Add(_pTreeNodeUI);
	}

	//************************************
	// ��������: AddAt
	// ��������: bool
	// ������Ϣ: Control * pControl
	// ������Ϣ: int iIndex				�ò�������Ե�ǰ�ڵ��µ��ֵ������������б���ͼ����
	// ����˵��: 
	//************************************
	bool TreeNode::AddAt( Control* pControl, int iIndex )
	{
		if(NULL == static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE)))
			return false;

		TreeNode* pIndexNode = static_cast<TreeNode*>(mTreeNodes.GetAt(iIndex));
		if(!pIndexNode){
			if(!mTreeNodes.Add(pControl))
				return false;
		}
		else if(pIndexNode && !mTreeNodes.InsertAt(iIndex,pControl))
			return false;

		if(!pIndexNode && pTreeView && pTreeView->GetItemAt(GetTreeIndex()+1))
			pIndexNode = static_cast<TreeNode*>(pTreeView->GetItemAt(GetTreeIndex()+1)->GetInterface(DUI_CTR_TREENODE));

		pControl = CalLocation((TreeNode*)pControl);

		if(pTreeView && pIndexNode)
			return pTreeView->AddAt((TreeNode*)pControl,pIndexNode);
		else 
			return pTreeView->Add((TreeNode*)pControl);

		return true;
	}

	//************************************
	// ��������: SetVisibleTag
	// ��������: void
	// ������Ϣ: bool _IsVisible
	// ����˵��: 
	//************************************
	void TreeNode::SetVisibleTag( bool _IsVisible )
	{
		m_bIsVisable = _IsVisible;
	}

	//************************************
	// ��������: GetVisibleTag
	// ��������: bool
	// ����˵��: 
	//************************************
	bool TreeNode::GetVisibleTag()
	{
		return m_bIsVisable;
	}

	//************************************
	// ��������: SetItemText
	// ��������: void
	// ������Ϣ: LPCTSTR pstrValue
	// ����˵��: 
	//************************************
	void TreeNode::SetItemText( LPCTSTR pstrValue )
	{
		pItemButton->SetText(pstrValue);
	}

	//************************************
	// ��������: GetItemText
	// ��������: UiLib::String
	// ����˵��: 
	//************************************
	String TreeNode::GetItemText()
	{
		return pItemButton->GetText();
	}

	//************************************
	// ��������: CheckBoxSelected
	// ��������: void
	// ������Ϣ: bool _Selected
	// ����˵��: 
	//************************************
	void TreeNode::CheckBoxSelected( bool _Selected )
	{
		pCheckBox->Selected(_Selected);
	}

	//************************************
	// ��������: IsCheckBoxSelected
	// ��������: bool
	// ����˵��: 
	//************************************
	bool TreeNode::IsCheckBoxSelected() const
	{
		return pCheckBox->IsSelected();
	}

	//************************************
	// ��������: IsHasChild
	// ��������: bool
	// ����˵��: 
	//************************************
	bool TreeNode::IsHasChild() const
	{
		return !mTreeNodes.IsEmpty();
	}
	
	//************************************
	// ��������: AddChildNode
	// ��������: bool
	// ������Ϣ: TreeNode * _pTreeNodeUI
	// ����˵��: 
	//************************************
	bool TreeNode::AddChildNode( TreeNode* _pTreeNodeUI )
	{
		if (!_pTreeNodeUI)
			return false;

		if (_tcsicmp(_pTreeNodeUI->GetClass(), DUI_CTR_TREENODE) != 0)
			return false;

		_pTreeNodeUI = CalLocation(_pTreeNodeUI);

		bool nRet = true;

		if(pTreeView){
			TreeNode* pNode = static_cast<TreeNode*>(mTreeNodes.GetAt(mTreeNodes.GetSize()-1));
			if(!pNode || !pNode->GetLastNode())
				nRet = pTreeView->AddAt(_pTreeNodeUI,GetTreeIndex()+1) >= 0;
			else nRet = pTreeView->AddAt(_pTreeNodeUI,pNode->GetLastNode()->GetTreeIndex()+1) >= 0;
		}

		if(nRet)
			mTreeNodes.Add(_pTreeNodeUI);

		return nRet;
	}

	//************************************
	// ��������: RemoveAt
	// ��������: bool
	// ������Ϣ: TreeNode * _pTreeNodeUI
	// ����˵��: 
	//************************************
	bool TreeNode::RemoveAt( TreeNode* _pTreeNodeUI )
	{
		int nIndex = mTreeNodes.Find(_pTreeNodeUI);
		TreeNode* pNode = static_cast<TreeNode*>(mTreeNodes.GetAt(nIndex));
		if(pNode && pNode == _pTreeNodeUI)
		{
			while(pNode->IsHasChild())
				pNode->RemoveAt(static_cast<TreeNode*>(pNode->mTreeNodes.GetAt(0)));

			mTreeNodes.Remove(nIndex);

			if(pTreeView)
				pTreeView->Remove(_pTreeNodeUI);

			return true;
		}
		return false;
	}

	//************************************
	// ��������: SetParentNode
	// ��������: void
	// ������Ϣ: TreeNode * _pParentTreeNode
	// ����˵��: 
	//************************************
	void TreeNode::SetParentNode( TreeNode* _pParentTreeNode )
	{
		pParentTreeNode = _pParentTreeNode;
	}

	//************************************
	// ��������: GetParentNode
	// ��������: TreeNode*
	// ����˵��: 
	//************************************
	TreeNode* TreeNode::GetParentNode()
	{
		return pParentTreeNode;
	}

	//************************************
	// ��������: GetCountChild
	// ��������: long
	// ����˵��: 
	//************************************
	long TreeNode::GetCountChild()
	{
		return mTreeNodes.GetSize();
	}

	//************************************
	// ��������: SetTreeView
	// ��������: void
	// ������Ϣ: TreeView * _CTreeViewUI
	// ����˵��: 
	//************************************
	void TreeNode::SetTreeView( TreeView* _CTreeViewUI )
	{
		pTreeView = _CTreeViewUI;
	}

	//************************************
	// ��������: GetTreeView
	// ��������: TreeView*
	// ����˵��: 
	//************************************
	TreeView* TreeNode::GetTreeView()
	{
		return pTreeView;
	}

	//************************************
	// ��������: SetAttribute
	// ��������: void
	// ������Ϣ: LPCTSTR pstrName
	// ������Ϣ: LPCTSTR pstrValue
	// ����˵��: 
	//************************************
	void TreeNode::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if(_tcscmp(pstrName, _T("text")) == 0 )
			pItemButton->SetText(pstrValue);
		else if(_tcscmp(pstrName, _T("horizattr")) == 0 )
			pHoriz->SetAttributeList(pstrValue);
		else if(_tcscmp(pstrName, _T("dotlineattr")) == 0 )
			pDottedLine->SetAttributeList(pstrValue);
		else if(_tcscmp(pstrName, _T("folderattr")) == 0 )
			pFolderButton->SetAttributeList(pstrValue);
		else if(_tcscmp(pstrName, _T("checkboxattr")) == 0 )
			pCheckBox->SetAttributeList(pstrValue);
		else if(_tcscmp(pstrName, _T("itemattr")) == 0 )
			pItemButton->SetAttributeList(pstrValue);
		else if(_tcscmp(pstrName, _T("itemtextcolor")) == 0 ){
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetItemTextColor(clrColor);
		}
		else if(_tcscmp(pstrName, _T("itemhottextcolor")) == 0 ){
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetItemHotTextColor(clrColor);
		}
		else if(_tcscmp(pstrName, _T("selitemtextcolor")) == 0 ){
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetSelItemTextColor(clrColor);
		}
		else if(_tcscmp(pstrName, _T("selitemhottextcolor")) == 0 ){
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetSelItemHotTextColor(clrColor);
		}
		else ListContainerElement::SetAttribute(pstrName,pstrValue);
	}

	//************************************
	// ��������: GetTreeNodes
	// ��������: UiLib::PtrArray
	// ����˵��: 
	//************************************
	PtrArray TreeNode::GetTreeNodes()
	{
		return mTreeNodes;
	}

	//************************************
	// ��������: GetChildNode
	// ��������: TreeNode*
	// ������Ϣ: int _nIndex
	// ����˵��: 
	//************************************
	TreeNode* TreeNode::GetChildNode( int _nIndex )
	{
		return static_cast<TreeNode*>(mTreeNodes.GetAt(_nIndex));
	}

	//************************************
	// ��������: SetVisibleFolderBtn
	// ��������: void
	// ������Ϣ: bool _IsVisibled
	// ����˵��: 
	//************************************
	void TreeNode::SetVisibleFolderBtn( bool _IsVisibled )
	{
		pFolderButton->SetVisible(_IsVisibled);
	}

	//************************************
	// ��������: GetVisibleFolderBtn
	// ��������: bool
	// ����˵��: 
	//************************************
	bool TreeNode::GetVisibleFolderBtn()
	{
		return pFolderButton->IsVisible();
	}

	//************************************
	// ��������: SetVisibleCheckBtn
	// ��������: void
	// ������Ϣ: bool _IsVisibled
	// ����˵��: 
	//************************************
	void TreeNode::SetVisibleCheckBtn( bool _IsVisibled )
	{
		pCheckBox->SetVisible(_IsVisibled);
	}

	//************************************
	// ��������: GetVisibleCheckBtn
	// ��������: bool
	// ����˵��: 
	//************************************
	bool TreeNode::GetVisibleCheckBtn()
	{
		return pCheckBox->IsVisible();
	}
	
	//************************************
	// ��������: GetNodeIndex
	// ��������: int
	// ����˵��: ȡ��ȫ������ͼ������
	//************************************
	int TreeNode::GetTreeIndex()
	{
		if(!pTreeView)
			return -1;

		for(int nIndex = 0;nIndex < pTreeView->GetCount();nIndex++){
			if(this == pTreeView->GetItemAt(nIndex))
				return nIndex;
		}

		return -1;
	}
	
	//************************************
	// ��������: GetNodeIndex
	// ��������: int
	// ����˵��: ȡ��������ֵܽڵ�ĵ�ǰ����
	//************************************
	int TreeNode::GetNodeIndex()
	{
		if(!GetParentNode() && !pTreeView)
			return -1;

		if(!GetParentNode() && pTreeView)
			return GetTreeIndex();

		return GetParentNode()->GetTreeNodes().Find(this);
	}

	//************************************
	// ��������: GetLastNode
	// ��������: TreeNode*
	// ����˵��:
	//************************************
	TreeNode* TreeNode::GetLastNode( )
	{
		if(!IsHasChild())
			return this;

		TreeNode* nRetNode = NULL;

		for(int nIndex = 0;nIndex < GetTreeNodes().GetSize();nIndex++){
			TreeNode* pNode = static_cast<TreeNode*>(GetTreeNodes().GetAt(nIndex));
			if(!pNode)
				continue;

			String aa = pNode->GetItemText();

			if(pNode->IsHasChild())
				nRetNode = pNode->GetLastNode();
			else 
				nRetNode = pNode;
		}
		
		return nRetNode;
	}
	
	//************************************
	// ��������: CalLocation
	// ��������: TreeNode*
	// ������Ϣ: TreeNode * _pTreeNodeUI
	// ����˵��: ��������
	//************************************
	TreeNode* TreeNode::CalLocation( TreeNode* _pTreeNodeUI )
	{
		_pTreeNodeUI->GetDottedLine()->SetVisible(true);
		_pTreeNodeUI->GetDottedLine()->SetFixedWidth(pDottedLine->GetFixedWidth()+16);
		_pTreeNodeUI->SetParentNode(this);
		_pTreeNodeUI->GetItemButton()->SetGroup(pItemButton->GetGroup());
		_pTreeNodeUI->SetTreeView(pTreeView);

		return _pTreeNodeUI;
	}

	//************************************
	// ��������: SetTextColor
	// ��������: void
	// ������Ϣ: DWORD _dwTextColor
	// ����˵��: 
	//************************************
	void TreeNode::SetItemTextColor( DWORD _dwItemTextColor )
	{
		m_dwItemTextColor	= _dwItemTextColor;
		pItemButton->SetTextColor(m_dwItemTextColor);
	}

	//************************************
	// ��������: GetTextColor
	// ��������: DWORD
	// ����˵��: 
	//************************************
	DWORD TreeNode::GetItemTextColor() const
	{
		return m_dwItemTextColor;
	}

	//************************************
	// ��������: SetTextHotColor
	// ��������: void
	// ������Ϣ: DWORD _dwTextHotColor
	// ����˵��: 
	//************************************
	void TreeNode::SetItemHotTextColor( DWORD _dwItemHotTextColor )
	{
		m_dwItemHotTextColor = _dwItemHotTextColor;
		Invalidate();
	}

	//************************************
	// ��������: GetTextHotColor
	// ��������: DWORD
	// ����˵��: 
	//************************************
	DWORD TreeNode::GetItemHotTextColor() const
	{
		return m_dwItemHotTextColor;
	}

	//************************************
	// ��������: SetSelItemTextColor
	// ��������: void
	// ������Ϣ: DWORD _dwSelItemTextColor
	// ����˵��: 
	//************************************
	void TreeNode::SetSelItemTextColor( DWORD _dwSelItemTextColor )
	{
		m_dwSelItemTextColor = _dwSelItemTextColor;
		Invalidate();
	}

	//************************************
	// ��������: GetSelItemTextColor
	// ��������: DWORD
	// ����˵��: 
	//************************************
	DWORD TreeNode::GetSelItemTextColor() const
	{
		return m_dwSelItemTextColor;
	}

	//************************************
	// ��������: SetSelHotItemTextColor
	// ��������: void
	// ������Ϣ: DWORD _dwSelHotItemTextColor
	// ����˵��: 
	//************************************
	void TreeNode::SetSelItemHotTextColor( DWORD _dwSelHotItemTextColor )
	{
		m_dwSelItemHotTextColor = _dwSelHotItemTextColor;
		Invalidate();
	}

	//************************************
	// ��������: GetSelHotItemTextColor
	// ��������: DWORD
	// ����˵��: 
	//************************************
	DWORD TreeNode::GetSelItemHotTextColor() const
	{
		return m_dwSelItemHotTextColor;
	}

	/*****************************************************************************/
	/*****************************************************************************/
	/*****************************************************************************/
	
	//************************************
	// ��������: TreeView
	// ��������: 
	// ������Ϣ: void
	// ����˵��: 
	//************************************
	TreeView::TreeView( void ) : m_bVisibleFolderBtn(true),m_bVisibleCheckBtn(false),m_uItemMinWidth(0)
	{
		this->GetHeader()->SetVisible(false);
	}
	
	//************************************
	// ��������: ~TreeView
	// ��������: 
	// ������Ϣ: void
	// ����˵��: 
	//************************************
	TreeView::~TreeView( void )
	{
		
	}

	//************************************
	// ��������: GetClass
	// ��������: LPCTSTR
	// ����˵��: 
	//************************************
	LPCTSTR TreeView::GetClass() const
	{
		return DUI_CTR_TREEVIEW;
	}

	LPVOID TreeView::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_TREEVIEW) == 0 ) return static_cast<TreeView*>(this);
		return List::GetInterface(pstrName);
	}

	bool TreeView::Add(Control* pControl)
	{
		if (!pControl) return false;

        TreeNode* pTreeNode = static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE));
        if (pTreeNode == NULL) return false;
#if !MODE_EVENTMAP
		pTreeNode->OnNotify += MakeDelegate(this,&TreeView::OnDBClickItem);
		pTreeNode->GetFolderButton()->OnNotify += MakeDelegate(this,&TreeView::OnFolderChanged);
		pTreeNode->GetCheckBox()->OnNotify += MakeDelegate(this,&TreeView::OnCheckBoxChanged);
#endif
		pTreeNode->SetVisibleFolderBtn(m_bVisibleFolderBtn);
		pTreeNode->SetVisibleCheckBtn(m_bVisibleCheckBtn);
		if(m_uItemMinWidth > 0)	pTreeNode->SetMinWidth(m_uItemMinWidth);

		List::Add(pTreeNode);

		if(pTreeNode->GetCountChild() > 0)
		{
			int nCount = pTreeNode->GetCountChild();
			for(int nIndex = 0;nIndex < nCount;nIndex++)
			{
				TreeNode* pNode = pTreeNode->GetChildNode(nIndex);
				if(pNode) Add(pNode);
			}
		}

		pTreeNode->SetTreeView(this);
		return true;
	}

    bool TreeView::AddAt(Control* pControl, int iIndex)
    {
        if (!pControl) return false;

        TreeNode* pTreeNode = static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE));
        if (pTreeNode == NULL) return false;
        return AddAt(pTreeNode, iIndex) >= 0;
    }

	bool TreeView::Remove(Control* pControl, bool bDoNotDestroy)
	{
        if (!pControl) return false;

        TreeNode* pTreeNode = static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE));
        if (pTreeNode == NULL) return List::Remove(pControl, bDoNotDestroy);

		if(pTreeNode->GetCountChild() > 0)
		{
			int nCount = pTreeNode->GetCountChild();
			for(int nIndex = 0;nIndex < nCount;nIndex++)
			{
				TreeNode* pNode = pTreeNode->GetChildNode(nIndex);
				if(pNode){
					pTreeNode->Remove(pNode, true);
				}
			}
		}
		return List::Remove(pControl, bDoNotDestroy);
	}

	bool TreeView::RemoveAt( int iIndex, bool bDoNotDestroy )
	{
        Control* pControl = GetItemAt(iIndex);
        if (pControl == NULL) return false;

        TreeNode* pTreeNode = static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE));
        if (pTreeNode == NULL) return List::Remove(pControl, bDoNotDestroy);

		return Remove(pTreeNode);
	}

	void TreeView::RemoveAll()
	{
		List::RemoveAll();
	}

    long TreeView::AddAt(TreeNode* pControl, int iIndex)
    {
        if (!pControl) return -1;

        TreeNode* pTreeNode = static_cast<TreeNode*>(pControl->GetInterface(DUI_CTR_TREENODE));
        if (pTreeNode == NULL) return -1;

        TreeNode* pParent = static_cast<TreeNode*>(GetItemAt(iIndex));
        if(!pParent) return -1;
#if !MODE_EVENTMAP
        pTreeNode->OnNotify += MakeDelegate(this,&TreeView::OnDBClickItem);
        pTreeNode->GetFolderButton()->OnNotify += MakeDelegate(this,&TreeView::OnFolderChanged);
        pTreeNode->GetCheckBox()->OnNotify += MakeDelegate(this,&TreeView::OnCheckBoxChanged);
#endif
        pTreeNode->SetVisibleFolderBtn(m_bVisibleFolderBtn);
        pTreeNode->SetVisibleCheckBtn(m_bVisibleCheckBtn);

        if(m_uItemMinWidth > 0) pTreeNode->SetMinWidth(m_uItemMinWidth);

        List::AddAt(pTreeNode,iIndex);

        if(pTreeNode->GetCountChild() > 0)
        {
            int nCount = pTreeNode->GetCountChild();
            for(int nIndex = 0;nIndex < nCount;nIndex++)
            {
                TreeNode* pNode = pTreeNode->GetChildNode(nIndex);
                if(pNode)
                    return AddAt(pNode,iIndex+1);
            }
        }
        else
            return iIndex+1;

        return -1;
    }

	bool TreeView::AddAt( TreeNode* pControl,TreeNode* _IndexNode )
	{
		if(!_IndexNode && !pControl)
			return false;

		int nItemIndex = -1;

		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			if(_IndexNode == GetItemAt(nIndex)){
				nItemIndex = nIndex;
				break;
			}
		}

		if(nItemIndex == -1)
			return false;

		return AddAt(pControl,nItemIndex) >= 0;
	}

	//************************************
	// ��������: Notify
	// ��������: void
	// ������Ϣ: TNotify & msg
	// ����˵��: 
	//************************************
	void TreeView::Notify( TNotify& msg )
	{
		
	}
	
	//************************************
	// ��������: OnCheckBoxChanged
	// ��������: bool
	// ������Ϣ: void * param
	// ����˵��: 
	//************************************
	bool TreeView::OnCheckBoxChanged( void* param )
	{
		TNotify* pMsg = (TNotify*)param;
		if(pMsg->sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			CheckBtn* pCheckBox = (CheckBtn*)pMsg->pSender;
			TreeNode* pItem = (TreeNode*)pCheckBox->GetParent()->GetParent();
			SetItemCheckBox(pCheckBox->GetCheck(),pItem);
			return true;
		}
		return true;
	}
	
	//************************************
	// ��������: OnFolderChanged
	// ��������: bool
	// ������Ϣ: void * param
	// ����˵��: 
	//************************************
	bool TreeView::OnFolderChanged( void* param )
	{
		TNotify* pMsg = (TNotify*)param;
		if(pMsg->sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			CheckBtn* pFolder = (CheckBtn*)pMsg->pSender;
			TreeNode* pItem = (TreeNode*)pFolder->GetParent()->GetParent();
			pItem->SetVisibleTag(!pFolder->GetCheck());
			SetItemExpand(!pFolder->GetCheck(),pItem);
			return true;
		}
		return true;
	}
	
	//************************************
	// ��������: OnDBClickItem
	// ��������: bool
	// ������Ϣ: void * param
	// ����˵��:
	//************************************
	bool TreeView::OnDBClickItem( void* param )
	{
		TNotify* pMsg = (TNotify*)param;
		if(pMsg->sType == DUI_MSGTYPE_ITEMDBCLICK)
		{
			TreeNode* pItem		= static_cast<TreeNode*>(pMsg->pSender);
			CheckBtn* pFolder	= pItem->GetFolderButton();
			pFolder->Selected(!pFolder->IsSelected());
			pItem->SetVisibleTag(!pFolder->GetCheck());
			SetItemExpand(!pFolder->GetCheck(),pItem);
			return true;
		}
		return false;
	}

	//************************************
	// ��������: SetItemCheckBox
	// ��������: bool
	// ������Ϣ: bool _Selected
	// ������Ϣ: TreeNode * _TreeNode
	// ����˵��: 
	//************************************
	bool TreeView::SetItemCheckBox( bool _Selected,TreeNode* _TreeNode /*= NULL*/ )
	{
		if(_TreeNode)
		{
			if(_TreeNode->GetCountChild() > 0)
			{
				int nCount = _TreeNode->GetCountChild();
				for(int nIndex = 0;nIndex < nCount;nIndex++)
				{
					TreeNode* pItem = _TreeNode->GetChildNode(nIndex);
					pItem->GetCheckBox()->Selected(_Selected);
					if(pItem->GetCountChild())
						SetItemCheckBox(_Selected,pItem);
				}
			}
			return true;
		}
		else
		{
			int nIndex = 0;
			int nCount = GetCount();
			while(nIndex < nCount)
			{
				TreeNode* pItem = (TreeNode*)GetItemAt(nIndex);
				pItem->GetCheckBox()->Selected(_Selected);
				if(pItem->GetCountChild())
					SetItemCheckBox(_Selected,pItem);

				nIndex++;
			}
			return true;
		}
		return false;
	}

	//************************************
	// ��������: SetItemExpand
	// ��������: void
	// ������Ϣ: bool _Expanded
	// ������Ϣ: TreeNode * _TreeNode
	// ����˵��: 
	//************************************
	void TreeView::SetItemExpand( bool _Expanded,TreeNode* _TreeNode /*= NULL*/ )
	{
		if(_TreeNode)
		{
			if(_TreeNode->GetCountChild() > 0)
			{
				int nCount = _TreeNode->GetCountChild();
				for(int nIndex = 0;nIndex < nCount;nIndex++)
				{
					TreeNode* pItem = _TreeNode->GetChildNode(nIndex);
					pItem->SetVisible(_Expanded);

					if(pItem->GetCountChild() && !pItem->GetFolderButton()->IsSelected())
						SetItemExpand(_Expanded,pItem);
				}
			}
		}
		else
		{
			int nIndex = 0;
			int nCount = GetCount();
			while(nIndex < nCount)
			{
				TreeNode* pItem = (TreeNode*)GetItemAt(nIndex);

				pItem->SetVisible(_Expanded);

				if(pItem->GetCountChild() && !pItem->GetFolderButton()->IsSelected())
					SetItemExpand(_Expanded,pItem);

				nIndex++;
			}
		}
	}

	//************************************
	// ��������: SetVisibleFolderBtn
	// ��������: void
	// ������Ϣ: bool _IsVisibled
	// ����˵��: 
	//************************************
	void TreeView::SetVisibleFolderBtn( bool _IsVisibled )
	{
		m_bVisibleFolderBtn = _IsVisibled;
		int nCount = this->GetCount();
		for(int nIndex = 0;nIndex < nCount;nIndex++)
		{
			TreeNode* pItem = static_cast<TreeNode*>(this->GetItemAt(nIndex));
			pItem->GetFolderButton()->SetVisible(m_bVisibleFolderBtn);
		}
	}

	//************************************
	// ��������: GetVisibleFolderBtn
	// ��������: bool
	// ����˵��: 
	//************************************
	bool TreeView::GetVisibleFolderBtn()
	{
		return m_bVisibleFolderBtn;
	}

	//************************************
	// ��������: SetVisibleCheckBtn
	// ��������: void
	// ������Ϣ: bool _IsVisibled
	// ����˵��: 
	//************************************
	void TreeView::SetVisibleCheckBtn( bool _IsVisibled )
	{
		m_bVisibleCheckBtn = _IsVisibled;
		int nCount = this->GetCount();
		for(int nIndex = 0;nIndex < nCount;nIndex++)
		{
			TreeNode* pItem = static_cast<TreeNode*>(this->GetItemAt(nIndex));
			pItem->GetCheckBox()->SetVisible(m_bVisibleCheckBtn);
		}
	}

	//************************************
	// ��������: GetVisibleCheckBtn
	// ��������: bool
	// ����˵��: 
	//************************************
	bool TreeView::GetVisibleCheckBtn()
	{
		return m_bVisibleCheckBtn;
	}

	//************************************
	// ��������: SetItemMinWidth
	// ��������: void
	// ������Ϣ: UINT _ItemMinWidth
	// ����˵��: 
	//************************************
	void TreeView::SetItemMinWidth( UINT _ItemMinWidth )
	{
		m_uItemMinWidth = _ItemMinWidth;

		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			TreeNode* pTreeNode = static_cast<TreeNode*>(GetItemAt(nIndex));
			if(pTreeNode)
				pTreeNode->SetMinWidth(GetItemMinWidth());
		}
		Invalidate();
	}

	//************************************
	// ��������: GetItemMinWidth
	// ��������: UINT
	// ����˵��: 
	//************************************
	UINT TreeView::GetItemMinWidth()
	{
		return m_uItemMinWidth;
	}
	
	//************************************
	// ��������: SetItemTextColor
	// ��������: void
	// ������Ϣ: DWORD _dwItemTextColor
	// ����˵��: 
	//************************************
	void TreeView::SetItemTextColor( DWORD _dwItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			TreeNode* pTreeNode = static_cast<TreeNode*>(GetItemAt(nIndex));
			if(pTreeNode)
				pTreeNode->SetItemTextColor(_dwItemTextColor);
		}
	}

	//************************************
	// ��������: SetItemHotTextColor
	// ��������: void
	// ������Ϣ: DWORD _dwItemHotTextColor
	// ����˵��: 
	//************************************
	void TreeView::SetItemHotTextColor( DWORD _dwItemHotTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			TreeNode* pTreeNode = static_cast<TreeNode*>(GetItemAt(nIndex));
			if(pTreeNode)
				pTreeNode->SetItemHotTextColor(_dwItemHotTextColor);
		}
	}

	//************************************
	// ��������: SetSelItemTextColor
	// ��������: void
	// ������Ϣ: DWORD _dwSelItemTextColor
	// ����˵��: 
	//************************************
	void TreeView::SetSelItemTextColor( DWORD _dwSelItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			TreeNode* pTreeNode = static_cast<TreeNode*>(GetItemAt(nIndex));
			if(pTreeNode)
				pTreeNode->SetSelItemTextColor(_dwSelItemTextColor);
		}
	}
		
	//************************************
	// ��������: SetSelItemHotTextColor
	// ��������: void
	// ������Ϣ: DWORD _dwSelHotItemTextColor
	// ����˵��: 
	//************************************
	void TreeView::SetSelItemHotTextColor( DWORD _dwSelHotItemTextColor )
	{
		for(int nIndex = 0;nIndex < GetCount();nIndex++){
			TreeNode* pTreeNode = static_cast<TreeNode*>(GetItemAt(nIndex));
			if(pTreeNode)
				pTreeNode->SetSelItemHotTextColor(_dwSelHotItemTextColor);
		}
	}

	//************************************
	// ��������: SetAttribute
	// ��������: void
	// ������Ϣ: LPCTSTR pstrName
	// ������Ϣ: LPCTSTR pstrValue
	// ����˵��: 
	//************************************
	void TreeView::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if(_tcscmp(pstrName,_T("visiblefolderbtn")) == 0)
			SetVisibleFolderBtn(_tcscmp(pstrValue,_T("true")) == 0);
		else if(_tcscmp(pstrName,_T("visiblecheckbtn")) == 0)
			SetVisibleCheckBtn(_tcscmp(pstrValue,_T("true")) == 0);
		else if(_tcscmp(pstrName,_T("itemminwidth")) == 0)
			SetItemMinWidth(_ttoi(pstrValue));
		else if(_tcscmp(pstrName, _T("itemtextcolor")) == 0 ){
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetItemTextColor(clrColor);
		}
		else if(_tcscmp(pstrName, _T("itemhottextcolor")) == 0 ){
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetItemHotTextColor(clrColor);
		}
		else if(_tcscmp(pstrName, _T("selitemtextcolor")) == 0 ){
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetSelItemTextColor(clrColor);
		}
		else if(_tcscmp(pstrName, _T("selitemhottextcolor")) == 0 ){
			DWORD clrColor = m_pManager->GetColor(pstrValue);
			if (clrColor == 0)
			{
				if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
				LPTSTR pstr = NULL;
				clrColor = _tcstoul(pstrValue, &pstr, 16);
			}
			SetSelItemHotTextColor(clrColor);
		}
		else List::SetAttribute(pstrName,pstrValue);
	}

}
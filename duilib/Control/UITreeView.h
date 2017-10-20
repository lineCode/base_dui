#ifndef __UITREEVIEW_H__
#define __UITREEVIEW_H__

#include <vector>
using namespace std;

#pragma once

namespace dui
{
	class TreeView;
	class CheckBtn;
	class Label;
	class OptionBtn;

	class DUILIB_API TreeNode : public ListContainerElement
	{
	public:
		TreeNode(TreeNode* _ParentNode = NULL);
		~TreeNode(void);

	public:
		LPCTSTR GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	DoEvent(TEvent& event);
		void	Invalidate();
		bool	Select(bool bSelect = true, bool bTriggerEvent=true);

		bool	Add(Control* _pTreeNodeUI);
		bool	AddAt(Control* pControl, int iIndex);

		void	SetVisibleTag(bool _IsVisible);
		bool	GetVisibleTag();
		void	SetItemText(LPCTSTR pstrValue);
		String	GetItemText();
		void	CheckBoxSelected(bool _Selected);
		bool	IsCheckBoxSelected() const;
		bool	IsHasChild() const;
		long	GetTreeLevel() const;
		bool	AddChildNode(TreeNode* _pTreeNodeUI);
		bool	RemoveAt(TreeNode* _pTreeNodeUI);
		void	SetParentNode(TreeNode* _pParentTreeNode);
		TreeNode* GetParentNode();
		long	GetCountChild();
		void	SetTreeView(TreeView* _CTreeViewUI);
		TreeView* GetTreeView();
		TreeNode* GetChildNode(int _nIndex);
		void	SetVisibleFolderBtn(bool _IsVisibled);
		bool	GetVisibleFolderBtn();
		void	SetVisibleCheckBtn(bool _IsVisibled);
		bool	GetVisibleCheckBtn();
		void	SetItemTextColor(DWORD _dwItemTextColor);
		DWORD	GetItemTextColor() const;
		void	SetItemHotTextColor(DWORD _dwItemHotTextColor);
		DWORD	GetItemHotTextColor() const;
		void	SetSelItemTextColor(DWORD _dwSelItemTextColor);
		DWORD	GetSelItemTextColor() const;
		void	SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor);
		DWORD	GetSelItemHotTextColor() const;

		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		PtrArray GetTreeNodes();

		int			 GetTreeIndex();
		int			 GetNodeIndex();

	private:
		TreeNode* GetLastNode();
		TreeNode* CalLocation(TreeNode* _pTreeNodeUI);
	public:
		HorizontalLayout*	GetTreeNodeHoriznotal() const {return pHoriz;};
		CheckBtn*			GetFolderButton() const {return pFolderButton;};
		Label*				GetDottedLine() const {return pDottedLine;};
		CheckBtn*			GetCheckBox() const {return pCheckBox;};
		OptionBtn*				GetItemButton() const {return pItemButton;};

	private:
		long	m_iTreeLavel;
		bool	m_bIsVisable;
		bool	m_bIsCheckBox;
		DWORD	m_dwItemTextColor;
		DWORD	m_dwItemHotTextColor;
		DWORD	m_dwSelItemTextColor;
		DWORD	m_dwSelItemHotTextColor;

		TreeView*			pTreeView;
		HorizontalLayout*	pHoriz;
		CheckBtn*			pFolderButton;
		Label*				pDottedLine;
		CheckBtn*			pCheckBox;
		OptionBtn*				pItemButton;

		TreeNode*			pParentTreeNode;

		PtrArray			mTreeNodes;
	};

	class DUILIB_API TreeView : public List,public INotify
	{
	public:
		TreeView(void);
		~TreeView(void);

	public:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID	GetInterface(LPCTSTR pstrName);
        virtual bool Add(Control* pControl);
        virtual bool AddAt(Control* pControl, int iIndex);
        virtual bool Remove(Control* pControl, bool bDoNotDestroy=false);
        virtual bool RemoveAt(int iIndex, bool bDoNotDestroy=false);
        virtual void RemoveAll();

        long AddAt(TreeNode* pControl, int iIndex);
        bool AddAt(TreeNode* pControl,TreeNode* _IndexNode);

		virtual bool OnCheckBoxChanged(void* param);
		virtual bool OnFolderChanged(void* param);
		virtual bool OnDBClickItem(void* param);
		virtual bool SetItemCheckBox(bool _Selected,TreeNode* _TreeNode = NULL);
		virtual void SetItemExpand(bool _Expanded,TreeNode* _TreeNode = NULL);
		virtual void Notify(TNotify& msg);
		virtual void SetVisibleFolderBtn(bool _IsVisibled);
		virtual bool GetVisibleFolderBtn();
		virtual void SetVisibleCheckBtn(bool _IsVisibled);
		virtual bool GetVisibleCheckBtn();
		virtual void SetItemMinWidth(UINT _ItemMinWidth);
		virtual UINT GetItemMinWidth();
		virtual void SetItemTextColor(DWORD _dwItemTextColor);
		virtual void SetItemHotTextColor(DWORD _dwItemHotTextColor);
		virtual void SetSelItemTextColor(DWORD _dwSelItemTextColor);
		virtual void SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor);
		
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	private:
		UINT m_uItemMinWidth;
		bool m_bVisibleFolderBtn;
		bool m_bVisibleCheckBtn;
	};
}


#endif // __UITREEVIEW_H__

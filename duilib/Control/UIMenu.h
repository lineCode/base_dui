#ifndef __UIMENU_H__
#define __UIMENU_H__

#pragma once

#include "../Utils/observer_impl_base.hpp"

#define MENUWND_OBSERVER	0
namespace dui {

struct ContextMenuParam
{
	// 1: remove all
	// 2: remove the sub menu
	WPARAM wParam;
	HWND hWnd;
};

struct MenuCmd
{
	TCHAR szName[256];
	TCHAR szUserData[1024];
	TCHAR szText[1024];
	BOOL bChecked;
};

enum MenuAlignment
{
	eMenuAlignment_Left = 1 << 1,
	eMenuAlignment_Top = 1 << 2,
	eMenuAlignment_Right = 1 << 3,
	eMenuAlignment_Bottom = 1 << 4,
};


enum MenuItemDefaultInfo
{
	ITEM_DEFAULT_HEIGHT = 30,		//每一个item的默认高度（只在竖状排列时自定义）
	ITEM_DEFAULT_WIDTH = 150,		//窗口的默认宽度

	ITEM_DEFAULT_ICON_WIDTH = 26,	//默认图标所占宽度
	ITEM_DEFAULT_ICON_SIZE = 16,	//默认图标的大小

	ITEM_DEFAULT_EXPLAND_ICON_WIDTH = 20,	//默认下级菜单扩展图标所占宽度
	ITEM_DEFAULT_EXPLAND_ICON_SIZE = 9,		//默认下级菜单扩展图标的大小

	DEFAULT_LINE_LEFT_PADDING = ITEM_DEFAULT_ICON_WIDTH + 3,	//默认分隔线的左边距
	DEFAULT_LINE_RIGHT_PADDING = 7,	//默认分隔线的右边距
	DEFAULT_LINE_HEIGHT = 6,		//默认分隔线所占高度
	DEFAULT_LINE_COLOR = 0xFFBCBFC4	//默认分隔线颜色

};

#define WM_MENUCLICK WM_USER + 121  //用来接收按钮单击的消息

#if MENUWND_OBSERVER
///////////////////////////////////////////////
class MenuMenuReceiverImplBase;
class MenuMenuObserverImplBase
{
public:
	virtual void AddReceiver(MenuMenuReceiverImplBase* receiver) = 0;
	virtual void RemoveReceiver(MenuMenuReceiverImplBase* receiver) = 0;
	virtual BOOL RBroadcast(ContextMenuParam param) = 0;
};
/////////////////////////////////////////////////
class MenuMenuReceiverImplBase
{
public:
	virtual void AddObserver(MenuMenuObserverImplBase* observer) = 0;
	virtual void RemoveObserver() = 0;
	virtual BOOL Receive(ContextMenuParam param) = 0;
};
/////////////////////////////////////////////////

class MenuReceiverImpl;
class DUILIB_API MenuObserverImpl : public MenuMenuObserverImplBase
{
	friend class Iterator;
public:
	MenuObserverImpl():
	m_pMainWndPaintManager(NULL),
	m_pMenuCheckInfo(NULL)
	{
		pReceivers_ = new ReceiversVector;
	}

	~MenuObserverImpl()
	{
		if (pReceivers_ != NULL)
		{
			delete pReceivers_;
			pReceivers_ = NULL;
		}
		
	}

	virtual void AddReceiver(MenuMenuReceiverImplBase* receiver)
	{
		if (receiver == NULL)
			return;

		pReceivers_->push_back(receiver);
		receiver->AddObserver(this);
	}

	virtual void RemoveReceiver(MenuMenuReceiverImplBase* receiver)
	{
		if (receiver == NULL)
			return;

		ReceiversVector::iterator it = pReceivers_->begin();
		for (; it != pReceivers_->end(); ++it)
		{
			if (*it == receiver)
			{
				pReceivers_->erase(it);
				break;
			}
		}
	}

	virtual BOOL RBroadcast(ContextMenuParam param)
	{
		ReceiversVector::reverse_iterator it = pReceivers_->rbegin();
		for (; it != pReceivers_->rend(); ++it)
		{
			(*it)->Receive(param);
		}

		return BOOL();
	}


	class Iterator
	{
		MenuObserverImpl & _tbl;
		DWORD index;
		MenuMenuReceiverImplBase* ptr;
	public:
		Iterator( MenuObserverImpl & table )
			: _tbl( table ), index(0), ptr(NULL)
		{}

		Iterator( const Iterator & v )
			: _tbl( v._tbl ), index(v.index), ptr(v.ptr)
		{}

		MenuMenuReceiverImplBase* next()
		{
			if ( index >= _tbl.pReceivers_->size() )
				return NULL;

			for ( ; index < _tbl.pReceivers_->size(); )
			{
				ptr = (*(_tbl.pReceivers_))[ index++ ];
				if ( ptr )
					return ptr;
			}
			return NULL;
		}
	};

	virtual void SetManger(CPaintManager* pManager)
	{
		if (pManager != NULL)
			m_pMainWndPaintManager = pManager;
	}

	virtual CPaintManager* GetManager() const
	{
		return m_pMainWndPaintManager;
	}

	virtual void SetMenuCheckInfo(std::map<String,bool>* pInfo)
	{
		if (pInfo != NULL)
			m_pMenuCheckInfo = pInfo;
		else
			m_pMenuCheckInfo = NULL;
	}

	virtual std::map<String,bool>* GetMenuCheckInfo() const
	{
		return m_pMenuCheckInfo;
	}

protected:
	typedef std::vector<MenuMenuReceiverImplBase*> ReceiversVector;
	ReceiversVector *pReceivers_;

	CPaintManager* m_pMainWndPaintManager;
	std::map<String,bool>* m_pMenuCheckInfo;
};

////////////////////////////////////////////////////
class DUILIB_API MenuReceiverImpl : public MenuMenuReceiverImplBase
{
public:
	MenuReceiverImpl()
	{
		pObservers_ = new ObserversVector;
	}

	~MenuReceiverImpl()
	{
		if (pObservers_ != NULL)
		{
			delete pObservers_;
			pObservers_ = NULL;
		}
	}

	virtual void AddObserver(MenuMenuObserverImplBase* observer)
	{
		pObservers_->push_back(observer);
	}

	virtual void RemoveObserver()
	{
		ObserversVector::iterator it = pObservers_->begin();
		for (; it != pObservers_->end(); ++it)
		{
			(*it)->RemoveReceiver(this);
		}
	}

	virtual BOOL Receive(ContextMenuParam param)
	{
		return BOOL();
	}

protected:
	typedef std::vector<MenuMenuObserverImplBase*> ObserversVector;
	ObserversVector* pObservers_;
};
#endif
/////////////////////////////////////////////////////////////////////////////////////
//


class List;
class DUILIB_API Menu : public List
{
public:
	Menu();
	virtual ~Menu();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

	virtual void DoEvent(TEvent& event);

    virtual bool Add(Control* pControl);
    virtual bool AddAt(Control* pControl, int iIndex);

    virtual int GetItemIndex(Control* pControl) const;
    virtual bool SetItemIndex(Control* pControl, int iIndex);
    virtual bool Remove(Control* pControl);

	SIZE EstimateSize(SIZE szAvailable) override;

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) ;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class MenuElement;
#if MENUWND_OBSERVER
class DUILIB_API CMenuWnd : public CWindowWnd, public MenuReceiverImpl, public INotify, public IDialogBuilderCallback
{
public:
	static MenuObserverImpl& GetGlobalContextMenuObserver()
	{
		static MenuObserverImpl s_context_menu_observer;
		return s_context_menu_observer;
	}
#else
class DUILIB_API CMenuWnd : public CWindowWnd, public INotify, public IDialogBuilderCallback
{
#endif
public:
	CMenuWnd();
	~CMenuWnd();

	/*
	 *	@pOwner 一级菜单不要指定这个参数，这是菜单内部使用的
	 *	@xml	菜单的布局文件
	 *	@point	菜单的左上角坐标
	 *	@pMainPaintManager	菜单的父窗体管理器指针
	 *	@pMenuCheckInfo	保存菜单的单选和复选信息结构指针
	 *	@dwAlignment		菜单的出现位置，默认出现在鼠标的右下侧。
	 */
	void Init(MenuElement* pOwner, STRINGorID xml, String folder, POINT point,
		CPaintManager* pMainPaintManager, std::map<String,bool>* pMenuCheckInfo = NULL,
		DWORD dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);
	void Notify(TEvent& msg);
	Control* CreateControl(LPCTSTR pstrClassName);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
#if MENUWND_OBSERVER
	BOOL Receive(ContextMenuParam param);
#endif
	// 获取根菜单控件，用于动态添加子菜单
	Menu* GetMenuUI();

	// 重新调整菜单的大小
	void ResizeMenu();

	// 重新调整子菜单的大小
	void ResizeSubMenu();

private:
	POINT			m_BasedPoint;
	STRINGorID		m_xml;
    MenuElement* m_pOwner;
    Menu*	m_pLayout;
	DWORD		m_dwAlignment;	//菜单对齐方式

	CPaintManager* m_pParentManager;
	CPaintManager m_pm;
	
};

class ListContainerElement;
class DUILIB_API MenuElement : public ListContainerElement
{
	friend CMenuWnd;
public:
    MenuElement();
	~MenuElement();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);
	void DrawItemText(HDC hDC, const RECT& rcItem);
	SIZE EstimateSize(SIZE szAvailable);

	void DoEvent(TEvent& event);

	CMenuWnd* GetMenuWnd();
	void CreateMenuWnd();
	
	void SetLineType();
	void SetLineColor(DWORD color);
	DWORD GetLineColor() const;
	void SetLinePadding(RECT rcPadding);
	RECT GetLinePadding() const;
	void SetIcon(LPCTSTR strIcon);
	void SetIconSize(LONG cx, LONG cy);
	void DrawItemIcon(HDC hDC, const RECT& rcItem);
	void SetChecked(bool bCheck = true);
	bool GetChecked() const;
	void SetCheckItem(bool bCheckItem = false);
	bool GetCheckItem() const;

	void SetShowExplandIcon(bool bShow);
	void DrawItemExpland(HDC hDC, const RECT& rcItem);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

protected:
	CMenuWnd*	m_pWindow;

	bool		m_bDrawLine;	//画分隔线
	DWORD		m_dwLineColor;  //分隔线颜色
	RECT		m_rcLinePadding;	//分割线的左右边距

	SIZE		m_szIconSize; 	//画图标
	String		m_strIcon;
	bool		m_bCheckItem;

	bool		m_bShowExplandIcon;
};

} // namespace dui

#endif // __UIMENU_H__

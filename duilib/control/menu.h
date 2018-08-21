#ifndef __UIMENU_H__
#define __UIMENU_H__
#pragma once

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

/////////////////////////////////////////////////////////////////////////////////////
//

typedef List Menu;
/////////////////////////////////////////////////////////////////////////////////////
//

class MenuElement;

class DUILIB_API CMenuWnd : public WindowImplBase
{
public:
	CMenuWnd();
	virtual ~CMenuWnd();

	virtual String GetSkinFolder() override;
	virtual String GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;

	/*
	 *	@pOwner 一级菜单不要指定这个参数，这是菜单内部使用的
	 *	@xml	菜单的布局文件
	 *	@point	菜单的左上角坐标
	 *	@pMainPaintManager	菜单的父窗体管理器指针
	 *	@pMenuCheckInfo	保存菜单的单选和复选信息结构指针
	 *	@dwAlignment		菜单的出现位置，默认出现在鼠标的右下侧。
	 */
	void Init(MenuElement* pOwner, String xml, String folder, POINT point,
		UIManager* pMainPaintManager, std::map<String,bool>* pMenuCheckInfo = NULL,
		DWORD dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top, bool isLayeredWindow = true);
    void OnFinalMessage(HWND hWnd);
	void Notify(Event& msg);
	Control* CreateControl(LPCTSTR pstrClassName);

	//LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// 获取根菜单控件，用于动态添加子菜单
	Menu* GetMenuUI();

	// 重新调整菜单的大小
	//void ResizeMenu();

	// 重新调整子菜单的大小
	void ResizeSubMenu();

	void Show();
private:
	String			m_folder;
	String			m_xmlfile;

	POINT			m_BasedPoint;
	//STRINGorID		m_xml;
    MenuElement*	m_pOwner;
    Menu*			m_pLayout;
	DWORD			m_dwAlignment;	//菜单对齐方式

	UIManager* m_pParentManager;
	
	bool			m_bLayeredWindow;
	
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
	SIZE EstimateSize(SIZE szAvailable);

	void DoEvent(Event& event);

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

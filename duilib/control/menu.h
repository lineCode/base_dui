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
	ITEM_DEFAULT_HEIGHT = 30,		//ÿһ��item��Ĭ�ϸ߶ȣ�ֻ����״����ʱ�Զ��壩
	ITEM_DEFAULT_WIDTH = 150,		//���ڵ�Ĭ�Ͽ��

	ITEM_DEFAULT_ICON_WIDTH = 26,	//Ĭ��ͼ����ռ���
	ITEM_DEFAULT_ICON_SIZE = 16,	//Ĭ��ͼ��Ĵ�С

	ITEM_DEFAULT_EXPLAND_ICON_WIDTH = 20,	//Ĭ���¼��˵���չͼ����ռ���
	ITEM_DEFAULT_EXPLAND_ICON_SIZE = 9,		//Ĭ���¼��˵���չͼ��Ĵ�С

	DEFAULT_LINE_LEFT_PADDING = ITEM_DEFAULT_ICON_WIDTH + 3,	//Ĭ�Ϸָ��ߵ���߾�
	DEFAULT_LINE_RIGHT_PADDING = 7,	//Ĭ�Ϸָ��ߵ��ұ߾�
	DEFAULT_LINE_HEIGHT = 6,		//Ĭ�Ϸָ�����ռ�߶�
	DEFAULT_LINE_COLOR = 0xFFBCBFC4	//Ĭ�Ϸָ�����ɫ

};

#define WM_MENUCLICK WM_USER + 121  //�������հ�ť��������Ϣ

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
	 *	@pOwner һ���˵���Ҫָ��������������ǲ˵��ڲ�ʹ�õ�
	 *	@xml	�˵��Ĳ����ļ�
	 *	@point	�˵������Ͻ�����
	 *	@pMainPaintManager	�˵��ĸ����������ָ��
	 *	@pMenuCheckInfo	����˵��ĵ�ѡ�͸�ѡ��Ϣ�ṹָ��
	 *	@dwAlignment		�˵��ĳ���λ�ã�Ĭ�ϳ������������²ࡣ
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

	// ��ȡ���˵��ؼ������ڶ�̬����Ӳ˵�
	Menu* GetMenuUI();

	// ���µ����˵��Ĵ�С
	//void ResizeMenu();

	// ���µ����Ӳ˵��Ĵ�С
	void ResizeSubMenu();

	void Show();
private:
	String			m_folder;
	String			m_xmlfile;

	POINT			m_BasedPoint;
	//STRINGorID		m_xml;
    MenuElement*	m_pOwner;
    Menu*			m_pLayout;
	DWORD			m_dwAlignment;	//�˵����뷽ʽ

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

	bool		m_bDrawLine;	//���ָ���
	DWORD		m_dwLineColor;  //�ָ�����ɫ
	RECT		m_rcLinePadding;	//�ָ��ߵ����ұ߾�

	SIZE		m_szIconSize; 	//��ͼ��
	String		m_strIcon;
	bool		m_bCheckItem;

	bool		m_bShowExplandIcon;
};

} // namespace dui

#endif // __UIMENU_H__

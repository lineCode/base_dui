#pragma once
//////////////BEGIN消息映射宏定义////////////////////////////////////////////////////
///

namespace dui
{

#define _USEIMM					1
#define MAX_FONT_ID				30000
#define LAYEREDUPDATE_TIMERID	0x2000

#define SCROLLBAR_LINESIZE      8

enum DuiSig
{
	DuiSig_end = 0, // [marks end of message map]
	DuiSig_lwl,     // LRESULT (WPARAM, LPARAM)
	DuiSig_vn,      // void (TEvent)
};

typedef enum EVENTTYPE_UI
{
	UIEVENT__FIRST = 1,

	UIEVENT__KEYBEGIN,			//2
	UIEVENT_KEYDOWN,
	UIEVENT_KEYUP,
	UIEVENT_CHAR,
	UIEVENT_SYSKEY,
	UIEVENT__KEYEND,			//7

	UIEVENT__MOUSEBEGIN,		//8
	UIEVENT_MOUSEMOVE,
	UIEVENT_MOUSELEAVE,
	UIEVENT_MOUSEENTER,
	UIEVENT_MOUSEHOVER,
	UIEVENT_BUTTONDOWN,
	UIEVENT_BUTTONUP,
	UIEVENT_RBUTTONDOWN,
	UIEVENT_DBLCLICK,
	UIEVENT_CONTEXTMENU,
	UIEVENT_SCROLLWHEEL,
	UIEVENT__MOUSEEND,			//19

	UIEVENT_KILLFOCUS,
	UIEVENT_SETFOCUS,
	UIEVENT_WINDOWINIT,
	UIEVENT_WINDOWSIZE,
	UIEVENT_SETCURSOR,
	//-------add for EVENTMAP--------		
	UIEVENT_CLICK,
	UIEVENT_ITEMCLICK,
	UIEVENT_SELECTCHANGE,
	UIEVENT_UNSELECT,
	UIEVENT_TEXTCHANGE,
	UIEVENT_CUSTOMLINKCLICK,

	UIEVENT_SCROLL,

	UIEVENT_VALUECHANGE,
	UIEVENT_RETURN,
	UIEVENT_TABSELECT,
	UIEVENT_WINDOWCLOSE,

	UIEVENT_RESIZE,
	//-------end EVENTMAP--------	
	UIEVENT_MENU,
	UIEVENT_TIMER,
	UIEVENT_NOTIFY,
	UIEVENT_COMMAND,
	//---------
	UIEVENT_ITEMSELECT,
	UIEVENT_ITEMEXPAND,
	UIEVENT_ITEMCOLLAPSE,
	UIEVENT_LINK,
	UIEVENT_DROPDOWN,
	UIEVENT_SHOWACTIVEX,
	UIEVENT_LISTHEADERCLICK,

	UIEVENT__LAST,
};

//////////////////////////////////////////////////////////////////////////
class Control;

// Structure for notifications to the outside world
typedef struct tagTEventUI
{
	EVENTTYPE_UI Type;
	Control* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	TCHAR chKey;
	WORD wKeyState;
	WPARAM wParam;
	LPARAM lParam;
	tagTEventUI(){
		Type = UIEVENT__FIRST;  pSender = NULL; ptMouse = {}; dwTimestamp = chKey = wKeyState = wParam = lParam = 0;
	}
} TEvent;


//////////////BEGIN控件名称宏定义//////////////////////////////////////////////////
///
#define  DUI_CTR_EDIT                            (_T("Edit"))
#define  DUI_CTR_LIST                            (_T("List"))
#define  DUI_CTR_TEXT                            (_T("Text"))
#define  DUI_CTR_TREE                            (_T("Tree"))

#define  DUI_CTR_ILIST                           (_T("IList"))
#define  DUI_CTR_COMBO                           (_T("Combo"))
#define  DUI_CTR_LABEL                           (_T("Label"))
#define  DUI_CTR_FLASH							 (_T("Flash"))
//6
#define  DUI_CTR_BUTTON                          (_T("Button"))
#define  DUI_CTR_OPTION                          (_T("Option"))
#define  DUI_CTR_SLIDER                          (_T("Slider"))
//7
#define  DUI_CTR_CONTROL                         (_T("Control"))
#define  DUI_CTR_ACTIVEX                         (_T("ActiveX"))
#define  DUI_CTR_GIFANIM                         (_T("GifAnim"))
#define  DUI_CTR_HLAYOUT                         (_T("HLayout"))
#define  DUI_CTR_VLAYOUT                         (_T("VLayout"))
//8
#define  DUI_CTR_PROGRESS                        (_T("Progress"))
#define  DUI_CTR_RICHEDIT                        (_T("RichEdit"))
#define  DUI_CTR_CHECKBOX                        (_T("CheckBox"))
#define  DUI_CTR_COMBOBOX                        (_T("ComboBox"))
#define  DUI_CTR_DATETIME                        (_T("DateTime"))
#define  DUI_CTR_LISTVIEW                        (_T("ListView"))
#define  DUI_CTR_TREEVIEW                        (_T("TreeView"))
#define  DUI_CTR_TREENODE                        (_T("TreeNode"))
//9
#define  DUI_CTR_ILISTVIEW                       (_T("IListView"))
#define  DUI_CTR_ILISTITEM                       (_T("IListItem"))
#define  DUI_CTR_CONTAINER                       (_T("Container"))
#define  DUI_CTR_TABLAYOUT                       (_T("TabLayout"))
#define  DUI_CTR_SCROLLBAR                       (_T("ScrollBar"))
//10
#define  DUI_CTR_ICONTAINER                      (_T("IContainer"))
#define  DUI_CTR_LISTHEADER                      (_T("ListHeader"))
#define  DUI_CTR_TILELAYOUT                      (_T("TileLayout"))
#define  DUI_CTR_WEBBROWSER                      (_T("WebBrowser"))
//11
#define  DUI_CTR_CHILDLAYOUT                     (_T("ChildLayout"))
#define  DUI_CTR_LISTELEMENT                     (_T("ListElement"))
#define  DUI_CTR_VIRTUALLIST                     (_T("VirtualList"))
//14
#define  DUI_CTR_LISTHEADERITEM                  (_T("ListHeaderItem"))
#define  DUI_CTR_LABELCONTAINER					 (_T("LabelContainer"))
//15
#define  DUI_CTR_LISTHBOXELEMENT                 (_T("ListHBoxElement"))
#define  DUI_CTR_BUTTONCONTAINER				 (_T("ButtonContainer"))
#define  DUI_CTR_OPTIONCONTAINER				 (_T("OptionContainer"))
#define  DUI_CTR_SCROLLCONTAINER                 (_T("ScrollContainer"))
//20
#define  DUI_CTR_LISTCONTAINERELEMENT            (_T("ListContainerElement"))





///
//////////////END控件名称宏定义//////////////////////////////////////////////////


}// namespace dui


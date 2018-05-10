#include "stdafx.h"
#include "resource.h"
#include "shared/pin_yin_helper.h"
#include "gui/main/control/friend_item.h"
#include "gui/main/control/session_item.h"
#include "gui/main/control/friend_itemex.h"
#include "gui/session/session_box.h"
#include "module/tray/tray_manager.h"
#include "module/login/login_manager.h"

#include "main_form.h"

using namespace dui;

const LPCTSTR MainForm::kClassName	= L"MainForm";

MainForm::MainForm()
{
	/*is_busy_ = false;
	is_trayicon_left_double_clicked_ = false;

	OnUserInfoChangeCallback cb1 = nbase::Bind(&MainForm::OnUserInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(nim_ui::UserManager::GetInstance()->RegUserInfoChange(cb1));

	OnPhotoReadyCallback cb2 = nbase::Bind(&MainForm::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(nim_ui::PhotoManager::GetInstance()->RegPhotoReady(cb2));*/
}

MainForm::~MainForm()
{

}

std::wstring MainForm::GetSkinFolder()
{
	return L"main_vv";
}

std::wstring MainForm::GetSkinFile()
{
	return L"main.xml";
}

LPCTSTR MainForm::GetWindowClassName() const
{
	return kClassName;
}

LPCTSTR MainForm::GetWindowId() const
{
	return kClassName;
}

UINT MainForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void MainForm::OnEsc( BOOL &bHandled )
{
	bHandled = TRUE;
}

void MainForm::OnFinalMessage(HWND hWnd)
{
	nim_comp::TrayManager::GetInstance()->Destroy();
	/*nim_comp::SessionListManager::GetInstance()->AttachListBox(nullptr);
	nim_comp::ContactsListManager::GetInstance()->AttachFriendListBox(nullptr);
	nim_comp::ContactsListManager::GetInstance()->AttachGroupListBox(nullptr);*/

	__super::OnFinalMessage(hWnd);
}

LRESULT MainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	::ShowWindow(m_hWnd, SW_HIDE);
	return 0;
}

void MainForm::InitWindow()
{
	SetIcon(IDI_ICON1);
	SetTaskbarTitle(L"Î¢Î¢");

	nim_comp::TrayManager::GetInstance()->Init();
	nim_comp::TrayManager::GetInstance()->SetTrayIcon(::LoadIconW(nbase::win32::GetCurrentModuleHandle(), MAKEINTRESOURCE(IDI_ICON1)), L"Î¢Î¢");
	nim_comp::TrayManager::GetInstance()->RegEventCallback(std::bind(&MainForm::TrayLeftClick, this), nim_comp::TrayEventType_LeftClick);
	nim_comp::TrayManager::GetInstance()->RegEventCallback(std::bind(&MainForm::TrayRightClick, this), nim_comp::TrayEventType_RightClick);

	btn_head_ = dynamic_cast<Button*>(m_PaintManager.FindControl(_T("btn_head")));
	if (btn_head_)
	{
		auto OnBtnHeadClicked = [this](TEvent *event){
			printf("MainForm OnBtnHeadClicked, name:%s\n", event->pSender->GetName().c_str());
			return false; };

		btn_head_->AttachClick(std::bind(OnBtnHeadClicked, std::placeholders::_1));
	}

	auto OnOptContainerClicked = [this](TEvent *event, int index){
		printf("MainForm OnBtnContainerClicked, name:%s\n", event->pSender->GetName().c_str());

		//if (index < 3)
		{
			tab_session_friend_->SelectItem(index);
		}
		
		return true; };
	OptionContainer *optbox_opt_session = dynamic_cast<OptionContainer *>(m_PaintManager.FindControl(_T("optbox_opt_session")));
	OptionContainer *optbox_opt_friends = dynamic_cast<OptionContainer *>(m_PaintManager.FindControl(_T("optbox_opt_friends")));
	OptionContainer *optbox_opt_groups = dynamic_cast<OptionContainer *>(m_PaintManager.FindControl(_T("optbox_opt_groups")));
	OptionContainer *optbox_opt_menu = dynamic_cast<OptionContainer *>(m_PaintManager.FindControl(_T("optbox_opt_menu")));
	optbox_opt_session->AttachClick(std::bind(OnOptContainerClicked, std::placeholders::_1, 0));
	optbox_opt_friends->AttachClick(std::bind(OnOptContainerClicked, std::placeholders::_1, 1));
	optbox_opt_groups->AttachClick(std::bind(OnOptContainerClicked, std::placeholders::_1, 2));
	optbox_opt_menu->AttachClick(std::bind(OnOptContainerClicked, std::placeholders::_1, 3));

	tab_session_friend_ = dynamic_cast<TabLayout*>(m_PaintManager.FindControl(_T("tab_session_friend")));

	list_session_ = dynamic_cast<ListView*>(m_PaintManager.FindControl(_T("list_session")));
	list_friend_ = dynamic_cast<ListView*>(m_PaintManager.FindControl(_T("list_friend")));
	tree_friend_ = dynamic_cast<Tree*>(m_PaintManager.FindControl(_T("tv_friend")));
	list_menu_ = dynamic_cast<ListView*>(m_PaintManager.FindControl(_T("list_menu")));
	{
		ListContainerElement *menu_dpi = dynamic_cast<ListContainerElement*>(list_menu_->FindSubControl(_T("menu_dpi")));
		auto cb = std::bind([this](TEvent *event){
			CPaintManager::SetAllDPI(120);
			return false;
		}, std::placeholders::_1);
		menu_dpi->AttachItemClick(cb);

		ListContainerElement *menu_about = dynamic_cast<ListContainerElement*>(list_menu_->FindSubControl(_T("menu_about")));
		auto cb2 = std::bind([this](TEvent *event){
			CPaintManager::SetAllDPI(96);
			return false;
		}, std::placeholders::_1);
		menu_about->AttachItemClick(cb2);
	}
	
	for (size_t i = 0; i < 26; i++)
	{
		TreeNode *node = new TreeNode;
		m_PaintManager.FillBox(node, _T("friend_tree_class_item.xml"), NULL, &m_PaintManager, NULL);
		Label *label = static_cast<Label *>(node->FindSubControl(_T("label_name")));
		Button *btn_expand = static_cast<Button *>(node->FindSubControl(_T("btn_expand")));
		if (label)
		{
			TCHAR text[2] = {};
			text[0] = 'A' + i;
			label->SetText(text);
		}
		if (btn_expand)
		{
			auto cb = [node](TEvent *event){
				node->SetNodeExpand(!node->GetNodeExpand());
				return false;
			};
			btn_expand->AttachClick(cb);
		}

		tree_friend_->AddChildNode(node);
		group_nodes_[i + 2] = node;
	}

	TreeNode *node = new TreeNode;
	m_PaintManager.FillBox(node, _T("friend_tree_class_item.xml"), NULL, &m_PaintManager, NULL);
	Label *label = static_cast<Label *>(node->FindSubControl(_T("label_name")));
	if (label)
	{
		TCHAR text[2] = {};
		text[0] = '*';
		label->SetText(text);
	}
	tree_friend_->AddChildNodeAt(node, 0);
	group_nodes_[0] = node;

	node = new TreeNode;
	m_PaintManager.FillBox(node, _T("friend_tree_class_item.xml"), NULL, &m_PaintManager, NULL);
	label = static_cast<Label *>(node->FindSubControl(_T("label_name")));
	if (label)
	{
		TCHAR text[2] = {};
		text[0] = '#';
		label->SetText(text);
	}
	tree_friend_->AddChildNodeAt(node, 1);
	group_nodes_[1] = node;
	//--------------------------------tab session-------------------------------
	tab_session_ = dynamic_cast<TabLayout*>(m_PaintManager.FindControl(_T("tab_session")));
	nim_comp::SessionBox *session = new nim_comp::SessionBox("1", nim_comp::kNIMSessionTypeP2P);
	m_PaintManager.FillBox(session, _T("session.xml"), this, &m_PaintManager);
	tab_session_->Add(session);

	tab_session_->SelectItem(0);

	nim_comp::UserManager::GetInstance()->DoLoadFriends(std::bind(&MainForm::OnGetAllFriendInfo, this, std::placeholders::_1));
	nim_comp::SessionManager::GetInstance()->DoLoadSession(std::bind(&MainForm::OnGetAllSessionInfo, this, std::placeholders::_1, std::placeholders::_2));
}

void MainForm::Notify(dui::TEvent& msg)
{
	bool bHandle = false;

	Control *pControl = msg.pSender;
	String name = pControl->GetName();
	//wprintf(L"MainForm::Notify name:%s, msgtype:%s\n", name.c_str(), msg.sType.c_str());
	if (msg.Type == UIEVENT_ITEMCLICK)
	{
		if (name == _T("menu_logoff"))
		{
			MenuLogoffClick(&msg);
		}
		else if (name == _T("menu_logout"))
		{
			MenuLogoutClick(&msg);
		}
		bHandle = true;
	}
	if (!bHandle)
	{
		__super::Notify(msg);
	}
}

void MainForm::OnClick(dui::TEvent& msg)
{
	bool bHandle = false;

	Control *pControl = msg.pSender;
	String name = pControl->GetName();
	wprintf(L"MainForm::OnClicked %s\n", name.c_str());
	if (name == _T("btnbox_opt_session"))
	{
		tab_session_friend_->SelectItem(0);
	}
	else if (name == _T("btnbox_opt_friends"))
	{
		tab_session_friend_->SelectItem(1);
	}
	else if (name == _T("btnbox_opt_groups"))
	{
		tab_session_friend_->SelectItem(2);
	}

	if (!bHandle)
	{
		__super::OnClick(msg);
	}
}

void MainForm::TrayLeftClick()
{
	::ShowWindow(m_hWnd, SW_SHOW);
	this->ActiveWindow();
	::SetForegroundWindow(m_hWnd);
	::BringWindowToTop(m_hWnd);
	nim_comp::TrayManager::GetInstance()->StopTrayIconAnimate();
}

void MainForm::TrayRightClick()
{
	POINT point;
	::GetCursorPos(&point);
	PopupTrayMenu(point);
}

void MainForm::PopupTrayMenu(POINT point)
{
	dui::CMenuWnd* pMenu = new dui::CMenuWnd;
	String xml(L"tray_menu.xml");
	pMenu->Init(NULL, xml, _T("menu"), point, &m_PaintManager, NULL, eMenuAlignment_Left | eMenuAlignment_Bottom);

	pMenu->Show();
}

void MainForm::OnGetAllFriendInfo(const std::list<nim_comp::UserNameCard>& list)
{
	printf("MainForm::OnGetAllFriendInfo size:%d\n", list.size());

	clock_t ck1 = clock();

	for (auto it = list.cbegin(); it != list.cend(); it++)
	{
		nim_comp::FriendItem *item = new nim_comp::FriendItem(*it);
		m_PaintManager.FillBox(item, _T("friend_item.xml"), this, &m_PaintManager, NULL);
		list_friend_->Add(item);
	}
	printf("load friends ui(in list) %d ms\n", clock()-ck1);

	for (auto it = list.cbegin(); it != list.cend(); it++)
	{
		nim_comp::FriendItemEx *item = new nim_comp::FriendItemEx(*it);
		m_PaintManager.FillBox(item, _T("friend_item.xml"), this, &m_PaintManager, NULL);

		wstring wname = nbase::UTF8ToUTF16(it->GetName());
		const char *piny = shared::PinYinHelper::GetInstance()->ConvertToFullSpell(wname);
		
		int index = piny[0] - 'a' + 2;
		if (index > 1 && index < 28);
		{
			group_nodes_[index]->AddChildNode(item);
		}
	}
	for (int i = 0; i < 28; i++)
	{
		if (group_nodes_[i]->HasChild())
		{
			group_nodes_[i]->SetVisible();
		}
	}
	printf("load friends ui(in treeview) %d ms\n", clock() - ck1);
}

void MainForm::OnGetAllSessionInfo(int unread_count, const nim_comp::SessionDataList& data_list)
{
	printf("MainForm::OnGetAllSessionInfo size:%d\n", data_list.sessions_.size());

	clock_t ck1 = clock();
	std::list<nim_comp::SessionData> list = (data_list.sessions_);
	for (auto it = list.cbegin(); it != list.cend(); it++)
	{
		nim_comp::SessionItem *item = new nim_comp::SessionItem(*it);
		m_PaintManager.FillBox(item, _T("session_item.xml"), this, &m_PaintManager, NULL);
		list_session_->Add(item);
	}
	printf("load Session ui %d ms\n", clock() - ck1);
}

bool MainForm::MenuLogoffClick(dui::TEvent* param)
{
#if 0
	QCommand::Set(kCmdRestart, L"true");
	std::wstring wacc = nbase::UTF8ToUTF16(nim_ui::LoginManager::GetInstance()->GetVVUser());
	QCommand::Set(kCmdAccount, wacc);
	nim_ui::LoginManager::GetInstance()->DoLogout(false, nim::kNIMLogoutChangeAccout);

	wstring neteaseid_w;
	std::string neteaseid = nim_comp::LoginManager::GetInstance()->GetAccount();
	nbase::win32::MBCSToUnicode(neteaseid, neteaseid_w);

	std::wstring mydoc_dir = QPath::GeMyDocDir();
	std::wstring appconfig_file2 = mydoc_dir + L"\\vv\\user\\" + neteaseid_w + +L"\\app.config";
	::WritePrivateProfileString(L"Login", L"AutoLogin", L"0", appconfig_file2.c_str());		//×Ô¶¯µÇÂ¼
#endif
	return true;
}

bool MainForm::MenuLogoutClick(dui::TEvent* param)
{
	nim_comp::LoginManager::GetInstance()->DoLogout(false);
	return true;
}


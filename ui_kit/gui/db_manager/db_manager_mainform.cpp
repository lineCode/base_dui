#include "stdafx.h"
//#include "../nim_win_demo/resource.h"
#include "db_manager_mainform.h"
//#include "../db_manager.h"
#include "treestruct/db_treestruct.h"
#include "control/db_treeitem.h"
#include "control/db_result_listitem.h"


//#include "../MySQLObject.h"

//#import "msado15.dll" no_namespace rename("EOF", "adoEOF")rename("BOF", "adoBOF")

namespace nim_comp
{
	const LPCTSTR DBManagerMainForm::kClassName = L"DBManagerMainForm";

	DBManagerMainForm::DBManagerMainForm(std::string dbcid, std::wstring company, int64_t sqlbits) :curr_combo_sel_(-1), btn_max_restore_(NULL), sqlbits_(sqlbits)
	{
		dbcid_ = dbcid;
		company_name_ = company;

		btn_max_restore_ = NULL;
		db_tree_ = NULL;
		result_list_ = NULL;
		result_label_ = NULL;
		combo_ip_ = NULL;
		info_label_ = NULL;
		status_label_ = NULL;
		re_input_ = NULL;
		btn_run_ = NULL;
	}

	DBManagerMainForm::~DBManagerMainForm()
	{
		DBManager::GetInstance()->CloseAllConn();
	}

	void DBManagerMainForm::OnFinalMessage(HWND hWnd)
	{
		DBManager::GetInstance()->OnDBMainFormClose();
		::RevokeDragDrop(GetHWND());
		__super::OnFinalMessage(hWnd);
	}

	LRESULT DBManagerMainForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_SIZE)
		{
			if (wParam == SIZE_RESTORED)
				OnWndSizeMax(false);
			else if (wParam == SIZE_MAXIMIZED)
				OnWndSizeMax(true);
		}
		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	void DBManagerMainForm::InitWindow()
	{
		//SetIcon(IDI_ICON);
#if 1
		std::wstring title = company_name_ + L" 的 数据库工具";
		((dui::Label *)m_PaintManager.FindControl(L"title_label"))->SetText(title.c_str());
		SetWindowText(m_hWnd, L"数据库工具");

		//m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&DBManagerMainForm::OnClicked, this, std::placeholders::_1));

		btn_max_restore_ = static_cast<dui::Button*>(m_PaintManager.FindControl(L"btn_max_restore"));

		btn_run_ = (dui::Button *)m_PaintManager.FindControl(L"btnrun");
		//btn_run_->AttachClick(nbase::Bind(&DBManagerMainForm::OnBtnRunClieked, this, std::placeholders::_1));

		re_input_ = (dui::RichEdit*)m_PaintManager.FindControl(L"input_edit");

		db_tree_ = (dui::Tree*)m_PaintManager.FindControl(L"dbtree");
		combo_ip_ = (dui::Combo*)m_PaintManager.FindControl(L"combo_ip");

		result_list_ = (dui::ListView*)m_PaintManager.FindControl(L"result_list");
		result_label_ = (dui::Label*)m_PaintManager.FindControl(L"result_label");
		info_label_ = (dui::Label*)m_PaintManager.FindControl(L"info_label");
		status_label_ = (dui::Label*)m_PaintManager.FindControl(L"status_label");
		oper_label_ = (dui::Label*)m_PaintManager.FindControl(L"oper_label");

		combo_ip_->AttachSelect(std::bind(&DBManagerMainForm::OnComboSelected, this, std::placeholders::_1));
		status_label_->SetText(L"正在获取数据库列表, 请等待...");

		std::wstring oper[7] = { L"select ", L"update ", L"insert ", L"delete ", L"create ", L"alter ", L"drop " };
		std::wstring stroper = L"可执行操作:";
		for (size_t i = 0; i < 7; i++)
		{
			if ((sqlbits_ & (1<<i)) > 0)
			{
				stroper += oper[i];
			}
		}
		if (sqlbits_ == 0)
			stroper += L"无";
		
		oper_label_->SetText(stroper.c_str());

	/*	combo_ip_->SetEnabled(false);
		btn_run_->SetEnabled(false);*/
		//info_label_->SetText(L"info");

		::RegisterDragDrop(GetHWND(), this);
#endif
	}

	bool DBManagerMainForm::OnClicked(dui::TEvent* args)
	{
		dui::Control *pSender = args->pSender;
		if (!pSender)
		{
			return false;
		}
		if (pSender->GetName() == L"btn_max_restore")
		{
			if (::IsZoomed(m_hWnd))
			{
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
			}
			else
			{
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			}
		}
	}

	void DBManagerMainForm::OnWndSizeMax(bool max)
	{
		/*if (btn_max_restore_)
		{
			btn_max_restore_->SetClass(max ? L"btn_wnd_restore" : L"btn_wnd_max");
		}*/
	}

	bool DBManagerMainForm::OnBtnOKClieked(dui::TEvent* args)
	{
#if 0
		std::vector<DB_CONN_INFO> infos = DBManager::GetInstance()->GetDBConnInfos(dbcid_);
		int sel = combo_ip_->GetCurSel();
		if (sel >= 0 && sel < infos.size())
		{
			if (curr_combo_sel_ != sel)
			{
				DBManager::GetInstance()->CloseAllConn();
			}
			DB_CONN_INFO info = infos[sel];
			DBManager::GetInstance()->OpenDB(info.type, (char*)info.ip.c_str(), (char*)info.user.c_str(), (char*)info.pwd.c_str(), (char*)info.db_name.c_str(), info.port, (nbase::Bind(&DBManagerMainForm::OnMySQLEvent, this, std::placeholders::_1, std::placeholders::_2)));
			curr_combo_sel_ = sel;
		}
#endif
		return false;
	}

	bool DBManagerMainForm::OnBtnCancelClieked(dui::TEvent* args)
	{
		DBManager::GetInstance()->CloseAllConn();
		return false;
	}

	bool DBManagerMainForm::OnBtnRunClieked(dui::TEvent* args)
	{
		std::string text;
		if (re_input_->GetText().empty())
		{
			return false;
		}
		std::wstring text_w = re_input_->GetSelText();
		if (text_w.empty())
		{
			text_w = re_input_->GetText();
		}

		nbase::win32::UnicodeToMBCS(text_w, text);

		const char *pstr = text.c_str();
		std::string query = nbase::StringTrimLeft(pstr);
		transform(query.begin(), query.end(), query.begin(), tolower);

		bool oper[7] = { false };
		bool bRight = true;
#if 1
		oper[0] = (query.find("select ") != std::string::npos || query.find("select*") != std::string::npos);
		oper[1] = (query.find("update ") != std::string::npos);
		oper[2] = (query.find("insert ") != std::string::npos);
		oper[3] = (query.find("delete ") != std::string::npos);
		oper[4] = (query.find("create ") != std::string::npos);
		oper[5] = (query.find("alter ") != std::string::npos);
		oper[6] = (query.find("drop ") != std::string::npos);
		for (size_t i = 0; i < 7; i++)
		{
			if (oper[i])
			{
				if ((sqlbits_ & (1<<i)) == 0)
				{
					bRight = false;
					break;
				}
			}
		}
		if (!bRight)
		{
			result_label_->SetText(L"操作权限不足!");
			result_list_->SetVisible(false);
			result_label_->SetVisible(true);
			return false;
		}
#else
		bool is_select = (query.find("select ") >= 0 || query.find("select*"));
		bool is_alter = (query.find("update ") >= 0);
		bool is_create = (query.find("create ") >= 0);
		bool is_delete = (query.find("delete ") >= 0);
		bool is_drop = (query.find("drop ") >= 0);
#endif
		if (DBManager::GetInstance()->Execute(text))
		{
			
		}
		return false;
	}

	bool DBManagerMainForm::OnComboSelected(dui::TEvent* args)
	{
		std::vector<DB_CONN_INFO> infos = DBManager::GetInstance()->GetDBConnInfos(dbcid_);
		int sel = combo_ip_->GetCurSel();
		if (curr_combo_sel_ == sel)
		{
			return false;
		}
		if (sel >= 0 && sel < infos.size())
		{	
			DBManager::GetInstance()->CloseAllConn();
			DB_CONN_INFO info = infos[sel];
			if (DBManager::GetInstance()->OpenDB(info.type, (char*)info.ip.c_str(), (char*)info.user.c_str(), (char*)info.pwd.c_str(), (char*)info.db_name.c_str(), info.port, /*nullptr*/(std::bind(&DBManagerMainForm::OnMySQLEvent, this, std::placeholders::_1, std::placeholders::_2))))
			{
				if (curr_combo_sel_ == -1)
				{
					combo_ip_->SetBkImage(L"file='combo2_220_26.png'");
				}
				curr_combo_sel_ = sel;
			}	
			else
			{
				curr_combo_sel_ = -1;
			}
		}
		return false;
	}

	bool DBManagerMainForm::OnMySQLEvent(DBM_EVENT event, void *p)
	{
		if (DBM_EVENT::DBM_EVENT_QLIST_SUCC == event)
		{
			if (!combo_ip_ || !btn_run_ || !result_label_ || !info_label_ || !status_label_)
			{
				return false;
			}
			std::vector<DB_CONN_INFO> infos = DBManager::GetInstance()->GetDBConnInfos(dbcid_);
			dui::ListContainerElement *ele;
			dui::Label *label;
			std::wstring text_w;
			//std::string text;
			for (int i = 0; i < infos.size(); i++)
			{
				nbase::win32::MBCSToUnicode(infos[i].db_name, text_w);
				ele = new dui::ListContainerElement;
				/*label = new dui::Label;
				label->SetText(text_w.c_str());
				ele->Add(label);*/ 
				ele->SetText(text_w.c_str());	
				ele->SetFixedHeight(20);
				combo_ip_->Add(ele);
			}

			combo_ip_->SetEnabled(true);
			btn_run_->SetEnabled(true);

			result_label_->SetText(L"");
			info_label_->SetText(L"");
			status_label_->SetText(L"已获取数据库列表");
		}
		else if (DBM_EVENT::DBM_EVENT_CONN_START == event)
		{
			db_tree_->/*GetRootNode()->*/RemoveAll();
			result_label_->SetText(L"");
			info_label_->SetText(L"");
			status_label_->SetText(L"正在连接数据库");
		}
		else if (DBM_EVENT::DBM_EVENT_CONN_SUCC == event)
		{
			result_label_->SetText(L"");
			info_label_->SetText(L"");
			status_label_->SetText(L"已连接数据库");
		}
		//------------------------LOAD--------------------------
		else if (DBM_EVENT::DBM_EVENT_LOAD_SUCC == event)
		{
			if (!p || !db_tree_)
			{
				return false;
			}
			DBTreeStruct *tree = (DBTreeStruct*)p;
			DBTreeNode *db_node, *fnode, *item_node, *root = tree->GetRoot();
			DBNode node;

			DBTreeItem *item = new DBTreeItem, *ui_db_node, *ui_fnode, *ui_item_node;
			/*dui::GlobalManager::FillBoxWithCache(item, L"db_manager/db_treeitem.xml");*/
			m_PaintManager.FillBox(item, _T("db_treeitem.xml"), nullptr, &m_PaintManager, NULL);
			item->Init(root->date_);

			dui::TreeNode * ui_root = nullptr/*db_tree_->GetRootNode()*/;
			dui::TreeNode * ui_father_node = ui_root;
			if (ui_father_node)
			{
				ui_father_node->AddChildNode(item);
			}

			db_node = root->child1_;
			while (db_node)
			{
				ui_father_node = ui_root;

				ui_db_node = new DBTreeItem;
				//dui::GlobalManager::FillBoxWithCache(ui_db_node, L"db_manager/db_treeitem.xml");
				m_PaintManager.FillBox(ui_db_node, _T("db_treeitem.xml"), nullptr, &m_PaintManager, NULL);
				ui_db_node->Init(db_node->date_);
				ui_father_node->AddChildNode(ui_db_node);

				fnode = db_node->child1_;
				while (fnode)	//表，视图的F节点
				{
					ui_father_node = ui_db_node;

					ui_fnode = new DBTreeItem;
					//dui::GlobalManager::FillBoxWithCache(ui_fnode, L"db_manager/db_treeitem.xml");
					m_PaintManager.FillBox(ui_fnode, _T("db_treeitem.xml"), nullptr, &m_PaintManager, NULL);
					ui_fnode->Init(fnode->date_);
					ui_father_node->AddChildNode(ui_fnode);

					item_node = fnode->child1_;
					while (item_node)		//item 节点
					{
						ui_father_node = ui_fnode;

						ui_item_node = new DBTreeItem;
						//dui::GlobalManager::FillBoxWithCache(ui_item_node, L"db_manager/db_treeitem.xml");
						m_PaintManager.FillBox(ui_item_node, _T("db_treeitem.xml"), nullptr, &m_PaintManager, NULL);
						ui_item_node->Init(item_node->date_);
						ui_father_node->AddChildNode(ui_item_node);

						item_node = item_node->next_;
					}
					fnode = fnode->next_;
				}
				db_node = db_node->next_;
			}

		}
		else if (DBM_EVENT::DBM_EVENT_LOAD_FAILE == event)
		{
		}
		//---------------------------QUERY------------------------
		else if (DBM_EVENT::DBM_EVENT_QUERY_START == event)
		{
		}
		else if (DBM_EVENT::DBM_EVENT_QUERY_SUCC == event)
		{
			result_list_->SetVisible(true);
			result_label_->SetVisible(false);

			if (!p || !result_list_)
			{
				return false;
			}

			result_list_->RemoveAll();		

			DB_RESULT_TEXT *db_result = (DB_RESULT_TEXT *)p;
			int col = db_result->col;
			int row = db_result->row;
			//char **results = db_result->result;
			if (db_result->type == DB_CONN_TYPE_SQLSERVER)
			{
				std::vector<std::wstring> *results = (std::vector<std::wstring> *)db_result->date;

				DBResultListItem *result_list_item = NULL;
				std::wstring text_w;

				int index = 0, i = 0;;
				for (; i < row; i++)
				{
					if (i == 0)
					{
						result_list_item = new DBResultListItem(DBResultListItem::DB_LISTITEM_HEADER);
						result_list_item->SetBkColor(/*L"bk_db_result_header"*/m_PaintManager.GetColor(L"bk_blacklist_tip"));
						
					}		
					else
					{
						result_list_item = new DBResultListItem(DBResultListItem::DB_LISTITEM_NORMAL);
						if ((i%2) == 0)
						{
							result_list_item->SetBkColor(m_PaintManager.GetColor(L"bk_db_result_item2"));
						}	
					}
					if (result_list_item)
						result_list_item->SetFixedWidth(col * DB_LIST_COL_WIDTH);
						
					for (int j = 0; j < col; j++)
					{
						text_w = results->at(index++);
						result_list_item->AddColumn(text_w);
					}
					result_list_->Add(result_list_item);

				}
			}
			else if (db_result->type == DB_CONN_TYPE_MYSQL)
			{
				char **results = (char **)db_result->date;

				DBResultListItem *result_list_item;
				std::string text;
				std::wstring text_w;

				int index = 0, i = 0;;
				for (; i < row; i++)
				{
					if (i == 0)
					{
						result_list_item = new DBResultListItem(DBResultListItem::DB_LISTITEM_HEADER);
						result_list_item->SetBkColor(m_PaintManager.GetColor(L"bk_blacklist_tip"));
					}		
					else
					{
						result_list_item = new DBResultListItem(DBResultListItem::DB_LISTITEM_NORMAL);
						if ((i % 2) == 0)
						{
							result_list_item->SetBkColor(m_PaintManager.GetColor(L"bk_db_result_item2"));
						}
					}	
					if (result_list_item)
						result_list_item->SetFixedWidth(col * DB_LIST_COL_WIDTH);
					for (int j = 0; j < col; j++)
					{
						text = results[index++];
						nbase::win32::MBCSToUnicode(text, text_w);
						result_list_item->AddColumn(text_w);
					}
					result_list_->Add(result_list_item);

				}
			}
			
		}
		else if (DBM_EVENT::DBM_EVENT_CONN_FAILE == event || DBM_EVENT::DBM_EVENT_QUERY_FAILE == event || DBM_EVENT::DBM_EVENT_EXEC_FAILE == event || DBM_EVENT::DBM_EVENT_EXEC_SUCC == event)
		{
			char *err = (char*)p;
			std::wstring text_w;
			if (err && strlen(err) > 0)
			{
				nbase::win32::MBCSToUnicode(err, text_w);
				result_label_->SetText(text_w.c_str());
			}
			result_label_->SetText(text_w.c_str());
			result_list_->SetVisible(false);
			result_label_->SetVisible(true);
		}
		//------------------------CLOSE-------------------------
		else if (DBM_EVENT::DBM_EVENT_CLOSE_SUCC == event)
		{
			/*dui::TreeNode *root = db_tree_->GetRootNode();
			root->RemoveAllChildNode();*/
			db_tree_->RemoveAll();
			
			char *db_name = (char*)p;
			std::wstring text_w;
			if (db_name && strlen(db_name) > 0)
			{
				nbase::win32::MBCSToUnicode(db_name, text_w);
				text_w.append(L" 已关闭");
				result_label_->SetText(text_w.c_str());
			}
			result_label_->SetText(L"");
			info_label_->SetText(L"");
			status_label_->SetText(text_w.c_str());

			result_list_->SetVisible(false);
			result_label_->SetVisible(true);
		}
	}

	//----------------------------------------------------
	HRESULT DBManagerMainForm::QueryInterface(REFIID iid, void ** ppvObject)
	{
		return S_OK;
	}

	ULONG DBManagerMainForm::AddRef(void)
	{
		return S_OK;
	}

	ULONG DBManagerMainForm::Release(void)
	{
		return S_OK;
	}

	HRESULT DBManagerMainForm::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
	{
		if (!CheckDropEnable(pt))
		{
			*pdwEffect = DROPEFFECT_NONE;
		}
		return S_OK;
	}

	HRESULT DBManagerMainForm::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
	{
		return S_OK;
	}

	HRESULT DBManagerMainForm::DragLeave(void)
	{
		return S_OK;
	}

	HRESULT DBManagerMainForm::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
	{
		if (pDataObj)
		{
			STGMEDIUM stgMedium;
			FORMATETC cFmt = { (CLIPFORMAT)CF_UNICODETEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			HRESULT hr = pDataObj->GetData(&cFmt, &stgMedium);
			if (hr == S_OK)
			{
				wchar_t *str = (wchar_t *)GlobalLock(stgMedium.hGlobal);
				std::wstring text = re_input_->GetText() + str;
				re_input_->SetText(text.c_str());
				GlobalUnlock(stgMedium.hGlobal);
				::ReleaseStgMedium(&stgMedium);
			}
		}
		return S_OK;
	}

	bool DBManagerMainForm::CheckDropEnable(POINTL pt)
	{
		RECT rect = re_input_->GetPos();
		ClientToScreen(GetHWND(), (LPPOINT)&rect);
		ClientToScreen(GetHWND(), ((LPPOINT)&rect) + 1);
		if (pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom)
			return true;
		
		return false;
	}
}
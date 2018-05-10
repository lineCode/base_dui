#include "stdafx.h"
#include "db_manager.h"
#include "shared/threads.h"
#include "module/window/windows_manager.h"
#include "module/http/http_interface.h"
#include "gui/db_manager/db_manager_mainform.h"
#include "MySQLObject.h"
#include "SqlServerObject.h"
#include "module/login/login_manager.h"
//#include "module/http/http_interface.h"

#define ONLY_ONE_DB_CONNED	1

namespace nim_comp
{

	DBManagerMainForm *DBManager::ShowDBManagerMainForm(std::string dbcid, std::wstring company, const int64_t sqlbits)
	{
		DBManagerMainForm* form = (DBManagerMainForm*)WindowsManager::GetInstance()->GetWindow(DBManagerMainForm::kClassName, DBManagerMainForm::kClassName);
#if 1
		if (form != NULL) //当前已经打开的名片正是希望打开的名片
		{
			::ShowWindow(form->GetHWND(), SW_SHOW);
			form->ActiveWindow();
			return form;
		}
		else
		{
			form = new DBManagerMainForm(dbcid, company, sqlbits);
			form->Create(NULL/*nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND()*/, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0L);
			// 获取用户信息
			form->CenterWindow();
			::ShowWindow(form->GetHWND(), SW_SHOW);

			//sqlbits_ = sqlbits;
		}
		if (form)
		{
			form->ActiveWindow();
			db_form_ = form;
		}
		
		if (db_conninfo_map_.find(dbcid) != db_conninfo_map_.cend())
		{
			form->OnMySQLEvent(DBM_EVENT_QLIST_SUCC, NULL);
		}
		else if (find(vct_querying_.begin(), vct_querying_.end(), dbcid) == vct_querying_.end())
		{
			vct_querying_.push_back(dbcid);
			auto task = nbase::Bind(&DBManager::QueryDBList, this, dbcid);
			nbase::ThreadManager::PostTask(shared::kThreadDatabase, task);
		}
#endif
		return form;
	}

	void DBManager::QueryDBList(std::string dbcid)
	{
#if 1
		std::string buf = http_Send("WeiweiInterfaceController/searchVvOdbc", "dbcid=" + dbcid);

		int nBegin = buf.find_first_of('{');
		int nEnd = buf.find_last_of('}');

		if (buf == "" || nBegin<0 || nEnd<0)
		{
			return;
		}

		DB_CONN_INFO info;
		std::vector<DB_CONN_INFO> infos;
		int itype;
		std::string sip;
		std::string strJson = buf.substr(nBegin, nEnd - nBegin + 1);
		Json::Reader reader;
		Json::Value value;
		Json::Value dbs, db;
		if (reader.parse(strJson, value))
		{
			Json::Value dbs = value["vvodbc"];
			int size = dbs.size();
			for (int i = 0; i < size; i++)
			{
				db = dbs[i];
				sip = db["dBServer"].asString();
				std::size_t pos = sip.rfind(":");
				if (pos == std::string::npos)
				{
					continue;
				}
				info.ip = sip.substr(0, pos);
				//const char *ppp = sip.substr(pos + 1).c_str();
				info.port = atoi(sip.substr(pos + 1).c_str());
				info.user = db["dBUser"].asString();
				info.pwd = db["dBPwd"].asString();
				info.db_name = db["dbName"].asString();

				itype = db["dBType"].asInt();
				if (itype == 1)
					info.type = DB_CONN_TYPE_SQLSERVER;
				else if (itype == 2)
					info.type = DB_CONN_TYPE_MYSQL;
				else
					info.type = DB_CONN_TYPE_MYSQL;
				infos.push_back(info);
			}
			AddDBConnInfo(dbcid, infos);
		
			for (auto iter = vct_querying_.begin(); iter != vct_querying_.end(); iter++)
			{
				if (*iter == dbcid)
				{
					vct_querying_.erase(iter);
					//iter = vct_querying_.begin(); //当erase后，旧的容器会被重新整理成一个新的容器
					break;
				}
			}

			if (db_form_)
			{
				db_form_->OnMySQLEvent(DBM_EVENT_QLIST_SUCC, NULL);
			}
			//is_querying_db_info_list_ = false;
		}
#endif
		return;
	}

	void DBManager::QueryDBCallBack(std::string info)
	{
		int i = 0;
		i++;
		return;
	}

	DBManager::DBManager() :curr_sel_obj_(NULL), db_form_(NULL)/*, sqlbits_(0)*/
	{
#if !ONLY_ONE_DB_CONNED
		mysql_obj_ = NULL;
		sqlserver_obj_ = NULL;
#endif
		/*DB_CONN_INFO info;
		info.ip = "rm-bp1010466c2drn229o.mysql.rds.aliyuncs.com";
		info.user = "djj";
		info.pwd = "abc123+-";
		info.db_name = "wd_common";
		info.port = 3306;
		info.type = DB_CONN_TYPE_MYSQL;
		AddDBConnInfo(info);

		info.ip = "139.196.160.148";
		info.user = "sa";
		info.pwd = "Chinajey2017";
		info.db_name = "YPX_HTLS";
		info.port = 1433;
		info.type = DB_CONN_TYPE_SQLSERVER;
		AddDBConnInfo(info);*/
	}

	DBManager::~DBManager()
	{
#if ONLY_ONE_DB_CONNED
		if (curr_sel_obj_)
		{
			curr_sel_obj_->Close();
			delete curr_sel_obj_;
			curr_sel_obj_ = nullptr;
		}
#else
		if (mysql_obj_)
		{
			mysql_obj_->Close();
			delete mysql_obj_;
			mysql_obj_ = nullptr;
		}
		if (sqlserver_obj_)
		{
			sqlserver_obj_->Close();
			delete sqlserver_obj_;
			sqlserver_obj_ = nullptr;
		}
#endif
	}

	bool DBManager::AddDBConnInfo(std::string dbcid, std::vector<DB_CONN_INFO> infos)
	{ 
		if (db_conninfo_map_.find(dbcid) != db_conninfo_map_.cend())
		{
			return false;
		}

		db_conninfo_map_[dbcid] = infos;
		return true;
	}

	DBConnObj* DBManager::OpenDB(DB_CONN_TYPE type, char *ip, char *user, char *pwd, char *db_name, unsigned short port, std::function<bool(DBM_EVENT, void*)> cb)
	{
#if ONLY_ONE_DB_CONNED
		if (curr_sel_obj_)
		{
			if (cb)
			{
				cb(DBM_EVENT_CONN_FAILE, (void*)"请先关闭打开的数据库!");
			}
			return NULL;
		}
		if (type == DB_CONN_TYPE_MYSQL)
		{
			curr_sel_obj_ = new MySQLObject(ip, user, pwd, db_name, port, cb);
		}
		else if (type == DB_CONN_TYPE_SQLSERVER)
		{
			curr_sel_obj_ = new SqlServerObject(ip, user, pwd, db_name, port, cb);
		}
		if (!curr_sel_obj_)
		{
			return NULL;
		}
		if (!curr_sel_obj_->Open())
		{
			delete curr_sel_obj_;
			curr_sel_obj_ = NULL;
			return NULL;
		}
		if (!curr_sel_obj_->LoadTables())	//表加载失败直接返回NULL
		{
			curr_sel_obj_->Close();
			delete curr_sel_obj_;	
			curr_sel_obj_ = NULL;
			return NULL;
		}
		if (!curr_sel_obj_->LoadViews())	//视图和函数加载失败返回curr_sel_obj_
		{
			return curr_sel_obj_;
		}
		if (!curr_sel_obj_->LoadFuncs())	//视图加载失败返回curr_sel_obj_
		{
			return curr_sel_obj_;
		}
		if (!curr_sel_obj_->LoadProcs())	//视图加载失败返回curr_sel_obj_
		{
			return curr_sel_obj_;
		}
#else
		if (type == DB_CONN_TYPE_MYSQL && !mysql_obj_)
		{
			mysql_obj_ = new MySQLObject;
			mysql_obj_->Open(ip, user, pwd, db_name, port, cb);
			curr_sel_obj_ = mysql_obj_;
		}
		else if (type == DB_CONN_TYPE_SQLSERVER && !sqlserver_obj_)
		{
			sqlserver_obj_ = new SqlServerObject;
			sqlserver_obj_->Open(ip, user, pwd, db_name, port, cb);
			curr_sel_obj_ = sqlserver_obj_;
		}
#endif
		return curr_sel_obj_;
	}

	bool DBManager::CloseAllConn()
	{
#if ONLY_ONE_DB_CONNED
		if (curr_sel_obj_)
		{
			curr_sel_obj_->Close();
			delete curr_sel_obj_;
			curr_sel_obj_ = nullptr;
		}
#else
		if (mysql_obj_)
		{
			mysql_obj_->Close();
			delete mysql_obj_;
			mysql_obj_ = nullptr;
		}
		if (sqlserver_obj_)
		{
			sqlserver_obj_->Close();
			delete sqlserver_obj_;
			sqlserver_obj_ = nullptr;
		}
		curr_sel_obj_ = NULL;
#endif
		return true;
	}

	bool DBManager::CloseDB(DBConnObj* obj)
	{
#if ONLY_ONE_DB_CONNED
		if (obj == curr_sel_obj_)
		{
			if (curr_sel_obj_->Close())
			{
				delete curr_sel_obj_;
				curr_sel_obj_ = nullptr;
				return true;
			}
		}
#else
		if (type == DB_CONN_TYPE_MYSQL && mysql_obj_)
		{
			mysql_obj_->Close();
			delete mysql_obj_;
			mysql_obj_ = NULL;
		}
		else if (type == DB_CONN_TYPE_SQLSERVER && sqlserver_obj_)
		{
			sqlserver_obj_->Close();
			delete sqlserver_obj_;
			sqlserver_obj_ = NULL;
		}
		return true;
#endif
		return false;
	}

	bool DBManager::Execute(std::string str)
	{
		if (curr_sel_obj_)
		{
			const char *pstr = str.c_str();
			std::string query = nbase::StringTrimLeft(pstr);
			std::string sel = query.substr(0, 7);
			transform(sel.begin(), sel.end(), sel.begin(), toupper);
			bool is_select = ((sel.compare("SELECT ") == 0) || (sel.compare("SELECT*") == 0));

			bool ret = curr_sel_obj_->Execute(str.c_str());
			if (ret)
			{
				if (!is_select)
				{
#if 0
					DBOperLogData data;
					data.user_id = nim_comp::LoginManager::GetInstance()->GetYYTAccount();
					data.netease_id = nim_comp::LoginManager::GetInstance()->GetAccount();
					data.user_name = nim_comp::LoginManager::GetInstance()->GetName();
					data.dbcid = nim_comp::LoginManager::GetInstance()->GetDbcId();
					data.dbcname = nim_comp::LoginManager::GetInstance()->GetDbcName();;
					data.sapaccount = nim_comp::LoginManager::GetInstance()->GetSapAccount();
					data.time = time(NULL);
					data.is_select = is_select ? 1 : 0;
					data.db_name = curr_sel_obj_->def_db_;
					data.sql = query;
					LoginDB::GetInstance()->WriteDBOperData(data);
#endif
				}
			}
#if 0
			std::string param = ("&userid=" + LoginManager::GetInstance()->GetYYTAccount()
				+ "&edbName=" + curr_sel_obj_->def_db_ + "&edbUrl=" + curr_sel_obj_->ip_ + "&edbText=" + str);
			auto task = [this, param]{
				std::string buf = http_Send("WeiweiInterfaceController/saveSqlLog", param);
				printf("%s\n", buf.c_str());
			};
			Post2GlobalMisc(task);
#endif
			return true;
		}
		return false;
	}

	void DBManager::OnDBMainFormClose()
	{
		db_form_ = NULL;
		//conn_infos_.clear();
	}

	void DBManager::ReadDBOperDataByUserid(std::string uid)
	{
#if 0
		LoginDB* login_db = LoginDB::GetInstance();
		if (login_db)
		{
			try
			{
				std::vector<DBOperLogData> all_data;
				DBOperLogData data;
				login_db->QueryDBOperDataByUid(uid, all_data);
				if (all_data.size() > 0)
				{
					std::vector<DBOperLogData>::iterator it = all_data.begin();
					std::string str, strname, strdbcname;
					std::wstring wstr;
					char cBuf[1024] = {}, cTime[64] = {};
					for (; it != all_data.end(); it++)
					{
						data = *it;

						nbase::win32::MBCSToUnicode(data.user_name, wstr, CP_UTF8);
						nbase::win32::UnicodeToMBCS(wstr, strname);
						nbase::win32::MBCSToUnicode(data.dbcname, wstr, CP_UTF8);
						nbase::win32::UnicodeToMBCS(wstr, strdbcname);
						strftime(cTime, 64, "%Y-%m-%d %H:%M:%S", localtime((time_t*)&data.time));
						sprintf(cBuf, "%s %s\n\t%s %s %s %s\n", strname.c_str(), strdbcname.c_str(), cTime, data.is_select == 0 ? "unselected" : "selected",
							data.db_name.c_str(), data.sql);
						OutputDebugStringA(cBuf);

					}
				}
			}
			catch (...)
			{
				//DEFLOG(nbase::LogInterface::LV_ERR, __FILE__, __LINE__, "decrypt user info error - kernel error");
			}
		}
#endif
	}


}
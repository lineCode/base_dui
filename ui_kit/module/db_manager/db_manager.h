#pragma once

namespace nim_comp
{
	typedef enum DB_MANAGER_EVENT
	{
		DBM_EVENT_QLIST_START	= 0,	//根据dbcid查询数据库列表
		DBM_EVENT_QLIST_SUCC,
		DBM_EVENT_QLIST_FAILE,

		DBM_EVENT_CONN_START	= 10,
		DBM_EVENT_CONN_SUCC,
		DBM_EVENT_CONN_FAILE,

		DBM_EVENT_LOAD_START	= 20,
		DBM_EVENT_LOAD_SUCC,
		DBM_EVENT_LOAD_FAILE,

		DBM_EVENT_QUERY_START	= 30,
		DBM_EVENT_QUERY_SUCC,
		DBM_EVENT_QUERY_FAILE,

		DBM_EVENT_EXEC_START	= 40,
		DBM_EVENT_EXEC_SUCC,
		DBM_EVENT_EXEC_FAILE,

		DBM_EVENT_CLOSE_START	= 90,
		DBM_EVENT_CLOSE_SUCC,
		DBM_EVENT_CLOSE_FAILE,
	}DBM_EVENT;

	enum DB_CONN_TYPE
	{
		DB_CONN_TYPE_UNDEFINE = 0,
		DB_CONN_TYPE_MYSQL = 1,
		DB_CONN_TYPE_SQLSERVER,
		DB_CONN_TYPE_MAX
	};
	struct DB_CONN_INFO
	{
		std::string ip;
		std::string user;
		std::string pwd;
		std::string db_name;
		unsigned short port;
		DB_CONN_TYPE type;

		std::wstring Describe()
		{
			if (type == DB_CONN_TYPE_MYSQL)
				return L"MySQL";
			else if (type == DB_CONN_TYPE_SQLSERVER)
				return L"SqlServer";
		}
	};

	struct DB_RESULT_TEXT
	{
		DB_CONN_TYPE type;
		int col;
		int row;
		//char **result;
		void *date;
	};

	class DBConnObj;
	class DBManagerMainForm;
	class DBManager : public nbase::SupportWeakCallback
	{
	public:
		SINGLETON_DEFINE(DBManager);
		DBManagerMainForm *ShowDBManagerMainForm(std::string dbcid, std::wstring company, const int64_t sqlbits);
	public:
		DBManager();
		~DBManager();

		DBConnObj *GetCurConnSqlObject(){ return curr_sel_obj_; };

		void OnDBMainFormClose();

		bool AddDBConnInfo(std::string dbcid, std::vector<DB_CONN_INFO> infos);
		std::vector<DB_CONN_INFO> GetDBConnInfos(std::string dbcid){ return db_conninfo_map_[dbcid]; };

		DBConnObj* OpenDB(DB_CONN_TYPE type, char *ip, char *user, char *pwd, char *db_name, unsigned short port, std::function<bool(DBM_EVENT, void*)> cb);
		bool CloseAllConn();
		bool CloseDB(DBConnObj* obj);

		bool Execute(std::string str);

		void ReadDBOperDataByUserid(std::string uid);
	private:
		//typedef std::function<void(std::string)> QueryDBCb;
		void QueryDBList(std::string dbcid);
		void QueryDBCallBack(std::string info);


	private:
	/*	DBConnObj *mysql_obj_;
		DBConnObj *sqlserver_obj_;*/
		//int64_t		sqlbits_;

		DBConnObj *curr_sel_obj_;
		//std::vector<DB_CONN_INFO> conn_infos_;
		std::map<std::string, std::vector<DB_CONN_INFO>> db_conninfo_map_;

		DBManagerMainForm *db_form_;

		std::vector<std::string>	vct_querying_;
	};

}
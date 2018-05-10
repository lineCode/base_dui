#pragma once 

#include "db_manager.h"

namespace nim_comp
{
	class DBConnObj
	{
	public:
		DBConnObj(DB_CONN_TYPE type, char *ip, char *user, char *pwd, char *db_name, unsigned short port, std::function<bool(DBM_EVENT, void*)> cb) 
			:type_(type), dbtree_struct_(NULL), cb_(cb), port_(port)
		{
			ip_ = ip;
			user_ = user;
			pwd_ = pwd;
			def_db_ = db_name;
			port_ = port;

			DBNode node;
			node.text = "数据库管理";
			node.type = DBNODETYPE_ROOT;
			dbtree_struct_ = new DBTreeStruct(new DBTreeNode(node));
		};
		virtual ~DBConnObj(){
			if (dbtree_struct_)
			{
				delete dbtree_struct_;
				dbtree_struct_ = NULL;
			}
			cb_ = nullptr;
		};
	public:
		virtual bool Open() = 0;
		virtual bool LoadTables() = 0;
		virtual bool LoadViews() = 0;
		virtual bool LoadFuncs() = 0;
		virtual bool LoadProcs() = 0;
		virtual bool Close() = 0;
		virtual bool Execute(const char* str) = 0;

	public:
		DB_CONN_TYPE type_;

		std::string ip_;
		std::string user_;
		std::string pwd_;
		std::string def_db_;
		unsigned short port_;

		DBTreeStruct *dbtree_struct_;
		std::function<bool(DBM_EVENT, void*)> cb_;
	};
}
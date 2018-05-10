#pragma once

#include "mysql6.0.2/mysql.h"
#include "gui/db_manager/treestruct/db_treestruct.h"
#include "db_connobj.h"

namespace nim_comp
{
	class MySQLObject : public DBConnObj
	{
	public:
		MySQLObject(char *ip, char *user, char *pwd, char *db_name, unsigned short port, std::function<bool(DBM_EVENT, void*)> cb);
		~MySQLObject();

	public:
		virtual bool Open() override;
		virtual bool LoadTables() override;
		virtual bool LoadViews() override;
		virtual bool LoadFuncs() override;
		virtual bool LoadProcs() override;
		virtual bool Close() override;
		virtual bool Execute(const char* str) override;
	private:
		MYSQL *mysql_;
	};
}
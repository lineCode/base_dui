#include "stdafx.h"
#include "MySQLObject.h"
#include "db_manager.h"

namespace nim_comp
{
	MySQLObject::MySQLObject(char *ip, char *user, char *pwd, char *db_name, unsigned short port, std::function<bool(DBM_EVENT, void*)> cb) 
		: DBConnObj(DB_CONN_TYPE_MYSQL, ip, user, pwd, db_name, port, cb), mysql_(NULL)
	{
		
	}
	MySQLObject::~MySQLObject()
	{
		Close();
	}


	bool MySQLObject::Open()
	{
		if (!dbtree_struct_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"数据库打开失败, 树结构不存在!");
			}
			return false;
		}

		if (cb_)
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_CONN_START, NULL);
		}
		
		if (!mysql_)
		{
			mysql_ = mysql_init(NULL);
		}
		if (mysql_)
		{
			MYSQL *psql_conn = mysql_real_connect(mysql_, ip_.c_str(), user_.c_str(), pwd_.c_str(), def_db_.c_str(), port_, NULL, 0);
			if (psql_conn && psql_conn == mysql_)
			{
				DBNode node; 
				node.text = def_db_;
				node.type = DBNODETYPE_DB;
				DBTreeNode *dbnode = new DBTreeNode(node);
				dbtree_struct_->Insert(dbnode, NULL);

				if (cb_)
				{
					cb_(DBM_EVENT::DBM_EVENT_CONN_SUCC, NULL);
				}
				return true;
			}
		}
		
		return false;
	}

	bool MySQLObject::LoadTables()
	{
		if (!mysql_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"表加载失败,未初始化mysql!");
			}
			return false;
		}

		if (!dbtree_struct_ || !dbtree_struct_->GetRoot() || !dbtree_struct_->GetRoot()->child1_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"表加载失败,树结构异常!");
			}
			return false;
		}

		DBTreeNode *dbnode = dbtree_struct_->GetRoot()->child1_;
		DBTreeNode *fnode = dbnode;
		DBNode node;
		bool first = true;

		char query[512] = {};
		sprintf(query, "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = '%s'", def_db_.c_str());
		int ret = mysql_real_query(mysql_, query, strlen(query));
		if (ret == 0)
		{
			MYSQL_RES *res = mysql_store_result(mysql_);		//将结果保存在res结构体中
			DBTreeNode *tb_node;
			while (MYSQL_ROW row = mysql_fetch_row(res))
			{
				if (first)
				{
					first = false;
					node.text = "表";
					node.type = DBNODETYPE_TABLE_F;
					tb_node = new DBTreeNode(node);
					dbtree_struct_->Insert(tb_node, fnode);
					fnode = tb_node;
				}

				node.text = row[0];
				node.type = DBNODETYPE_TABLE_ITEM;
				tb_node = new DBTreeNode(node);
				dbtree_struct_->Insert(tb_node, fnode);
			}
			mysql_free_result(res);
			return true;
		}

		return false;
	}

	bool MySQLObject::LoadViews()
	{
		if (!mysql_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"视图加载失败, 未初始化mysql!");
			}
			return false;
		}

		if (!dbtree_struct_ || !dbtree_struct_->GetRoot() || !dbtree_struct_->GetRoot()->child1_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"视图加载失败,树结构异常!");
			}
			return false;
		}

		DBTreeNode *dbnode = dbtree_struct_->GetRoot()->child1_;
		DBTreeNode *fnode = dbnode;
		DBNode node;

		bool first = true;

		char query[512] = {};
		sprintf(query, "SELECT VIEW_DEFINITION FROM INFORMATION_SCHEMA.VIEWS WHERE TABLE_SCHEMA = '%s'", def_db_.c_str());
		int ret = mysql_real_query(mysql_, query, strlen(query));
		if (ret == 0)
		{
			MYSQL_RES *res = mysql_store_result(mysql_);		//将结果保存在res结构体中
			DBTreeNode *tb_node;
			fnode = dbnode;
			while (MYSQL_ROW row = mysql_fetch_row(res))
			{
				if (first)
				{
					first = false;
					node.text = "视图";
					node.type = DBNODETYPE_VIEW_F;
					tb_node = new DBTreeNode(node);
					dbtree_struct_->Insert(tb_node, fnode);
					fnode = tb_node;
				}
				
				node.text = row[0];
				node.type = DBNODETYPE_VIEW_ITEM;
				tb_node = new DBTreeNode(node);
				dbtree_struct_->Insert(tb_node, fnode);
				
			}
			mysql_free_result(res);
			return true;
		}
		if (cb_)
		{
			cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
		}
		return false;
	}

	bool MySQLObject::LoadFuncs()
	{
		if (!mysql_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"函数加载失败, 未初始化mysql!");
			}
			return false;
		}

		if (!dbtree_struct_ || !dbtree_struct_->GetRoot() || !dbtree_struct_->GetRoot()->child1_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"函数加载失败,树结构异常!");
			}
			return false;
		}

		DBTreeNode *dbnode = dbtree_struct_->GetRoot()->child1_;
		DBTreeNode *fnode = dbnode;
		DBNode node;

		bool first = true;

		char query[512] = {};
		sprintf(query, "select name from mysql.proc where db = '%s' and type = 'FUNCTION'", def_db_.c_str());
		int ret = mysql_real_query(mysql_, query, strlen(query));
		if (ret == 0)
		{
			MYSQL_RES *res = mysql_store_result(mysql_);		//将结果保存在res结构体中
			DBTreeNode *tb_node;
			fnode = dbnode;
			while (MYSQL_ROW row = mysql_fetch_row(res))
			{
				if (first)
				{
					first = false;
					node.text = "函数";
					node.type = DBNODETYPE_FUNC_F;
					tb_node = new DBTreeNode(node);
					dbtree_struct_->Insert(tb_node, fnode);
					fnode = tb_node;
				}

				node.text = row[0];
				node.type = DBNODETYPE_FUNC_ITEM;
				tb_node = new DBTreeNode(node);
				dbtree_struct_->Insert(tb_node, fnode);

			}
			mysql_free_result(res);

			/*if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
			}*/
			return true;
		}
		if (cb_)
		{
			cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
		}
		return false;
	}

	bool MySQLObject::LoadProcs()
	{
		if (!mysql_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"存储过程加载失败, 未初始化mysql!");
			}
			return false;
		}

		if (!dbtree_struct_ || !dbtree_struct_->GetRoot() || !dbtree_struct_->GetRoot()->child1_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"存储过程加载失败,树结构异常!");
			}
			return false;
		}

		DBTreeNode *dbnode = dbtree_struct_->GetRoot()->child1_;
		DBTreeNode *fnode = dbnode;
		DBNode node;

		bool first = true;

		char query[512] = {};
		sprintf(query, "select name from mysql.proc where db = '%s' and type='PROCEDURE'", def_db_.c_str());
		int ret = mysql_real_query(mysql_, query, strlen(query));
		if (ret == 0)
		{
			MYSQL_RES *res = mysql_store_result(mysql_);		//将结果保存在res结构体中
			DBTreeNode *tb_node;
			fnode = dbnode;
			while (MYSQL_ROW row = mysql_fetch_row(res))
			{
				if (first)
				{
					first = false;
					node.text = "存储过程";
					node.type = DBNODETYPE_PROC_F;
					tb_node = new DBTreeNode(node);
					dbtree_struct_->Insert(tb_node, fnode);
					fnode = tb_node;
				}

				node.text = row[0];
				node.type = DBNODETYPE_PROC_ITEM;
				tb_node = new DBTreeNode(node);
				dbtree_struct_->Insert(tb_node, fnode);

			}
			mysql_free_result(res);

			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
			}
			return true;
		}
		if (cb_)
		{
			cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
		}
		return false;
	}

	bool MySQLObject::Close()
	{
		mysql_close(mysql_);
		mysql_ = NULL;
		if (cb_)
		{
			cb_(DBM_EVENT::DBM_EVENT_CLOSE_SUCC, (void*)def_db_.c_str());
		}
		cb_ = nullptr;
		return true;
	}

	bool MySQLObject::Execute(const char* str)
	{
		if (!mysql_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_QUERY_FAILE, (void*)"数据库未初始化!");
			}
			return false;
		}

		std::string query = nbase::StringTrimLeft(str);
		std::string sel = query.substr(0, 7);
		transform(sel.begin(), sel.end(), sel.begin(), toupper);
		bool is_select = ((sel.compare("SELECT ") == 0) || (sel.compare("SELECT*") == 0));

		
		if (is_select)
		{
			int ret = mysql_real_query(mysql_, str, strlen(str));
			if (ret == 0)
			{
				
				MYSQL_RES *res = mysql_store_result(mysql_);		//将结果保存在res结构体中
				if (res)
				{
					int num_fields = mysql_num_fields(res);
					int num_rows = mysql_num_rows(res);
					char **results = new char*[num_fields*(num_rows + 1)];

					MYSQL_FIELD* fields;		//数组，包含所有field的元数据
					fields = mysql_fetch_fields(res);
					int i = 0;
					for (; i < num_fields; ++i)
					{
						results[i] = fields[i].name;
					}

					DBNode node;
					//DBTreeNode *tb_node;
					while (MYSQL_ROW row = mysql_fetch_row(res))
					{
						for (int t = 0; t < num_fields; t++)
						{
							char *pc = row[t];
							if (row[t] == NULL)
							{
								results[i++] = "NULL";
							}
							else if (strcmp(row[t], "0x1") == 0 || strcmp(row[t], "\x1") == 0)
							{
								results[i++] = "true";
							}
							else if (strcmp(row[t], "") == 0)
							{
								results[i++] = "false";
							}
							else
								results[i++] = row[t];
						}
					}

					if (cb_)
					{
						DB_RESULT_TEXT db_result;
						db_result.type = DB_CONN_TYPE_MYSQL;
						db_result.col = num_fields;
						db_result.row = num_rows + 1;
						db_result.date = results;
						cb_(DBM_EVENT::DBM_EVENT_QUERY_SUCC, &db_result);
					}

					delete[]results;
					mysql_free_result(res);
					return true;
				}
			}
			else
			{
				if (cb_)
				{
					cb_(DBM_EVENT::DBM_EVENT_QUERY_FAILE, (void*)"查询失败!");
				}
			}
		}
		else
		{
			int ret = mysql_query(mysql_, str);
			if (ret == 0)
			{
				if (cb_)
				{
					cb_(DBM_EVENT::DBM_EVENT_EXEC_SUCC, (void*)"操作成功!");
				}
				return true;
			}
			else
			{
				//const char * err = mysql_error(mysql_);
				if (cb_)
				{
					cb_(DBM_EVENT::DBM_EVENT_EXEC_FAILE, (void*)mysql_error(mysql_));
				}
				
			}
		}
		return false;
	}
}
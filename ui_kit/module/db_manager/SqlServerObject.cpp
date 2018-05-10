#include "stdafx.h"
#include "SqlServerObject.h"





namespace nim_comp
{
	SqlServerObject::SqlServerObject(char *ip, char *user, char *pwd, char *db_name, unsigned short port, std::function<bool(DBM_EVENT, void*)> cb) 
		: DBConnObj(DB_CONN_TYPE_SQLSERVER, ip, user, pwd, db_name, port, cb)
	{
		connection_ptr_ = NULL;
		connection_ptr_.CreateInstance(__uuidof(Connection));
		recordset_ptr_ = NULL;
		recordset_ptr_.CreateInstance(__uuidof(Recordset));
		cmd_ptr_ = NULL;
		//cmd_ptr_.CreateInstance(__uuidof(Command));
	}
	SqlServerObject::~SqlServerObject()
	{
		Close();	
	}


	bool SqlServerObject::Open()
	{
		if (cb_ == nullptr)
		{
			return false;
		}
		if (!dbtree_struct_ || connection_ptr_ == NULL || recordset_ptr_ == NULL)
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_CONN_FAILE, (void*)"数据库基础组件还未创建!");
			return false;
		}
		if (IsOpen())
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_CONN_FAILE, (void*)"已打开");
			return false;
		}

		cb_(DB_MANAGER_EVENT::DBM_EVENT_CONN_START, NULL);
	
		try
		{
			char conn[1024] = {};
			sprintf(conn, "Driver={sql server};server=%s,%d;uid=%s;pwd=%s; database=%s; "
				, ip_.c_str(), port_, user_.c_str(), pwd_.c_str(), def_db_.c_str());
			connection_ptr_->Open(conn, "", "", adModeUnknown);

			DBNode node;
			node.text = def_db_;
			node.type = DBNODETYPE_DB;
			DBTreeNode *dbnode = new DBTreeNode(node);
			dbtree_struct_->Insert(dbnode, NULL);

			cb_(DB_MANAGER_EVENT::DBM_EVENT_CONN_SUCC, NULL);
			return true;
		}
		catch (/*_com_error &e*/...) 
		{
			if (cb_)
			{
				//std::wstring err = e.Description();
				cb_(DB_MANAGER_EVENT::DBM_EVENT_CONN_FAILE, (void*)"连接失败!");
			}
			return false;
		}

		return false;
	}

	bool SqlServerObject::LoadTables()
	{ 
		if (cb_ == nullptr)
		{
			return false;
		}
		if (!dbtree_struct_ || connection_ptr_ == NULL || recordset_ptr_ == NULL)
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"表加载失败,数据库基础组件还未创建!");
			return false;
		}
		if (IsOpen() == false)
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"表加载失败,数据库还未连接!");
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
		DBTreeNode *fnode = dbnode/*, *db_node = NULL*/;
		DBNode node;
		bool first = true;

		char query[512] = {};
		sprintf(query, "SELECT Name FROM SysObjects Where XType='U' ORDER BY Name");
		try 
		{
			if (IsRecordSetOpen())
			{
				recordset_ptr_->Close();
			}

			recordset_ptr_->Open(query, (IDispatch*)connection_ptr_, adOpenForwardOnly, adLockOptimistic, adCmdText);

		}
		catch (/*_com_error &e*/...)
		{
			//std::wstring err = e.Description();
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"表加载失败, 查询表出错!");
			return false;
		}

		try 
		{
			//db_node = dbnode;

			DBTreeNode *tb_node;
			recordset_ptr_->MoveFirst();

			while (!recordset_ptr_->adoEOF)
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

				std::string tbl_name = (char*)(_bstr_t)(recordset_ptr_->Fields->GetItem(_variant_t("Name"))->Value);
				node.text = tbl_name;
				node.type = DBNODETYPE_TABLE_ITEM;
				tb_node = new DBTreeNode(node);
				dbtree_struct_->Insert(tb_node, fnode);
				recordset_ptr_->MoveNext();
			}
			return true;
		}
		catch (_com_error &e)
		{
			std::wstring err = e.Description();
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"表加载失败, 轮询表时出错!");
			return false;
		}

		return false; 
	}

	bool SqlServerObject::LoadViews()
	{
		if (cb_ == nullptr)
		{
			return false;
		}
		if (!dbtree_struct_ || connection_ptr_ == NULL || recordset_ptr_ == NULL)
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"视图加载失败,数据库基础组件还未创建!");
			return false;
		}
		if (IsOpen() == false)
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"视图加载失败,数据库还未连接!");
			return false;
		}

		if (!dbtree_struct_ || !dbtree_struct_->GetRoot() || !dbtree_struct_->GetRoot()->child1_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"视图加载失败,树结构异常!");
			}
			return false;
		}

		DBTreeNode *dbnode = dbtree_struct_->GetRoot()->child1_;
		DBTreeNode *fnode = dbnode/*, *db_node = NULL*/;
		DBNode node;
		bool first = true;

		char query[512] = {};
		sprintf(query, "SELECT Name FROM SysObjects Where XType='V' ORDER BY Name");
		try
		{
			if (IsRecordSetOpen())
			{
				recordset_ptr_->Close();
			}

			recordset_ptr_->Open(query, (IDispatch*)connection_ptr_, adOpenForwardOnly, adLockOptimistic, adCmdText);

		}
		catch (_com_error &e)
		{
			cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"视图加载失败, 查询表出错!");
			return false;
		}

		try
		{
			DBTreeNode *tb_node;
			recordset_ptr_->MoveFirst();

			while (!recordset_ptr_->adoEOF)
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

				std::string view_name = (char*)(_bstr_t)(recordset_ptr_->Fields->GetItem(_variant_t("Name"))->Value);
				node.text = view_name;
				node.type = DBNODETYPE_VIEW_ITEM;
				tb_node = new DBTreeNode(node);
				dbtree_struct_->Insert(tb_node, fnode);
				recordset_ptr_->MoveNext();
			}
			return true;
		}
		catch (_com_error &e)
		{
			//std::wstring err = e.Description();
			cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"视图加载失败, 轮询表时出错!");
			return false;
		}

		cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
		return false;
	}

	bool SqlServerObject::LoadFuncs()
	{
		if (cb_ == nullptr)
		{
			return false;
		}
		if (!dbtree_struct_ || connection_ptr_ == NULL || recordset_ptr_ == NULL)
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"函数加载失败,数据库基础组件还未创建!");
			return false;
		}
		if (IsOpen() == false)
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"函数加载失败,数据库还未连接!");
			return false;
		}

		if (!dbtree_struct_ || !dbtree_struct_->GetRoot() || !dbtree_struct_->GetRoot()->child1_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"函数加载失败,树结构异常!");
			}
			return false;
		}

		DBTreeNode *dbnode = dbtree_struct_->GetRoot()->child1_;
		DBTreeNode *fnode = dbnode/*, *db_node = NULL*/;
		DBNode node;
		bool first = true;

		char query[512] = {};
		sprintf(query, "SELECT Name FROM SysObjects Where (XType='FN' or XType='TF') ORDER BY Name");
		try
		{
			if (IsRecordSetOpen())
			{
				recordset_ptr_->Close();
			}

			recordset_ptr_->Open(query, (IDispatch*)connection_ptr_, adOpenForwardOnly, adLockOptimistic, adCmdText);

		}
		catch (_com_error &e)
		{
			cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"函数加载失败, 查询表出错!");
			return false;
		}

		try
		{
			DBTreeNode *tb_node;
			recordset_ptr_->MoveFirst();

			while (!recordset_ptr_->adoEOF)
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

				std::string view_name = (char*)(_bstr_t)(recordset_ptr_->Fields->GetItem(_variant_t("Name"))->Value);
				node.text = view_name;
				node.type = DBNODETYPE_FUNC_ITEM;
				tb_node = new DBTreeNode(node);
				dbtree_struct_->Insert(tb_node, fnode);
				recordset_ptr_->MoveNext();
			}
			//cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);

			return true;
		}
		catch (_com_error &e)
		{
			//std::wstring err = e.Description();
			cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"函数加载失败, 轮询表时出错!");
			return false;
		}

		cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
		return false;
	}

	bool SqlServerObject::LoadProcs()
	{
		if (cb_ == nullptr)
		{
			return false;
		}
		if (!dbtree_struct_ || connection_ptr_ == NULL || recordset_ptr_ == NULL)
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"存储过程加载失败,数据库基础组件还未创建!");
			return false;
		}
		if (IsOpen() == false)
		{
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"存储过程加载失败,数据库还未连接!");
			return false;
		}

		if (!dbtree_struct_ || !dbtree_struct_->GetRoot() || !dbtree_struct_->GetRoot()->child1_)
		{
			if (cb_)
			{
				cb_(DBM_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"存储过程加载失败,树结构异常!");
			}
			return false;
		}

		DBTreeNode *dbnode = dbtree_struct_->GetRoot()->child1_;
		DBTreeNode *fnode = dbnode/*, *db_node = NULL*/;
		DBNode node;
		bool first = true;

		char query[512] = {};
		sprintf(query, "SELECT Name FROM SysObjects Where XType='P' ORDER BY Name");
		try
		{
			if (IsRecordSetOpen())
			{
				recordset_ptr_->Close();
			}

			recordset_ptr_->Open(query, (IDispatch*)connection_ptr_, adOpenForwardOnly, adLockOptimistic, adCmdText);

		}
		catch (_com_error &e)
		{
			cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"存储过程加载失败, 查询表出错!");
			return false;
		}

		try
		{
			DBTreeNode *tb_node;
			recordset_ptr_->MoveFirst();

			while (!recordset_ptr_->adoEOF)
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

				std::string view_name = (char*)(_bstr_t)(recordset_ptr_->Fields->GetItem(_variant_t("Name"))->Value);
				node.text = view_name;
				node.type = DBNODETYPE_PROC_ITEM;
				tb_node = new DBTreeNode(node);
				dbtree_struct_->Insert(tb_node, fnode);
				recordset_ptr_->MoveNext();
			}
			cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);

			return true;
		}
		catch (_com_error &e)
		{
			//std::wstring err = e.Description();
			cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
			cb_(DB_MANAGER_EVENT::DBM_EVENT_LOAD_FAILE, (void*)"存储过程加载失败, 轮询表时出错!");
			return false;
		}

		cb_(DBM_EVENT::DBM_EVENT_LOAD_SUCC, (void*)dbtree_struct_);
		return false;
	}

	bool SqlServerObject::Close()
	{
		if (IsOpen())
			connection_ptr_->Close();
		if (IsRecordSetOpen())
		{
			recordset_ptr_->Close();
		}

		if (connection_ptr_)
		{
			connection_ptr_.Release();
			connection_ptr_ = NULL;
		}
		if (recordset_ptr_)
		{
			recordset_ptr_.Release();
			recordset_ptr_ = NULL;
		}
		if (cmd_ptr_)
		{
			cmd_ptr_.Release();
			cmd_ptr_ = NULL;
		}
		
		if (cb_)
		{
			cb_(DBM_EVENT::DBM_EVENT_CLOSE_SUCC, (void*)def_db_.c_str());
		}
		cb_ = nullptr;
		
		return true;
	}

	bool SqlServerObject::Execute(const char* str)
	{
		if (!IsOpen())
		{
			if (cb_)
			{
				cb_(DB_MANAGER_EVENT::DBM_EVENT_QUERY_FAILE, (void*)"数据库未打开!");
			}
			return false;
		}

		if (IsRecordSetOpen())
		{
			recordset_ptr_->Close();
		}

		std::string query = nbase::StringTrimLeft(str);
		std::string sel = query.substr(0, 7);
		transform(sel.begin(), sel.end(), sel.begin(), toupper);
		bool is_select = ((sel.compare("SELECT ") == 0) || (sel.compare("SELECT*") == 0));

		try
		{
			recordset_ptr_->Open(str, (IDispatch*)connection_ptr_, adOpenForwardOnly, adLockOptimistic, adCmdText);
		}
		catch (_com_error &e)
		{
			std::wstring err = e.Description();
			if (cb_)
			{
				if (is_select)
				{
					cb_(DB_MANAGER_EVENT::DBM_EVENT_QUERY_FAILE, (void*)"查询错误!");
				}
				else 
					cb_(DB_MANAGER_EVENT::DBM_EVENT_QUERY_FAILE, (void*)"执行错误!");
			}
			
			return false;
		}

		if (!is_select)
		{
			if (cb_)
				cb_(DB_MANAGER_EVENT::DBM_EVENT_QUERY_FAILE, (void*)"执行成功!");
			return true;
		}
		else
		{
			try
			{
			
				int num_fields = recordset_ptr_->Fields->GetCount();
				int num_rows = recordset_ptr_->GetRecordCount();
			
				if (num_rows == -1)
				{
					num_rows = 0;
					if (recordset_ptr_->adoEOF != VARIANT_TRUE)
						recordset_ptr_->MoveFirst();

					while (recordset_ptr_->adoEOF != VARIANT_TRUE)
					{
						num_rows++;
						recordset_ptr_->MoveNext();
					}
					if (num_rows > 0)
						recordset_ptr_->MoveFirst();
				}
				char **results = new char*[num_fields*(num_rows + 1)];
				memset(results, 0, sizeof(char*)*num_fields*(num_rows + 1));

				_variant_t vtIndex, vtVar;
				vtIndex.vt = VT_I2;
				std::vector<std::wstring> texts(num_fields*(num_rows + 1));

				int i = 0;
				for (; i < num_fields; ++i)
				{
					vtIndex.iVal = i;
					texts[i] = recordset_ptr_->Fields->GetItem(vtIndex)->GetName();
					/*nbase::win32::UnicodeToMBCS(recordset_ptr_->Fields->GetItem(vtIndex)->GetName(), texts[i]);
					results[i] = (char*)texts[i].c_str();		*/
				}

				DBNode node;
				//DBTreeNode *tb_node;
				int ival;
				long lval;
				unsigned long ulval;
				double dval;


				wchar_t wbuf_num[128];
				while (!recordset_ptr_->adoEOF)
				{
					for (int j = 0; j < num_fields; j++)
					{
						vtIndex = (long)j;
						vtVar = recordset_ptr_->GetCollect(vtIndex);//要查询表中的字段名
						
						if (vtVar.vt == VT_NULL)
						{
							texts[i++] = L"NULL";
						}
						else if (vtVar.vt == VT_EMPTY)
						{
							texts[i++] = L"EMPTY";
						}
						//--------int----------
						else if (vtVar.vt == VT_I2 || vtVar.vt == VT_UI1)
						{
							ival = vtVar.iVal;
							texts[i++] = _itow(ival, wbuf_num, 10);
						}
						else if (vtVar.vt == VT_INT)
						{
							ival = vtVar.intVal;
							texts[i++] = _itow(ival, wbuf_num, 10);
						}
						//--------------long------------
						else if (vtVar.vt == VT_I4)
						{
							lval = vtVar.lVal;
							texts[i++] = _itow(lval, wbuf_num, 10);
						}
						else if (vtVar.vt == VT_UI4)
						{
							ulval = vtVar.ulVal;
							texts[i++] = _itow(ulval, wbuf_num, 10);
						}
						//--------double----------
						else if (vtVar.vt == VT_R4)
						{
							dval = vtVar.fltVal;
							/*_gcvt(dval, 20, wbuf_num);
							texts[i++] = _itow(dval, wbuf_num, 10);*/
							texts[i++] = std::to_wstring(dval);
						}
						else if (vtVar.vt == VT_R8)
						{
							dval = vtVar.dblVal;
							//texts[i++] = _itow(dval, wbuf_num, 10);
							texts[i++] = std::to_wstring(dval);
						}
						else if (vtVar.vt == VT_DECIMAL)
						{
							dval = vtVar.decVal.Lo32;
							dval *= (vtVar.decVal.sign == 128) ? -1 : 1;
							dval /= pow(10.0, vtVar.decVal.scale);
							//texts[i++] = _itow(dval, wbuf_num, 10);
							texts[i++] = std::to_wstring(dval);
						}
						//-----------------------------
						else if (vtVar.vt == VT_BOOL)
						{
							texts[i++] = vtVar.boolVal == VARIANT_TRUE ? L"true" : L"false";
						}
						//-------------str-----------------
						else if (vtVar.vt == VT_BSTR)
						{
							texts[i++] = vtVar.bstrVal;
						}
						//-------------date------------
						else if (vtVar.vt == VT_DATE)
						{
							SYSTEMTIME systime;
							VariantTimeToSystemTime(vtVar.date, &systime);
							wsprintf(wbuf_num, L"%d-%d-%d %d:%d:%d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
							texts[i++] = wbuf_num;
						}
						//-------------money------------
						else if (vtVar.vt == VT_CY)
						{
							
							//VarUI1FromCy(vtVar.cyVal, wbuf_num);
							BSTR pbstrOut;
							VarFormatCurrency(&vtVar, -1, -2, -2, -2, VAR_CALENDAR_HIJRI, &pbstrOut);
							
							texts[i++] = (_bstr_t)pbstrOut;
						}
						else
						{
							texts[i++] = L"XXX";
						}
					}
					recordset_ptr_->MoveNext();
				}

				if (cb_)
				{
					DB_RESULT_TEXT db_result;
					db_result.type = DB_CONN_TYPE_SQLSERVER;
					db_result.col = num_fields;
					db_result.row = num_rows + 1;
					//db_result.result = results;
					db_result.date = &texts;
					cb_(DB_MANAGER_EVENT::DBM_EVENT_QUERY_SUCC, &db_result);
				}

				delete[]results;
				return true;
			}
			catch (_com_error &e)
			{
				//std::wstring err = e.Description();
				cb_(DB_MANAGER_EVENT::DBM_EVENT_QUERY_FAILE, (void*)"查询错误!");
				return false;
			}
		}

		return false;
	}
}
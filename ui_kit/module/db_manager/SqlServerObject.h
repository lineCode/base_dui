#pragma once

#import "c:\program files\common files\system\ado\msado15.dll"  no_namespace rename("EOF", "adoEOF")

#include "gui/db_manager/treestruct/db_treestruct.h"
#include "db_connobj.h"


namespace nim_comp
{
	class SqlServerObject : public DBConnObj
	{
	public:
		SqlServerObject(char *ip, char *user, char *pwd, char *db_name, unsigned short port, std::function<bool(DBM_EVENT, void*)> cb);
		~SqlServerObject();

		bool IsOpen()
		{
			if (connection_ptr_)
				return connection_ptr_->GetState() != adStateClosed;
			return false;
		}
		bool IsRecordSetOpen()
		{
			if (recordset_ptr_ != NULL)
				return recordset_ptr_->GetState() != adStateClosed;
			return false;
		}

	public:
		virtual bool Open() override;
		virtual bool LoadTables() override;
		virtual bool LoadViews() override;
		virtual bool LoadFuncs() override;
		virtual bool LoadProcs() override;
		virtual bool Close() override;
		virtual bool Execute(const char* str) override;

	private:
		BOOL GetFieldValue(LPCTSTR lpFieldName, int& nValue);
		BOOL GetFieldValue(int nIndex, int& nValue);
		BOOL GetFieldValue(LPCTSTR lpFieldName, long& lValue);
		BOOL GetFieldValue(int nIndex, long& lValue);
		BOOL GetFieldValue(LPCTSTR lpFieldName, unsigned long& ulValue);
		BOOL GetFieldValue(int nIndex, unsigned long& ulValue);
		BOOL GetFieldValue(LPCTSTR lpFieldName, double& dbValue);
		BOOL GetFieldValue(int nIndex, double& dbValue);
		//BOOL GetFieldValue(LPCTSTR lpFieldName, CString& strValue, CString strDateFormat = _T(""));
		//BOOL GetFieldValue(int nIndex, CString& strValue, CString strDateFormat = _T(""));
	/*	BOOL GetFieldValue(LPCTSTR lpFieldName, COleDateTime& time);
		BOOL GetFieldValue(int nIndex, COleDateTime& time);*/
		BOOL GetFieldValue(int nIndex, bool& bValue);
		BOOL GetFieldValue(LPCTSTR lpFieldName, bool& bValue);
		/*BOOL GetFieldValue(int nIndex, COleCurrency& cyValue);
		BOOL GetFieldValue(LPCTSTR lpFieldName, COleCurrency& cyValue);*/
		BOOL GetFieldValue(int nIndex, _variant_t& vtValue);
		BOOL GetFieldValue(LPCTSTR lpFieldName, _variant_t& vtValue);

		BOOL IsFieldNull(LPCTSTR lpFieldName);
		BOOL IsFieldNull(int nIndex);
		BOOL IsFieldEmpty(LPCTSTR lpFieldName);
		BOOL IsFieldEmpty(int nIndex);
	private:
		_ConnectionPtr	connection_ptr_;
		_RecordsetPtr	recordset_ptr_;
		_CommandPtr		cmd_ptr_;
		_ParameterPtr	parameter_ptr_;

	};
}
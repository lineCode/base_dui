#include "stdafx.h"
#include "user_db.h"
#include "module/login/login_manager.h"
#include "util/user_path.h"

namespace nim_comp
{
#define MSG_EX_FILE		"msg_extend.db"
#define APP_DB			"app.db"
static std::vector<UTF8String> kCreateDBSQLs;

UserDB::UserDB()
{
	static bool sqls_created = false;
	if (!sqls_created)
	{
		kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS account_info(iid INTEGER, account_id TEXT PRIMARY KEY, account_name TEXT, password TEXT)");

		kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS account_status_info(iid INTEGER, account_id TEXT PRIMARY KEY, status INTEGER, last_login_time INTEGER, last_login_dvc_type INTEGER, last_login_dvc_name TEXT, last_login_addr TEXT)");

		//kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS account_login_info(iid INTEGER, account_id TEXT, login_time INTEGER, login_dvc_type INTEGER, login_dvc_name TEXT, login_addr TEXT)");

		kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS config_info(account_id TEXT, key TEXT, value TEXT)");

		/*kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS msg_local_file(msg_id TEXT PRIMARY KEY, path TEXT, extend TEXT)");

		kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS custom_msglog(serial INTEGER PRIMARY KEY, "
							  "to_account TEXT, from_account TEXT, msg_type INTEGER, msg_time INTEGER, msg_id INTEGER, save_flag INTEGER, "
							  "msg_body TEXT, msg_attach TEXT, apns_text TEXT, msg_status INTEGER, msg_param TEXT)");

		kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS force_push_data(session_id TEXT PRIMARY KEY, data TEXT)");

		kCreateDBSQLs.push_back("CREATE TABLE IF NOT EXISTS user_timetag(type TEXT PRIMARY KEY, timetag INTEGER, longex INTERGER, textex TEXT)");*/

		sqls_created = true;
	}

	this->Load();
}

UserDB::~UserDB()
{
	this->Close();
}

bool UserDB::Load()
{
	return CreateDBFile();
}

void UserDB::Close()
{
	db_.Close();
}

bool UserDB::CreateDBFile()
{
	bool result = false;
	std::string acc = LoginManager::GetInstance()->GetAccount();
	std::wstring dirctory = GetUserOtherResPath();
	UTF8String dbfile = nbase::UTF16ToUTF8(dirctory) + APP_DB;
	db_filepath_ = dbfile;
	std::string key =/* nim::Tool::GetMd5*/(LoginManager::GetInstance()->GetAccount());
	result = db_.Open(dbfile.c_str(),
		key,
		ndb::SQLiteDB::modeReadWrite|ndb::SQLiteDB::modeCreate|ndb::SQLiteDB::modeSerialized
		);
	if (result)
	{
		int dbresult = SQLITE_OK;
		for (size_t i = 0; i < kCreateDBSQLs.size(); i++)
		{
			dbresult |= db_.Query(kCreateDBSQLs[i].c_str());
		}
		result = dbresult == SQLITE_OK;
	}

	return result;
}
#if 1
bool UserDB::InsertAccountInfo(account_info &info)
{
	nbase::NAutoLock auto_lock(&lock_);
	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "INSERT into account_info (account_id, account_name, password) values (?, ?, ?);");

	stmt.BindText(1, info.account_id.c_str(), info.account_id.size());
	stmt.BindText(2, info.account_name.c_str(), info.account_name.size());
	stmt.BindText(3, info.password.c_str(), info.password.size());
	int32_t result = stmt.NextRow();
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (no_error)
	{
		return true;
	}
	else
	{
		QLOG_ERR(L"error: INSERT account_info for account_id: {0}, account_name: {1}, password: {2}") 
			<< info.account_id << info.account_name << info.password;
	}

	return false;
}

bool UserDB::QueryAccountInfo(std::string account_id, account_info &info)
{
	nbase::NAutoLock auto_lock(&lock_);

	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "SELECT * FROM account_info WHERE account_id=?");
	stmt.BindText(1, account_id.c_str(), account_id.size());
	int32_t result = stmt.NextRow();
	bool find = false;
	if (result == SQLITE_ROW)
	{
		//info.iid = atoi(stmt.GetTextField(0));
		info.account_id = stmt.GetTextField(1);
		info.account_name = stmt.GetTextField(2);
		info.password = stmt.GetTextField(3);
		//info.last_login_time = stmt.GetTextField(5);
		find = true;
	}
	bool no_error = (result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE);
	if (!no_error)
	{
		QLOG_ERR(L"error: QueryDataWithMsgId for id: {0}, reason: {1}") << account_id << result;
	}
	return find;
}
#else
bool UserDB::InsertData(const std::string& msg_id, const std::string& path, const std::string& extend)
{
	nbase::NAutoLock auto_lock(&lock_);
	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "INSERT OR REPLACE into msg_local_file (msg_id, path, extend) values (?, ?, ?);");

	stmt.BindText(1, msg_id.c_str(), msg_id.size());
	stmt.BindText(2, path.c_str(), path.size());
	stmt.BindText(3, extend.c_str(), extend.size());
	int32_t result = stmt.NextRow();
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (no_error)
	{
		return true;
	}
	else
	{
		QLOG_ERR(L"error: insert MsgExDB for id: {0}, path: {1}, reason: {2}") << msg_id << path << result;
	}
	return false;
}

bool UserDB::QueryDataWithMsgId(const std::string& msg_id, std::string& path, std::string& extend)
{
	nbase::NAutoLock auto_lock(&lock_);
	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "SELECT * FROM msg_local_file WHERE msg_id=?");
	stmt.BindText(1, msg_id.c_str(), msg_id.size());
	int32_t result = stmt.NextRow();
	bool find = false;
	if (result == SQLITE_ROW)
	{
		path = stmt.GetTextField(1);
		extend = stmt.GetTextField(2);
		find = true;
	}
	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
	if (!no_error)
	{
		QLOG_ERR(L"error: QueryDataWithMsgId for id: {0}, reason: {1}") << msg_id << result;
	}
	return find;
}
#endif
//用于保存一些自定义通知消息
//bool UserDB::InsertMsgData(const nim::SysMessage& msg)
//{
//	nbase::NAutoLock auto_lock(&lock_);
//	ndb::SQLiteStatement stmt;
//
//	db_.Query(stmt, "insert into custom_msglog(serial, to_account, from_account, \
//					msg_type, msg_time, msg_id, save_flag, msg_body, msg_attach, apns_text, \
//					msg_status, msg_param) values(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
//
//	stmt.BindText(1, msg.receiver_accid_.c_str(), msg.receiver_accid_.size());
//	stmt.BindText(2, msg.sender_accid_.c_str(), msg.sender_accid_.size());
//	stmt.BindInt(3, msg.type_);
//	stmt.BindInt64(4, msg.timetag_);
//	stmt.BindInt64(5, msg.id_);
//	stmt.BindInt(6, msg.msg_setting_.need_offline_);
//	stmt.BindText(7, msg.content_.c_str(), msg.content_.size());
//	stmt.BindText(8, msg.attach_.c_str(), msg.attach_.size());
//	stmt.BindText(9, msg.msg_setting_.push_content_.c_str(), msg.msg_setting_.push_content_.size());
//	stmt.BindInt(10, msg.status_);
//	stmt.BindText(11, "");
//
//	int32_t result = stmt.NextRow();
//	bool no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
//	if (!no_error)
//	{
//		QLOG_ERR(L"error: InsertMsgData, reason: {0}") << result;
//	}
//	return no_error;
//}
//
//std::vector<nim::SysMessage> UserDB::QueryMsgData(int64_t time, int limit)
//{
//	nbase::NAutoLock auto_lock(&lock_);
//	std::vector<nim::SysMessage> ret_msgs;
//	ndb::SQLiteStatement stmt;
//	if (time <= 0)
//	{
//		db_.Query(stmt, "SELECT * FROM custom_msglog ORDER BY msg_time DESC LIMIT ? ");
//		stmt.BindInt(1, limit);
//	} 
//	else
//	{
//		db_.Query(stmt, "SELECT * FROM custom_msglog WHERE msg_time < ? ORDER BY msg_time DESC LIMIT ? ");
//		stmt.BindInt64(1, time);
//		stmt.BindInt(2, limit);
//	}
//	int32_t result = stmt.NextRow();
//	while (result == SQLITE_ROW)
//	{
//		nim::SysMessage msg;
//		msg.receiver_accid_ = stmt.GetTextField(1);
//		msg.sender_accid_ = stmt.GetTextField(2);
//		msg.type_ = (nim::NIMSysMsgType)stmt.GetIntField(3);
//		msg.timetag_ = stmt.GetInt64Field(4);
//		msg.id_ = stmt.GetInt64Field(5);
//		msg.msg_setting_.need_offline_ = stmt.GetIntField(6) > 0 ? nim::BS_TRUE : nim::BS_FALSE;
//		msg.content_ = stmt.GetTextField(7);
//		msg.attach_ = stmt.GetTextField(8);
//		msg.msg_setting_.push_content_ = stmt.GetTextField(9);
//		msg.status_ = (nim::NIMSysMsgStatus)stmt.GetIntField(10);
//		ret_msgs.push_back(msg);
//		result = stmt.NextRow();
//	}
//	return ret_msgs;
//}

//bool UserDB::InsertForcePushData(std::map<std::string, std::string> &data)
//{
//	nbase::NAutoLock auto_lock(&lock_);
//	ndb::SQLiteAutoTransaction transaction(&db_);
//	transaction.Begin();
//
//	ndb::SQLiteStatement stmt;
//	db_.Query(stmt, "INSERT OR REPLACE into force_push_data (session_id, data) values (?, ?);");
//
//	bool no_error = true;
//	for (auto &i : data)
//	{
//		stmt.BindText(1, i.first.c_str(), i.first.size());
//		stmt.BindText(2, i.second.c_str(), i.second.size());
//		int32_t result = stmt.NextRow();
//		no_error = result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE;
//		if (!no_error)
//		{
//		 	QLOG_ERR(L"error: InsertForcePushData for session_id: {0}, reason: {1}") << i.first << result;
//		 	break;
//		}
//		stmt.Rewind();
//	}
//	stmt.Finalize();
//
//	if (no_error)
//		return transaction.Commit();
//	else
//		return transaction.Rollback();
//}
//
//void UserDB::QueryAllForcePushData(std::map<std::string, std::string> &data)
//{
//	nbase::NAutoLock auto_lock(&lock_);
//	ndb::SQLiteStatement stmt;
//
//	db_.Query(stmt, "SELECT * FROM force_push_data");
//	int32_t result = stmt.NextRow();
//
//	std::string session_id;
//	std::string session_data;
//	while (result == SQLITE_ROW)
//	{
//		session_id = stmt.GetTextField(0);
//		session_data = stmt.GetTextField(1);
//	
//		data[session_id] = session_data;
//		result = stmt.NextRow();
//	}
//}
//
//void UserDB::ClearForcePushData()
//{
//	nbase::NAutoLock auto_lock(&lock_);
//
//	ndb::SQLiteStatement stmt;
//	db_.Query(stmt, "delete from force_push_data;");
//	stmt.NextRow();
//}

bool UserDB::InsertTimetag(TimeTagType type, uint64_t timetag)
{
	nbase::NAutoLock auto_lock(&lock_);

	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "INSERT OR REPLACE into timetag (type, timetag, longex, textex) values (?, ?, ?, ?);");
	std::string string_type = nbase::StringPrintf("%d", type);
	stmt.BindText(1, string_type.c_str(), (int)string_type.size());
	stmt.BindInt64(2, timetag);
	stmt.BindInt64(3, 0);
	stmt.BindText(4, "", 0);

	int32_t result = stmt.NextRow();
	bool no_error = (result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE);
	if (!no_error)
	{
		QLOG_ERR(L"error: Set user db timetag For {0}, reason: {1}") << type << result;
	}

	return no_error;
}

bool UserDB::QueryTimetag(TimeTagType type, uint64_t &timetag)
{
	nbase::NAutoLock auto_lock(&lock_);

	timetag = 0;
	ndb::SQLiteStatement stmt;
	db_.Query(stmt, "SELECT timetag FROM timetag WHERE type=?");
	std::string string_type = nbase::StringPrintf("%d", type);
	stmt.BindText(1, string_type.c_str(), (int)string_type.size());

	int32_t result = stmt.NextRow();
	bool no_error = (result == SQLITE_OK || result == SQLITE_ROW || result == SQLITE_DONE);
	if (no_error)
		timetag = stmt.GetInt64Field(0);

	return no_error;
}

}
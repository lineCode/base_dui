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
	std::string key =/* Tool::GetMd5*/(LoginManager::GetInstance()->GetAccount());
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
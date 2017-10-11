#include "stdafx.h"
#include "util.h"
#include "shared/log.h"
#include "shellapi.h"
#include "xml_util.h"

//
#if 0
std::string QString::GetGUID()
{
	return nim::Tool::GetUuid();
}

std::string QString::GetMd5(const std::string& input)
{
	return nim::Tool::GetMd5(input);
}

void QString::NIMFreeBuf(void *data)
{
	return nim::Global::FreeBuf(data);
}
#endif
//
std::wstring QPath::GetAppPath()
{
	return nbase::win32::GetCurrentModuleDirectory();
}

std::wstring QPath::GetUserAppDataDir(const std::string& app_account)
{
#if 1
	return GetAppPath();
#else
	return nbase::UTF8ToUTF16(nim::Tool::GetUserAppdataDir(app_account));
#endif
}

std::wstring QPath::GetLocalAppDataDir()
{
#if 1
	return L"";
#else
	return nbase::UTF8ToUTF16(nim::Tool::GetLocalAppdataDir());
#endif
}

std::wstring QPath::GetNimAppDataDir(const std::wstring& app_data_dir)
{
	std::wstring dir = nbase::win32::GetLocalAppDataDir();
	if (app_data_dir.empty() || app_data_dir.find(L"\\") != std::wstring::npos)
	{
		dir.append(L"Netease");
	}
	else
	{
		dir.append(app_data_dir);
	}

#ifdef _DEBUG
	dir.append(L"\\NIM_Debug\\");
#else
	dir.append(L"\\NIM\\");
#endif

	return dir;
}

//
std::map<std::wstring,std::wstring> QCommand::key_value_;

void QCommand::ParseCommand( const std::wstring &cmd )
{
#if 0
	std::list<std::wstring> arrays = ui::StringHelper::Split(cmd, L"/");
	for(std::list<std::wstring>::const_iterator i = arrays.begin(); i != arrays.end(); i++)
	{
		std::list<std::wstring> object = ui::StringHelper::Split(*i, L" ");
		assert(object.size() == 2);
		key_value_[ *object.begin() ] = *object.rbegin();
	}
#endif
}

std::wstring QCommand::Get( const std::wstring &key )
{
	std::map<std::wstring,std::wstring>::const_iterator i = key_value_.find(key);
	if(i == key_value_.end())
		return L"";
	else
		return i->second;
}

void QCommand::Set( const std::wstring &key, const std::wstring &value )
{
	key_value_[key] = value;
}

void QCommand::Erase(const std::wstring &key)
{
	key_value_.erase(key);
}

bool QCommand::AppStartWidthCommand( const std::wstring &app, const std::wstring &cmd )
{
	HINSTANCE hInst = ::ShellExecuteW(NULL, L"open", app.c_str(), cmd.c_str(), NULL, SW_SHOWNORMAL);
	return (int)hInst > 32;
}

bool QCommand::RestartApp(const std::wstring &cmd)
{
	wchar_t app[1024] = { 0 };
	GetModuleFileName(NULL, app, 1024);
	HINSTANCE hInst = ::ShellExecuteW(NULL, L"open", app, cmd.c_str(), NULL, SW_SHOWNORMAL);
	return (int)hInst > 32;
}

std::string GetConfigValue(const std::string& key)
{
	std::string value;
#if 0
	std::wstring server_conf_path = QPath::GetAppPath();
	server_conf_path.append(L"server_conf.txt");
	TiXmlDocument document;
	if (shared::LoadXmlFromFile(document, server_conf_path))
	{
		TiXmlElement* root = document.RootElement();
		if (root)
		{
			Json::Value srv_config;
			if (auto pchar = root->Attribute(key.c_str()))
			{
				value = pchar;
			}
		}
	}
#endif
	return value;
}

std::string GetConfigValueAppKey()
{
	std::string app_key = DEMO_GLOBAL_APP_KEY;
	std::string new_app_key = GetConfigValue("kAppKey");
	if (!new_app_key.empty())
	{
		app_key = new_app_key;
	}
	return app_key;
}

bool IsNimDemoAppKey(const std::string& appkey)
{
	if (appkey == DEMO_GLOBAL_APP_KEY || appkey == DEMO_GLOBAL_TEST_APP_KEY)
		return true;

	return false;
}
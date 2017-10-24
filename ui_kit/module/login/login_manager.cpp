#include "stdafx.h"
#include "login_manager.h"
//#include "shared/xml_util.h"
//#include "login_callback.h"
#include "util/user_path.h"
#include "module/db/user_db.h"

namespace nim_comp
{

const wchar_t kMutexName[] = L"Netease.IM.PC.Instance.";

LoginManager::LoginManager()
{
	account_ = "";
	password_ = "";
	status_ = LoginStatus_NONE;
	active_ = true;
	ReadDemoLogLevel();
}

bool LoginManager::IsSelf( const std::string &account )
{
	return (account == account_);
}

void LoginManager::SetAccount( const std::string &account )
{
	account_ = account;
	nbase::LowerString(account_);
}

std::string LoginManager::GetAccount()
{
	return account_;
}

bool LoginManager::IsEqual(const std::string& account)
{
	std::string new_account = account;
	nbase::LowerString(new_account);
	return account_ == new_account;
}

void LoginManager::SetPassword( const std::string &password )
{
	password_ = password;
}

std::string LoginManager::GetPassword()
{
	return password_;
}

void LoginManager::SetLoginStatus( LoginStatus status )
{
	status_ = status;
}

LoginStatus LoginManager::GetLoginStatus()
{
	return status_;
}

void LoginManager::SetLinkActive( bool active )
{
#if 0
	active_ = active;
	
	Json::Value json;
	json["link"] = active;
	NotifyCenter::GetInstance()->InvokeNotify(NT_LINK, json);
#endif
}

bool LoginManager::IsLinkActive()
{
	return active_;
}

void LoginManager::CreateSingletonRunMutex()
{
	std::string config = GetConfigValue("check_singleton");
	if (!config.empty() && 0 == atoi(config.c_str()))
		return;

	std::wstring mutex_name = kMutexName + nbase::UTF8ToUTF16(account_);
	::CreateMutex(NULL, TRUE, mutex_name.c_str());
}

bool LoginManager::CheckSingletonRun(const std::wstring& username)
{
	std::string config = GetConfigValue("check_singleton");
	if (!config.empty() && 0 == atoi(config.c_str()))
		return true;

	// 同一PC不允许重复登录同一账号
	std::wstring mutex_name = kMutexName + username;
	HANDLE mutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex_name.c_str());
	if (mutex != NULL)
	{
		::CloseHandle(mutex);
		return false;
	}
	return true;
}

void LoginManager::ReadDemoLogLevel()
{
#if 0
	std::wstring server_conf_path = QPath::GetAppPath();
	server_conf_path.append(L"server_conf.txt");
	nim::SDKConfig config;
	TiXmlDocument document;
	if (shared::LoadXmlFromFile(document, server_conf_path))
	{
		TiXmlElement* root = document.RootElement();
		if (root)
		{
			if (auto pchar = root->Attribute("kNIMSDKLogLevel")){
				int log_level = 5;
				nbase::StringToInt((std::string)pchar, &log_level);
				SetDemoLogLevel(log_level);
			}
			if (auto pchar = root->Attribute("LimitFileSize")){
				int file_size = 15;
				nbase::StringToInt((std::string)pchar, &file_size);
				SetFileSizeLimit(file_size);
			}
		}
	}
#endif
}

void LoginManager::DoAfterLogin()
{
	QLOG_APP(L"-----{0} account login-----") << LoginManager::GetInstance()->GetAccount();
#if 1
	/*bool ret = AudioManager::GetInstance()->InitAudio(GetUserDataPath());
	assert(ret);*/

	LoginManager::GetInstance()->CreateSingletonRunMutex();
	//TeamService::GetInstance()->QueryAllTeamInfo();

	//ForcePushManager::GetInstance()->Load();
#endif

	/*auto _LogRobot = []{
		const long m2 = 2 * 1024 * 1024, m1 = 1024 * 1024;
		QLogImpl::GetInstance()->HalfTo(m2, m1);

		StdClosure task = nbase::Bind(&_LogRobot);
		nbase::ThreadManager::PostDelayedTask(shared::kThreadGlobalMisc, task, nbase::TimeDelta::FromMinutes(10));
	};

	StdClosure task = nbase::Bind(&_LogRobot);
	nbase::ThreadManager::PostDelayedTask(shared::kThreadGlobalMisc, task, nbase::TimeDelta::FromMinutes(1));*/
}

void LoginManager::RegLoginManagerCallback(const OnStartLogin& cb_start, const OnLoginError& cb_result, const OnCancelLogin& cb_cancel, const OnHideWindow& cb_hide, const OnDestroyWindow& cb_destroy, const OnShowMainWindow& cb_show_main)
{
	cb_start_login_ = cb_start;
	cb_login_error_ = cb_result;
	cb_cancel_login_ = cb_cancel;
	cb_hide_window_ = cb_hide;
	cb_destroy_window_ = cb_destroy;
	cb_show_main_window_ = cb_show_main;
}

void LoginManager::InvokeLoginError(int error)
{
	if (cb_login_error_)
		cb_login_error_(error);
}

void LoginManager::InvokeCancelLogin()
{
	if (cb_cancel_login_)
		cb_cancel_login_();
}

void LoginManager::InvokeHideWindow()
{
	if (cb_hide_window_)
		cb_hide_window_();
}

void LoginManager::InvokeDestroyWindow()
{
	if (cb_destroy_window_)
		cb_destroy_window_();
}

void LoginManager::InvokeShowMainForm()
{
	if (cb_show_main_window_)
		cb_show_main_window_();
}

bool LoginManager::IsLoginFormValid()
{
	bool is_valid = cb_start_login_ && cb_cancel_login_ && cb_destroy_window_ && cb_hide_window_ && cb_login_error_ && cb_show_main_window_;
	return is_valid;
}

void LoginManager::DoLogout(bool over, NIMLogoutType type )
{
	//LoginCallbackObject::DoLogout(over, type);
}

void LoginManager::DoLogin(std::string user, std::string pass, LoginCallback cb)
{
	assert(GetLoginStatus() == LoginStatus_NONE);
	SetLoginStatus(LoginStatus_LOGIN);

	SetAccount(user);
	//std::string pass_md5 = QString::GetMd5(pass); //密码MD5加密（用户自己的应用请去掉加密）
	SetPassword(pass);

	auto _InitUserFolder = [](){
		nbase::CreateDirectory(GetUserDataPath());
		nbase::CreateDirectory(GetUserImagePath());
		nbase::CreateDirectory(GetUserAudioPath());
		nbase::CreateDirectory(GetUserOtherResPath());
	};
	_InitUserFolder();

	auto _InitLog = [](){
#ifdef _DEBUG
		QLogImpl::GetInstance()->SetLogLevel(LV_PRO);
#else
		QLogImpl::GetInstance()->SetLogLevel(LoginManager::GetInstance()->GetDemoLogLevel());
#endif
		std::wstring dir = GetUserDataPath();
		QLogImpl::GetInstance()->SetLogFile(dir + kLogFile);
	};
	_InitLog();
	{
		int ver = 0;
		//std::wstring vf;
		//LocalHelper::GetAppLocalVersion(ver, vf);
		QLOG_APP(L"App Version {0}") << ver;
		QLOG_APP(L"Account {0}") << LoginManager::GetInstance()->GetAccount();
		QLOG_APP(L"UI ThreadId {0}") << GetCurrentThreadId();
		QLOG_APP(L"-----login begin-----");
	}

	//----------------------------------------------------------------
	LoginCallback *pcb = new LoginCallback(cb);
	StdClosure task = std::bind(&LoginManager::DoLoginAsyn, this, pcb);
	shared::Post2GlobalMisc(task);

	if (cb_start_login_)
	{
		cb_start_login_();
	}
}

void LoginManager::DoLoginAsyn(LoginCallback *pcb)
{
	printf("login in thread GlobalMisc\n");

	Sleep(1000);

	LoginRes login_res;
	login_res.login_step_ = kNIMLoginStepLogin;

	UserDB::account_info info;
	if (!UserDB::GetInstance()->QueryAccountInfo(GetAccount(), info))
	{
		login_res.res_code_ = kNIMResUidNotExist;
	}
	else
	{
		if (info.password == GetPassword())
		{
			login_res.res_code_ = kNIMResSuccess;
		}
		else
		{
			login_res.res_code_ = kNIMResUidPassError;
		}
	}

	if (pcb)
	{
		if (*pcb)
		{
			shared::Post2UI(std::bind(*pcb, login_res));
		}
		delete pcb;
	}
	return;
}

void LoginManager::CancelLogin()
{
	assert(GetLoginStatus() == LoginStatus_LOGIN);
	SetLoginStatus(LoginStatus_CANCEL);
	QLOG_APP(L"-----login cancel begin-----");
}

}
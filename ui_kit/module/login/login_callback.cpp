#include "stdafx.h"
#include "login_callback.h"
//#include "export/nim_ui_all.h"
//#include "gui/link/link_form.h"
//#include "module/local/local_helper.h"
#include "module/login/login_manager.h"
//#include "module/session/session_manager.h"
//#include "module/session/force_push_manager.h"
//#include "module/subscribe_event/subscribe_event_manager.h"
//#include "module/service/user_service.h"
//#include "module/audio/audio_manager.h"
//#include "cef/cef_module/cef_manager.h"
#include "util/user_path.h"
#include "shared/threads.h"
#include "shared/closure.h"
//#include "nim_cpp_client.h"

namespace nim_comp
{
//void _InitUserFolder()
//{
//	nbase::CreateDirectory(GetUserDataPath());
//	nbase::CreateDirectory(GetUserImagePath());
//	nbase::CreateDirectory(GetUserAudioPath());
//	nbase::CreateDirectory(GetUserOtherResPath());
//}
//
//void _InitLog()
//{
//#ifdef _DEBUG
//	QLogImpl::GetInstance()->SetLogLevel(LV_PRO);
//#else
//	QLogImpl::GetInstance()->SetLogLevel(LoginManager::GetInstance()->GetDemoLogLevel());
//#endif
//	std::wstring dir = GetUserDataPath();
//	QLogImpl::GetInstance()->SetLogFile(dir + kLogFile);
//}
//
//void _LogRobot()
//{
//	const long m2 = 2*1024*1024, m1 = 1024*1024;
//	QLogImpl::GetInstance()->HalfTo(m2, m1);
//
//	StdClosure task = nbase::Bind(&_LogRobot);
//	nbase::ThreadManager::PostDelayedTask(shared::kThreadGlobalMisc, task, nbase::TimeDelta::FromMinutes(10));
//}
//
////登录之后的处理：比如读取数据
//void _DoAfterLogin()
//{
//	QLOG_APP(L"-----{0} account login-----") << LoginManager::GetInstance()->GetAccount();
//#if 0
//	bool ret = AudioManager::GetInstance()->InitAudio(GetUserDataPath());
//	assert(ret);
//
//	LoginManager::GetInstance()->CreateSingletonRunMutex();
//	TeamService::GetInstance()->QueryAllTeamInfo();
//
//	ForcePushManager::GetInstance()->Load();
//#endif
//	StdClosure task = nbase::Bind(&_LogRobot);
//	nbase::ThreadManager::PostDelayedTask(shared::kThreadGlobalMisc, task, nbase::TimeDelta::FromMinutes(1));
//}
//
////退出程序前的处理：比如保存数据
//void _DoBeforeAppExit()
//{
//#if 0
//	ForcePushManager::GetInstance()->Save();
//#endif
//	QLOG_APP(L"-----{0} account logout-----") << LoginManager::GetInstance()->GetAccount();
//}
//
////执行sdk退出函数
//void NimLogout(NIMLogoutType type = kNIMLogoutAppExit)
//{
//	QLOG_APP(L"-----logout begin {0}-----") << type;
//	//nim::Client::Logout( type, &LoginCallbackObject::OnLogoutCallback );
//}

//void LoginCallbackObject::DoLogin(std::string user, std::string pass)
//{
//#if 1
//	auto cb = std::bind(OnLoginCallback, std::placeholders::_1, nullptr);
//	//void *p = &cb;
//	StdClosure task = std::bind([](void *user_date)
//	{
//		printf("login in thread GlobalMisc\n");
//		if (user_date)
//		{
//
//		}
//		
//		return;
//	}, &cb);
//	shared::Post2GlobalMisc(task);
//#else
//	assert(LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_NONE);
//	LoginManager::GetInstance()->SetLoginStatus(LoginStatus_LOGIN);
//
//	LoginManager::GetInstance()->SetAccount(user);
//	std::string pass_md5 = QString::GetMd5(pass); //密码MD5加密（用户自己的应用请去掉加密）
//	LoginManager::GetInstance()->SetPassword(pass_md5);
//
//	_InitUserFolder();
//	_InitLog();
//	{
//		int ver = 0;
//		std::wstring vf;
//		LocalHelper::GetAppLocalVersion(ver, vf);
//		QLOG_APP(L"App Version {0}") << ver;
//		QLOG_APP(L"Account {0}") << LoginManager::GetInstance()->GetAccount();
//		QLOG_APP(L"UI ThreadId {0}") << GetCurrentThreadId();
//		QLOG_APP(L"-----login begin-----");
//	}
//
//	//注意：
//	//1. app key是应用的标识，不同应用之间的数据（用户、消息、群组等）是完全隔离的。开发自己的应用时，请替换为自己的app key。
//	//2. 用户登录自己的应用是不需要对密码md5加密的，替换app key之后，请记得去掉加密。
//	std::string app_key = GetConfigValueAppKey();
//	auto cb = std::bind(OnLoginCallback, std::placeholders::_1, nullptr);
//	nim::Client::Login(app_key, LoginManager::GetInstance()->GetAccount(), LoginManager::GetInstance()->GetPassword(), cb);
//#endif
//}

void LoginCallbackObject::OnLoginCallback(LoginRes& login_res, const void* user_data)
{
	QLOG_APP(L"OnLoginCallback: {0} - {1}") << login_res.login_step_ << login_res.res_code_;
	
	/*if (login_res.res_code_ == kNIMResSuccess)
	{
		if (login_res.login_step_ == kNIMLoginStepLogin)
		{
			shared::Post2UI(nbase::Bind(&UILoginCallback, login_res));
			if (!login_res.other_clients_.empty())
			{
				shared::Post2UI(nbase::Bind(LoginCallbackObject::OnMultispotChange, true, login_res.other_clients_));
			}
		}
	}
	else
	{
		shared::Post2UI(nbase::Bind(&UILoginCallback, login_res));
	}*/
}

void LoginCallbackObject::UILoginCallback(const LoginRes& login_res)
{
#if 1
	LoginManager::GetInstance()->SetErrorCode(login_res.res_code_);
#else
	LoginManager::GetInstance()->SetErrorCode(login_res.res_code_);
	if (login_res.relogin_)
	{
		QLOG_APP(L"-----relogin end {0}-----") << login_res.res_code_;

		if (login_res.res_code_ == nim::kNIMResSuccess)
		{
			LoginManager::GetInstance()->SetLoginStatus(LoginStatus_SUCCESS);
			LoginManager::GetInstance()->SetLinkActive(true);
		}
		else if (login_res.res_code_ == nim::kNIMResTimeoutError 
			|| login_res.res_code_ == nim::kNIMResConnectionError
			|| login_res.res_code_ == nim::kNIMLocalResNetworkError
			|| login_res.res_code_ == nim::kNIMResTooBuzy)
		{
			LoginManager::GetInstance()->SetLoginStatus(LoginStatus_NONE);
			LoginManager::GetInstance()->SetLinkActive(false);

			ShowLinkForm(login_res.res_code_, login_res.retrying_);
		}
		else
		{
			LoginManager::GetInstance()->SetLoginStatus(LoginStatus_NONE);

			QCommand::Set(kCmdRestart, L"true");
			std::wstring wacc = nbase::UTF8ToUTF16(LoginManager::GetInstance()->GetAccount());
			QCommand::Set(kCmdAccount, wacc);
			QCommand::Set(kCmdExitWhy, nbase::IntToString16(login_res.res_code_));
			DoLogout(false, nim::kNIMLogoutChangeAccout);
		}
	}
	else
	{
		QLOG_APP(L"-----login end {0}-----") << login_res.res_code_;

		if (nim_ui::LoginManager::GetInstance()->IsLoginFormValid())
		{
			if (LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_CANCEL)
			{
				QLOG_APP(L"-----login cancel end-----");
				if (login_res.res_code_ == nim::kNIMResSuccess)
					NimLogout(nim::kNIMLogoutChangeAccout);
				else
					UILogoutCallback();
				return;
			}
			else
				LoginManager::GetInstance()->SetLoginStatus(login_res.res_code_ == nim::kNIMResSuccess ? LoginStatus_SUCCESS : LoginStatus_NONE);

			if (login_res.res_code_ == nim::kNIMResSuccess)
			{
				nim_ui::LoginManager::GetInstance()->InvokeHideWindow();
				_DoAfterLogin();
				// 登录成功，显示主界面
				nim_ui::LoginManager::GetInstance()->InvokeShowMainForm();
				nim_ui::LoginManager::GetInstance()->InvokeDestroyWindow();
			}
			else
			{
				nim_ui::LoginManager::GetInstance()->InvokeLoginError(login_res.res_code_);
			}
		}
		else
		{
			QLOG_APP(L"login form has been closed");
			LoginManager::GetInstance()->SetLoginStatus(login_res.res_code_ == nim::kNIMResSuccess ? LoginStatus_SUCCESS : LoginStatus_NONE);
			LoginCallbackObject::DoLogout(false);
		}
	}
#endif
}

//void LoginCallbackObject::CacelLogin()
//{
//	assert(LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_LOGIN);
//	LoginManager::GetInstance()->SetLoginStatus(LoginStatus_CANCEL);
//	QLOG_APP(L"-----login cancel begin-----");
//}
//
//void LoginCallbackObject::DoLogout(bool over, NIMLogoutType type)
//{
//	QLOG_APP(L"DoLogout: {0} {1}") <<over <<type;
//#if 1
//#else
//	LoginStatus status = LoginManager::GetInstance()->GetLoginStatus();
//	if(status == LoginStatus_EXIT)
//		return;
//	LoginManager::GetInstance()->SetLoginStatus(LoginStatus_EXIT);
//
//	WindowsManager::GetInstance()->SetStopRegister(true);
//	WindowsManager::GetInstance()->DestroyAllWindows();
//
//	if(status == LoginStatus_NONE)
//	{
//		UILogoutCallback();
//		return;
//	}
//
//	if(over)
//	{
//		if (type == nim::kNIMLogoutKickout || type == nim::kNIMLogoutRelogin)
//		{
//			QCommand::Set(kCmdAccount, nbase::UTF8ToUTF16(LoginManager::GetInstance()->GetAccount()));
//			QCommand::Set(kCmdRestart, L"true");
//			std::wstring param = nbase::StringPrintf(L"%d", type);
//			QCommand::Set(kCmdExitWhy, param);
//		}
//		UILogoutCallback();
//	}
//	else
//	{
//		NimLogout(type);
//	}
//#endif
//}

void LoginCallbackObject::OnLogoutCallback(NIMResCode res_code)
{
	QLOG_APP(L"OnLogoutCallback: {0}") << res_code;
	QLOG_APP(L"-----logout end-----");
	UILogoutCallback();
}

void LoginCallbackObject::UILogoutCallback()
{
#if 1
#else
	if (LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_CANCEL)
	{
		LoginManager::GetInstance()->SetLoginStatus(LoginStatus_NONE);

		nim_ui::LoginManager::GetInstance()->InvokeCancelLogin();
	}
	else
	{
		nim_cef::CefManager::GetInstance()->PostQuitMessage(0);
		_DoBeforeAppExit();
	}
#endif
}

//void LoginCallbackObject::ReLogin()
//{
//#if 1
//#else
//	assert(LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_NONE);
//	LoginManager::GetInstance()->SetLoginStatus(LoginStatus_LOGIN);
//
//	QLOG_APP(L"-----relogin begin-----");
//	nim::Client::Relogin();
//#endif
//}

void LoginCallbackObject::OnKickoutCallback(const KickoutRes& res)
{
#if 1
#else
	QLOG_APP(L"OnKickoutCallback: {0} - {1}") << res.client_type_ << res.kick_reason_;
	DoLogout(true, nim::kNIMLogoutKickout);
#endif
}

void LoginCallbackObject::OnDisconnectCallback()
{
	QLOG_APP(L"OnDisconnectCallback");
	LoginManager::GetInstance()->SetLinkActive(false);
}

void LoginCallbackObject::OnReLoginCallback(const LoginRes& login_res)
{
#if 1
#else
	QLOG_APP(L"OnReLoginCallback: {0} - {1}") << login_res.login_step_ << login_res.res_code_;

	if (login_res.res_code_ == nim::kNIMResSuccess)
	{
		if (login_res.login_step_ == nim::kNIMLoginStepLogin)
		{
			Post2UI(nbase::Bind(&UILoginCallback, login_res));
		}
	}
	else
	{
		Post2UI(nbase::Bind(&UILoginCallback, login_res));
	}
#endif
}

//多端
void LoginCallbackObject::OnMultispotLoginCallback(const MultiSpotLoginRes& res)
{
#if 1
#else
	QLOG_APP(L"OnMultispotLoginCallback: {0} - {1}") << res.notify_type_ << res.other_clients_.size();

	bool online = res.notify_type_ == nim::kNIMMultiSpotNotifyTypeImIn;
	if (!res.other_clients_.empty())
		Post2UI(nbase::Bind(LoginCallbackObject::OnMultispotChange, online, res.other_clients_));
#endif
}

void LoginCallbackObject::OnMultispotChange(bool online, const std::list<OtherClientPres>& clients)
{
#if 1
#else
	nim_ui::SessionListManager::GetInstance()->OnMultispotChange(online, clients);
#endif
}

void LoginCallbackObject::OnKickoutOtherClientCallback(const KickOtherRes& res)
{
#if 1
#else
	bool success = res.res_code_ == nim::kNIMResSuccess;
	if (success && !res.device_ids_.empty())
	{
		nim_ui::SessionListManager::GetInstance()->OnMultispotKickout(res.device_ids_);
	}
#endif
}
}
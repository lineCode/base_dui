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

void LoginCallbackObject::OnLoginCallback(nim_comp::LoginRes& login_res, const void* user_data)
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

void LoginCallbackObject::UILoginCallback(const nim_comp::LoginRes& login_res)
{
#if 1
	nim_comp::LoginManager::GetInstance()->SetErrorCode(login_res.res_code_);

	QLOG_APP(L"-----login end {0}-----") << login_res.res_code_;

	if (nim_comp::LoginManager::GetInstance()->IsLoginFormValid())
	{
		if (nim_comp::LoginManager::GetInstance()->GetLoginStatus() == nim_comp::LoginStatus_CANCEL)
		{
			QLOG_APP(L"-----login cancel end-----");
#if 0
			if (login_res.res_code_ == kNIMResSuccess)
				NimLogout(kNIMLogoutChangeAccout);
			else
				UILogoutCallback();
#else
			UILogoutCallback();
#endif
			return;
		}
		else
			nim_comp::LoginManager::GetInstance()->SetLoginStatus(login_res.res_code_ == nim_comp::kNIMResSuccess ? nim_comp::LoginStatus_SUCCESS : nim_comp::LoginStatus_NONE);

		if (login_res.res_code_ == nim_comp::kNIMResSuccess)
		{
			nim_comp::LoginManager::GetInstance()->InvokeHideWindow();
			nim_comp::LoginManager::GetInstance()->DoAfterLogin();
			// 登录成功，显示主界面
			nim_comp::LoginManager::GetInstance()->InvokeShowMainForm();
			nim_comp::LoginManager::GetInstance()->InvokeDestroyWindow();
		}
		else
		{
			nim_comp::LoginManager::GetInstance()->InvokeLoginError(login_res.res_code_);
		}
	}
	else
	{
		QLOG_APP(L"login form has been closed");
		//LoginManager::GetInstance()->SetLoginStatus(login_res.res_code_ == kNIMResSuccess ? LoginStatus_SUCCESS : LoginStatus_NONE);
		//LoginCallbackObject::DoLogout(false);
	}
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

void LoginCallbackObject::OnLogoutCallback(nim_comp::NIMResCode res_code)
{
	QLOG_APP(L"OnLogoutCallback: {0}") << res_code;
	QLOG_APP(L"-----logout end-----");
	UILogoutCallback();
}

void LoginCallbackObject::UILogoutCallback()
{
#if 0
	if (LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_CANCEL)
	{
		LoginManager::GetInstance()->SetLoginStatus(LoginStatus_NONE);

		LoginManager::GetInstance()->InvokeCancelLogin();
	}
	else
	{
		nim_cef::CefManager::GetInstance()->PostQuitMessage(0);
		_DoBeforeAppExit();
	}
#else
	nim_comp::LoginManager::GetInstance()->SetLoginStatus(nim_comp::LoginStatus_NONE);
	nim_comp::LoginManager::GetInstance()->InvokeCancelLogin();
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

void LoginCallbackObject::OnKickoutCallback(const nim_comp::KickoutRes& res)
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
	nim_comp::LoginManager::GetInstance()->SetLinkActive(false);
}

void LoginCallbackObject::OnReLoginCallback(const nim_comp::LoginRes& login_res)
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
void LoginCallbackObject::OnMultispotLoginCallback(const nim_comp::MultiSpotLoginRes& res)
{
#if 1
#else
	QLOG_APP(L"OnMultispotLoginCallback: {0} - {1}") << res.notify_type_ << res.other_clients_.size();

	bool online = res.notify_type_ == nim::kNIMMultiSpotNotifyTypeImIn;
	if (!res.other_clients_.empty())
		Post2UI(nbase::Bind(LoginCallbackObject::OnMultispotChange, online, res.other_clients_));
#endif
}

void LoginCallbackObject::OnMultispotChange(bool online, const std::list<nim_comp::OtherClientPres>& clients)
{
#if 1
#else
	nim_ui::SessionListManager::GetInstance()->OnMultispotChange(online, clients);
#endif
}

void LoginCallbackObject::OnKickoutOtherClientCallback(const nim_comp::KickOtherRes& res)
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

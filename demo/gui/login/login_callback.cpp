#include "stdafx.h"
#include "login_callback.h"
#include "module/login/login_manager.h"
#include "util/user_path.h"
#include "shared/threads.h"
#include "shared/closure.h"

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
	nim_comp::LoginManager::GetInstance()->SetErrorCode(login_res.res_code_);

	QLOG_APP(L"-----login end {0}-----") << login_res.res_code_;

	if (nim_comp::LoginManager::GetInstance()->IsLoginFormValid())
	{
		if (nim_comp::LoginManager::GetInstance()->GetLoginStatus() == nim_comp::LoginStatus_CANCEL)
		{
			QLOG_APP(L"-----login cancel end-----");
			UILogoutCallback();
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
}

void LoginCallbackObject::OnLogoutCallback(nim_comp::NIMResCode res_code)
{
	QLOG_APP(L"OnLogoutCallback: {0}") << res_code;
	QLOG_APP(L"-----logout end-----");
	UILogoutCallback();
}

void LoginCallbackObject::UILogoutCallback()
{
	nim_comp::LoginManager::GetInstance()->SetLoginStatus(nim_comp::LoginStatus_NONE);
	nim_comp::LoginManager::GetInstance()->InvokeCancelLogin();
}

void LoginCallbackObject::OnKickoutCallback(const nim_comp::KickoutRes& res)
{
#if 1
#else
	QLOG_APP(L"OnKickoutCallback: {0} - {1}") << res.client_type_ << res.kick_reason_;
	DoLogout(true, kNIMLogoutKickout);
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

	if (login_res.res_code_ == kNIMResSuccess)
	{
		if (login_res.login_step_ == kNIMLoginStepLogin)
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

	bool online = res.notify_type_ == kNIMMultiSpotNotifyTypeImIn;
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
	bool success = res.res_code_ == kNIMResSuccess;
	if (success && !res.device_ids_.empty())
	{
		nim_ui::SessionListManager::GetInstance()->OnMultispotKickout(res.device_ids_);
	}
#endif
}

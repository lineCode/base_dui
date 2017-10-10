#include "stdafx.h"
#include "login_form.h"
//#include "kit_define.h"
#include "module/login/login_manager.h"


void LoginForm::StartLogin(std::string account, std::string pwd)
{
	/*if (!nim_comp::LoginManager::GetInstance()->CheckSingletonRun(nbase::UTF8ToUTF16(account)))
	{
		ShowMsgBox(this->GetHWND(), MsgboxCallback(), L"STRID_CHECK_SINGLETON_RUN", true);
		return;
	}*/

	nim_comp::LoginManager::GetInstance()->DoLogin(account, pwd);
}

void LoginForm::RegLoginManagerCallback()
{
	nim_comp::OnLoginError cb_result = [this](int error){
		this->OnLoginError(error);
	};

	nim_comp::OnCancelLogin cb_cancel = [this]{
		this->OnCancelLogin();
	};

	nim_comp::OnHideWindow cb_hide = [this]{
		this->ShowWindow(false, false);
	};

	nim_comp::OnDestroyWindow cb_destroy = [this]{
		//PublicDB::GetInstance()->SaveLoginData();
		::DestroyWindow(this->GetHWND());
	};

	nim_comp::OnShowMainWindow cb_show_main = [this]{
		//nim_comp::WindowsManager::SingletonShow<MainForm>(MainForm::kClassName);
	};

	nim_comp::LoginManager::GetInstance()->RegLoginManagerCallback(/*ToWeakCallback*/(cb_result),
		/*ToWeakCallback*/(cb_cancel),
		/*ToWeakCallback*/(cb_hide),
		/*ToWeakCallback*/(cb_destroy),
		/*ToWeakCallback*/(cb_show_main));
}

void LoginForm::OnLoginError(int error)
{
	OnCancelLogin();
#if 0
	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	if (error == nim::kNIMResUidPassError)
	{
		usericon_->SetEnabled(false);
		passwordicon_->SetEnabled(false);
		ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_PASSWORD_ERROR"));
	}
	else if (error == nim::kNIMResConnectionError)
	{
		ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_NETWORK_ERROR"));
	}
	else if (error == nim::kNIMResExist)
	{
		ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_LOCATION_CHANGED"));
	}
	else
	{
		std::wstring tip = nbase::StringPrintf(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_ERROR_CODE").c_str(), error);
		ShowLoginTip(tip);
	}
#endif
}

void LoginForm::OnCancelLogin()
{
#if 0
	usericon_->SetEnabled(true);
	passwordicon_->SetEnabled(true);

	user_name_edit_->SetEnabled(true);
	password_edit_->SetEnabled(true);

	login_ing_tip_->SetVisible(false);
	login_error_tip_->SetVisible(false);

	btn_login_->SetVisible(true);
	btn_cancel_->SetVisible(false);
	btn_cancel_->SetEnabled(true);
#endif
}
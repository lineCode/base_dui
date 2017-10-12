#include "stdafx.h"
#include "login_form.h"
//#include "kit_define.h"
#include "module/login/login_manager.h"
#include "gui/msgbox/msgbox.h"


void LoginForm::StartLogin()
{
	if (re_account_->GetText().empty())
	{
		label_info_->SetText(L"用户名空");
		return;
	}
	if (re_pwd_->GetText().empty())
	{
		label_info_->SetText(L"密码空");
		return;
	}
	if (!nim_comp::LoginManager::GetInstance()->CheckSingletonRun(re_account_->GetText()))
	{
		ShowMsgBox(this->GetHWND(), MsgboxCallback(), L"STRID_CHECK_SINGLETON_RUN", true);
		return;
	}

	std::string account = nbase::UTF16ToUTF8(re_account_->GetText());
	std::string pwd = nbase::UTF16ToUTF8(re_pwd_->GetText());

	nim_comp::LoginManager::GetInstance()->DoLogin(account, pwd);
}

void LoginForm::CancelLogin()
{
	btn_cancel_login_->SetEnabled(false);
	btn_cancel_login_->SetText(L"正在取消...");
	nim_comp::LoginManager::GetInstance()->CancelLogin();
}

void LoginForm::RegLoginManagerCallback()
{
	nim_comp::OnStartLogin cb_start = [this](){
		//re_account_->SetEnabled();
		btn_login_->SetVisible(false);
		btn_cancel_login_->SetVisible(true);
	};

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

	nim_comp::LoginManager::GetInstance()->RegLoginManagerCallback(/*ToWeakCallback*/(cb_start),
		/*ToWeakCallback*/(cb_result),
		/*ToWeakCallback*/(cb_cancel),
		/*ToWeakCallback*/(cb_hide),
		/*ToWeakCallback*/(cb_destroy),
		/*ToWeakCallback*/(cb_show_main));
}

void LoginForm::OnLoginError(int error)
{
	//OnCancelLogin();
#if 1
	//MutiLanSupport* mls = MutiLanSupport::GetInstance();
	if (error == nim_comp::kNIMResUidNotExist)
	{
		label_info_->SetText(L"账号未注册");
	}
	else if (error == nim_comp::kNIMResUidPassError)
	{
		label_info_->SetText(L"用户名或密码错");
	}
	else if (error == nim_comp::kNIMResExist)
	{
		label_info_->SetText(L"账号已登录");
	}
	else
	{
		std::wstring tip = nbase::StringPrintf(L"其他错误", error);
		label_info_->SetText(tip.c_str());
	}
#endif
}

void LoginForm::OnCancelLogin()
{
#if 1
	re_account_->SetEnabled(true);
	re_pwd_->SetEnabled(true);

	label_info_->SetText(L"已取消登录");

	btn_login_->SetVisible(true);
	btn_cancel_login_->SetVisible(false);
	btn_cancel_login_->SetText(L"取消登录");
	btn_cancel_login_->SetEnabled(true);
#endif
}
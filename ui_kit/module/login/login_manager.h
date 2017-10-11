#pragma once

#include "base/memory/singleton.h"
#include "login_define.h"

namespace nim_comp
{
/** @class LoginManager
  * @brief 登陆管理器
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/18
  */
class LoginManager
{
public:
	SINGLETON_DEFINE(LoginManager);	
	LoginManager();

	bool IsSelf(const std::string &account);

	void SetAccount(const std::string &account);
	std::string GetAccount();

	/**
	* 判断帐号是否为已登录帐号(自动转换为小写来判断)
	* @param[in] account 用户id
	* @return bool true 是，false 否
	*/
	bool IsEqual(const std::string& account);

	void SetPassword(const std::string &password);
	std::string GetPassword();

	void SetLoginStatus(LoginStatus status);
	LoginStatus GetLoginStatus();

	/**
	* 设置断网重连标记
	* @param[in] active 是否处于连接状态
	* @return void	无返回值
	*/
	void SetLinkActive(bool active);

	/**
	* 判断是否处于连接状态
	* @return bool true 连接，false 断开
	*/
	bool IsLinkActive();

	/**
	* 设置日记记录等级
	* @param[in] level 记录等级
	* @return void	无返回值
	*/
	void SetDemoLogLevel(int level) { demo_log_level_ = (LOG_LEVEL)level; }

	/**
	* 获取日志记录等级
	* @return LOG_LEVEL 记录等级
	*/
	LOG_LEVEL GetDemoLogLevel() { return demo_log_level_; }

	void SetFileSizeLimit(int file_size) { limit_file_size_ = file_size; }
	int GetFileSizeLimit() { return limit_file_size_; }

	void SetErrorCode(int error_code) { error_code_ = error_code; }
	int GetErrorCode() { return error_code_; }

	/**
	* 创建互斥量用于检测帐号重复登录
	* @return void	无返回值
	*/
	void CreateSingletonRunMutex();

	/**
	* 检查是否重复登录帐号
	* @param[in] username 用户id
	* @return bool	true：没有重复，false：重复
	*/
	bool CheckSingletonRun(const std::wstring& username);
	
public:
	/**
	* 进行登录操作
	* @param[in] user 用户帐号
	* @param[in] pass 用户密码
	* @return void	无返回值
	*/
	void DoLogin(std::string user, std::string pass);

	/**
	* 退出登录
	* @param[in] over 外部调用此函数时，over设置为false。true表示直接退出程序，内部使用
	* @param[in] type 退出原因（主动退出或被踢）
	* @return void	无返回值
	*/
	void DoLogout(bool over, NIMLogoutType type = kNIMLogoutAppExit);

	/**
	* 取消登录
	* @return void	无返回值
	*/
	void CancelLogin();



	void DoAfterLogin();

	/**
	* 注册登录窗体的回调函数，用来让UI组件控制登录窗体行为，登陆之前应该调用此函数注册相关回调
	* @param[in] cb_result		通知登录错误并返回错误原因的回调函数
	* @param[in] cb_cancel		通知取消登录的回调函数
	* @param[in] cb_hide		通知隐藏登录窗体的回调函数
	* @param[in] cb_destroy		通知销毁登录窗体的回调函数
	* @param[in] cb_show_main	通知显示主窗体的回调函数
	* @return void 无返回值
	*/
	void RegLoginManagerCallback(const OnStartLogin& cb_start,
		const OnLoginError& cb_result,
		const OnCancelLogin& cb_cancel,
		const OnHideWindow& cb_hide,
		const OnDestroyWindow& cb_destroy,
		const OnShowMainWindow& cb_show_main
		);
	/**
	* 通知显示主窗体
	* @return void 无返回值
	*/
	void InvokeShowMainForm();

	/**
	* 通知登录错误并返回错误原因
	* @param[in] error	错误码
	* @return void 无返回值
	*/
	void InvokeLoginError(int error);

	/**
	* 通知取消登录
	* @return void 无返回值
	*/
	void InvokeCancelLogin();

	/**
	* 通知隐藏登录窗体
	* @return void 无返回值
	*/
	void InvokeHideWindow();

	/**
	* 通知销毁登录窗体
	* @return void 无返回值
	*/
	void InvokeDestroyWindow();

	/**
	* 判断登录窗口是否有效
	* @return bool true 有效，false 无效
	*/
	bool IsLoginFormValid();
private:

	/**
	* 从配置文件读取日志记录等级
	* @return void	无返回值
	*/
	void ReadDemoLogLevel();

	

private:
	std::string account_;
	std::string password_;
	LoginStatus status_;
	bool active_;

	LOG_LEVEL demo_log_level_ = LV_APP;
	int limit_file_size_ = 15;

	int error_code_ = 200;

	OnStartLogin cb_start_login_;
	OnLoginError cb_login_error_;
	OnCancelLogin cb_cancel_login_;
	OnHideWindow cb_hide_window_;
	OnDestroyWindow cb_destroy_window_;
	OnShowMainWindow cb_show_main_window_;
};
}
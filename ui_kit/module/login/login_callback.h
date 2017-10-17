﻿#pragma once
//#include "nim_client_helper.h"
#include "login_define.h"

namespace nim_comp
{

	typedef std::function<void(const LoginRes&)>	LoginCallback; /**< 登录回调模板 */
	typedef std::function<void(NIMResCode)>			LogoutCallback;		/**< 登出回调模板 */
	typedef std::function<void(const KickoutRes&)>	KickoutCallback;	/**< 被踢通知回调模板 */
	typedef std::function<void(void)>				DisconnectCallback;		/**< 断网通知回调模板 */
	typedef std::function<void(const MultiSpotLoginRes&)>	MultiSpotLoginCallback;	/**< 多端登录通知回调模板 */
	typedef std::function<void(const KickOtherRes&)>		KickOtherCallback;	/**< 将多端下线回调模板 */
	typedef std::function<void(int, bool)>					MultiportPushConfigCallback;	/**< 多端推送控制开关回调模板 */

/** @class LoginCallbackObject
  * @brief 登录注销相关回调函数类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class LoginCallbackObject
{
public:
	/**
	* 登录
	* @param[in] user 输入的用户名
	* @param[in] pass 输入的密码
	* @return void	无返回值
	*/
	//static void DoLogin(std::string user, std::string pass);

	/**
	* 登录结果回调
	* @param[in] login_res 登录的信息
	* @param[in] user_data 登录时用户传的数据，sdk原封不动传给回调
	* @return void	无返回值
	*/
	static void OnLoginCallback(LoginRes& login_res, const void* user_data);

	/**
	* 登录成功或失败时，做一些界面上的显示。
	* @param[in] login_res 登录的信息
	* @return void	无返回值
	*/
	static void UILoginCallback(const LoginRes& login_res);

	/**
	* 取消登录
	* @return void	无返回值
	*/
	//static void CacelLogin();

	/**
	* 退出登录
	* @param[in] over 外部调用此函数时，over设置为false。true表示直接退出程序，内部使用
	* @param[in] type 退出原因（主动退出或被踢）
	* @return void	无返回值
	*/
	//static void DoLogout(bool over, NIMLogoutType type = kNIMLogoutAppExit);

	/**
	* 退出登录结果回调
	* @param[in] res_code 错误码
	* @return void	无返回值
	*/
	static void OnLogoutCallback(NIMResCode res_code);

	/**
	* 退出登录时，做一些界面上的显示。
	* @return void	无返回值
	*/
	static void UILogoutCallback();

	/**
	* 主动重新登录
	* @return void	无返回值
	*/
	//static void ReLogin();

	/**
	* 被其他端踢下线的回调，要在程序开始运行时就注册好
	* @param[in] res 被踢的信息
	* @return void	无返回值
	*/
	static void OnKickoutCallback(const KickoutRes& res);

	/**
	* 掉线的回调，要在程序开始运行时就注册好。
	* @return void	无返回值
	*/
	static void OnDisconnectCallback();
	
	/**
	* 自动重连的回调，要在程序开始运行时就注册好。
	* @param[in] login_res 重连结果
	* @return void	无返回值
	*/
	static void OnReLoginCallback(const LoginRes& login_res);
	
	/**
	* 移动端登录时的回调，要在程序开始运行时就注册好。
	* @param[in] res 多端登录回调信息
	* @return void	无返回值
	*/
	static void OnMultispotLoginCallback(const MultiSpotLoginRes& res);

	/**
	* 移动端登录发生变化时的回调，要在程序开始运行时就注册好。
	* @param[in] online 是否在线
	* @param[in] clients 其他客户端信息
	* @return void	无返回值
	*/
	static void OnMultispotChange(bool online, const std::list<OtherClientPres>& clients);

	/**
	* 把移动端踢下线的结果回调，要在程序开始运行时就注册好。
	* @param[in] res 踢人结果信息
	* @return void	无返回值
	*/
	static void OnKickoutOtherClientCallback(const KickOtherRes& res);
};

}
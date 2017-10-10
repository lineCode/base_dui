#pragma once

#include "kit_define.h"

namespace nim_comp
{
	typedef std::function<void(int error)> OnLoginError;
	typedef std::function<void()> OnCancelLogin;
	typedef std::function<void()> OnHideWindow;
	typedef std::function<void()> OnDestroyWindow;
	typedef std::function<void()> OnShowMainWindow;

	enum LoginStatus
	{
		LoginStatus_NONE,		//初始状态
		LoginStatus_LOGIN,		//正在登录
		LoginStatus_CANCEL,		//取消登录
		LoginStatus_SUCCESS,	//已经登录
		LoginStatus_EXIT		//退出登录
	};
	/** @enum NIMLoginState 登录状态 */
	enum NIMLoginState
	{
		kNIMLoginStateLogin = 1,		/**< 登录状态*/
		kNIMLoginStateUnLogin = 2,		/**< 未登录状态*/
	};

	/** @enum NIMLogoutType Logout类型 */
	enum NIMLogoutType
	{
		kNIMLogoutChangeAccout = 1,	/**< 注销/切换帐号（返回到登录界面）*/
		kNIMLogoutKickout = 2,	/**< 被踢（返回到登录界面）*/
		kNIMLogoutAppExit = 3,	/**< 程序退出*/
		kNIMLogoutRelogin = 4,	/**< 重连操作，包括保存密码时启动程序伪登录后的重连操作以及掉线后的重连操作（帐号未变化）*/
	};

	/** @enum NIMKickReason 被踢原因 */
	enum NIMKickReason
	{
		kNIMKickReasonSameGeneric = 1,  /**< 互斥类型的客户端互踢*/
		kNIMKickReasonServerKick = 2,  /**< 服务器端发起踢客户端指令*/
		kNIMKickReasonKickBySelfOtherClient = 3,  /**< 被自己的其他端踢掉*/
	};

	/** @enum NIMClientType 客户端类型 */
	enum NIMClientType
	{
		kNIMClientTypeDefault = 0,  /**< Default, unset*/
		kNIMClientTypeAndroid = 1,  /**< Android*/
		kNIMClientTypeiOS = 2,  /**< iOS*/
		kNIMClientTypePCWindows = 4,  /**< PC Windows*/
		kNIMClientTypeWindowsPhone = 8,  /**< Windows Phone*/
		kNIMClientTypeWeb = 16, /**< Web*/
		kNIMClientTypeRestAPI = 32, /**< RestAPI*/
	};

	/** @enum NIMLoginStep 登录步骤 */
	enum NIMLoginStep
	{
		kNIMLoginStepLinking = 0,	/**< 正在连接*/
		kNIMLoginStepLink = 1,	/**< 连接服务器*/
		kNIMLoginStepLogining = 2,	/**< 正在登录*/
		kNIMLoginStepLogin = 3,	/**< 登录验证*/
	};

	/** @enum NIMMultiSpotNotifyType 多点登录通知类型 */
	enum NIMMultiSpotNotifyType
	{
		kNIMMultiSpotNotifyTypeImIn = 2,		/**< 通知其他在线端自己登录了*/
		kNIMMultiSpotNotifyTypeImOut = 3,		/**< 通知其他在线端自己退出*/
	};

	/** @brief 多端登陆客户端信息 */
	struct OtherClientPres
	{
		std::string	app_account_;			/**< 第三方账号 */
		NIMClientType	client_type_;		/**< 客户端类型, 见NIMClientType */
		std::string	client_os_;				/**< 登录系统类型,比如ios 6.0.1 */
		std::string	mac_address_;			/**< 登录设备的mac地址 */
		std::string	device_id_;				/**< 设备id，uuid */
		int64_t		login_time_;			/**< 本次登陆时间, 精度到ms */

		/** 构造函数 */
		OtherClientPres() : login_time_(0) {}
	};


	/** @brief 登录结果回调信息 */
	struct LoginRes
	{
		NIMResCode res_code_;				/**< 返回的错误码NIMResCode */
		bool relogin_;						/**< 是否为重连过程 */
		NIMLoginStep	login_step_;		/**< 登录步骤NIMLoginStep */
		std::list<OtherClientPres> other_clients_;	/**< 其他端的在线状态列表，登录成功才会返回这部分内容 */
		bool retrying_;						/**< SDK是否在重试，如果为false，开发者需要检查登录步骤和错误码，明确问题后调用手动重连接口进行登录操作 */
		/** 构造函数 */
		LoginRes() : relogin_(false), retrying_(true) {}
	};

	/** @brief 被踢结果回调信息 */
	struct KickoutRes
	{
		NIMClientType client_type_;			/**< int, 客户端类型NIMClientType */
		NIMKickReason kick_reason_;			/**< 返回的被踢原因NIMKickReason */
	};

	/** @brief 多端登录回调信息 */
	struct MultiSpotLoginRes
	{
		NIMMultiSpotNotifyType	notify_type_;			/**< NIMMultiSpotNotifyType 多点登录通知类型 */
		std::list<OtherClientPres> other_clients_;		/**< 其他端的在线状态列表 */
	};

	/** @brief 踢人结果回调信息 */
	struct KickOtherRes
	{
		NIMResCode res_code_;					/**< 返回的错误码NIMResCode */
		std::list<std::string> device_ids_;		/**< 设备id，uuid */
	};
}

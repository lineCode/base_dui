#pragma once

//#include "kit_define.h"

namespace nim_comp
{
	/** @enum NIMSessionType 会话类型 */
	enum NIMSessionType
	{
		kNIMSessionTypeP2P = 0,			/**< 个人，即点对点 */
		kNIMSessionTypeTeam = 1,			/**< 群组 */
	};

	/** @enum NIMSessionCommand 会话操作命令 */
	enum NIMSessionCommand
	{
		kNIMSessionCommandAdd = 0,	/**< 添加会话项 */
		kNIMSessionCommandRemove = 1,	/**< 删除单个会话项 */
		kNIMSessionCommandRemoveAll = 2,	/**< 删除所有会话项 */
		kNIMSessionCommandRemoveAllP2P = 3,	/**< 删除所有点对点的会话项 */
		kNIMSessionCommandRemoveAllTeam = 4,	/**< 删除所有群的会话项 */
		kNIMSessionCommandMsgDeleted = 5,	/**< 单个会话项的消息已删除 */
		kNIMSessionCommandAllMsgDeleted = 6,	/**< 所有会话项的消息已删除 */
		kNIMSessionCommandAllP2PMsgDeleted = 7,	/**< 所有点对点会话项的消息已删除 */
		kNIMSessionCommandAllTeamMsgDeleted = 8,	/**< 所有群会话项的消息已删除 */
		kNIMSessionCommandUpdate = 9,	/**< 更新会话项 */
	};

	/** @enum NIMMessageType Message Type */
	enum NIMMessageType
	{
		kNIMMessageTypeText = 0,			/**< 文本类型消息*/
		kNIMMessageTypeImage = 1,			/**< 图片类型消息*/
		kNIMMessageTypeAudio = 2,			/**< 声音类型消息*/
		kNIMMessageTypeVideo = 3,			/**< 视频类型消息*/
		kNIMMessageTypeLocation = 4,			/**< 位置类型消息*/
		kNIMMessageTypeNotification = 5,		/**< 系统类型通知（包括入群出群通知等） NIMNotificationId*/
		kNIMMessageTypeFile = 6,			/**< 文件类型消息*/
		kNIMMessageTypeTips = 10,			/**< 提醒类型消息,Tip内容根据格式要求填入消息结构中的kNIMMsgKeyServerExt字段*/
		kNIMMessageTypeCustom = 100,			/**< 自定义消息*/
		kNIMMessageTypeRemote = 256,		/**< 远程协助*/
		kNIMMessageTypeRemoteACK = 257,		/**< 远程协助ack*/

		kNIMMessageTypeUnknown = 1000,		/**< 未知类型消息，作为默认值*/
	};

	/** @enum NIMMessageFeature 消息种类 */
	enum NIMMessageFeature
	{
		kNIMMessageFeatureDefault = 0,			/**< 默认*/
		kNIMMessageFeatureLeaveMsg = 1,			/**< 离线消息*/
		kNIMMessageFeatureRoamMsg = 2,			/**< 漫游消息*/
		kNIMMessageFeatureSyncMsg = 3,			/**< 同步消息*/
		kNIMMessageFeatureCustomizedMsg = 4,		/**< 透传消息*/
	};

	/** @enum NIMMsgLogStatus 消息状态 */
	enum NIMMsgLogStatus
	{
		kNIMMsgLogStatusNone = 0,			/**< 默认,不能当查询条件,意义太多*/
		kNIMMsgLogStatusUnread = 1,			/**< 收到消息,未读*/
		kNIMMsgLogStatusRead = 2,			/**< 收到消息,已读*/
		kNIMMsgLogStatusDeleted = 3,			/**< 已删*/
		kNIMMsgLogStatusSending = 4,			/**< 发送中*/
		kNIMMsgLogStatusSendFailed = 5,			/**< 发送失败*/
		kNIMMsgLogStatusSent = 6,			/**< 已发送*/
		kNIMMsgLogStatusReceipt = 7,			/**< 对方已读发送的内容*/
		kNIMMsgLogStatusDraft = 8,			/**< 草稿*/
		kNIMMsgLogStatusSendCancel = 9,			/**< 发送取消*/
	};

	/** @enum NIMNotificationId 通知类型 */
	enum NIMNotificationId
	{
		kNIMNotificationIdTeamInvite = 0,			/**< 普通群拉人，{"ids":["a1", "a2"],"user_namecards":["namecard1", "namecard2"], "attach" : ""} attach为可选字段，作为应用自定义扩展字段,解析前需要判断有没有这个字段 */
		kNIMNotificationIdTeamKick = 1,			/**< 普通群踢人，{"ids":["a1", "a2"],"user_namecards":["namecard1", "namecard2"], "attach" : ""} attach为可选字段，作为应用自定义扩展字段,解析前需要判断有没有这个字段 */
		kNIMNotificationIdTeamLeave = 2,			/**< 退出群，{"id" : "a1","user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamUpdate = 3,			/**< team_info更新，{"team_info":team_info,"user_namecards":["namecard1", "namecard2"]} //群组信息(Keys SEE MORE `nim_team_def.h` 『群组信息 Json Keys』)*/
		kNIMNotificationIdTeamDismiss = 4,			/**< 群解散，{"user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamApplyPass = 5,			/**< 高级群申请加入成功，{"id":"a1","user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamOwnerTransfer = 6,			/**< 高级群移交群主，{"id":"a1", "leave" : bool,"user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamAddManager = 7,			/**< 增加管理员，{"ids":["a1","a2"],"user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamRemoveManager = 8,			/**< 删除管理员，{"ids":["a1","a2"],"user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamInviteAccept = 9,			/**< 高级群接受邀请进群，{"id":"a1","user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamMuteMember = 10,			/**< 禁言/解禁群成员，{"user_namecards":["namecard1", "namecard2"],"team_info":team_info,"id":"a1","mute":1-禁言,0-解禁} */

		kNIMNotificationIdNetcallMiss = 101,			/**< 未接电话,{"calltype":1,"channel":6146078138783760761,"from":"id1","ids":["id1","id2"],"time":1430995380471}*/
		kNIMNotificationIdNetcallBill = 102,			/**< 话单,{"calltype":2,"channel":6146077129466446197,"duration":8,"ids":["id1","id2"],"time":1430995117398}*/

		//服务器在线同步协议返回的结果
		kNIMNotificationIdTeamSyncCreate = 1000,			/**< 创建群 {"team_info" : team_info} //群组信息(Keys SEE MORE `nim_team_def.h` 『群组信息 Json Keys』)*/
		kNIMNotificationIdTeamMemberChanged = 1001,			/**< 群成员变更{"team_member" : team_member_info} //群组成员信息（不包括自己）(Keys SEE MORE `nim_team_def.h` 『群组成员信息 Json Keys』)*/
		kNIMNotificationIdTeamSyncUpdateMemberProperty = 1002,	/**< 同步通知：修改群成员属性（可能是自己的或别人的）{"team_member" : team_member_info} //目前只需kNIMTeamUserKeyNick和kNIMTeamUserKeyBits (Keys SEE MORE `nim_team_def.h` 『群组成员信息 Json Keys』)*/

		//本地发起的操作通知APP上层
		kNIMNotificationIdLocalCreateTeam = 2000,			/**< 本地操作创建群 {"ids" : ["a1", "a2"]}*/
		kNIMNotificationIdLocalApplyTeam = 2001,			/**< 本地操作申请加入群 {}*/
		kNIMNotificationIdLocalRejectApply = 2002,			/**< 本地操作拒绝申请 {"id":"a1"}*/
		kNIMNotificationIdLocalRejectInvite = 2003,			/**< 本地操作拒绝邀请 {"id":"a1"}*/
		kNIMNotificationIdLocalUpdateMemberProperty = 2004,		/**< 本地操作更新群成员属性  {"team_member" : team_member_info} */
		kNIMNotificationIdLocalUpdateOtherNick = 2005,			/**< 本地操作更新他人nickname {}*/
		kNIMNotificationIdLocalGetTeamInfo = 2006,			/**< 本地操作获取群信息 {"team_info":team_info} //群组信息(Keys SEE MORE `nim_team_def.h` 『群组信息 Json Keys』)*/
		kNIMNotificationIdLocalGetTeamList = 2007,			/**< 本地操作获取群成员信息结束*/
		kNIMNotificationIdLocalMuteMember = 2008,			/**< 本地操作对群成员禁言 {"id":"a1", "mute":1-禁言,0-解禁} */
		//Netcall本地操作通知
		kNIMNotificationIdLocalNetcallReject = 3103,			/**< 拒绝电话,{"calltype":1,"channel":6146078138783760761,"from":"id1","ids":["id1","id2"],"time":1430995380471}*/
		kNIMNotificationIdLocalNetcallNoResponse = 3104,			/**< 无应答，未接通电话,{"calltype":1,"channel":6146078138783760761,"from":"id1","ids":["id1","id2"],"time":1430995380471}*/
	};

	/** @enum NIMMsgLogSubStatus 消息子状态 */
	enum NIMMsgLogSubStatus
	{
		kNIMMsgLogSubStatusNone = 0,			/**< 默认状态*/

		//这二个标志适用于所有
		kNIMMsgLogSubStatusNotPlaying = 20,			/**< 未播放*/
		kNIMMsgLogSubStatusPlayed = 21,			/**< 已播放*/
	};

	struct SessionData
	{
		std::string		id_;					/**< 会话ID */
		NIMSessionType	type_;					/**< 会话类型 */
		int				unread_count_;			/**< 当前会话消息未读数 */
		NIMSessionCommand	command_;			/**< 会话修改命令 */
		std::string		msg_id_;				/**< 当前会话最新一条消息ID */
		std::string		msg_sender_accid_;		/**< 当前会话最新一条消息发送方ID */
		__int64			msg_timetag_;			/**< 当前会话最新一条消息时间戳（毫秒） */
		NIMMessageType	msg_type_;				/**< 当前会话最新一条消息类型 */
		std::string		msg_content_;			/**< 当前会话最新一条消息内容 */
		std::string		msg_attach_;			/**< 当前会话最新一条消息附件 */
		NIMMsgLogStatus	msg_status_;			/**< 当前会话最新一条消息状态 */
		NIMMsgLogSubStatus	msg_sub_status_;	/**< 当前会话最新一条消息子状态 */
		bool			last_updated_msg_;		/**< (批量)消息变更或增加时是否是最后一条变更的信息 */

		/** 构造函数 */
		SessionData() : unread_count_(0)
			, msg_timetag_(0)
			, type_(kNIMSessionTypeP2P)
			, command_(kNIMSessionCommandAdd)
			, msg_type_(kNIMMessageTypeUnknown)
			, msg_status_(kNIMMsgLogStatusNone)
			, msg_sub_status_(kNIMMsgLogSubStatusNone)
			, last_updated_msg_(true){}
	};

	/** @brief 会话列表数据 */
	struct SessionDataList
	{
		int			count_;					/**< 会话列表项数量 */
		int			unread_count_;			/**< 会话列表总的未读消息数 */
		std::list<SessionData> sessions_;	/**< 会话列表项数据 */

		SessionDataList() : count_(0), unread_count_(0) {}
	};
}

#pragma once

//#include "kit_define.h"

namespace nim_comp
{
	/** @enum NIMSessionType �Ự���� */
	enum NIMSessionType
	{
		kNIMSessionTypeP2P = 0,			/**< ���ˣ�����Ե� */
		kNIMSessionTypeTeam = 1,			/**< Ⱥ�� */
	};

	/** @enum NIMSessionCommand �Ự�������� */
	enum NIMSessionCommand
	{
		kNIMSessionCommandAdd = 0,	/**< ��ӻỰ�� */
		kNIMSessionCommandRemove = 1,	/**< ɾ�������Ự�� */
		kNIMSessionCommandRemoveAll = 2,	/**< ɾ�����лỰ�� */
		kNIMSessionCommandRemoveAllP2P = 3,	/**< ɾ�����е�Ե�ĻỰ�� */
		kNIMSessionCommandRemoveAllTeam = 4,	/**< ɾ������Ⱥ�ĻỰ�� */
		kNIMSessionCommandMsgDeleted = 5,	/**< �����Ự�����Ϣ��ɾ�� */
		kNIMSessionCommandAllMsgDeleted = 6,	/**< ���лỰ�����Ϣ��ɾ�� */
		kNIMSessionCommandAllP2PMsgDeleted = 7,	/**< ���е�Ե�Ự�����Ϣ��ɾ�� */
		kNIMSessionCommandAllTeamMsgDeleted = 8,	/**< ����Ⱥ�Ự�����Ϣ��ɾ�� */
		kNIMSessionCommandUpdate = 9,	/**< ���»Ự�� */
	};

	/** @enum NIMMessageType Message Type */
	enum NIMMessageType
	{
		kNIMMessageTypeText = 0,			/**< �ı�������Ϣ*/
		kNIMMessageTypeImage = 1,			/**< ͼƬ������Ϣ*/
		kNIMMessageTypeAudio = 2,			/**< ����������Ϣ*/
		kNIMMessageTypeVideo = 3,			/**< ��Ƶ������Ϣ*/
		kNIMMessageTypeLocation = 4,			/**< λ��������Ϣ*/
		kNIMMessageTypeNotification = 5,		/**< ϵͳ����֪ͨ��������Ⱥ��Ⱥ֪ͨ�ȣ� NIMNotificationId*/
		kNIMMessageTypeFile = 6,			/**< �ļ�������Ϣ*/
		kNIMMessageTypeTips = 10,			/**< ����������Ϣ,Tip���ݸ��ݸ�ʽҪ��������Ϣ�ṹ�е�kNIMMsgKeyServerExt�ֶ�*/
		kNIMMessageTypeCustom = 100,			/**< �Զ�����Ϣ*/
		kNIMMessageTypeRemote = 256,		/**< Զ��Э��*/
		kNIMMessageTypeRemoteACK = 257,		/**< Զ��Э��ack*/

		kNIMMessageTypeUnknown = 1000,		/**< δ֪������Ϣ����ΪĬ��ֵ*/
	};

	/** @enum NIMMessageFeature ��Ϣ���� */
	enum NIMMessageFeature
	{
		kNIMMessageFeatureDefault = 0,			/**< Ĭ��*/
		kNIMMessageFeatureLeaveMsg = 1,			/**< ������Ϣ*/
		kNIMMessageFeatureRoamMsg = 2,			/**< ������Ϣ*/
		kNIMMessageFeatureSyncMsg = 3,			/**< ͬ����Ϣ*/
		kNIMMessageFeatureCustomizedMsg = 4,		/**< ͸����Ϣ*/
	};

	/** @enum NIMMsgLogStatus ��Ϣ״̬ */
	enum NIMMsgLogStatus
	{
		kNIMMsgLogStatusNone = 0,			/**< Ĭ��,���ܵ���ѯ����,����̫��*/
		kNIMMsgLogStatusUnread = 1,			/**< �յ���Ϣ,δ��*/
		kNIMMsgLogStatusRead = 2,			/**< �յ���Ϣ,�Ѷ�*/
		kNIMMsgLogStatusDeleted = 3,			/**< ��ɾ*/
		kNIMMsgLogStatusSending = 4,			/**< ������*/
		kNIMMsgLogStatusSendFailed = 5,			/**< ����ʧ��*/
		kNIMMsgLogStatusSent = 6,			/**< �ѷ���*/
		kNIMMsgLogStatusReceipt = 7,			/**< �Է��Ѷ����͵�����*/
		kNIMMsgLogStatusDraft = 8,			/**< �ݸ�*/
		kNIMMsgLogStatusSendCancel = 9,			/**< ����ȡ��*/
	};

	/** @enum NIMNotificationId ֪ͨ���� */
	enum NIMNotificationId
	{
		kNIMNotificationIdTeamInvite = 0,			/**< ��ͨȺ���ˣ�{"ids":["a1", "a2"],"user_namecards":["namecard1", "namecard2"], "attach" : ""} attachΪ��ѡ�ֶΣ���ΪӦ���Զ�����չ�ֶ�,����ǰ��Ҫ�ж���û������ֶ� */
		kNIMNotificationIdTeamKick = 1,			/**< ��ͨȺ���ˣ�{"ids":["a1", "a2"],"user_namecards":["namecard1", "namecard2"], "attach" : ""} attachΪ��ѡ�ֶΣ���ΪӦ���Զ�����չ�ֶ�,����ǰ��Ҫ�ж���û������ֶ� */
		kNIMNotificationIdTeamLeave = 2,			/**< �˳�Ⱥ��{"id" : "a1","user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamUpdate = 3,			/**< team_info���£�{"team_info":team_info,"user_namecards":["namecard1", "namecard2"]} //Ⱥ����Ϣ(Keys SEE MORE `nim_team_def.h` ��Ⱥ����Ϣ Json Keys��)*/
		kNIMNotificationIdTeamDismiss = 4,			/**< Ⱥ��ɢ��{"user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamApplyPass = 5,			/**< �߼�Ⱥ�������ɹ���{"id":"a1","user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamOwnerTransfer = 6,			/**< �߼�Ⱥ�ƽ�Ⱥ����{"id":"a1", "leave" : bool,"user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamAddManager = 7,			/**< ���ӹ���Ա��{"ids":["a1","a2"],"user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamRemoveManager = 8,			/**< ɾ������Ա��{"ids":["a1","a2"],"user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamInviteAccept = 9,			/**< �߼�Ⱥ���������Ⱥ��{"id":"a1","user_namecards":["namecard1", "namecard2"]}*/
		kNIMNotificationIdTeamMuteMember = 10,			/**< ����/���Ⱥ��Ա��{"user_namecards":["namecard1", "namecard2"],"team_info":team_info,"id":"a1","mute":1-����,0-���} */

		kNIMNotificationIdNetcallMiss = 101,			/**< δ�ӵ绰,{"calltype":1,"channel":6146078138783760761,"from":"id1","ids":["id1","id2"],"time":1430995380471}*/
		kNIMNotificationIdNetcallBill = 102,			/**< ����,{"calltype":2,"channel":6146077129466446197,"duration":8,"ids":["id1","id2"],"time":1430995117398}*/

		//����������ͬ��Э�鷵�صĽ��
		kNIMNotificationIdTeamSyncCreate = 1000,			/**< ����Ⱥ {"team_info" : team_info} //Ⱥ����Ϣ(Keys SEE MORE `nim_team_def.h` ��Ⱥ����Ϣ Json Keys��)*/
		kNIMNotificationIdTeamMemberChanged = 1001,			/**< Ⱥ��Ա���{"team_member" : team_member_info} //Ⱥ���Ա��Ϣ���������Լ���(Keys SEE MORE `nim_team_def.h` ��Ⱥ���Ա��Ϣ Json Keys��)*/
		kNIMNotificationIdTeamSyncUpdateMemberProperty = 1002,	/**< ͬ��֪ͨ���޸�Ⱥ��Ա���ԣ��������Լ��Ļ���˵ģ�{"team_member" : team_member_info} //Ŀǰֻ��kNIMTeamUserKeyNick��kNIMTeamUserKeyBits (Keys SEE MORE `nim_team_def.h` ��Ⱥ���Ա��Ϣ Json Keys��)*/

		//���ط���Ĳ���֪ͨAPP�ϲ�
		kNIMNotificationIdLocalCreateTeam = 2000,			/**< ���ز�������Ⱥ {"ids" : ["a1", "a2"]}*/
		kNIMNotificationIdLocalApplyTeam = 2001,			/**< ���ز����������Ⱥ {}*/
		kNIMNotificationIdLocalRejectApply = 2002,			/**< ���ز����ܾ����� {"id":"a1"}*/
		kNIMNotificationIdLocalRejectInvite = 2003,			/**< ���ز����ܾ����� {"id":"a1"}*/
		kNIMNotificationIdLocalUpdateMemberProperty = 2004,		/**< ���ز�������Ⱥ��Ա����  {"team_member" : team_member_info} */
		kNIMNotificationIdLocalUpdateOtherNick = 2005,			/**< ���ز�����������nickname {}*/
		kNIMNotificationIdLocalGetTeamInfo = 2006,			/**< ���ز�����ȡȺ��Ϣ {"team_info":team_info} //Ⱥ����Ϣ(Keys SEE MORE `nim_team_def.h` ��Ⱥ����Ϣ Json Keys��)*/
		kNIMNotificationIdLocalGetTeamList = 2007,			/**< ���ز�����ȡȺ��Ա��Ϣ����*/
		kNIMNotificationIdLocalMuteMember = 2008,			/**< ���ز�����Ⱥ��Ա���� {"id":"a1", "mute":1-����,0-���} */
		//Netcall���ز���֪ͨ
		kNIMNotificationIdLocalNetcallReject = 3103,			/**< �ܾ��绰,{"calltype":1,"channel":6146078138783760761,"from":"id1","ids":["id1","id2"],"time":1430995380471}*/
		kNIMNotificationIdLocalNetcallNoResponse = 3104,			/**< ��Ӧ��δ��ͨ�绰,{"calltype":1,"channel":6146078138783760761,"from":"id1","ids":["id1","id2"],"time":1430995380471}*/
	};

	/** @enum NIMMsgLogSubStatus ��Ϣ��״̬ */
	enum NIMMsgLogSubStatus
	{
		kNIMMsgLogSubStatusNone = 0,			/**< Ĭ��״̬*/

		//�������־����������
		kNIMMsgLogSubStatusNotPlaying = 20,			/**< δ����*/
		kNIMMsgLogSubStatusPlayed = 21,			/**< �Ѳ���*/
	};

	struct SessionData
	{
		std::string		id_;					/**< �ỰID */
		NIMSessionType	type_;					/**< �Ự���� */
		int				unread_count_;			/**< ��ǰ�Ự��Ϣδ���� */
		NIMSessionCommand	command_;			/**< �Ự�޸����� */
		std::string		msg_id_;				/**< ��ǰ�Ự����һ����ϢID */
		std::string		msg_sender_accid_;		/**< ��ǰ�Ự����һ����Ϣ���ͷ�ID */
		__int64			msg_timetag_;			/**< ��ǰ�Ự����һ����Ϣʱ��������룩 */
		NIMMessageType	msg_type_;				/**< ��ǰ�Ự����һ����Ϣ���� */
		std::string		msg_content_;			/**< ��ǰ�Ự����һ����Ϣ���� */
		std::string		msg_attach_;			/**< ��ǰ�Ự����һ����Ϣ���� */
		NIMMsgLogStatus	msg_status_;			/**< ��ǰ�Ự����һ����Ϣ״̬ */
		NIMMsgLogSubStatus	msg_sub_status_;	/**< ��ǰ�Ự����һ����Ϣ��״̬ */
		bool			last_updated_msg_;		/**< (����)��Ϣ���������ʱ�Ƿ������һ���������Ϣ */

		/** ���캯�� */
		SessionData() : unread_count_(0)
			, msg_timetag_(0)
			, type_(kNIMSessionTypeP2P)
			, command_(kNIMSessionCommandAdd)
			, msg_type_(kNIMMessageTypeUnknown)
			, msg_status_(kNIMMsgLogStatusNone)
			, msg_sub_status_(kNIMMsgLogSubStatusNone)
			, last_updated_msg_(true){}
	};

	/** @brief �Ự�б����� */
	struct SessionDataList
	{
		int			count_;					/**< �Ự�б������� */
		int			unread_count_;			/**< �Ự�б��ܵ�δ����Ϣ�� */
		std::list<SessionData> sessions_;	/**< �Ự�б������� */

		SessionDataList() : count_(0), unread_count_(0) {}
	};
}

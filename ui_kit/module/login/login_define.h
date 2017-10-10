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
		LoginStatus_NONE,		//��ʼ״̬
		LoginStatus_LOGIN,		//���ڵ�¼
		LoginStatus_CANCEL,		//ȡ����¼
		LoginStatus_SUCCESS,	//�Ѿ���¼
		LoginStatus_EXIT		//�˳���¼
	};
	/** @enum NIMLoginState ��¼״̬ */
	enum NIMLoginState
	{
		kNIMLoginStateLogin = 1,		/**< ��¼״̬*/
		kNIMLoginStateUnLogin = 2,		/**< δ��¼״̬*/
	};

	/** @enum NIMLogoutType Logout���� */
	enum NIMLogoutType
	{
		kNIMLogoutChangeAccout = 1,	/**< ע��/�л��ʺţ����ص���¼���棩*/
		kNIMLogoutKickout = 2,	/**< ���ߣ����ص���¼���棩*/
		kNIMLogoutAppExit = 3,	/**< �����˳�*/
		kNIMLogoutRelogin = 4,	/**< ����������������������ʱ��������α��¼������������Լ����ߺ�������������ʺ�δ�仯��*/
	};

	/** @enum NIMKickReason ����ԭ�� */
	enum NIMKickReason
	{
		kNIMKickReasonSameGeneric = 1,  /**< �������͵Ŀͻ��˻���*/
		kNIMKickReasonServerKick = 2,  /**< �������˷����߿ͻ���ָ��*/
		kNIMKickReasonKickBySelfOtherClient = 3,  /**< ���Լ����������ߵ�*/
	};

	/** @enum NIMClientType �ͻ������� */
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

	/** @enum NIMLoginStep ��¼���� */
	enum NIMLoginStep
	{
		kNIMLoginStepLinking = 0,	/**< ��������*/
		kNIMLoginStepLink = 1,	/**< ���ӷ�����*/
		kNIMLoginStepLogining = 2,	/**< ���ڵ�¼*/
		kNIMLoginStepLogin = 3,	/**< ��¼��֤*/
	};

	/** @enum NIMMultiSpotNotifyType ����¼֪ͨ���� */
	enum NIMMultiSpotNotifyType
	{
		kNIMMultiSpotNotifyTypeImIn = 2,		/**< ֪ͨ�������߶��Լ���¼��*/
		kNIMMultiSpotNotifyTypeImOut = 3,		/**< ֪ͨ�������߶��Լ��˳�*/
	};

	/** @brief ��˵�½�ͻ�����Ϣ */
	struct OtherClientPres
	{
		std::string	app_account_;			/**< �������˺� */
		NIMClientType	client_type_;		/**< �ͻ�������, ��NIMClientType */
		std::string	client_os_;				/**< ��¼ϵͳ����,����ios 6.0.1 */
		std::string	mac_address_;			/**< ��¼�豸��mac��ַ */
		std::string	device_id_;				/**< �豸id��uuid */
		int64_t		login_time_;			/**< ���ε�½ʱ��, ���ȵ�ms */

		/** ���캯�� */
		OtherClientPres() : login_time_(0) {}
	};


	/** @brief ��¼����ص���Ϣ */
	struct LoginRes
	{
		NIMResCode res_code_;				/**< ���صĴ�����NIMResCode */
		bool relogin_;						/**< �Ƿ�Ϊ�������� */
		NIMLoginStep	login_step_;		/**< ��¼����NIMLoginStep */
		std::list<OtherClientPres> other_clients_;	/**< �����˵�����״̬�б���¼�ɹ��Ż᷵���ⲿ������ */
		bool retrying_;						/**< SDK�Ƿ������ԣ����Ϊfalse����������Ҫ����¼����ʹ����룬��ȷ���������ֶ������ӿڽ��е�¼���� */
		/** ���캯�� */
		LoginRes() : relogin_(false), retrying_(true) {}
	};

	/** @brief ���߽���ص���Ϣ */
	struct KickoutRes
	{
		NIMClientType client_type_;			/**< int, �ͻ�������NIMClientType */
		NIMKickReason kick_reason_;			/**< ���صı���ԭ��NIMKickReason */
	};

	/** @brief ��˵�¼�ص���Ϣ */
	struct MultiSpotLoginRes
	{
		NIMMultiSpotNotifyType	notify_type_;			/**< NIMMultiSpotNotifyType ����¼֪ͨ���� */
		std::list<OtherClientPres> other_clients_;		/**< �����˵�����״̬�б� */
	};

	/** @brief ���˽���ص���Ϣ */
	struct KickOtherRes
	{
		NIMResCode res_code_;					/**< ���صĴ�����NIMResCode */
		std::list<std::string> device_ids_;		/**< �豸id��uuid */
	};
}

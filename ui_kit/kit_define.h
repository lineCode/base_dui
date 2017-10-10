#pragma once


namespace nim_comp
{

	enum NIMResCode
	{
		//ͨ�ô�����
		kNIMResSuccess = 200,		/**< û�д���һ������ */
		kNIMResVersionError = 201,		/**< �ͻ��˰汾����ȷ */
		kNIMResUidPassError = 302,		/**< �û������������ */
		kNIMResForbidden = 403,		/**< ��ֹ���� */
		kNIMResNotExist = 404,		/**< �����Ŀ�꣨�û�����󣩲����� */
		kNIMResNoModify = 406,		/**< �������ϴβ�ѯ����δ�����仯�������������£� */
		kNIMResTimeoutError = 408,		/**< ������̳�ʱ */
		kNIMResParameterError = 414,		/**< �������� */
		kNIMResConnectionError = 415,		/**< �������ӳ��ִ��� */
		kNIMResFrequently = 416,		/**< ����̫��Ƶ�� */
		kNIMResExist = 417,		/**< �����Ѿ�����/�ظ����� */
		kNIMResOverrun = 419,		/**< ���� */
		kNIMResAccountBlock = 422,		/**< �ʺű����� */
		kNIMResUnknownError = 500,		/**< δ֪���󣬻��߲���������� */
		kNIMResServerDataError = 501,		/**< ���������ݴ��� */
		kNIMResTooBuzy = 503,		/**< ������̫æ */
		kNIMResExceedLimit = 508,		/**< ����������Ч�� */
		kNIMResInvalid = 509,		/**< Э����Ч, ��������ʵ�Э�� */
		//Ⱥ������
		kNIMResTeamECountLimit = 801,		/**< �Ѵﵽ�������� */
		kNIMResTeamENAccess = 802,		/**< û��Ȩ�� */
		kNIMResTeamENotExist = 803,		/**< Ⱥ������ */
		kNIMResTeamEMemberNotExist = 804,		/**< �û�������Ȥ������ */
		kNIMResTeamErrType = 805,		/**< Ⱥ���Ͳ��� */
		kNIMResTeamLimit = 806,		/**< ����Ⱥ�������� */
		kNIMResTeamUserStatusErr = 807,		/**< Ⱥ��Ա״̬���� */
		kNIMResTeamApplySuccess = 808,		/**< ����ɹ� */
		kNIMResTeamAlreadyIn = 809,		/**< �Ѿ���Ⱥ�� */
		kNIMResTeamInviteSuccess = 810,		/**< ����ɹ� */
		kNIMResForcePushCountLimit = 811,		/**< ǿ���б��˺��������� */
		//���ݽ��������
		kNIMResInvalidProtocol = 997,		/**< Э����ʧЧ */
		kNIMResEUnpacket = 998,		/**< ������� */
		kNIMResEPacket = 999,		/**< ������� */
		//
		kNIMResInBlack = 7101,		/**< �����շ���������� SDK�汾����2.5.0֧��*/
		//�ͻ����Զ���Ĵ����
		kNIMLocalRes = 10000,		/**< ֵ���ڸô���ŵĶ��ǿͻ����Զ���Ĵ���š��������������ֵ�� */
		kNIMResRoomLocalNeedRequestAgain = 10001,	/**< �ͻ��˱��ش���ţ���Ҫ������IM��������ȡ����������Ȩ�� */
		kNIMLocalResNetworkError = 10010,	/**< �ͻ��˱��ش���ţ��������������Ҫ��鱾������ */

		//�ͻ����Զ������Ϣ�����
		kNIMLocalResMsgNosUploadCancel = 10200,		/**< (�����ļ���Ϣ����stop_upload_ex)HTTP upload to NOS�ϴ���ͣ */
		kNIMLocalResMsgNosDownloadCancel = 10206,		/**< (�յ��ļ���Ϣ����stop_download_ex)HTTP download from NOS������ͣ */
		kNIMLocalResMsgNosDownloadCheckError = 10207,	/**< �յ��ļ���Ϣ��NOS������ɺ󱾵��ļ�������һ��Ϊ���ص��ļ���С���ļ���Ϣ���� */
		kNIMLocalResMsgFileNotExist = 10404,		/**< ������Դ������ */
		kNIMLocalResParameterError = 10414,		/**< ���ش����룬��������(�յ���Ϣ����Դ���ص�ַ��Ч���޷�����) */
		kNIMLocalResExist = 10417,		/**< ���ش����룬�����Ѿ�����/�ظ�������(�յ���Ϣ��������Դ�Ѵ��ڣ�����Ҫ�ظ�����) */
		kNIMLocalResParaError = 10450,		/**< ����api������Ĳ������� */
		kNIMLocalResMsgSendNosError = 10502,		/**< ������Ϣ���ϴ�NOSʧ�� */
		kNIMLocalResExceedLimit = 10508,		/**< ���ش�����,����������Ч�ڻ���������������� */
		kNIMLocalResCheckMsgDBFailed = 10600,		/**< ������Ϣ��ʷʱ��֤��ݺͼ�����Կ��ͨ�� */
		kNIMLocalResImportMsgDBFailed = 10601,		/**< ������Ϣ��ʷʱд��¼ʧ�� */
		//�ͻ����Զ����RTS�����
		kNIMLocalResRtsError = 11100,		/**< rts�Ự δ֪���� */
		kNIMLocalResRtsIdNotExist = 11101,		/**< rts�Ự id������ */
		kNIMLocalResRtsVChatExist = 11417,		/**< rts�Ự ����Ƶ�Ѵ��� */
		kNIMLocalResRtsStatusError = 11501,		/**< rts�Ự ͨ��״̬����ȷ */
		kNIMLocalResRtsChannelNotExist = 11510,		/**< rts�Ự ͨ�������� */

		kNIMResRoomLinkError = 13001,		/**< �����Ӵ��� */
		kNIMResRoomError = 13002,		/**< ������״̬�쳣 */
		kNIMResRoomBlackBeOut = 13003,		/**< �������û���ֹ���� */
		kNIMResRoomBeMuted = 13004,		/**< ������ */
		kNIMResRoomAllMuted = 13006,		/**< �����Ҵ����������״̬,ֻ�й���Ա�ܷ��� */

		//�ͻ����Զ����api��������
		kNIMLocalResAPIErrorInitUndone = 20000,		/**< ��δ��ʼ�����ʼ��δ������� */
		kNIMLocalResAPIErrorLoginUndone = 20001,		/**< ��δ��½���¼δ��� */
		kNIMLocalResAPIErrorLogined = 20002,		/**< �Ѿ���¼ */
		kNIMLocalResAPIErrorVersionError = 20003,		/**< SDK�汾���ԣ����ܻ������������� */
	};

	//log file
	static const std::wstring kLogFile = L"log.txt";

}
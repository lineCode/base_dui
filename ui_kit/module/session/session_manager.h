#pragma once

#include "base/memory/singleton.h"
#include "module/session/session_define.h"

namespace nim_comp
{
	typedef std::function<void(NIMResCode, const SessionData&, int)> ChangeCallback;	/**< �Ự���֪ͨ�ص�ģ�� */
	typedef std::function<void(int, const SessionDataList&)> QuerySessionListCallabck;		/**< ��ѯ�Ự�б�ص�ģ�� */
	typedef ChangeCallback DeleteRecentSessionCallabck;										/**< ɾ���Ự�ص�ģ�� */
	typedef ChangeCallback DeleteAllRecentSessionCallabck;									/**< ɾ��ȫ���Ự�ص�ģ�� */
	typedef ChangeCallback SetUnreadCountZeroCallback;										/**< �Ựδ����Ϣ������ص�ģ�� */

	class SessionManager
	{
	public:
		SINGLETON_DEFINE(SessionManager);
	public:
		SessionManager();
		~SessionManager();
	public:
		void DoLoadSession(const QuerySessionListCallabck& cb);
	private:
		void DoLoadSessionAsyn(const QuerySessionListCallabck* pcb);
	};
};
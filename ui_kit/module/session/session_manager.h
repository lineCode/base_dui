#pragma once

#include "base/memory/singleton.h"
#include "module/session/session_define.h"

namespace nim_comp
{
	typedef std::function<void(NIMResCode, const SessionData&, int)> ChangeCallback;	/**< 会话变更通知回调模板 */
	typedef std::function<void(int, const SessionDataList&)> QuerySessionListCallabck;		/**< 查询会话列表回调模板 */
	typedef ChangeCallback DeleteRecentSessionCallabck;										/**< 删除会话回调模板 */
	typedef ChangeCallback DeleteAllRecentSessionCallabck;									/**< 删除全部会话回调模板 */
	typedef ChangeCallback SetUnreadCountZeroCallback;										/**< 会话未读消息数清零回调模板 */

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
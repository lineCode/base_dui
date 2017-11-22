﻿#pragma once
#include "module/session/session_define.h"

namespace nim_comp
{
//class AtMeView;
	class SessionBox : public dui::VerticalLayout, public IDropTarget
	{
	public:
		SessionBox(std::string id, NIMSessionType type);
		~SessionBox();

		virtual void Init() override;
		void InitRichEdit();

		//virtual bool Notify(void* param) override;
		bool OnClicked(dui::TEvent *event);
#if 0
		std::string	GetSessionID(){return session_id_;};
		nim::NIMSessionType GetSessionType(){ return session_type_; };
		nim::TeamInfo* GetSessionTeamInfo(){ return session_type_ == nim::NIMSessionType::kNIMSessionTypeTeam? &team_info_:NULL; };

		void InitControl();
		void InitRichEdit();
		/** 
		* 获取窗体的ID，用于唯一标识这个窗体
		* @return wstring 返回了会话id,用于区别其他会话窗体
		*/

		/*virtual void OnFinalMessage(HWND hWnd) override;*/
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam ,bool &bHandle);


		virtual bool OnSelChanged(ui::EventArgs* param);
		virtual bool OnEditEnter(ui::EventArgs* param);
		virtual bool OnFrameRightKillFouce(ui::EventArgs* param);
		virtual bool OnFrameRightSetFouce(ui::EventArgs* param);
		virtual bool OnShowGroupInfo(ui::EventArgs* param);
		void SendSystemTip(const std::wstring &tip);

		//////////////////////////////////////////////////////////////////////////
		//会话消息相关的操作

		/** 
		* 当收到新的会话消息后，添加新消息到聊天框里，并且根据消息的类型来调整会话框状态
		*（一般由TalkCallback类来自动调用此函数，不需要自己调用，调用ShowMsg函数完成真正的消息添加）
		* @param[in] msg	新消息的数据
		* @param[in] create	如果收到新消息时对应的会话窗体还没有创建，则设置为true	
		* @return void 无返回值
		*/
		void AddNewMsg(const nim::IMMessage &msg, bool create);

		/** 
		* 在聊天框内添加一个“对方正在输入”的状态消息（当在输入框编辑时调用）
		* @param[in] msg	“对方正在输入”消息的数据
		* @return void 无返回值
		*/
		void AddWritingMsg(const nim::IMMessage &msg);

		/**
		* 添加一个被转发的消息到聊天框
		* @param[in] msg 消息数据
		* @return void 无返回值
		*/
		void AddRetweetMsg(const nim::IMMessage &msg);

		/**
		* 添加一条新消息到聊天框里
		* @param[in] msg	新消息的数据
		* @param[in] first	是否把消息添加到聊天框的开头位置
		* @param[in] show_time	是否显示这条消息的接收时间
		* @return MsgBubbleItem* 新添加的消息控件的指针
		*/
		MsgBubbleItem* ShowMsg(const nim::IMMessage &msg, bool first, bool show_time);

		void MsgClear();

		/** 
		* 执行加载历史消息的操作
		* @param[in] first_show_msg 是否是本会话窗体第一次加载历史消息(刚创建会话窗体时设置为true)
		* @return void 无返回值
		*/
		void InvokeShowMsgs(bool first_show_msg);

		/** 
		* 加载历史消息，把一组历史消息添加到聊天框的开头位置
		* @param[in] msg	历史消息数据	
		* @return void 无返回值
		*/
		void ShowMsgs(const std::vector<nim::IMMessage> &msg);

		/** 
		* 新消息发送给对方后，获取消息发送状态的回调函数（是否发送成功等）
		* @param[in] cid	消息id 
		* @param[in] code	错误码(200代表无错误)
		* @param[in] msg_timetag	时间戳
		* @return void 无返回值
		*/
		void OnSendMsgCallback(const std::string &cid, int code, __int64 msg_timetag);

		/**
		* 新消息发送给对方后，获取消息发送状态的回调函数（对方是否已读）
		* @param[in] from_accid	发送方id
		* @param[in] timetag	临界消息time
		* @param[in] status	消息状态
		* @return void 无返回值
		*/
		void OnMsgStatusChangedCallback(const std::string &from_accid, const __int64 timetag, nim::NIMMsgLogStatus status);

		/**
		* 阅后即焚消息被（发送者或接收者）读了之后，从消息列表删除
		* @param[in] from_accid	发送方id
		* @param[in] timetag	临界消息time
		* @param[in] status	消息状态
		* @return void 无返回值
		*/
		void OnSnapchatRead(const std::string& client_msg_id);

		/** 
		* 对方发送文件过来，获取是否成功下载文件的回调函数
		* @param[in] cid		消息id 
		* @param[in] success	是否下载成功			
		* @return void 无返回值
		*/
		void OnDownloadCallback(const std::string &cid, bool success);

		/**
		* 被转发的消息的资源文件被清理了，则重新下载，否则加载本地资源文件的消息会显示有错误
		* @param[in] code 返回码
		* @param[in] file_path 文件下载保存的路径
		* @param[in] sid 会话id（好友id或群id）
		* @param[in] cid 消息id
		* @return void	无返回值
		*/
		void OnRetweetResDownloadCallback(nim::NIMResCode code, const std::string& file_path, const std::string& sid, const std::string& cid);
		//////////////////////////////////////////////////////////////////////////
		//音频相关的操作

		/** 
		* 单击了语音消息后，开始播放语音的回调函数
		* @param[in] cid	消息id 
		* @param[in] code	错误码(200代表无错误)	
		* @return void 无返回值
		*/
		void OnPlayAudioCallback(const std::string &cid, int code);

		/**
		* 第二次单击了语音消息后，停止播放语音的回调函数
		* @param[in] cid	消息id
		* @param[in] code	错误码(200代表无错误)
		* @return void 无返回值
		*/
		void OnStopAudioCallback(const std::string &cid, int code);

		//////////////////////////////////////////////////////////////////////////
		//群相关的操作

		/** 
		* 执行获取群信息的操作	
		* @return void 无返回值
		*/
		void InvokeGetTeamInfo(bool sync_block = false);

		/** 
		* 获取了群信息后自动调用的回调函数，根据群信息设置会话窗体的显示状态
		* @param[in] tid	群ID 
		* @param[in] result	群信息结构体			
		* @return void 无返回值
		*/
		void OnGetTeamInfoCb(const std::string& tid, const nim::TeamInfo& result);

		/**
		* 调整为高级群会话窗体的大小
		* @return void 无返回值
		*/
		void AdjustSizeForAdvancedTeam();

		/**
		* 执行获取群成员信息的操作
		* @return void 无返回值
		*/
		void InvokeGetTeamMember();

		/**
		* 获取了群成员信息后自动调用的回调函数，填充高级群会话右侧的群成员列表
		* @param[in] tid	群ID
		* @param[in] count	群成员数
		* @param[in] team_member_info_list 群成员信息表
		* @return void 无返回值
		*/
		void OnGetTeamMemberCb(const std::string& tid, int count, const std::list<nim::TeamMemberProperty>& team_member_info_list);

		bool IsValid() { return is_valid_;  };

		nim::TeamMemberProperty GetTeamMemberInfo(const std::string& uid)
		{
			auto iter = team_member_info_list_.find(uid);
			if (iter != team_member_info_list_.cend())
				return iter->second;
			else
				return nim::TeamMemberProperty();
		}

		void RecallMsg(nim::NIMResCode code, const nim::RecallMsgNotify &notify);

		void ShowGroupInfo();

		void OnBtnClip();

		void OnBtnRemoteDesktop();

		void SendFile(const std::wstring &src);

		void SendImage(const std::wstring &src);

		void PopupRemoteDesktopMenu();
		bool OnRemoteDesktopMenu(ui::EventArgs* param);
		void OnRemoteDesktopMenuControl(RDCHANNEL_TYPE type);

		void GetImageListInfo(ImageViewManager::ImageViewInfoList *info_list);

		//////////////////////////////////////////////////////////////////////////
		//@功能相关的操作
#pragma region At
	public:

		/**
		* 填充AtMeView控件内容
		* @param[in] infos @me消息
		* @return void	无返回值
		*/
		void InitAtMeView(std::vector<ForcePushManager::ForcePushInfo> &infos);

	private:

		/**
		* 如果msg包含atme消息，就添加到AtMeView控件
		* @param[in] msg 消息
		* @return void	无返回值
		*/
		void AddAtMessage(const nim::IMMessage &msg);

		/**
		* 开始执行滚动到某个控件位置的动画，用于@me消息查看
		* @param[in] control 滚动的目标控件
		* @return void	无返回值
		*/
		void ScrollToControl(const ui::Control *control);

		/**
		* 当前位置滚动到某个控件位置的动画过程的回调函数
		* @param[in] control 滚动的目标控件
		* @return void	无返回值
		*/
		void ScrollToControlCallback(const ui::Control *control);

		/**
		* 获取某uid显示的名称，优先显示备注名，其次显示群名片，最后显示昵称
		* @param[in] uid 用户id
		* @return string 显示名字
		*/
		std::string GetShowName(const std::string& uid);

		/**
		* 处理@我预览窗被单击的消息
		* @param[in] param 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool OnAtMeViewClick(ui::EventArgs* param);

		/**
		* 处理@我预览窗关闭按钮被单击的消息
		* @param[in] param 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool OnCloseAtMeView(ui::EventArgs* param);

		/**
		* 处理输入框的@相关的按键消息
		* @param[in] wParam 附加消息
		* @param[in] lParam 附加消息
		* @return bool true 不继续传递消息，false 继续传递消息
		*/
		bool HandleAtMsg(WPARAM wParam, LPARAM lParam);

		/**
		* 处理输入框的@相关的鼠标滚轮消息
		* @param[in] wParam 附加消息
		* @param[in] lParam 附加消息
		* @return bool true 不继续传递消息，false 继续传递消息
		*/
		bool HandleAtMouseWheel(WPARAM wParam, LPARAM lParam);

		/**
		* 隐藏@列表窗口
		* @return void	无返回值
		*/
		void HideAtListForm();

		/**
		* 响应@列表被单击的回调函数
		* @param[in] uid 被选择的用户id
		* @return void	无返回值
		*/
		void OnSelectAtItemCallback(const std::string& uid, bool is_robot);

		/**
		* 获取最近发消息的5个人（不包括自己）,最新发言的在列表最前
		* @param[out] uid_last_five 最近发消息的5个人
		* @return void	无返回值
		*/
		void GetLastFiveSender(std::list<std::string> &uid_last_five);
#pragma endregion At

	private:
		/** 
		* 执行截图操作		
		* @return void 无返回值
		*/
		void DoClip();

		/** 
		* 截图完毕后的回调函数
		* @param[in] ret	是否截图成功 
		* @param[in] file_path	截图文件的保存路径		
		* @return void 无返回值
		*/
		void OnClipCallback(bool ret, const std::wstring& file_path);

		void OnBtnSend();

	private:
		// @功能相关变量
		nbase::WeakCallbackFlag	scroll_weak_flag_;		//一键顶部查看和@me滚动条动画所用的弱引用标志
		LONG					scroll_step_;			//滚动条步长
		bool					scroll_top_;			//单击@me消息滚动的方向

		AtMeView*				atme_view_;			//@我 消息预览

		struct AtSomeone
		{
			std::string uid_;
			bool is_robot_ = false;
		};
		std::map<std::string, AtSomeone>	uid_at_someone_;//当前输入框被@的人的昵称和uid

	private:
		void ShowMsgWriting(const nim::IMMessage &msg);
		void CancelWriting();
		void SendText(const std::string &text);
		void SendSnapChat(const std::wstring &src);
		bool CheckFileSize(const std::wstring &src);
		void SendJsb(const std::string &attach);
		void SendSticker(const std::string &catalog, const std::string &name);
		void SendTip(const std::wstring &tip);
		void AddSendingMsg(const nim::IMMessage &msg);
		void ReSendMsg(nim::IMMessage &msg);
		void PackageMsg(nim::IMMessage &msg);
		void PackageSystemMsg(nim::IMMessage &msg);
		void CheckLastReceiptMsg();
		bool GetLastNeedSendReceiptMsg(nim::IMMessage &msg);
		int RemoveMsgItem(const std::string& client_msg_id); //移除消息气泡

		void CallbackWriteMsglog(const std::wstring &notify_text, nim::NIMResCode res_code, const std::string& msg_id);



	private:
		void FlashTaskbar();

		/** 
		* 收到新消息时,改变窗体状态来通知用户
		* @param[in] create	是否刚创建会话窗体 
		* @param[in] flash	是否需要让任务栏图标闪烁			
		* @return void 无返回值
		*/
		void OnNewMsg(bool create, bool flash);

		void OnDropFile(HDROP hDrop);

		/** 
		* 响应发送图片按钮消息
		* @param[in] is_snapchat 是否阅后即焚 
		* @return void 无返回值
		*/
		void OnBtnImage(bool is_snapchat);

		/** 
		* 图片被选择后的回调函数
		* @param[in] is_snapchat 是否阅后即焚 
		* @param[in] ret		 是否选择了图片	
		* @param[in] file_path	 选择的图片的路径
		* @return void 无返回值
		*/
		void OnImageSelected(bool is_snapchat, BOOL ret, std::wstring file_path);

		/**
		* 响应发送文件按钮消息
		* @return void 无返回值
		*/
		void OnBtnFile();

		/**
		* 文件被选择后的回调函数
		* @param[in] ret		 是否选择了文件
		* @param[in] file_path	 选择的文件的路径
		* @return void 无返回值
		*/
		void OnFileSelected(BOOL ret, std::wstring file_path);

		/**
		* 响应“剪刀石头布”按钮消息
		* @return void 无返回值
		*/
		void OnBtnJsb();

		/**
		* 响应表情按钮消息
		* @return void 无返回值
		*/
		void OnBtnEmoji();

		/**
		* 某个表情被选择后的回调函数
		* @param[in] emo		 选择的表情
		* @return void 无返回值
		*/
		void OnEmotionSelected(std::wstring emo);

		/**
		* 某个贴图表情被选择后的回调函数
		* @param[in] catalog	贴图所在目录
		* @param[in] name		贴图名字
		* @return void 无返回值
		*/
		void OnEmotionSelectedSticker(const std::wstring &catalog, const std::wstring &name);

		/**
		* 表情选择窗体关闭后的回调函数
		* @return void 无返回值
		*/
		void OnEmotionClosed();

		void OnBtnAudio();
		void OnBtnVideo();
		void OnBtnRts();
		/**
		* 响应单击转发菜单项的消息
		* @param[in] msg 被转发的消息
		* @return void 无返回值
		*/
		void OnMenuRetweetMessage(const nim::IMMessage &msg);

		/**
		* 收到转发列表后的回调函数
		* @param[in] msg 被转发的消息
		* @param[in] friend_list 转发的好友帐号数组
		* @param[in] team_list 转发的群组数组
		* @return void 无返回值
		*/
		void OnSelectedRetweetList(nim::IMMessage msg, const std::list<std::string>& friend_list/*, const std::list<std::string>& team_list*/);

		/**
		* 关闭会话窗体时，如果有输入内容，会出现确认对话框，这里响应用户的选择
		* @return void 无返回值
		*/
		void OnCloseInputMsgBoxCallback(MsgBoxRet ret);

		/**
		* 移除聊天窗中的某个提示内容（比如对方正在输入、对方非好友）
		* @param[in] type
		* @return void 无返回值
		*/
		void AddTip(SessionTipType type);

		/** 
		* 移除聊天窗中的某个提示内容
		* @param[in] type 	
		* @return void 无返回值
		*/
		void RemoveTip(SessionTipType type);

		void EnterTeamHandle();
		void LeaveTeamHandle();
		void DismissTeamHandle();

		/** 
		* 响应邀请按钮消息，邀请他人加入群聊
		* @param[in] param 被单击的按钮的相关信息
		* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
		*/
		bool OnBtnInvite(ui::EventArgs* param);

		/** 
		* 邀请完毕后的回调函数，根据邀请的名单把个人会话升级带讨论组会话
		* @param[in] id_list 邀请的好友帐号数组
		* @return void 无返回值
		*/
		void CreateGroup(const std::list<UTF8String>& id_list);

		/**
		* 响应个人信息(群信息)按钮消息，显示个人信息（群信息）
		* @param[in] param 被单击的按钮的相关信息
		* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
		*/
		bool OnBtnHeaderClick(ui::EventArgs* param);

		/**
		* 对方个人资料改变的回调函数
		* @param[in] uinfos 新的个人资料列表
		* @return void 无返回值
		*/
		void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);

		/**
		* 会话框中某人的头像下载完成的回调函数
		* @param[in] accid 头像下载完成的用户id
		* @param[in] photo_path 头像本地路径
		* @return void 无返回值
		*/
		void OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path);
	private:

		/** 
		* 根据会话的信息，设置投降
		* @return void 无返回值
		*/
		void CheckHeader();

		/** 
		* 用户掉线重连后的回调函数
		* @param[in] json 消息体Json，包含是否重连成功的信息	
		* @return void 无返回值
		*/
		void OnRelink(const Json::Value &json);

		/** 
		* 更新群公告
		* @param[in] broad 消息体Json，包含群公告信息			
		* @return void 无返回值
		*/
		void UpdateBroad(const Json::Value &broad);

		/** 
		* 根据群组类型设置会话窗体的外观
		* @param[in] type 群组类型
		* @return void 无返回值
		*/
		void CheckTeamType(nim::NIMTeamType type);

		/**
		* 是否正在与本帐号的手机端通信
		* @return bool 返回值true: 是， false: 否
		*/ 
		bool IsFileTransPhone();

		/** 
		* 显示自定义通知窗体	
		* @return void 无返回值
		*/
		void ShowCustomMsgForm();
	private:
		void UpdateSessionIcon(const std::wstring &icon);
		void RefreshMsglistShowname(const std::string& uid); //刷新消息列表中的名字
	private:
		/** 
		* 有群成员增加的回调函数
		* @param[in] tid 群ID 
		* @param[in] team_member_info 新增群成员的信息			
		* @return void 无返回值
		*/
		void OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberProperty& team_member_info);

		/**
		* 有群成员退出的回调函数
		* @param[in] tid 群ID
		* @param[in] uid 退出的群成员的帐号
		* @return void 无返回值
		*/
		void OnTeamMemberRemove(const std::string& tid, const std::string& uid);

		/**
		* 有群成员信息改变的回调函数
		* @param[in] tid_uid 群ID和群成员的帐号，用'#'字符连接起来
		* @param[in] team_card 群昵称
		* @return void 无返回值
		*/
		void OnTeamMemberChange(const std::string& tid_uid, const std::string& team_card);

		/**
		* 有群成员成为/取消管理员身份的回调函数
		* @param[in] tid 群ID
		* @param[in] uid 群成员的帐号
		* @param[in] admin 设置/取消管理员身份
		* @return void 无返回值
		*/
		void OnTeamAdminSet(const std::string& tid, const std::string& uid, bool admin);

		/**
		* 有群成员被禁言/被取消禁言的回调函数
		* @param[in] tid 群ID
		* @param[in] uid 群成员的帐号
		* @param[in] set_mute 禁言/取消禁言
		* @return void 无返回值
		*/
		void OnTeamMuteMember(const std::string& tid, const std::string& uid, bool set_mute);

		/**
		* 有群成员成为群主的回调函数
		* @param[in] tid 群ID
		* @param[in] uid 群成员的帐号
		* @return void 无返回值
		*/
		void OnTeamOwnerChange(const std::string& tid, const std::string& uid);

		/**
		* 群名更新的回调函数
		* @param[in] team_info 更新后的群信息
		* @return void 无返回值
		*/
		void OnTeamNameChange(const nim::TeamInfo& team_info);

		/**
		* 退群或解散群的回调函数
		* @param[in] tid 退出的群id
		* @return void 无返回值
		*/
		void OnTeamRemove(const std::string& tid);

		/**
		* 判断用户类型是否可以显示在群成员中
		* @param[in] user_type 用户信息
		* @return void 无返回值
		*/
		bool IsTeamMemberType(const nim::NIMTeamUserType user_type);

		void SendReceiptIfNeeded(bool auto_detect = false);
		void SetTeamMuteUI(bool mute);

	public:
		//////////////////////////////////////////////////////////////////////////
		// 实现系统的文件拖拽接口
		BOOL CheckDropEnable(POINTL pt);
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		ULONG STDMETHODCALLTYPE AddRef(void);
		ULONG STDMETHODCALLTYPE Release(void);
		HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
		HRESULT STDMETHODCALLTYPE DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
		HRESULT STDMETHODCALLTYPE DragLeave(void);
		HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);
	private:
		//HICON			icon_handle_;

		/*ui::Button*		btn_max_restore_;
		ui::Label*		label_title_;
		ui::Label*		label_tid_;
		ui::Button*		btn_header_;

		ui::Button*		btn_invite_;*/

		ui::Box*		msg_content_;
		ui::TabBox*		frame_right_;
		ui::ListBox*	msg_list_;
		ui::CheckBox*	btn_face_;
		ui::RichEdit*	input_edit_;
		ui::Button*		btn_send_;

		ui::Button*		btn_new_broad_;
		ui::RichEdit*	re_broad_;
		ui::Label*		label_member_;
		ui::Button*		btn_refresh_member_;
		ui::ListBox*	member_list_;
		std::map<std::string, nim::TeamMemberProperty> team_member_info_list_;

		std::string		session_id_;
		nim::NIMSessionType session_type_;

		bool			first_show_msg_;
		long long		last_msg_time_;  //最近收到的消息时间
		long long		farst_msg_time_; //最远收到的消息时间

		//接收
		bool			has_writing_cell_;
		nbase::WeakCallbackFlag cancel_writing_timer_;
		//发送
		long long		writing_time_;

		typedef std::map<std::string,MsgBubbleItem*> IdBubblePair;
		IdBubblePair	id_bubble_pair_;
		std::list<MsgBubbleItem*> cached_msgs_bubbles_; //记录消息前后顺序 lty

		nim::TeamInfo	team_info_;
		bool			is_valid_;
		AutoUnregister	unregister_cb;

		std::string		last_receipt_msg_id_; //最近一条发送
		bool			receipt_need_send_ = false; //当下会话是否有已读回执需要发送

		bool			is_frame_right_focus_;

		AtlistForm*		at_list_form;
	private:
		IDropTarget		*droptarget_;
		IRichEditOleCallbackEx *richeditolecallback_;
#endif

	public:
		// 实现系统的文件拖拽接口
		//BOOL CheckDropEnable(POINTL pt);
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		ULONG STDMETHODCALLTYPE AddRef(void);
		ULONG STDMETHODCALLTYPE Release(void);
		HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
		HRESULT STDMETHODCALLTYPE DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
		HRESULT STDMETHODCALLTYPE DragLeave(void);
		HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);
	private:
		dui::RichEdit*	input_edit_;

		IDropTarget		*droptarget_;
		//IRichEditOleCallbackEx *richeditolecallback_;

	};

}
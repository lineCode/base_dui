﻿#include "stdafx.h"
#include "session_box.h"
#include "module/session/session_util.h"
#include "module/login/login_manager.h"
#include "gui/msgbox/msgbox.h"
//#include "custom_msg_form.h"
//#include "module/session/session_manager.h"
//#include "callback/session/session_callback.h"
//#include "util/user.h"
//#include "export/nim_ui_window_manager.h"

using namespace dui;
using namespace std;

namespace nim_comp
{
	const int kMsgLogNumberShow = 20;
#if 0
	void SessionBox::AddNewMsg(const nim::IMMessage &msg, bool create)
	{
		bool at_end = msg_list_->IsAtEnd();

		bool show_time = false;
		if (first_show_msg_)
		{
			show_time = true;
			farst_msg_time_ = msg.timetag_;
		}
		else
		{
			show_time = CheckIfShowTime(last_msg_time_, msg.timetag_);
		}
		ShowMsg(msg, false, show_time);

		if(!IsNoticeMsg(msg))
			last_msg_time_ = msg.timetag_;

		if (first_show_msg_)
		{
			first_show_msg_ = false;
			AddTip(STT_LOAD_MORE_MSG);
		}

		RemoveTip(STT_WRITING);
		writing_time_ = 0;

		bool flash = true;
		if (msg.feature_ == nim::kNIMMessageFeatureSyncMsg || msg.type_ == nim::kNIMMessageTypeNotification)
			flash = false;
		OnNewMsg(create, flash);

		if(at_end)
		{
			msg_list_->EndDown(true, false);
		}

		SendReceiptIfNeeded(true);
		AddAtMessage(msg);
	}

	void SessionBox::AddWritingMsg(const nim::IMMessage &msg)
	{
		bool at_end = msg_list_->IsAtEnd();

		Json::Value json;
		if (StringToJson(msg.attach_, json))
		{
			std::string id = json["id"].asString();
			if (id == "1")
			{
				ShowMsgWriting(msg);
			}
		}

		if (at_end)
		{
			msg_list_->EndDown(true, false);
		}
	}

	void SessionBox::AddRetweetMsg(const nim::IMMessage &msg)
	{
		// 如果这个消息对应的资源文件被清理掉，就重新下载
		if (!IsResourceExist(msg))
		{
			nim::NOS::FetchMedia(msg, nbase::Bind(&SessionBox::OnRetweetResDownloadCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), nim::NOS::ProgressCallback());
		}
		this->AddSendingMsg(msg);
	}

	MsgBubbleItem* SessionBox::ShowMsg(const nim::IMMessage &msg, bool first, bool show_time)
	{
#if MODE_NO_TYPE_VIDEO
		if (msg.type_ == nim::NIMMessageType::kNIMMessageTypeVideo)		//屏蔽视频
		{
			return NULL;
		}
#endif
		nim::IMMessage message = msg;
		const std::string &bubble_id = msg.client_msg_id_;
		if(bubble_id.empty())
		{
			QLOG_WAR(L"msg id empty");
			return nullptr;
		}

		IdBubblePair::iterator it = id_bubble_pair_.find(bubble_id);
		if(it != id_bubble_pair_.end())
		{
			QLOG_WAR(L"repeat msg: {0}") <<bubble_id;
			return nullptr;
		}

		MsgBubbleItem* item = NULL;

		if (msg.type_ == nim::kNIMMessageTypeText || IsNetCallMsg(msg.type_, msg.attach_))
		{
			Json::Value values;
			Json::Reader reader;
			if (reader.parse(msg.attach_, values) 
				&& values.isObject() 
				&& values.isMember("comment")
				&& values["comment"].asString() == "is_recall_notification")
			{
				std::wstring notify_text = nbase::UTF8ToUTF16(msg.content_);
				if (values.isMember("notify_from"))
				{
					std::string from_id = values["notify_from"].asString();
					if (from_id == LoginManager::GetInstance()->GetAccount())
					{
						notify_text = L"我撤回了一条消息";
					}
					else
					{
						if (session_type_ == nim::kNIMSessionTypeP2P)
						{
							notify_text = L"对方撤回了一条消息";
						}
						else
						{
							auto info = GetTeamMemberInfo(from_id);
							UTF8String name = info.GetNick();
							if (name.empty())
							{
								nim::UserNameCard name_card;
								UserService::GetInstance()->GetUserInfo(from_id, name_card);
								name = name_card.GetName();
							}
							if (name.empty())
								name = from_id;
							notify_text = nbase::UTF8ToUTF16(name) + L" 撤回了一条消息";
						}
					}
				}

				MsgBubbleNotice* cell = new MsgBubbleNotice;
				GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
				if (first)
					msg_list_->AddAt(cell, 0);
				else
					msg_list_->Add(cell);
				cell->InitControl();
				cell->InitCustomInfo(notify_text, session_id_, msg.client_msg_id_);
				return nullptr;
			}
			else
				item = new MsgBubbleText;
		}
		else if (msg.type_ == nim::kNIMMessageTypeImage)
			item = new MsgBubbleImage;
		else if (msg.type_ == nim::kNIMMessageTypeAudio)
			item = new MsgBubbleAudio;
		else if (msg.type_ == nim::kNIMMessageTypeLocation)
			item = new MsgBubbleLocation;
		else if (msg.type_ == nim::kNIMMessageTypeFile)
			item = new MsgBubbleFile;
		else if (msg.type_ == nim::kNIMMessageTypeVideo)
			item = new MsgBubbleVideo;
		else if (msg.type_ == nim::kNIMMessageTypeRemote)
			item = new MsgBubbleText;
		else if (msg.type_ == nim::kNIMMessageTypeRemoteACK)
			item = new MsgBubbleText;
		else if (msg.type_ == nim::kNIMMessageTypeNotification || msg.type_ == nim::kNIMMessageTypeTips)
		{
			id_bubble_pair_[bubble_id] = NULL;

			MsgBubbleNotice* cell = new MsgBubbleNotice;
			GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
			if(first)
				msg_list_->AddAt(cell, 0);
			else
				msg_list_->Add(cell);
			cell->InitControl();
			cell->InitInfo(msg, session_id_);
			return nullptr;
		}
		else if (msg.type_ == nim::kNIMMessageTypeCustom)
		{
			Json::Value json;
			if (StringToJson(msg.attach_, json))
			{
				int sub_type = json["type"].asInt();
				if (sub_type == CustomMsgType_Jsb) //finger
				{
					item = new MsgBubbleFinger;
				}
				else if (sub_type == CustomMsgType_SnapChat)
				{
					item = new MsgBubbleSnapChat;
				}
				else if (sub_type == CustomMsgType_Sticker)
				{
					item = new MsgBubbleSticker;
				}
				else if (sub_type == CustomMsgType_Rts)
				{
					if (json["data"].isObject())
					{
						int flag = json["data"]["flag"].asInt();
						if (flag == 0)
						{
							item = new MsgBubbleText;
						}
						else if (flag == 1)
						{
							id_bubble_pair_[bubble_id] = NULL;

							MsgBubbleNotice* cell = new MsgBubbleNotice;
							GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
							if (first)
								msg_list_->AddAt(cell, 0);
							else
								msg_list_->Add(cell);
							cell->InitControl();
							cell->InitInfo(msg, session_id_);
							return nullptr;
						}
					}
				}
				else if (sub_type == CustomMsgType_PhoneFile)	//add by scz
				{
					item = new MsgBubbleFile;

					Json::Value value = msg.msg_setting_.server_ext_["file"];

					nim::IMFile file;
					file.md5_ = GetFileMD5(L"");
					string file_size = value["fileSize"].asString();
					file_size = file_size.substr(0, file_size.find_first_of('B'));
					file.size_ = atoi(file_size.c_str());

					nbase::PathString file_name = nbase::UTF8ToUTF16(value["fileName"].asString());
					file.file_extension_ = value["fileType"].asString();
					file.display_name_ = nbase::UTF16ToUTF8(file_name) + file.file_extension_;
					file.url_ = value["url"].asString();

					message.attach_ = file.ToJsonString();

				}
			}
		}
		if (item == nullptr)
		{
			QLOG_WAR(L"unknown msg: cid={0} msg_type={1}") << bubble_id << msg.type_;
			item = new MsgBubbleUnknown;
		}

		bool bubble_right = IsBubbleRight(msg);
		if(bubble_right)
			GlobalManager::FillBoxWithCache( item, L"session/bubble_right.xml" );
		else
			GlobalManager::FillBoxWithCache( item, L"session/bubble_left.xml" );

		if(first)
			msg_list_->AddAt(item, 0);
		else
			msg_list_->Add(item);

		item->InitControl(bubble_right);
		//----------add by scz---------
		Json::Value json;
		StringToJson(msg.attach_, json);
		if (msg.type_ == nim::kNIMMessageTypeCustom && json["type"].asInt() == CustomMsgType_PhoneFile)
			item->InitInfo(message);
		else
			item->InitInfo(msg);
		//---------end-------------------
		item->SetSessionId(session_id_);
		item->SetSessionType(session_type_);
		item->SetShowTime(show_time);
		if (bubble_right || msg.session_type_ == nim::kNIMSessionTypeP2P)
			item->SetShowName(false, "");
		else
		{
			auto iter = team_member_info_list_.find(msg.sender_accid_);
			if (iter != team_member_info_list_.cend() && !iter->second.GetNick().empty())
				item->SetShowName(true, iter->second.GetNick()); //显示群名片
			else
			{
				std::string show_name = nbase::UTF16ToUTF8(UserService::GetInstance()->GetUserName(msg.sender_accid_));
				item->SetShowName(true, show_name); //显示备注名或昵称
			}
		}

		id_bubble_pair_[bubble_id] = item;
		if (first)//第一次打开，顺序倒序
			cached_msgs_bubbles_.insert(cached_msgs_bubbles_.begin(), 1, item);
		else
			cached_msgs_bubbles_.push_back(item);

		return item;
	}

	void SessionBox::ShowMsgWriting(const nim::IMMessage &msg)
	{
		cancel_writing_timer_.Cancel();
		StdClosure cb = nbase::Bind(&SessionBox::CancelWriting, this);
		auto weak_cb = cancel_writing_timer_.ToWeakCallback(cb);
		nbase::ThreadManager::PostDelayedTask(weak_cb, nbase::TimeDelta::FromSeconds(kCellCancelWriting));

		if(has_writing_cell_)
			return;

		MsgBubbleWriting* item = new MsgBubbleWriting;
		GlobalManager::FillBoxWithCache(item, L"session/bubble_left.xml");
		msg_list_->Add(item);

		item->InitControl(false);
		item->InitInfo(msg);
		item->SetShowTime(false);
		item->SetShowName(false, "");

		item->SetName(CELL_WRITING);

		has_writing_cell_ = true;
	}

	void SessionBox::CancelWriting()
	{
		bool at_end = msg_list_->IsAtEnd();

		RemoveTip(STT_WRITING);

		if(at_end)
			msg_list_->EndDown(true, false);
	}

	void SessionBox::SendText( const std::string &text )
	{
		nim::IMMessage msg;
		//TODO(litianyi)
		PackageMsg(msg);
		msg.type_ = nim::kNIMMessageTypeText;
		msg.content_ = text;
		nim::MessageSetting setting;
		//nickname客户端不需要填写
		//UserInfo user_info;
		//UserService::GetInstance()->GetUserInfo(msg.from_account, user_info);
		//msg.from_nick = user_info.name;

		if (!uid_at_someone_.empty())
		{
			//检查文本消息中是否存在“@xxx ”的文本
			for (auto it = uid_at_someone_.begin(); it != uid_at_someone_.end(); ++it)
			{
				std::string nick_name = it->first;
				std::string at_str = "@";
				at_str.append(nick_name);
				at_str.append(" ");

				int index = text.find(at_str);
				if (index != std::string::npos)
				{
					if (!it->second.is_robot_)
					{
						setting.force_push_ids_list_.push_back(it->second.uid_);
					}
				}
			}

			if (!setting.force_push_ids_list_.empty())
			{
				setting.is_force_push_ = nim::BS_TRUE;
				setting.force_push_content_ = text;
			}

			uid_at_someone_.clear();
		}
		AddSendingMsg(msg);
		//Json::Reader reader;
		//std::string test_string = "{\"remote\":{\"mapmap\":{\"int\":1,\"boolean\":false,\"list\":[1,2,3],\"string\":\"string, lalala\"}}}";
		//if (reader.parse(test_string, setting.server_ext_))
		//{
		std::string json_msg = nim::Talk::CreateTextMessage(msg.receiver_accid_, msg.session_type_, msg.type_, msg.client_msg_id_, msg.content_, setting, msg.timetag_);
		nim::Talk::SendMsg(json_msg);
		//}
	}

	void SessionBox::SendImage( const std::wstring &src )
	{
		nim::IMMessage msg;
		PackageMsg(msg);
		msg.type_ = nim::kNIMMessageTypeImage;

		std::wstring filename = nbase::UTF8ToUTF16(msg.client_msg_id_);
		std::wstring dest = GetUserImagePath();
		if (!nbase::FilePathIsExist(dest, true))
			nbase::CreateDirectory(dest);

		dest += filename;
		GenerateUploadImage(src, dest);
		msg.local_res_path_ = nbase::UTF16ToUTF8(dest);

		nim::IMImage img;
		img.md5_ = GetFileMD5(dest);
		img.size_ = (long)nbase::GetFileSize(dest);

		Gdiplus::Image image(dest.c_str());
		if (image.GetLastStatus() != Gdiplus::Ok)
		{
			assert(0);
		}
		else
		{
			img.width_ = image.GetWidth();
			img.height_ = image.GetHeight();
		}

		msg.attach_ = img.ToJsonString();

		AddSendingMsg(msg);
		std::string json_msg = nim::Talk::CreateImageMessage(msg.receiver_accid_, msg.session_type_, msg.client_msg_id_, img, msg.local_res_path_, nim::MessageSetting(), msg.timetag_);
		nim::Talk::SendMsg(json_msg);
	}

	void SessionBox::SendSnapChat(const std::wstring &src)
	{
		auto weak_flag = this->GetWeakFlag();
		nim::IMMessage msg;
		PackageMsg(msg);
		msg.msg_setting_.server_history_saved_ = nim::BS_FALSE;
		msg.msg_setting_.roaming_ = nim::BS_FALSE;
		msg.msg_setting_.self_sync_ = nim::BS_FALSE;
		//TODO(litianyi)
		std::wstring filename = nbase::UTF8ToUTF16(msg.client_msg_id_);
		std::wstring dest = GetUserImagePath() + filename;
		GenerateUploadImage(src, dest);
		msg.local_res_path_ = nbase::UTF16ToUTF8(dest);

		nim::NOS::UploadResource(msg.local_res_path_, [this, msg, weak_flag](nim::NIMResCode res_code, const std::string& url) {
			if (!weak_flag.expired() && res_code == nim::kNIMResSuccess)
			{
				nim::IMMessage new_msg = msg;
				std::string md5 = GetFileMD5(nbase::UTF8ToUTF16(new_msg.local_res_path_));
				int file_size = (int)nbase::GetFileSize(nbase::UTF8ToUTF16(new_msg.local_res_path_));
				new_msg.type_ = nim::kNIMMessageTypeCustom;
				Json::Value json;
				Json::Value json_data;
				Json::FastWriter writer;
				json_data["size"] = file_size;
				json_data["md5"] = md5;
				json_data["url"] = url;
				json["type"] = CustomMsgType_SnapChat;
				json["data"] = json_data;
				new_msg.content_ = nbase::UTF16ToUTF8(L"阅后即焚");
				new_msg.attach_ = writer.write(json);
				AddSendingMsg(new_msg);

				nim::Talk::SendMsg(new_msg.ToJsonString(true));
			}
		});
	}

	void SessionBox::SendFile(const std::wstring &src)
	{
		nim::IMMessage msg;
		PackageMsg(msg);
		msg.type_ = nim::kNIMMessageTypeFile;
		msg.local_res_path_ = nbase::UTF16ToUTF8(src);

		nim::IMFile file;
		file.md5_ = GetFileMD5(src);
		file.size_ = (long)nbase::GetFileSize(src);

		nbase::PathString file_name;
		nbase::FilePathApartFileName(src, file_name);
		std::wstring file_exten;
		nbase::FilePathExtension(file_name, file_exten);
		file.display_name_ = nbase::UTF16ToUTF8(file_name);
		file.file_extension_ = nbase::UTF16ToUTF8(file_exten);
		msg.attach_ = file.ToJsonString();

		AddSendingMsg(msg);

		nim::Talk::FileUpPrgCallback* cb_pointer = nullptr;
		MsgBubbleFile* bubble = dynamic_cast<MsgBubbleFile*>(msg_list_->FindSubControl(nbase::UTF8ToUTF16(msg.client_msg_id_)));
		if (!msg.local_res_path_.empty() && nbase::FilePathIsExist(nbase::UTF8ToUTF16(msg.local_res_path_), false) && bubble)
		{
			cb_pointer = new nim::Talk::FileUpPrgCallback(bubble->GetFileUpPrgCallback());
			SessionManager::GetInstance()->AddFileUpProgressCb(msg.client_msg_id_, cb_pointer);
		}
		std::string json_msg = nim::Talk::CreateFileMessage(msg.receiver_accid_, msg.session_type_, msg.client_msg_id_, file, msg.local_res_path_, nim::MessageSetting(), msg.timetag_);
		nim::Talk::SendMsg(json_msg, msg.client_msg_id_, cb_pointer);
	}

	void SessionBox::SendJsb( const std::string &attach )
	{
		nim::IMMessage msg;
		PackageMsg(msg);
		msg.type_ = nim::kNIMMessageTypeCustom;

		msg.content_ = nbase::UTF16ToUTF8(L"这是一个猜拳");
		msg.attach_ = attach;

		AddSendingMsg(msg);

		nim::Talk::SendMsg(msg.ToJsonString(true));
	}

	void SessionBox::SendSticker(const std::string &catalog, const std::string &name)
	{
		nim::IMMessage msg;
		PackageMsg(msg);
		msg.type_ = nim::kNIMMessageTypeCustom;

		Json::Value json;
		Json::FastWriter writer;
		json["type"] = CustomMsgType_Sticker;
		json["data"]["catalog"] = catalog;
		json["data"]["chartlet"] = name;

		msg.content_ = nbase::UTF16ToUTF8(L"贴图");
		msg.attach_ = writer.write(json);

		AddSendingMsg(msg);

		nim::Talk::SendMsg(msg.ToJsonString(true));
	}

	void SessionBox::SendTip(const std::wstring &tip)
	{
		nim::IMMessage msg;
		PackageMsg(msg);
		msg.type_ = nim::kNIMMessageTypeTips;
		msg.content_ = nbase::UTF16ToUTF8(tip);
		msg.msg_setting_.need_push_ = nim::BS_FALSE;
		msg.status_ = nim::kNIMMsgLogStatusSent;

		AddSendingMsg(msg);
		nim::Talk::SendMsg(msg.ToJsonString(true));
	}

	void SessionBox::SendSystemTip(const std::wstring &tip)
	{
		nim::IMMessage msg;
		msg.session_type_ = nim::kNIMSessionTypeTeam;
		msg.receiver_accid_ = nim_comp::LoginManager::GetInstance()->GetImId();
		msg.sender_accid_ = LoginManager::GetInstance()->GetAccount();
		msg.client_msg_id_ = QString::GetGUID();
		msg.msg_setting_.resend_flag_ = nim::BS_FALSE;

		//base获取的时间单位是s，服务器的时间单位是ms
		msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();

		msg.status_ = nim::kNIMMsgLogStatusSending;
		msg.type_ = nim::kNIMMessageTypeTips;
		msg.content_ = nbase::UTF16ToUTF8(tip);
		msg.msg_setting_.need_push_ = nim::BS_FALSE;
		msg.status_ = nim::kNIMMsgLogStatusSent;

		//	AddSendingMsg(msg);
		nim::Talk::SendMsg(msg.ToJsonString(true));
	}

	void SessionBox::AddSendingMsg(const nim::IMMessage &msg)
	{
		writing_time_ = 0;
		RemoveTip(STT_WRITING);

		bool show_time = false;
		if(last_msg_time_ == 0)
		{
			show_time = true;
			farst_msg_time_ = msg.timetag_;
		}
		else
		{
			show_time = CheckIfShowTime(last_msg_time_, msg.timetag_);
		}

		if (!IsNoticeMsg(msg))
			last_msg_time_ = msg.timetag_;

		MsgBubbleItem* item = ShowMsg(msg, false, show_time);
		msg_list_->EndDown(true, false);
	}

	void SessionBox::ReSendMsg(nim::IMMessage &msg)
	{
		msg.msg_setting_.resend_flag_ = nim::BS_TRUE;
		msg.status_ = nim::kNIMMsgLogStatusSending;
		msg.timetag_= 1000 * nbase::Time::Now().ToTimeT();

		AddSendingMsg(msg);

		nim::Talk::SendMsg(msg.ToJsonString(true));
	}

	void SessionBox::PackageMsg(nim::IMMessage &msg)
	{
		msg.session_type_		=		session_type_;
		//	msg.receiver_accid_		=		session_id_;
		if (strcmp(session_id_.c_str(), LoginManager::GetInstance()->GetUserId().c_str()) != 0)
			msg.receiver_accid_	=		UserService::GetInstance()->GetFriendnetEaseId(session_id_);
		else 
			msg.receiver_accid_ =		LoginManager::GetInstance()->GetAccount();
		msg.sender_accid_		=		LoginManager::GetInstance()->GetAccount();
		msg.client_msg_id_		=		QString::GetGUID();
		msg.msg_setting_.resend_flag_ = nim::BS_FALSE;

		//base获取的时间单位是s，服务器的时间单位是ms
		msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();

		msg.status_ = nim::kNIMMsgLogStatusSending;
	}

	void SessionBox::MsgClear()
	{
		for (int i = msg_list_->GetCount(); i > 0; i--)
		{
			msg_list_->Remove(dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i - 1)));
		}
	}

	void SessionBox::InvokeShowMsgs(bool first_show_msg)
	{
		first_show_msg_ = first_show_msg;

		QLOG_APP(L"query begin: id={0} type={1} farst_time={2}") <<session_id_ <<session_type_ <<farst_msg_time_;

		nim::MsgLog::QueryMsgAsync(session_id_, session_type_, kMsgLogNumberShow, farst_msg_time_, 
			nbase::Bind(&TalkCallback::OnQueryMsgCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}

	void SessionBox::ShowMsgs(const std::vector<nim::IMMessage> &msg)
	{
		int pos = msg_list_->GetScrollRange().cy - msg_list_->GetScrollPos().cy;

		bool show_time = false;
		//msg倒序排列
		size_t len = msg.size();
		for(size_t i = 0; i < len; i++)
		{
			if(len == 1 || i == len-1)
			{
				show_time = true;
			}
			else
			{
				long long older_time = 0;
				for (size_t j = i + 1; j < len; j++)
				{
					if (!IsNoticeMsg(msg[j]))
					{
						older_time = msg[j].timetag_;
						break;
					}
				}
				show_time = CheckIfShowTime(older_time, msg[i].timetag_);
			}
			ShowMsg(msg[i], true, show_time);
		}
		//加载更多历史消息
		AddTip(STT_LOAD_MORE_MSG);
		if(len < kMsgLogNumberShow)
		{
			Box* box = (Box*) msg_list_->FindSubControl(CELL_LOAD_MORE_MSG);
			assert(box);
			Button* btn = (Button*) box->FindSubControl(CELL_BTN_LOAD_MORE_MSG);
			assert(btn);
			btn->SetText(L"已显示全部历史消息");
			btn->SetEnabled(false);
		}
		//修正最近时间
		if(first_show_msg_)
		{
			first_show_msg_ = false;

			msg_list_->EndDown(true, false);

			if (len > 0 && last_msg_time_ == 0)
			{
				for (const auto &i : msg)
				{
					if (!IsNoticeMsg(i))
					{
						last_msg_time_ = i.timetag_;
						break;
					}
				}
			}

			if (session_type_ == nim::kNIMSessionTypeP2P)
			{
				//检查回执
				CheckLastReceiptMsg();
				nim::IMMessage msg;
				if (GetLastNeedSendReceiptMsg(msg))
				{
					nim::MsgLog::SendReceiptAsync(msg.ToJsonString(false), [](const nim::MessageStatusChangedResult& res) {
						auto iter = res.results_.begin();
						QLOG_APP(L"mark receipt id:{0} time:{1} rescode:{2}") << iter->talk_id_ << iter->msg_timetag_ << res.rescode_;
					});
				}
			}
		}
		else
		{
			msg_list_->SetPos( msg_list_->GetPos() );

			dui::CSize sz = msg_list_->GetScrollPos();
			sz.cy = msg_list_->GetScrollRange().cy - pos - 50;
			msg_list_->SetScrollPos(sz);
		}
		//修正最远时间
		if(len > 0)
		{
			farst_msg_time_ = msg[len-1].timetag_;
		}
	}

	void SessionBox::OnSendMsgCallback(const std::string &cid, int code, __int64 msg_timetag)
	{
		IdBubblePair::iterator it = id_bubble_pair_.find(cid);
		if(it != id_bubble_pair_.end())
		{
			MsgBubbleItem* item = it->second;
			if(item)
			{
				if (code == nim::kNIMResSuccess)
				{
					item->SetMsgStatus(nim::kNIMMsgLogStatusSent);
					item->UpdateMsgTime(msg_timetag);
				}
				else if (code == nim::kNIMLocalResMsgFileNotExist)
					item->SetMsgStatus(nim::kNIMMsgLogStatusSendFailed);
				else if (code == nim::kNIMLocalResMsgNosUploadCancel)
					item->SetMsgStatus(nim::kNIMMsgLogStatusSendCancel);
				else
				{
					QLOG_WAR(L"unknown send msg callback code {0}") << code;
					item->SetMsgStatus(nim::kNIMMsgLogStatusSendFailed);
				}
			}
		}
	}

	void SessionBox::CheckLastReceiptMsg()
	{
		if (!last_receipt_msg_id_.empty())
			return;

		std::string my_id = LoginManager::GetInstance()->GetAccount();
		auto iter = cached_msgs_bubbles_.rbegin();
		for (; iter != cached_msgs_bubbles_.rend(); ++iter)
		{
			MsgBubbleItem* item = (MsgBubbleItem*)(*iter);
			if (item)
			{
				nim::IMMessage msg = item->GetMsg();
				if (msg.sender_accid_ == my_id && item->IsMyMsg() && msg.status_ == nim::kNIMMsgLogStatusReceipt)
				{
					item->SetMsgStatus(nim::kNIMMsgLogStatusReceipt);
					last_receipt_msg_id_ = msg.client_msg_id_;
					break;
				}
			}
		}
	}

	bool SessionBox::GetLastNeedSendReceiptMsg(nim::IMMessage &msg)
	{
		std::string my_id = LoginManager::GetInstance()->GetAccount();
		auto iter = cached_msgs_bubbles_.rbegin();
		for (; iter != cached_msgs_bubbles_.rend(); ++iter)
		{
			MsgBubbleItem* item = (MsgBubbleItem*)(*iter);
			if (item)
			{
				nim::IMMessage message = item->GetMsg();
				if (message.sender_accid_ != my_id || !item->IsMyMsg())
				{
					if (!nim::MsgLog::QueryMessageBeReaded(message))
					{
						msg = message;
						return true;
					}
					return false;
				}
			}
		}
		return false;
	}

	int SessionBox::RemoveMsgItem(const std::string& client_msg_id)
	{
		int index = -1;
		MsgBubbleItem* msg_item = NULL;
		MsgBubbleNotice* msg_cell = NULL;
		for (int i = 0; i < msg_list_->GetCount(); i++)
		{
			if (msg_list_->GetItemAt(i)->GetUTF8Name() == client_msg_id)
			{
				msg_item = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
				msg_cell = dynamic_cast<MsgBubbleNotice*>(msg_list_->GetItemAt(i));
				index = i;
				break;
			}
		}
		if (!msg_item)
		{
			if (msg_cell) //要删除的是一个通知消息，删除后直接return
				msg_list_->Remove(msg_cell);
			return index;
		}

		auto iter2 = std::find(cached_msgs_bubbles_.begin(), cached_msgs_bubbles_.end(), msg_item);
		if (iter2 != cached_msgs_bubbles_.end())
		{
			if (last_receipt_msg_id_ == client_msg_id) //本条要被删除的消息是显示已读的消息，则把上一条自己发的消息显示为已读。
			{
				auto iter3 = std::reverse_iterator<decltype(iter2)>(iter2); //iter3现在指向iter2的上一个元素
				std::string my_id = LoginManager::GetInstance()->GetAccount();
				while (iter3 != cached_msgs_bubbles_.rend())
				{
					MsgBubbleItem* item = dynamic_cast<MsgBubbleItem*>(*iter3);
					if (item)
					{
						nim::IMMessage msg = item->GetMsg();
						if (msg.sender_accid_ == my_id)
						{
							item->SetMsgStatus(nim::kNIMMsgLogStatusReceipt);
							last_receipt_msg_id_ = msg.client_msg_id_;
							break;
						}
					}
					iter3++;
				}
				if (iter3 == cached_msgs_bubbles_.rend())
					last_receipt_msg_id_ = "";
			}
			cached_msgs_bubbles_.erase(iter2); //从cached_msgs_bubbles_中删除
		}

		auto iter1 = id_bubble_pair_.find(client_msg_id);
		if (iter1 != id_bubble_pair_.end())
			id_bubble_pair_.erase(client_msg_id); //从id_bubble_pair_删除

		msg_list_->Remove(msg_item); //最后从msg_list_中删除并销毁该MsgBubbleItem

		return index;
	}

	void SessionBox::OnMsgStatusChangedCallback(const std::string &from_accid, const __int64 timetag, nim::NIMMsgLogStatus status)
	{
		if (!last_receipt_msg_id_.empty())
		{
			auto it = id_bubble_pair_.find(last_receipt_msg_id_);
			if (it != id_bubble_pair_.end())
			{
				MsgBubbleItem* item = it->second;
				if (item)
					item->SetMsgStatus(nim::kNIMMsgLogStatusSent);
			}
		}

		std::string my_id = LoginManager::GetInstance()->GetAccount();
		auto iter = cached_msgs_bubbles_.rbegin();
		for (; iter != cached_msgs_bubbles_.rend(); ++iter)
		{
			MsgBubbleItem* item = (MsgBubbleItem*)(*iter);
			if (item)
			{
				nim::IMMessage msg = item->GetMsg();
				if (msg.sender_accid_ == my_id && item->IsMyMsg() &&/* msg.timetag_ <= timetag*/nim::MsgLog::QueryMessageBeReaded(msg))
				{
					item->SetMsgStatus(nim::kNIMMsgLogStatusReceipt);
					last_receipt_msg_id_ = msg.client_msg_id_;
					break;
				}
			}
		}
	}

	void SessionBox::OnSnapchatRead(const std::string& client_msg_id)
	{
		RemoveMsgItem(client_msg_id);
	}

	void SessionBox::OnDownloadCallback( const std::string &cid, bool success )
	{
		IdBubblePair::iterator it = id_bubble_pair_.find(cid);
		if(it != id_bubble_pair_.end())
		{
			MsgBubbleItem* item = it->second;
			if(item)
			{
				item->OnDownloadCallback(success);
			}
		}
	}

	void SessionBox::OnRetweetResDownloadCallback(nim::NIMResCode code, const std::string& file_path, const std::string& sid, const std::string& cid)
	{
		IdBubblePair::iterator it = id_bubble_pair_.find(cid);
		if (it != id_bubble_pair_.end())
		{
			MsgBubbleItem* item = it->second;
			if (item)
			{
				item->OnDownloadCallback(code == nim::kNIMResSuccess);
			}
		}
	}

	void SessionBox::OnRelink( const Json::Value &json )
	{
		bool link = json["link"].asBool();
		if(link)
		{
			RemoveTip(STT_LINK);
		}
	}

	void SessionBox::ShowCustomMsgForm()
	{
#if 0
		CustomMsgForm* f = WindowsManager::SingletonShow<CustomMsgForm>(CustomMsgForm::kClassName);
		f->SetSession(session_id_, session_type_, label_title_->GetText());
#endif
	}

	void SessionBox::CallbackWriteMsglog(const std::wstring &notify_text, nim::NIMResCode res_code, const std::string& msg_id)
	{
		if (res_code == nim::kNIMResSuccess)
		{
			MsgBubbleNotice* cell = new MsgBubbleNotice;
			GlobalManager::FillBoxWithCache(cell, L"session/cell_notice.xml");
			msg_list_->Add(cell);
			cell->InitControl();
			cell->InitCustomInfo(notify_text, session_id_, msg_id);
		}
	}

	void SessionBox::RecallMsg(nim::NIMResCode code, const nim::RecallMsgNotify &notify)
	{
		if (code != nim::kNIMResSuccess)
		{
			std::wstring toast;
			if (code == nim::kNIMResExceedLimit)
			{
				toast = nbase::StringPrintf(L"撤回失败, 原因:%s", L"消息撤回时间超限");
			}
			else
			{
				toast = nbase::StringPrintf(L"recall msg error, code:%d, id:%s", code, nbase::UTF8ToUTF16(notify.msg_id_).c_str());
			}

			nim_ui::ShowToast(toast, 5000, nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND());
			return;
		}
		int index = RemoveMsgItem(notify.msg_id_);
		if (index > -1 && notify.msglog_exist_)			//撤回本地不存在的消息的通知不在消息流中插入通知
		{
			std::wstring notify_text;
			if (notify.from_id_ == LoginManager::GetInstance()->GetAccount())
			{
				notify_text = L"我撤回了一条消息";
			}
			else
			{
				if (notify.session_type_ == nim::kNIMSessionTypeP2P)
				{
					notify_text = L"对方撤回了一条消息";
				}
				else
				{
					auto info = GetTeamMemberInfo(notify.from_id_);
					UTF8String name = info.GetNick();
					if (name.empty())
					{
						nim::UserNameCard name_card;
						UserService::GetInstance()->GetUserInfo(notify.from_id_, name_card);
						name = name_card.GetName();
					}
					if (name.empty())
						name = notify.from_id_;
					notify_text = nbase::UTF8ToUTF16(name) + L" 撤回了一条消息";
				}
			}

			nim::IMMessage msg;
			msg.timetag_ = notify.notify_timetag_;
			msg.client_msg_id_ = QString::GetGUID();
			msg.receiver_accid_ = session_id_;
			msg.session_type_ = session_type_;
			msg.sender_accid_ = notify.from_id_;
			msg.status_ = nim::kNIMMsgLogStatusSent;
			msg.type_ = nim::kNIMMessageTypeText;
			Json::Value values;
			values["comment"] = "is_recall_notification";
			values["notify_from"] = notify.from_id_;
			msg.attach_ = values.toStyledString();
			msg.content_ = nbase::UTF16ToUTF8(notify_text);
			msg.msg_setting_.push_need_badge_ = nim::BS_FALSE; //设置会话列表不需要计入未读数
			nim::MsgLog::WriteMsglogToLocalAsync(session_id_, msg, true, nbase::Bind(&SessionBox::CallbackWriteMsglog, this, notify_text, std::placeholders::_1, std::placeholders::_2));
		}
	}

	void SessionBox::GetImageListInfo(ImageViewManager::ImageViewInfoList *info_list)
	{
		int size = msg_list_->GetCount();
		if (size <= 0)
		{
			return;
		}

		ImageViewManager::ImageViewInfo info;
		for (int i = 0; i < size; i++)
		{
			MsgBubbleImage *image_item = dynamic_cast<MsgBubbleImage *>(msg_list_->GetItemAt(i));
			if (image_item)
			{
				nim::IMMessage msg = image_item->GetMsg();
				info.id_ = nbase::UTF8ToUTF16(msg.client_msg_id_);
				info.path_ = image_item->GetImageFilePath();

				info_list->push_back(info);
			}
		}
	}
#endif
	bool SessionBox::CheckDropEnable(POINTL pt)
	{
		RECT rect = input_edit_->GetPos();
		ClientToScreen(m_pManager->GetPaintWindow(), (LPPOINT)&rect);
		ClientToScreen(m_pManager->GetPaintWindow(), ((LPPOINT)&rect) + 1);
		if (pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom)
			return true;
		else
			return false;
	}

	bool SessionBox::CheckFileSize(const std::wstring &src)
	{
		int64_t sz = nbase::GetFileSize(src);
		if (sz > LoginManager::GetInstance()->GetFileSizeLimit() * 1024 * 1024 || sz <= 0)
		{
			return false;
		}
		return true;
	}

	void SessionBox::OnDropFile(HDROP hDrop)
	{
		const int MAX_PATH_SIZE = 1024;

		TCHAR buff[MAX_PATH_SIZE] = { 0 };
		int iFile = ::DragQueryFile(hDrop, 0xFFFFFFFF, buff, MAX_PATH_SIZE);
		bool large_file = false;
		for (int i = 0; i < iFile; i++)
		{
			int len = ::DragQueryFile(hDrop, i, buff, MAX_PATH_SIZE);
			
			if (!nbase::FilePathIsExist(buff, false))
			{
				ShowMsgBox(NULL, MsgboxCallback(), _T("文件不存在"));
				continue;
			}

			if (CheckFileSize(buff))
				InsertFileToEdit(input_edit_, buff);
			else
				large_file = true;
			
			if (large_file)
				ShowMsgBox(NULL, MsgboxCallback(), L"STRID_SESSION_SUPPORT_15MB");
		}
	}

}//namespace nim_comp
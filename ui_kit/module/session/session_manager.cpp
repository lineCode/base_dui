#include "stdafx.h"
#include <fstream>
#include "session_manager.h"

namespace nim_comp
{
	SessionManager::SessionManager()
	{

	}
	SessionManager::~SessionManager()
	{

	}

	void SessionManager::DoLoadSession(const QuerySessionListCallabck& cb)
	{
		QuerySessionListCallabck *pcb = new QuerySessionListCallabck(cb);
		StdClosure task = std::bind(&SessionManager::DoLoadSessionAsyn, this, pcb);
		shared::Post2GlobalMisc(task);
	}

	void SessionManager::DoLoadSessionAsyn(const QuerySessionListCallabck* pcb)
	{
		printf("SessionManager::DoLoadSessionAsyn in thread GlobalMisc\n");

		if (pcb)
		{
			if (*pcb)
			{
				Json::Reader reader;
				Json::Value root, value;
				std::string file, document;
				nbase::win32::UnicodeToMBCS(QPath::GetAppPath(), file);
				file += "other/sessionlist.txt";
				ifstream is(file.c_str());

				if (is && reader.parse(is, root) && root.isObject() && root["code"].isInt())
				{
					if (root["code"] != 120000)
					{
						printf("root[\"code\"] != 120000\n");
						goto faile;
					}
					if (!root["object"].isObject() || !root["object"]["sessionList"].isArray())
					{
						printf("Json::Reader parse, data error\n");
						goto faile;
					}
					value = root["object"]["sessionList"];
					int len = value.size();
					SessionDataList session_list;
					std::list<SessionData> list;
					SessionData data;
					for (int i = 0; i < len; i++)
					{
						data.msg_sender_accid_ = (value[i]["netEaseId"].asString());
						data.msg_content_ = (value[i]["userPhoto"].asString());
						
						data.msg_timetag_ = (value[i]["timetag"].asInt());
						//data.SetMobile(value[i]["friendMobile"].asString());
						list.push_back(data);
					}
					session_list.count_ = list.size();
					session_list.sessions_ = list;
					shared::Post2UI(std::bind(*pcb, session_list.unread_count_, session_list));
				}
				else
				{
					printf("Json::Reader parse error\n");
					goto faile;
				}

			faile:
				is.close();
			}
			delete pcb;
		}
		return;
	}
}
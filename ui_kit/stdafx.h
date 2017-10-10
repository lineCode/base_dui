// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ

#include <Windows.h>
#include <ctime>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <set>
#include <map>
#include <memory>
#include <functional>
#include <atlbase.h>
#include <WTypes.h>
#include <shellapi.h>

#include "kit_define.h"

//base header
#include "base/callback/callback.h"
#include "base/util/string_util.h"
#include "base/util/unicode.h"
#include "base/util/string_number_conversions.h"
#include "base/memory/deleter.h"
#include "base/memory/singleton.h"
#include "base/win32/platform_string_util.h"
#include "base/win32/path_util.h"
#include "base/win32/win_util.h"
#include "base/thread/thread_manager.h"
#include "base/macros.h"
#include "base/base_types.h"
#include "base/file/file_util.h"
#include "base/time/time.h"
#include "base/framework/task.h"

//third_party
#include "duilib/UIlib.h"

//json
//#include "jsoncpp/include/json/json.h"

//xml
//#include "tinyxml/tinyxml.h"



//shared
//#include "shared/utf8_file_util.h"
//#include "shared/tool.h"
#include "shared/log.h"
#include "shared/util.h"
#include "shared/closure.h"

//msg
//#include "module/session/session_util.h"

//notify
//#include "module/service/notify_center.h"
//#include "module/service/user_service.h"
//#include "module/service/team_service.h"
//#include "module/service/mute_black_service.h"
//#include "module/service/http_service.h"
//#include "module/service/photo_service.h"

//
//#include "gui/window/window_ex.h"
//#include "module/window/windows_manager.h"



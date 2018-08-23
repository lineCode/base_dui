// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include <SDKDDKVer.h>

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

#include "duilib/UIlib.h"

//ffmpeg
extern "C" {
#include "libavformat/avformat.h"

#include "libavcodec/avcodec.h"

#include "libavutil/imgutils.h"
#include "libavutil/adler32.h"
#include "libavutil/opt.h"

#include "libswscale/swscale.h"
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

//nbase
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

//shared
#include "shared/log.h"
#include "shared/auto_unregister.h"


// TODO:  在此处引用程序需要的其他头文件

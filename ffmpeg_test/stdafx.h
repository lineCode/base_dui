// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include <SDKDDKVer.h>

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wingdi.h>

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


// TODO:  在此处引用程序需要的其他头文件

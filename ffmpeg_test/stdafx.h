// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
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


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

#pragma once
//#include "util.h"
#include "base/callback/callback.h"

namespace shared
{
	//Ͷ������UI�߳�
	void Post2UI(const StdClosure &closure);

	//Ͷ������ȫ��Misc�߳�
	void Post2GlobalMisc(const StdClosure &closure);

	//Ͷ������DB�߳�
	void Post2Database(const StdClosure &closure);

	//Ͷ������DB�߳�
	void Post2VideoPlay(const StdClosure &closure);
}
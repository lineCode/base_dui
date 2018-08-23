#include "stdafx.h"
#include "closure.h"
#include "threads.h"

namespace shared
{
	void Post2UI(const StdClosure &closure)
	{
		nbase::ThreadManager::PostTask(shared::kThreadUI, closure);
	}

	void Post2GlobalMisc(const StdClosure &closure)
	{
		nbase::ThreadManager::PostTask(shared::kThreadGlobalMisc, closure);
	}

	void Post2Database(const StdClosure &closure)
	{
		nbase::ThreadManager::PostTask(shared::kThreadDatabase, closure);
	}

	void Post2VideoPlay(const StdClosure &closure)
	{
		nbase::ThreadManager::PostTask(shared::kThreadVideoPlay, closure);
	}
}
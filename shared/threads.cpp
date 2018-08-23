#include "stdafx.h"
#include "threads.h"
#include "util.h"

namespace shared
{
	//-----------------------------------------MainThread-----------------------------------------
	void MainThreadBase::RegMainThreadEvent(MainThreadEventCallBack init_event, MainThreadEventCallBack end_event, MainThreadEventCallBack err_event)
	{
		init_event_cb_ = init_event;
		end_event_cb_ = end_event;
		err_event_cb_ = err_event;
	}

	void MainThreadBase::Init()
	{
		nbase::ThreadManager::RegisterThread(shared::kThreadUI);
		PreMessageLoop();

		std::wstring theme_dir = QPath::GetAppPath();

		if (init_event_cb_)
		{
			init_event_cb_();
		}
	}

	void MainThreadBase::Cleanup()
	{
		PostMessageLoop();
		SetThreadWasQuitProperly(true);
		nbase::ThreadManager::UnregisterThread();
	}
}
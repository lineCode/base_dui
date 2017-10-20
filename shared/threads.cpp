#include "stdafx.h"
#include "threads.h"
#include "util.h"

namespace shared
{
	//-----------------------------------------MainThread-----------------------------------------
	void MainThread::RegMainThreadEvent(MainThreadEventCallBack init_event, MainThreadEventCallBack end_event, MainThreadEventCallBack err_event)
	{
		/*if (init_event)
		{
			init_event_cb_.reset(new MainThreadEventCallBack(init_event));
		}
		if (end_event)
		{
			end_event_cb_.reset(new MainThreadEventCallBack(end_event));
		}
		if (err_event)
		{
			err_event_cb_.reset(new MainThreadEventCallBack(err_event));
		}*/
		init_event_cb_ = init_event;
		end_event_cb_ = end_event;
		err_event_cb_ = err_event;
	}

	void MainThread::Init()
	{
		nbase::ThreadManager::RegisterThread(shared::kThreadUI);
		PreMessageLoop();

		std::wstring theme_dir = QPath::GetAppPath();
#if 1
		if (init_event_cb_)
		{
			init_event_cb_();
		}
#else
		dui::GlobalManager::Startup(theme_dir + L"themes\\default", dui::CreateControlCallback());

		nim_ui::UserConfig::GetInstance()->SetIcon(IDI_ICON);

		std::wstring app_crash = QCommand::Get(kCmdAppCrash);
		if (app_crash.empty())
		{
			nim_ui::WindowsManager::SingletonShow<LoginForm>(LoginForm::kClassName);
		}
		else
		{
			std::wstring content(L"程序崩溃了，崩溃日志：");
			content.append(app_crash);

			MsgboxCallback cb = nbase::Bind(&MainThread::OnMsgBoxCallback, this, std::placeholders::_1);
			ShowMsgBox(NULL, content, cb, L"提示", L"打开", L"取消");
		}
#endif
	}

	void MainThread::Cleanup()
	{
		//dui::GlobalManager::Shutdown();

		PostMessageLoop();
		SetThreadWasQuitProperly(true);
		nbase::ThreadManager::UnregisterThread();
#if 0
		nim_chatroom::ChatRoom::Cleanup();
#endif
	}

	void MainThread::PreMessageLoop()
	{
		misc_thread_.reset(new MiscThread(shared::kThreadGlobalMisc, "Global Misc Thread"));
		misc_thread_->Start();

		screen_capture_thread_.reset(new MiscThread(shared::kThreadScreenCapture, "screen capture"));
		screen_capture_thread_->Start();

		db_thread_.reset(new DBThread(shared::kThreadDatabase, "Database Thread"));
		db_thread_->Start();
	}

	void MainThread::PostMessageLoop()
	{
		misc_thread_->Stop();
		misc_thread_.reset(NULL);

		screen_capture_thread_->Stop();
		screen_capture_thread_.reset(NULL);

		db_thread_->Stop();
		db_thread_.reset(NULL);
	}
#if MODE_COMMENT
	void MainThread::OnMsgBoxCallback(MsgBoxRet ret)
	{
		if (ret == MB_YES)
		{
#if 0
			std::wstring dir = QPath::GetNimAppDataDir();
			QCommand::AppStartWidthCommand(dir, L"");
#endif
		}

		nim_comp::WindowsManager::SingletonShow<LoginForm>(LoginForm::kClassName);
	}
#endif
}
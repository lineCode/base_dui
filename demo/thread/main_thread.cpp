#include "stdafx.h"
#include "main_thread.h"


void MainThread::PreMessageLoop()
{
	misc_thread_.reset(new shared::MiscThread(shared::kThreadGlobalMisc, "Global Misc Thread"));
	misc_thread_->Start();

	screen_capture_thread_.reset(new shared::MiscThread(shared::kThreadScreenCapture, "screen capture"));
	screen_capture_thread_->Start();

	db_thread_.reset(new shared::DBThread(shared::kThreadDatabase, "Database Thread"));
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
#include "stdafx.h"
#include "main_thread.h"


void MainThread::PreMessageLoop()
{
	misc_thread_.reset(new shared::MiscThread(shared::kThreadGlobalMisc, "Global Misc Thread"));
	misc_thread_->Start();

	db_thread_.reset(new shared::DBThread(shared::kThreadDatabase, "Database Thread"));
	db_thread_->Start();

	video_play_thread_.reset(new shared::VideoThread(shared::kThreadVideoPlay, "video play"));
	video_play_thread_->Start();
}

void MainThread::PostMessageLoop()
{
	misc_thread_->Stop();
	misc_thread_.reset(NULL);

	db_thread_->Stop();
	db_thread_.reset(NULL);

	video_play_thread_->Stop();
	video_play_thread_.reset(NULL);
}
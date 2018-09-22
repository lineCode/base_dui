#include "stdafx.h"
#include "media_player.h"
#include "../shared/closure.h"
//#include "../extra/buf.h"

MediaPlayer::MediaPlayer() { 
};

void MediaPlayer::Init()
{
    m_pScreen = static_cast<Screen*>(FindSubControl(_T("screen")));
    assert(m_pScreen);
    m_pScreen->SetMediaPlayer(this);
    m_pBoxSlider = static_cast<dui::VBox*>(FindSubControl(_T("vbox_slider")));
    m_pSlider = static_cast<dui::Slider*>(FindSubControl(_T("slider")));
    m_pLbTime = static_cast<dui::Label*>(FindSubControl(_T("lb_video_time")));
}

void MediaPlayer::StartVideo(String file)
{
#ifdef _UNICODE
	std::string file_;
	nbase::win32::UnicodeToMBCS(file, file_);
#else
	std::string file_ = file;
#endif
	if (m_play_status == PS_PAUSE)
	{
		if (m_curr_file == file_)
		{
			m_play_status = PS_PLAY;
		}
	}
	else if (m_play_status == PS_PLAY)
	{
		if (m_curr_file != file_)
		{
			m_play_status = PS_STOP;
			StdClosure task = std::bind(&MediaPlayer::StartPlayCallback, this, file_);
			shared::Post2VideoPlay(task);
		}
	}
	else
	{
		StdClosure task = std::bind(&MediaPlayer::StartPlayCallback, this, file_);
		shared::Post2VideoPlay(task);
	}
}

void MediaPlayer::StartRtsp(std::string url)
{
    if (m_play_status == PS_PAUSE)
    {
        if (m_curr_file == url)
        {
            m_play_status = PS_PLAY;
        }
    }
    else if (m_play_status == PS_PLAY)
    {
        if (m_curr_file != url)
        {
            m_play_status = PS_STOP;
            StdClosure task = std::bind(&MediaPlayer::StartPlayRtspCallback, this, url);
            shared::Post2VideoPlay(task);
        }
    }
    else
    {
        StdClosure task = std::bind(&MediaPlayer::StartPlayRtspCallback, this, url);
        shared::Post2VideoPlay(task);
    }
}

void MediaPlayer::PauseOrStartVideo()
{
	if (m_play_status == PS_PLAY)
		m_play_status = PS_PAUSE;
	else if (m_play_status == PS_PAUSE)
		m_play_status = PS_PLAY;
}

void MediaPlayer::StopVideo(bool exit)
{	
	m_play_status = PS_STOP;
	m_curr_file.clear();

	if (m_diCurrFrame.bLoaded)
	{
		if (m_diCurrFrame.pImageInfo)
		{
			assert(m_diCurrFrame.pImageInfo->hBitmap);
			::DeleteObject(m_diCurrFrame.pImageInfo->hBitmap);
		}
		delete m_diCurrFrame.pImageInfo;
		m_diCurrFrame.pImageInfo = nullptr;
		m_diCurrFrame.bLoaded = false;
	}
	while (!m_bStoped)
	{
		printf("MediaPlayer::StopVideo() m_bStoped = false\n");
		Sleep(20);
	}
	//可能有BitbltCallback任务放在ui队列最后，所以不能直接Invalidate()，但是程序关闭会引起崩溃
	if (!exit)
	{
		shared::Post2UI(std::bind([this]{
			Invalidate();
		}));
	}
}
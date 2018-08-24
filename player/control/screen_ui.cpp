#include "stdafx.h"
#include "screen.h"
#include "../shared/closure.h"

bool Screen::Paint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
{
	printf("Screen::Paint\n");
	if (pStopControl == this) return false;
	if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) return true;
	
	if (m_play_status == PS_STOP /*|| m_play_status == PS_END*/)
	{
		PaintBkColor(hDC);
	}
	else
	{
		assert(m_diCurrFrame.bLoaded);
		Render::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, m_diCurrFrame);
	}
	
	//PaintBkImage(hDC);
	//PaintStatusImage(hDC);
	//PaintText(hDC);
	//PaintBorder(hDC);

	return true;
}

void Screen::StartVideo(String file)
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
			StdClosure task = std::bind(&Screen::StartPlayCallback, this, file_);
			shared::Post2VideoPlay(task);
		}
	}
	else
	{
		StdClosure task = std::bind(&Screen::StartPlayCallback, this, file_);
		shared::Post2VideoPlay(task);
	}
}

void Screen::PauseOrStartVideo()
{
	if (m_play_status == PS_PLAY)
		m_play_status = PS_PAUSE;
	else if (m_play_status == PS_PAUSE)
		m_play_status = PS_PLAY;
}

void Screen::StopVideo(bool exit)
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
		printf("Screen::StopVideo() m_bStoped = false\n");
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
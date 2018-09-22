#include "stdafx.h"
#include "media_player.h"

MediaPlayer::Screen::Screen(){
    SetBkColor(0xff222222);
}

bool MediaPlayer::Screen::Paint(HDC hDC, const RECT& rcPaint, Control* pStopControl /*= NULL*/)
{
    printf("Screen::Paint\n");
    if (pStopControl == this) return false;
    if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) return true;

    if (!m_pMediaPlayer)
        return true;
    if (m_pMediaPlayer->m_play_status == PS_STOP /*|| m_play_status == PS_END*/)
    {
        PaintBkColor(hDC);
    }
    else
    {
        assert(m_pMediaPlayer->m_diCurrFrame.bLoaded);
        Render::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, m_pMediaPlayer->m_diCurrFrame);
    }

    return true;
}
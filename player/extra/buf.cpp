#include "stdafx.h"
#include "buf.h"
#include "../control/media_player.h"

TempBuf *g_buf = nullptr;

void TempBuf::afterGettingRtspFrame(char *buf, int size)
{
    if (belong_to_)
        belong_to_->afterGettingRtspFrame(buf, size);
}
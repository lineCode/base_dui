#pragma once
class MediaPlayer;
class TempBuf
{
public:
    TempBuf(MediaPlayer *belong_to) :belong_to_(belong_to){};
    ~TempBuf();
    static void afterGettingRtspFrame(char *buf, int size, void* param){
        TempBuf *pthis = static_cast<TempBuf*>(param);
        if (pthis)
            pthis->afterGettingRtspFrame(buf, size);
    }
protected:
    void afterGettingRtspFrame(char *buf, int size);
private:
    MediaPlayer *belong_to_;
};

extern TempBuf *g_buf;







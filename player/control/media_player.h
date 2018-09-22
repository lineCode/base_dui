#pragma once
//#include <atomic>

using namespace dui;

class MediaPlayer : public Box
{
public:
	enum PlayStatus
	{
		PS_STOP = 0,
		PS_PLAY,
        PS_LOADING,
		PS_PAUSE,
	};
public:
    MediaPlayer();
	~MediaPlayer(){ printf("~MediaPlayer()\n"); };

    virtual void Init() override;

    void SetPlayStatus(PlayStatus status){ if (m_play_status != status) m_play_status = status; };
    PlayStatus GetPlayStatus(){ m_play_status; }

	//播放业务逻辑
	void StartVideo(String file);
    void StartRtsp(std::string url);
	void PauseOrStartVideo();
	void StopVideo(bool exit = false);		//exit:是否是推出程序时的关闭

public:
    void StartPlayCallback(std::string file);
    void StartPlayRtspCallback(std::string url);
    void BitbltCallback(HBITMAP hbmp, int width, int height);
    void afterGettingRtspFrame(char *buf, int size);
public:
    class Screen : public Control
    {
    public:
        Screen();
        void SetMediaPlayer(MediaPlayer *player){ m_pMediaPlayer = player; };
        // // 返回要不要继续绘制
        virtual bool Paint(HDC hDC, const RECT& rcPaint, Control* pStopControl = NULL) override;
   
    friend class MediaPlayer;
    private:
        MediaPlayer *m_pMediaPlayer = nullptr;
    };
    friend class Screen;
private:
	bool m_bStoped = true;			//指定是否已推出video线程函数
	std::string	m_curr_file;

    volatile PlayStatus m_play_status = PS_STOP;
    DrawInfo	m_diCurrFrame;

    Screen      *m_pScreen = nullptr;
    dui::VBox   *m_pBoxSlider = nullptr;
    dui::Slider *m_pSlider = nullptr;
    dui::Label  *m_pLbTime = nullptr;
};




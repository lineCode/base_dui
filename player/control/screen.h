#pragma once
//#include <atomic>

using namespace dui;

class Screen : public Control
{
public:
	enum PlayStatus
	{
		PS_STOP = 0,
		PS_PLAY,
		PS_PAUSE,
	};

public:
	Screen() : m_play_status(PS_STOP){ SetBkColor(0xff2222222); };
	~Screen(){ printf("~Screen()\n"); };

	virtual bool Paint(HDC hDC, const RECT& rcPaint, Control* pStopControl = NULL) override; // 返回要不要继续绘制

	//播放业务逻辑
	void StartVideo(String file);
	void PauseOrStartVideo();
	void StopVideo(bool exit = false);		//exit:是否是推出程序时的关闭

	void StartPlayCallback(std::string file);
	void BitbltCallback(HBITMAP hbmp, int width, int height);
private:
	volatile PlayStatus m_play_status;
	bool m_bStoped = true;			//指定是否已推出video线程函数
	std::string	m_curr_file;

	DrawInfo	m_diCurrFrame;
};

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

	virtual bool Paint(HDC hDC, const RECT& rcPaint, Control* pStopControl = NULL) override; // ����Ҫ��Ҫ��������

	//����ҵ���߼�
	void StartVideo(String file);
	void PauseOrStartVideo();
	void StopVideo(bool exit = false);		//exit:�Ƿ����Ƴ�����ʱ�Ĺر�

	void StartPlayCallback(std::string file);
	void BitbltCallback(HBITMAP hbmp, int width, int height);
private:
	volatile PlayStatus m_play_status;
	bool m_bStoped = true;			//ָ���Ƿ����Ƴ�video�̺߳���
	std::string	m_curr_file;

	DrawInfo	m_diCurrFrame;
};

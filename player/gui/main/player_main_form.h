#pragma once

#include "gui/window/window_ex.h"
#include "control/media_player.h"

class PlayerMainForm : public nim_comp::WindowEx
{
public:
	PlayerMainForm();
	~PlayerMainForm();

	virtual std::wstring GetSkinFolder() override{ return _T("player"); };
	virtual std::wstring GetSkinFile() override{ return _T("player_main_form.xml"); };
	virtual LPCTSTR GetWindowClassName() const override{ return kClassName; };
	virtual LPCTSTR GetWindowId() const override{ return kClassName; };
	virtual UINT GetClassStyle() const override{ return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS); };
	virtual Control* CreateControl(LPCTSTR pstrClass) override{
		if (_tcscmp(pstrClass, _T("MediaPlayer")) == 0) return new MediaPlayer;
        else if (_tcscmp(pstrClass, _T("Screen")) == 0) return new MediaPlayer::Screen;
        else return nullptr;
	};

	virtual void Notify(dui::Event& msg) override;

    virtual void OnFinalMessage(HWND hWnd) override;

    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

    virtual void OnEsc(BOOL &bHandled) override;

	virtual void InitWindow() override;

	//托盘
	void TrayLeftClick();
	void TrayRightClick();
	void PopupTrayMenu(POINT point);

	bool MenuLogoffClick(dui::Event* param);
	bool MenuLogoutClick(dui::Event* param);


	//播放业务逻辑
	void StartVideo(String file);
	void PauseOrStartVideo();
	void StopVideo();	//exit:是否是推出程序时的关闭

public:
	static const LPCTSTR kClassName;

private:
	/* @return bool true 继续传递控件消息，false 停止传递控件消息*/
	virtual void OnClick(dui::Event& msg) override;

	void OnBtnOpenFileClicked();
	void BtnOpenFileClickedCallback(BOOL, std::wstring);

	void OnBtnOpenRTSPClicked();

private:
	MediaPlayer *m_media_player;
};
#pragma once

#include "gui/window/window_ex.h"
#include "control/screen.h"

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
		if (_tcscmp(pstrClass, _T("Screen")) == 0)
		{
			return new Screen;
		}
	};

	virtual void Notify(dui::Event& msg) override;

	/**
	* 处理窗口销毁消息
	* @return void	无返回值
	*/
	virtual void OnFinalMessage(HWND hWnd);

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void OnEsc(BOOL &bHandled);

	virtual void InitWindow() override;

	//托盘
	void TrayLeftClick();
	void TrayRightClick();
	void PopupTrayMenu(POINT point);

	bool MenuLogoffClick(dui::Event* param);
	bool MenuLogoutClick(dui::Event* param);


	//播放业务逻辑
	void StartPlay(String file);
	void PausePlay();
	void StopPlay();

public:
	static const LPCTSTR kClassName;

private:
	/* @return bool true 继续传递控件消息，false 停止传递控件消息*/
	virtual void OnClick(dui::Event& msg) override;

	void OnBtnOpenFileClicked();
	void BtnOpenFileClickedCallback(BOOL, std::wstring);

private:
	Screen *m_video_screen;
};
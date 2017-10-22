#pragma once
#include "gui/window/window_ex.h"

class LoginForm : public nim_comp::WindowEx
{
public:
	LoginForm();
	~LoginForm();
	
	virtual String GetSkinFolder() override;
	virtual String GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName() const override;
	virtual LPCTSTR GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	virtual void InitWindow() override;
	virtual void Notify(dui::TNotify& msg) override;

	virtual LRESULT OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled) override;
	

	virtual void OnClick(dui::TNotify& msg) override;

	/**
	* 注册UIKIT回调函数，让UIKIT控制登录窗体的一些行为
	* @return void	无返回值
	*/
	void RegLoginManagerCallback();

	/**
	* 响应登录结果的回调，
	* @return void	无返回值
	*/
	void OnLoginError(int error);

	/**
	* 响应取消登录的回调，重置界面控件效果
	* @return void	无返回值
	*/
	void OnCancelLogin();

protected:
	void StartLogin();
	void CancelLogin();
	void ShowMenu(bool show, POINT pt);

	bool ImportDataToDB();
public:
	static const LPCTSTR kClassName;


private:
	dui::RichEdit*	re_account_;
	dui::RichEdit*	re_pwd_;

	dui::Label*		label_info_;
	dui::Button*		btn_login_;
	dui::Button*		btn_cancel_login_;
	dui::CheckBtn*	chk_arrow_down_;
};

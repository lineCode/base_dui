#pragma once


class LoginForm : public DuiLib::WindowImplBase
{
public:
	LoginForm();
	~LoginForm();
	
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName() const override;
	virtual UINT GetClassStyle() const override;

	//window_ex
	//virtual std::wstring GetWindowId(void) const override{ return kClassName; };

	virtual void InitWindow() override;

	virtual LRESULT OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled) override;
	

	virtual void OnClick(DuiLib::TNotifyUI& msg) override;

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
public:
	static const LPCTSTR kClassName;


private:
	DuiLib::CRichEditUI*	re_account_;
	DuiLib::CRichEditUI*	re_pwd_;

	DuiLib::CLabelUI*		label_info_;
	DuiLib::CButtonUI*		btn_login_;
	DuiLib::CButtonUI*		btn_cancel_login_;
};

#pragma once
#include "gui/window/window_ex.h"

class LoginForm : public nim_comp::WindowEx
{
public:
	LoginForm();
	~LoginForm();
	
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName() const override;
	virtual LPCTSTR GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	virtual void InitWindow() override;

	virtual LRESULT OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled) override;
	

	virtual void OnClick(DuiLib::TNotifyUI& msg) override;

	/**
	* ע��UIKIT�ص���������UIKIT���Ƶ�¼�����һЩ��Ϊ
	* @return void	�޷���ֵ
	*/
	void RegLoginManagerCallback();

	/**
	* ��Ӧ��¼����Ļص���
	* @return void	�޷���ֵ
	*/
	void OnLoginError(int error);

	/**
	* ��Ӧȡ����¼�Ļص������ý���ؼ�Ч��
	* @return void	�޷���ֵ
	*/
	void OnCancelLogin();

protected:
	void StartLogin();
	void CancelLogin();
	void ShowMenu(bool show);
public:
	static const LPCTSTR kClassName;


private:
	DuiLib::CRichEditUI*	re_account_;
	DuiLib::CRichEditUI*	re_pwd_;

	DuiLib::CLabelUI*		label_info_;
	DuiLib::CButtonUI*		btn_login_;
	DuiLib::CButtonUI*		btn_cancel_login_;
	DuiLib::CCheckBoxUI*	chk_arrow_down_;
};

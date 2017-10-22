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

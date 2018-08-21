#pragma once
#include "gui/window/window_ex.h"
#include "module/user/user_manager.h"
#include "module/session/session_manager.h"

class MainForm : public nim_comp::WindowEx/*, public ITrayIconDelegate*/
{
public:
	MainForm();
	~MainForm();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName() const override;
	virtual LPCTSTR GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	virtual void Notify(dui::Event& msg) override;
	
	/**
	* ������������Ϣ
	* @return void	�޷���ֵ
	*/
	virtual void OnFinalMessage(HWND hWnd);

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void OnEsc(BOOL &bHandled);

	virtual void InitWindow() override;

private:
	/* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ*/
	virtual void OnClick(dui::Event& msg) override;

	void TrayLeftClick();
	void TrayRightClick();

	void OnGetAllFriendInfo(const std::list<nim_comp::UserNameCard>& list);
	void OnGetAllSessionInfo(int unread_count, const nim_comp::SessionDataList& data_list);
public:
	void PopupTrayMenu(POINT point);

	bool MainForm::MenuLogoffClick(dui::Event* param);
	bool MainForm::MenuLogoutClick(dui::Event* param);
public:
	static const LPCTSTR kClassName;

private:
	dui::Button		*btn_head_;
	dui::Label		*label_name_;
	dui::TabBox	*tab_session_friend_;

	dui::List		*list_friend_;
	dui::List		*list_session_;
	dui::Tree		*tree_friend_;
	dui::List		*list_menu_;
	
	dui::TreeNode	*group_nodes_[28];

	dui::TabBox	*tab_session_;
};

using namespace nbase;
DISABLE_RUNNABLE_METHOD_REFCOUNT(MainForm);

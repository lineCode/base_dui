#pragma once

//#include "gui/tray_icon/tray_icon.h"
//#include "shared/ui/ui_menu.h"
//#include "shared/ui/msgbox.h"
//#include "base/framework/task.h"
#include "gui/window/window_ex.h"

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
	bool OnClicked(dui::TNotify& msg);
#if 0
	/**
	* ���������ڵ����˵���ť�ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool MainMenuButtonClick(ui::EventArgs* param);

	/**
	* �������˵�
	* @param[in] point ��������
	* @return void	�޷���ֵ
	*/
	void PopupMainMenu(POINT point);

	/**
	* ����鿴��־Ŀ¼�˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool LookLogMenuItemClick(ui::EventArgs* param);

	/**
	* �����ҵ��ֻ��˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool FileTransMenuItemClick(ui::EventArgs* param);

	/**
	* ����ˢ��ͨѶ¼�˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool AddressMenuItemClick(ui::EventArgs* param);

	/**
	* �����������¼�˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool ExportMsglogMenuItemClick(ui::EventArgs* param);

	/**
	* �����������¼�˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool ImportMsglogMenuItemClick(ui::EventArgs* param);

	/**
	* ����������������¼�˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool ClearChatRecordMenuItemClick(bool del_session, ui::EventArgs* param);

	/**
	* ������ռ�¼�˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool ClearChatRecordBySessionTypeMenuItemClick(bool del_session, nim::NIMSessionType type, ui::EventArgs* param);

	/**
	* ��������Ƶ���ò˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool VChatSettingMenuItemClick(ui::EventArgs* param);

	/**
	* ����װ�طŲ˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool RtsReplayMenuItemClick(ui::EventArgs* param);

	/**
	* ��������DPI�˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool AdaptDpiMenuItemClick(ui::EventArgs* param);

	/**
	* ��ʾ�����б�
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool ShowLanguageList(ui::EventArgs* param);

	/**
	* �ر������б�
	* @param[in] msg ��Ϣ�������Ϣ
	* @param[in] check_mouse true: ��鵽����������б��ϾͲ��رգ�false����������λ�ã�ֱ�ӹر�
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool CloseLanguageList(ui::EventArgs* param, bool check_mouse);

	/**
	* ѡ������
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool OnSelectLanguage(ui::EventArgs* param);

	/**
	* ѡ�����ԵĻص�
	* @param[in] ret ѡ��İ�ť
	* @return void �޷���ֵ
	*/
	void OnSelectLanguageCallback(MsgBoxRet ret, const std::string& language);

	/**
	* ������ʾ�Ự�б�˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool SessionListMenuItemClick(ui::EventArgs* param);

	/**
	* ������ڲ˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool AboutMenuItemClick(ui::EventArgs* param);

	/**
	* ����ע���˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool LogoffMenuItemClick(ui::EventArgs* param);

	/**
	* �����˳��˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool QuitMenuItemClick(ui::EventArgs* param);

	/**
	* ����״̬�˵���ť�ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool OnlineStateMenuButtonClick(ui::EventArgs* param);

	/**
	* ����״̬�˵���ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	bool OnlineStateMenuItemClick(ui::EventArgs* param);

	/**
	* ������������������·�������״̬
	* @param[in] json ����������Ϣ
	* @return void	�޷���ֵ
	*/
	void CheckOnlineState(const Json::Value& json);

	/**
	* ��������״̬
	* @return void	�޷���ֵ
	*/
	void SetOnlineState();
#endif
public:
	/**
	* ʵ��ITrayIconDelegate�ӿں���
	* @return void	�޷���ֵ
	*/
	/*virtual void LeftClick() override;
	virtual void LeftDoubleClick() override;
	virtual void RightClick() override;*/

private:
	/**
	* �������������ݸı����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	//bool SearchEditChange(ui::EventArgs* param);

	/**
	* ����������������ť�ĵ�����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	//bool OnClearInputBtnClicked(ui::EventArgs* param);

	/**
	* �������̲˵�
	* @param[in] point ��������
	* @return void	�޷���ֵ
	*/
	void PopupTrayMenu(POINT point);
#if 0
private:
	/**
	* ��Ӧ�������ϸı�Ļص�����
	* @param[in] uinfos �µĸ��������б�
	* @return void �޷���ֵ
	*/
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);

	/**
	* ��Ӧͷ��������ɵĻص�����
	* @param[in] type ͷ������
	* @param[in] account ͷ��������ɵ��û�id
	* @param[in] photo_path ͷ�񱾵�·��
	* @return void �޷���ֵ
	*/
	void OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path);

	/**
	* ��Ӧͷ����Ự�б�δ����Ϣ�����ı�Ļص�����
	* @param[in] unread_count δ������
	* @return void �޷���ֵ
	*/
	void OnUnreadCountChange(int unread_count);

	/**
	* ��ʼ���û�ͷ��
	* @return void	�޷���ֵ
	*/
	void InitHeader();
#endif
public:
	static const LPCTSTR kClassName;

private:
	/*dui::Button*	btn_header_;
	dui::Label*		label_name_;
	dui::Button*	btn_online_state_;
	bool			is_busy_;

	dui::Box*		box_unread_;
	dui::Label*		label_unread_;

	dui::RichEdit*	search_edit_;
	dui::Button*	btn_clear_input_;
	dui::ListBox*	search_result_list_;
	bool			is_trayicon_left_double_clicked_;

	AutoUnregister	unregister_cb;*/
};

using namespace nbase;
DISABLE_RUNNABLE_METHOD_REFCOUNT(MainForm);

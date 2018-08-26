#pragma once

#include "module/db_manager/db_manager.h"
#include "gui/window/window_ex.h"

namespace nim_comp
{
	class DBManagerMainForm : public WindowEx, IDropTarget
	{
		friend class DBManager;
	private:
		DBManagerMainForm(std::string dbcid, std::wstring company, int64_t sqlbits);
		~DBManagerMainForm();

	private:
		//接口实现
		virtual std::wstring GetSkinFolder() override{ return L"db_manager"; };
		virtual std::wstring GetSkinFile() override{ return L"db_manager_mainform.xml"; };

		virtual LPCTSTR GetWindowClassName() const override{ return kClassName; };
		virtual LPCTSTR GetWindowId() const override{ return kClassName; };

		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		virtual void InitWindow() override;

		virtual void OnFinalMessage(HWND hWnd) override;
	private:
		bool OnClicked(dui::Event* args);

		bool OnBtnOKClieked(dui::Event* args);
		bool OnBtnCancelClieked(dui::Event* args);
		bool OnBtnRunClieked(dui::Event* args);
		bool OnComboSelected(dui::Event* args);

		void OnWndSizeMax(bool max);
		
	private:
		bool OnMySQLEvent(DBM_EVENT event, void *p);

		bool CheckDropEnable(POINTL pt);
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		ULONG STDMETHODCALLTYPE AddRef(void);
		ULONG STDMETHODCALLTYPE Release(void);
		HRESULT STDMETHODCALLTYPE DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
		HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
		HRESULT STDMETHODCALLTYPE DragLeave(void);
		HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);

	private:
		static const LPCTSTR kClassName;

	private:
		dui::Button		*btn_max_restore_;

		dui::Tree		*db_tree_;
		dui::ListView	*result_list_;
		dui::Label		*result_label_;
		dui::Combo		*combo_ip_;
		dui::Label		*info_label_;
		dui::Label		*status_label_;
		dui::Label		*oper_label_;

		dui::RichEdit	*re_input_;

		dui::Button		*btn_run_;

		std::string		dbcid_;
		std::wstring	company_name_;

		int			curr_combo_sel_;
		int64_t		sqlbits_;
	};
}
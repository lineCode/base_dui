#pragma once
//#include "window_ex.h"

enum MsgBoxRet
{
	MB_YES,
	MB_NO
};

typedef std::function<void(MsgBoxRet)> MsgboxCallback; 

class MsgBox : public dui::WindowImplBase	//WindowEx
{
public:
	friend void ShowMsgBox(HWND hParentWnd, MsgboxCallback cb,
		const std::wstring &content = L"", bool content_is_id = true,
		const std::wstring &title = L"STRING_TIPS", bool title_is_id = true,
		const std::wstring &yes = L"STRING_OK", bool btn_yes_is_id = true,
		const std::wstring &no = L"", bool btn_no_is_id = false);
public:
	MsgBox(MsgboxCallback cb = nullptr);
	virtual ~MsgBox();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual dui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnEsc(BOOL &bHandled);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LPCTSTR GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const /*override*/;
	virtual UINT GetClassStyle() const override;
	virtual void InitWindow() override;
	virtual void OnFinalMessage(HWND hWnd);
	
	virtual void OnClick(dui::TNotify& msg) override;
private:
	void SetTitle(const std::wstring &str);
	void SetContent(const std::wstring &str);
	void SetButton(const std::wstring &yes, const std::wstring &no);
	void Show();

	void Close(UINT nRet = IDOK);
	void EndMsgBox(MsgBoxRet ret);
public:
	static const LPCTSTR kClassName;
private:
	dui::Label*		title_;
	dui::RichEdit*	content_;
	dui::Button*		btn_yes_;
	dui::Button*		btn_no_;

	MsgboxCallback	 msgbox_callback_;
};

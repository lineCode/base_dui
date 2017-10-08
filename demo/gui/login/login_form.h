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
	
public:
	static const LPCTSTR kClassName;


private:
	DuiLib::CListUI *m_pList;
};

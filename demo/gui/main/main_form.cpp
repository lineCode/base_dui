#include "stdafx.h"
#include "main_form.h"

using namespace dui;
#if 0
bool MainForm::SearchEditChange(ui::EventArgs* param)
{
	UTF8String search_key = search_edit_->GetUTF8Text();
	bool has_serch_key = !search_key.empty();
	btn_clear_input_->SetVisible(has_serch_key);
	search_result_list_->SetVisible(has_serch_key);
	FindControl(L"no_search_result_tip")->SetVisible(has_serch_key);
	if (has_serch_key)
	{
		nim_ui::ContactsListManager::GetInstance()->FillSearchResultList(search_result_list_, search_key);
		FindControl(L"no_search_result_tip")->SetVisible(search_result_list_->GetCount() == 0);
	}
	return true;
}

bool MainForm::OnClearInputBtnClicked(ui::EventArgs* param)
{
	search_edit_->SetText(L"");
	return true;
}

void MainForm::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
	{
		if (nim_ui::LoginManager::GetInstance()->IsEqual(iter->GetAccId()))
		{
			InitHeader();
			break;
		}
	}
}

void MainForm::OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path)
{
	if (type == kUser && nim_ui::LoginManager::GetInstance()->GetAccount() == account)
		btn_header_->SetBkImage(photo_path);
}

void MainForm::OnUnreadCountChange(int unread_count)
{
	if (unread_count > 0)
	{
		label_unread_->SetText(nbase::StringPrintf(L"%d", unread_count));
		box_unread_->SetVisible(true);
	}
	else
	{
		box_unread_->SetVisible(false);
	}
}

void MainForm::InitHeader()
{
	std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();
	label_name_->SetText(nim_ui::UserManager::GetInstance()->GetUserName(my_id, false));
	btn_header_->SetBkImage(nim_ui::PhotoManager::GetInstance()->GetUserPhoto(my_id));
}

void MainForm::LeftClick()
{
	this->ActiveWindow();
	if (is_trayicon_left_double_clicked_)
	{
		is_trayicon_left_double_clicked_ = false;
		return;
	}
	::SetForegroundWindow(m_hWnd);
	::BringWindowToTop(m_hWnd);
}

void MainForm::LeftDoubleClick()
{
	is_trayicon_left_double_clicked_ = true;
}

void MainForm::RightClick()
{
	POINT point;
	::GetCursorPos(&point);
	PopupTrayMenu(point);
}
#endif


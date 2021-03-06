#include "stdafx.h"
#include "player_main_form.h"

using namespace dui;

const LPCTSTR PlayerMainForm::kClassName = L"PlayerMainForm";

void PlayerMainForm::StartVideo(String file)
{
	m_media_player->StartVideo(file);
}

void PlayerMainForm::PauseOrStartVideo()
{
	m_media_player->PauseOrStartVideo();
}

void PlayerMainForm::StopVideo()
{
	m_media_player->StopVideo();
}

void PlayerMainForm::BtnOpenFileClickedCallback(BOOL ret, std::wstring path)
{
	if (ret)
	{
		if (!nbase::FilePathIsExist(path, FALSE) || 0 == nbase::GetFileSize(path))
			return;

		/*std::wstring file_ext;
		nbase::FilePathExtension(path, file_ext);
		nbase::LowerString(file_ext);
		if (file_ext != L".jpg" && file_ext != L".jpeg" && file_ext != L".png" && file_ext != L".bmp")
			return;*/


		StartVideo(path);
	}
}
#include "stdafx.h"
#include "player_main_form.h"

using namespace dui;

const LPCTSTR PlayerMainForm::kClassName = L"PlayerMainForm";

void PlayerMainForm::StartPlay(String file)
{
	m_video_screen->StartPlay(file);
}

void PlayerMainForm::PausePlay()
{
	m_video_screen->PausePlay();
}

void PlayerMainForm::StopPlay()
{
	m_video_screen->StopPlay();
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

		StartPlay(path);
	}
}
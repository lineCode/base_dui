﻿#include "stdafx.h"
#include "user_path.h"
#include "module/login/login_manager.h"

namespace nim_comp
{
static const std::wstring kResImage = L"image\\";
static const std::wstring kResAudio = L"audio\\";
static const std::wstring kResOther = L"other\\";

std::wstring GetUserDataPath()
{
#if 1
	return QPath::GetAppPath();
#else
	std::string account = LoginManager::GetInstance()->GetAccount();
	assert( !account.empty() );
	std::wstring user_app_data_dir = QPath::GetUserAppDataDir( account );
	return user_app_data_dir;
#endif
}

std::wstring GetUserImagePath()
{
	std::wstring dir = GetUserDataPath();
	dir.append(kResImage);
	return dir;
}

std::wstring GetUserAudioPath()
{
	std::wstring dir = GetUserDataPath();
	dir.append(kResAudio);
	return dir;
}

std::wstring GetUserOtherResPath()
{
	std::wstring dir = GetUserDataPath();
	dir.append(kResOther);
	return dir;
}

}
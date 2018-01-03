#include "stdafx.h"
#include "photo_manager.h"
#include "module/login/login_manager.h"
#include "module/user/user_manager.h"
#include "shared/pin_yin_helper.h"
//#include "user_service.h"
//#include "module/login/login_manager.h"
//#include <UrlMon.h>
//#include "shared/pin_yin_helper.h"

//#pragma comment(lib,"urlmon.lib")

namespace nim_comp
{

	PhotoManager::PhotoManager() 
	{

	}

std::wstring PhotoManager::GetUserPhoto(const std::string &accid)
{
	std::wstring default_photo = QPath::GetAppPath() + L"res\\public\\default\\face\\default.png";
	if (!nbase::FilePathIsExist(default_photo, false))
		default_photo = L"";
	if (accid.empty())
	{
		return default_photo;
	}

	UserNameCard info;
	if (!UserManager::GetInstance()->GetUserInfo(accid, info))
	{
		return default_photo;
	}
	std::wstring photo_path = GetPhotoDir(kUser) + nbase::UTF8ToUTF16(info.GetIconUrl());
	if (!CheckPhotoOK(photo_path))
	{
		return default_photo;
	}
	return photo_path;
}

std::wstring PhotoManager::GetMyPhoto(const std::string &iconurl)
{
	std::wstring default_photo = QPath::GetAppPath() + L"res\\faces\\default\\default.png";
	if (!nbase::FilePathIsExist(default_photo, false))
		default_photo = L"";

	// 检查图片是否存在
	std::wstring photo_path = GetPhotoDir(kUser) + nbase::UTF8ToUTF16(iconurl);
	/*int nLen = (int)photo_path.length();
	std::string path;
	path = Wstr2Str(photo_path);*/

	if (!CheckPhotoOK(photo_path)){
	/*	HRESULT ret = DownloadPhoto(iconurl, path);
		if (ret == S_OK)
			return photo_path;
		else*/
			return default_photo;
	}

	return photo_path;
}

//HRESULT PhotoManager::DownloadPhoto(std::string url, std::string path)
//{
//	size_t size = url.length();
//	wchar_t *buffer = new wchar_t[size + 1];
//	MultiByteToWideChar(CP_ACP, 0, url.c_str(), size, buffer, size * sizeof(wchar_t));
//	buffer[size] = 0;
//	
//	size = path.length();
//	wchar_t *buffer2 = new wchar_t[size + 1];
//	MultiByteToWideChar(CP_ACP, 0, path.c_str(), size, buffer2, size * sizeof(wchar_t));
//	buffer2[size] = 0;
//	HRESULT ret = URLDownloadToFile(NULL, buffer, buffer2, 0, NULL);
//	delete buffer;
//	delete buffer2;
//	return ret;
//}

bool PhotoManager::CheckPhotoOK(std::wstring photo_path)
{
	if (!nbase::FilePathIsExist(photo_path, false))
		return false;

	// 检查图片是否损坏
	return (Gdiplus::Image(photo_path.c_str()).GetLastStatus() == Gdiplus::Status::Ok);
}

std::wstring PhotoManager::GetPhotoDir(PhotoType type)
{
	std::wstring photo_dir = QPath::GetUserAppDataDir(LoginManager::GetInstance()->GetAccount());
	if (type == kUser)
		photo_dir = photo_dir.append(L"photo\\");
	else if (type == kTeam)
		photo_dir = photo_dir.append(L"team_photo\\");
	else if (type == kDefault)
		photo_dir = photo_dir.append(L"default\\");
	else if (type == kCustomer)
		photo_dir = photo_dir.append(L"customer\\");
	else if (type == kOthers)
		photo_dir = photo_dir.append(L"others\\");

	if (!nbase::FilePathIsExist(photo_dir, true))
		nbase::win32::CreateDirectoryRecursively(photo_dir.c_str());
	return photo_dir;
}

//UnregisterCallback PhotoManager::RegPhotoReady(const OnPhotoReadyCallback & callback)
//{
//	OnPhotoReadyCallback* new_callback = new OnPhotoReadyCallback(callback);
//	int cb_id = (int)new_callback;
//	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
//	photo_ready_cb_list_[cb_id].reset(new_callback);
//	auto cb = ToWeakCallback([this, cb_id]() {
//		photo_ready_cb_list_.erase(cb_id);
//	});
//	return cb;
//}
//
//int PhotoManager::CheckForDownload(PhotoType type, const std::string& url)
//{
//	if (url.find_first_of("http") != 0) //info.head_image不是正确的url
//		return -1;
//
//	std::wstring photo_path = GetPhotoDir(type) + nbase::UTF8ToUTF16(QString::GetMd5(url));
//	if (CheckPhotoOK(photo_path)) // 如果头像已经存在且完好，就不下载
//		return 1;
//
//	return 0;
//}
//
//void PhotoManager::DownloadUserPhoto(const nim::UserNameCard &info)
//{
//	std::string url = info.GetIconUrl();
//	std::wstring photo_path = GetPhotoDir(kUser) + nbase::UTF8ToUTF16(QString::GetMd5(url));
//	int valid = CheckForDownload(kUser, url);
//	if (valid != 0)
//	{
//		if (valid == 1)
//		{
//			for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
//				(*it.second)(kUser, info.GetAccId(), photo_path);
//		}
//		return;
//	}
//
//	nim::NOS::DownloadMediaCallback cb = ToWeakCallback([this, info, photo_path](nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
//		if (res_code == nim::kNIMResSuccess)
//		{
//			std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
//			if (nbase::FilePathIsExist(ws_file_path, false))
//			{
//				nbase::CopyFileW(ws_file_path, photo_path);
//				nbase::DeleteFile(ws_file_path);
//
//				for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
//					(*it.second)(kUser, info.GetAccId(), photo_path);
//			}
//		}
//	});
//	nim::NOS::DownloadResource(url, cb);
//}
//
//void PhotoManager::DownloadTeamIcon(const nim::TeamInfo &info)
//{
//	std::string url = info.GetIcon();
//	std::wstring photo_path = GetPhotoDir(kTeam) + nbase::UTF8ToUTF16(QString::GetMd5(url));
//	int valid = CheckForDownload(kTeam, url);
//	if (valid != 0)
//	{
//		if (valid == 1)
//		{
//			for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
//			(*it.second)(kTeam, info.GetTeamID(), photo_path);
//		}
//		return;
//	}
//
//	nim::NOS::DownloadMediaCallback cb = ToWeakCallback([this, info, photo_path](nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
//		if (res_code == nim::kNIMResSuccess)
//		{
//			std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
//			if (nbase::FilePathIsExist(ws_file_path, false))
//			{
//				nbase::CopyFileW(ws_file_path, photo_path);
//				nbase::DeleteFile(ws_file_path);
//
//				for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
//					(*it.second)(kTeam, info.GetTeamID(), photo_path);
//			}
//		}
//	});
//	nim::NOS::DownloadResource(url, cb);
//}
//
//std::wstring PhotoManager::GetTeamPhoto(const std::string &tid, bool full_path/* = true*/)
//{
//	std::wstring default_photo = QPath::GetAppPath();
//	if (full_path)
//		default_photo.append(L"themes\\default\\public\\header\\head_team.png");
//	else
//		default_photo = L"../public/header/head_team.png";
//
//	if (full_path && !nbase::FilePathIsExist(default_photo, false))
//		default_photo = L"";
//
//	std::string icon;
//	if (!TeamService::GetInstance()->GetTeamIcon(tid, icon) || icon.empty())
//		return default_photo;
//
//	std::wstring photo_path = GetPhotoDir(kTeam) + nbase::UTF8ToUTF16(QString::GetMd5(icon));
//	if (!CheckPhotoOK(photo_path))
//	{
//		HRESULT ret = DownloadPhoto(icon, nbase::UTF16ToUTF8(photo_path));
//		if (ret == S_OK)
//			return photo_path;
//		else
//			return default_photo;
//	}
//	else
//		return photo_path;
//}

//bool PhotoManager::CreateDefaultFaceImage(std::wstring path, std::wstring name)
//{
//	//HBITMAP hbmp = ::CreateBitmap()
//	return false;
//}

bool PhotoManager::CreateHead(const std::string name,const std::wstring save_path)
{
	/*int kColors[13] = { 0x78c06e, 0xa46ec0, 0xf8c027, 0x72b3d3,
		0xed6157, 0x6fc564, 0x6bc2ed, 0xf19965,
		0x48bdb9, 0xf6608d, 0x5f6bc1, 0x4fb8bc, 0x5f52a0
	};*/
	//b g r
	int kColors[13] = { 0x6ec078, 0xc06ea4, 0x27c0f8, 0xd3b372,
		0x5761ed, 0x64c56f, 0xedc26b, 0x6599f1,
		0xb9bd48, 0x8d60f6, 0xc16b5f, 0xbcb84f, 0xa0525f
	};

	HDC hDC = ::GetDC(NULL);
	if (!hDC)
	{
		return false;
	}
	HDC hMemDC = CreateCompatibleDC(hDC);
	if (!hMemDC)
	{
		ReleaseDC(NULL, hDC);
		return false;
	}
	HBITMAP hMemBmp = ::CreateCompatibleBitmap(hDC, 64, 64);
	if (!hMemBmp)
	{
		DeleteDC(hMemDC);
		ReleaseDC(NULL, hDC);
		return false;
	}
//	std::wstring base_photo = L"C:\\Users\\sun\\AppData\\Local\\Netease\\NIM\\4b3642e915b2197611adb479145ffded\\default\\base.bmp";
	//std::wstring base_photo = GetPhotoDir(kDefault) + L"base.bmp";
	//if (!CheckPhotoOK(base_photo))
	//	CreateBaseBmp(nbase::UTF16ToUTF8(base_photo).c_str());

	//HBITMAP hBmp = OpenBmpFile(hDC, (wchar_t*)base_photo.c_str());
	/*if (!hMemBmp)
	{
		::DeleteObject(hMemDC);
		::ReleaseDC(FindActWnd(true), hDC);
		return;
	}*/
	HBITMAP oldBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);

	HBRUSH hb, oldBrush;
	std::string nick_name_full_spell = nbase::MakeLowerString(shared::PinYinHelper::GetInstance()->ConvertToFullSpell(nbase::UTF8ToUTF16(name)));
	int num = 0;
	if (nick_name_full_spell.length() > 0)
		(nick_name_full_spell[nick_name_full_spell.length() - 1] - 'a') % 13;
	if (num < 0)
		num = -num;
	//std::string color = kColors[num];
	hb = CreateSolidBrush(kColors[num]);
	oldBrush = (HBRUSH)SelectObject(hMemDC, hb);
	Rectangle(hMemDC, 0, 0, 64, 64);

	HFONT hf, oldFont;
	hf = CreateFont(30, 15, 0, 0, 150, false, false, false, CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, FF_SCRIPT, L"Arial");

	oldFont = (HFONT)SelectObject(hMemDC, hf);
	SetTextColor(hMemDC, RGB(255, 255, 255));
	RECT rect = { 0, 17, 64, 47 };
	SetBkMode(hMemDC, TRANSPARENT);

	std::wstring name_ = nbase::UTF8ToUTF16(name);
	if (name_.length() > 2)
	{
		name_ = name_.substr(name_.length() - 2);
	}
	DrawText(hMemDC, (wchar_t*)name_.c_str(), -1, &rect, DT_VCENTER|DT_CENTER);

	PBITMAPINFO pBmpInfo = CreateBitmapInfoStruct(NULL, hMemBmp);
//	std::wstring my_photo = L"C:\\Users\\sun\\AppData\\Local\\Netease\\NIM\\4b3642e915b2197611adb479145ffded\\default\\base1.bmp";

	CreateBMPFile(NULL, (wchar_t*)save_path.c_str(), pBmpInfo, hMemBmp, hDC);

	SelectObject(hMemDC, oldFont);
	SelectObject(hMemDC, oldBrush);
	SelectObject(hMemDC, oldBmp);
	DeleteDC(hMemDC);
	DeleteObject(hMemBmp);
	DeleteObject(hf);
	DeleteObject(hb);
	::ReleaseDC(NULL, hDC);

	return true;
}

//void PhotoManager::CreatGroupHead(const std::wstring name[], const std::wstring save_path)
//{
//	std::string kColors[13] = { "0x78c06e", "0xa46ec0", "0xf8c027", "0x72b3d3", "0xed6157", "0x6fc564", "0x6bc2ed", "0xf19965", "0x48bdb9", "0xf6608d", "0x5f6bc1", "0x4fb8bc", "0x5f52a0" };
//
//	HDC hDC = ::GetDC(FindActWnd(true));
//	HDC hMemDC = CreateCompatibleDC(hDC);
//	//	std::wstring base_photo = L"C:\\Users\\sun\\AppData\\Local\\Netease\\NIM\\4b3642e915b2197611adb479145ffded\\default\\base.bmp";
//	std::wstring base_photo = GetPhotoDir(kDefault) + L"base.bmp";
//	if (!CheckPhotoOK(base_photo))
//		CreateBaseBmp(nbase::UTF16ToUTF8(base_photo).c_str());
//
//	HBITMAP hBmp = OpenBmpFile(hDC, (wchar_t*)base_photo.c_str());
//	SelectObject(hMemDC, hBmp);
//
//	HBRUSH hb;
//	hb = CreateSolidBrush(RGB(195, 195, 195));
//	SelectObject(hMemDC, hb);
//	Rectangle(hMemDC, 0, 0, 64, 64);
//	DeleteObject(hb);
//
//	for (int i = 0; i < 4; i++)
//	{
//		if (name[i] != L"")
//		{
//			HBRUSH hb;
//			HPEN hp;
//			std::string nick_name_full_spell = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(name[i]));
//			int num = (nick_name_full_spell[nick_name_full_spell.length() - 1] - 'a') % 13;
//			if (num < 0)
//				num = -num;
//			std::string color = kColors[num];
//			hb = CreateSolidBrush(RGB(strtol(("0x" + color.substr(2, 2)).c_str(), NULL, 16),
//				strtol(("0x" + color.substr(4, 2)).c_str(), NULL, 16),
//				strtol(("0x" + color.substr(6, 2)).c_str(), NULL, 16)));
//			hp = CreatePen(PS_NULL,0,RGB(195,195,195));
//			SelectObject(hMemDC, hb);
//			SelectObject(hMemDC, hp);
//			Ellipse(hMemDC, i % 2 * 32, i / 2 * 32, (i % 2 + 1)* 32, (i / 2 + 1) * 32 );
//			DeleteObject(hb);
//			DeleteObject(hp);
//		}
//	}
//
//	HFONT hf;
//	//LOGFONT lf;
//	//lf.lfHeight = 25;
//	//lf.lfWidth = 10;
//	//lf.lfEscapement = 0;
//	//lf.lfUnderline = 0;
//	//lf.lfItalic = false;
//	//lf.lfStrikeOut = false;
//	//hf = CreateFontIndirect(&lf);
//
//	hf = CreateFont(15, 7, 0, 0, 150, false, false, false, CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, FF_SCRIPT, L"Arial");
//
//	SelectObject(hMemDC, hf);
//	SetTextColor(hMemDC, RGB(255, 255, 255));
//	SetBkMode(hMemDC, TRANSPARENT);
//
//	for (int i = 0; i < 4; i++)
//	{
//		if (name[i] != L"")
//		{
//			RECT rect = { i % 2 * 32, i / 2 * 32 + 8, (i % 2 + 1) * 32, (i / 2 + 1) * 32 + 23 };
//			std::wstring name_ = name[i];
//			if (name_.length() > 2)
//			{
//				name_ = name_.substr(name_.length() - 2);
//			}
//			DrawText(hMemDC, (wchar_t*)name_.c_str(), -1, &rect, DT_VCENTER | DT_CENTER);
//		}
//	}
//
//	PBITMAPINFO pBmpInfo = CreateBitmapInfoStruct(FindActWnd(true), hBmp);
//	//	std::wstring my_photo = L"C:\\Users\\sun\\AppData\\Local\\Netease\\NIM\\4b3642e915b2197611adb479145ffded\\default\\base1.bmp";
//
//	CreateBMPFile(FindActWnd(true), (wchar_t*)save_path.c_str(), pBmpInfo, hBmp, hDC);
//
//
//	DeleteDC(hMemDC);
//	DeleteObject(hBmp);
//	DeleteObject(hf);
//}

void PhotoManager::CreateBaseBmp(const char* filename)
{
	int size = 64 * 64 * 3;
	struct {
		BYTE b;
		BYTE g;
		BYTE r;
	} pRGB[64][64];
	memset(pRGB, 0, sizeof(pRGB));

	BITMAPFILEHEADER bfh;
	bfh.bfType = 0x4d42;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfSize = size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bfh.bfOffBits = bfh.bfSize - size;

	BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = 64;
	bih.biHeight = 64;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = 0;
	bih.biSizeImage = size;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	FILE* fp = fopen(filename, "wb");
	if (!fp)
		return;

	fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);
	fwrite((BYTE*)pRGB, 1, size, fp);
	fclose(fp);
}

HBITMAP PhotoManager::OpenBmpFile(HDC hDC, LPWSTR lpszFileName)
{
	HBITMAP hBmp = NULL;
	HANDLE hFile = CreateFile(
		lpszFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	//读取位图文件头
	BITMAPFILEHEADER bmpFileHeader;
	DWORD dwNumberOFBytesRead;
	if (ReadFile(hFile, (LPVOID)&bmpFileHeader, sizeof(BITMAPFILEHEADER), &dwNumberOFBytesRead, NULL) == 0)
	{
		CloseHandle(hFile);
		return NULL;
	}

	//读取位图信息
	BITMAPINFO *pBmpInfo = new BITMAPINFO;
	if (ReadFile(hFile, pBmpInfo, sizeof(BITMAPINFOHEADER), &dwNumberOFBytesRead, NULL) == 0)
	{
		CloseHandle(hFile);
		return NULL;
	}
	//读取位图数据
	LPVOID pBmpData;
	//创建DIB位图句柄
	hBmp = CreateDIBSection(hDC, pBmpInfo, DIB_PAL_COLORS, &pBmpData, NULL, 0);
	if (hBmp == NULL)
	{
		DWORD dwErr = GetLastError();
		return NULL;
	}
	else
	{
		if (ReadFile(hFile, pBmpData, pBmpInfo->bmiHeader.biSizeImage, &dwNumberOFBytesRead, 0) == 0)
		{
			CloseHandle(hFile);
			return NULL;
		}

		CloseHandle(hFile);
		return hBmp;
	}

}

PBITMAPINFO PhotoManager::CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD cClrBits;

	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
		return NULL;

	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else
		cClrBits = 32;

	if (cClrBits != 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
		sizeof(BITMAPINFOHEADER)+
		sizeof(RGBQUAD)* (1 << cClrBits));
	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
		sizeof(BITMAPINFOHEADER));

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;

	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	pbmi->bmiHeader.biCompression = BI_RGB;
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31)&~31) / 8 * pbmi->bmiHeader.biHeight;
	pbmi->bmiHeader.biClrImportant = 0;

	return pbmi;

}

void PhotoManager::CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC)
{
	HANDLE hf;
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER pbih;
	LPBYTE lpBits;
	DWORD dwTotal;
	DWORD cb;
	BYTE * hp;
	DWORD dwTmp;

	pbih = pbi->bmiHeader;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih.biSizeImage);

	if (!lpBits)
		return;

	if (!GetDIBits(hDC, hBMP, 0, (WORD)pbih.biHeight, lpBits, pbi, DIB_RGB_COLORS))
		return;

	//创建BMP文件
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	if (hf == INVALID_HANDLE_VALUE)
		return;
	hdr.bfType = 0x4d42;	//BM 0x42:B 0x4d:M 
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER)+pbih.biSize + pbih.biClrUsed * sizeof(RGBQUAD)+pbih.biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+pbih.biSize + pbih.biClrUsed * sizeof(RGBQUAD);

	if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER), (LPDWORD)&dwTmp, NULL))
		return;

	if (!WriteFile(hf, (LPVOID)&pbih, sizeof(BITMAPINFOHEADER)+pbih.biClrUsed * sizeof(RGBQUAD), (LPDWORD)&dwTmp, NULL))
		return;

	dwTotal = cb = pbih.biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL))
		return;
	if (!CloseHandle(hf))
		return;

	GlobalFree((HGLOBAL)lpBits);
}

}
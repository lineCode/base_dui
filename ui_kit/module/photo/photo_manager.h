#pragma once
//#include "shared/threads.h"
#include "base/memory/singleton.h"
#include "shared/auto_unregister.h"

#include "module/user/user_define.h"

enum PhotoType
{
	kUser,
	kTeam,
	kDefault,
	kCustomer,	//客户和客服
	kOthers,
};


typedef std::function<void(PhotoType type, const std::string& id, const std::wstring &photo_path)> OnPhotoReadyCallback;

namespace nim_comp
{

class PhotoManager 
{
public:
	SINGLETON_DEFINE(PhotoManager);

private:
	PhotoManager();
	~PhotoManager() {}

public:
	//UnregisterCallback RegPhotoReady(const OnPhotoReadyCallback& callback); // 注册头像下载完成的回调
	std::wstring GetUserPhoto(const std::string &accid); // 从UserService::all_user_中查询用户头像
	std::wstring GetMyPhoto(const std::string &iconurl);
	//std::wstring GetTeamPhoto(const std::string &tid, bool full_path = true);

public:
	//不开放
	//void DownloadUserPhoto(const UserNameCard &info); //获取用户信息后，或者用户信息修改后，下载用户头像
	//void DownloadTeamIcon(const TeamInfo &info);
	std::wstring GetPhotoDir(PhotoType type);
	//bool CreateDefaultFaceImage(std::wstring path, std::wstring name);
	//void CreatGroupHead(const std::wstring name[], const std::wstring save_path);
	bool CreateHead(const std::string name, const std::wstring save_path);
	bool CheckPhotoOK(std::wstring photo_path); // 检查某个头像图片是否完好

private:
	int CheckForDownload(PhotoType type, const std::string& url);
	HRESULT DownloadPhoto(std::string url, std::string path);

	
	void CreateBaseBmp(const char* filename);
	HBITMAP OpenBmpFile(HDC hDC, LPWSTR lpszFileName);	//打开位图文件，得到位图句柄
	PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);	//保存位图于文件
	void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);

private:
	//std::map<int, std::unique_ptr<OnPhotoReadyCallback>> photo_ready_cb_list_; //用户头像下载完成回调列表
};


}
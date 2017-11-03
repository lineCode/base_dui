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
	kCustomer,	//�ͻ��Ϳͷ�
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
	//UnregisterCallback RegPhotoReady(const OnPhotoReadyCallback& callback); // ע��ͷ��������ɵĻص�
	std::wstring GetUserPhoto(const std::string &accid); // ��UserService::all_user_�в�ѯ�û�ͷ��
	std::wstring GetMyPhoto(const std::string &iconurl);
	//std::wstring GetTeamPhoto(const std::string &tid, bool full_path = true);

public:
	//������
	//void DownloadUserPhoto(const UserNameCard &info); //��ȡ�û���Ϣ�󣬻����û���Ϣ�޸ĺ������û�ͷ��
	//void DownloadTeamIcon(const TeamInfo &info);
	std::wstring GetPhotoDir(PhotoType type);
	//bool CreateDefaultFaceImage(std::wstring path, std::wstring name);
	//void CreatGroupHead(const std::wstring name[], const std::wstring save_path);
	bool CreateHead(const std::string name, const std::wstring save_path);
	bool CheckPhotoOK(std::wstring photo_path); // ���ĳ��ͷ��ͼƬ�Ƿ����

private:
	int CheckForDownload(PhotoType type, const std::string& url);
	HRESULT DownloadPhoto(std::string url, std::string path);

	
	void CreateBaseBmp(const char* filename);
	HBITMAP OpenBmpFile(HDC hDC, LPWSTR lpszFileName);	//��λͼ�ļ����õ�λͼ���
	PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);	//����λͼ���ļ�
	void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);

private:
	//std::map<int, std::unique_ptr<OnPhotoReadyCallback>> photo_ready_cb_list_; //�û�ͷ��������ɻص��б�
};


}
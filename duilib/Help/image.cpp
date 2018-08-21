#include "stdafx.h"
#include "image.h"
#include <atlbase.h>
#include <textserv.h>

#include "../utils/image_ole_i.h"

namespace duihelp
{

	bool BaseSaveBitmap(HBITMAP hbitmap, const std::wstring save_path)
	{
		HDC     hDC;
		
		int     iBits;		//��ǰ�ֱ�����ÿ������ռ�ֽ���         
		
		WORD     wBitCount;	//λͼ��ÿ������ռ�ֽ���         
		//�����ɫ���С��     λͼ�������ֽڴ�С     ��λͼ�ļ���С     ��     д���ļ��ֽ���             
		DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
		//λͼ���Խṹ             
		BITMAP     Bitmap;
		//λͼ�ļ�ͷ�ṹ         
		BITMAPFILEHEADER     bmfHdr;
		//λͼ��Ϣͷ�ṹ             
		BITMAPINFOHEADER     bi;
		//ָ��λͼ��Ϣͷ�ṹ                 
		LPBITMAPINFOHEADER     lpbi;
		//�����ļ��������ڴ�������ɫ����             
		HANDLE     fh, hDib, hPal, hOldPal = NULL;

		//����λͼ�ļ�ÿ��������ռ�ֽ���             
		hDC = ::CreateDC(L"DISPLAY", NULL, NULL, NULL);
		iBits = GetDeviceCaps(hDC, BITSPIXEL)     *     GetDeviceCaps(hDC, PLANES);
		DeleteDC(hDC);
		if (iBits <= 1)
			wBitCount = 1;
		else  if (iBits <= 4)
			wBitCount = 4;
		else if (iBits <= 8)
			wBitCount = 8;
		else
			wBitCount = 24;

		GetObject(hbitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = Bitmap.bmWidth;
		bi.biHeight = Bitmap.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = wBitCount;
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 0;
		bi.biXPelsPerMeter = 0;
		bi.biYPelsPerMeter = 0;
		bi.biClrImportant = 0;
		bi.biClrUsed = 0;

		dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

		//Ϊλͼ���ݷ����ڴ�             
		hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
		lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
		*lpbi = bi;

		//     �����ɫ��                 
		hPal = GetStockObject(DEFAULT_PALETTE);
		if (hPal)
		{
			hDC = ::GetDC(NULL);
			hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
			RealizePalette(hDC);
		}

		//     ��ȡ�õ�ɫ�����µ�����ֵ             
		GetDIBits(hDC, hbitmap, 0, (UINT)Bitmap.bmHeight,
			(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
			(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

		//�ָ���ɫ��                 
		if (hOldPal)
		{
			::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
			RealizePalette(hDC);
			::ReleaseDC(NULL, hDC);
		}

		//����λͼ�ļ�                 
		fh = CreateFile(save_path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

		if (fh == INVALID_HANDLE_VALUE)
		{
			GlobalUnlock(hDib);
			GlobalFree(hDib);
			return false;
		}
			

		//     ����λͼ�ļ�ͷ             
		bmfHdr.bfType = 0x4D42;     //     "BM"             
		dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
		bmfHdr.bfSize = dwDIBSize;
		bmfHdr.bfReserved1 = 0;
		bmfHdr.bfReserved2 = 0;
		bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
     
		::WriteFile(fh, (LPVOID)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);	//     д��λͼ�ļ�ͷ     
		::WriteFile(fh, (LPVOID)lpbi,	dwDIBSize, &dwWritten, NULL);			//     д��λͼ�ļ���������      

		//���                 
		GlobalUnlock(hDib);
		GlobalFree(hDib);
		CloseHandle(fh);

		return true;
	}

	//------------------------------add by djj------------------------------------
#define IMAGE_OLE_NAME	L"image_ole.dll"

	static HMODULE image_ole_module_ = NULL;

	typedef HRESULT(WINAPI* DLLGETCLASSOBJECTFUNC) (REFCLSID rclsid, REFIID riid, LPVOID* ppv);

	void FreeImageoleModule()
	{
		if (image_ole_module_)
		{
			FreeLibrary(image_ole_module_);
			image_ole_module_ = NULL;
		}
	}

	bool CreateImageObject(void **ppv)
	{
		*ppv = NULL;
		if (image_ole_module_ == NULL)
		{
			std::wstring dll_path = dui::UIManager::GetInstancePath();
			dll_path.append(_T("dll\\"));
			dll_path.append(IMAGE_OLE_NAME);
			image_ole_module_ = LoadLibrary(dll_path.c_str());
		}
		if (image_ole_module_ == NULL)
		{
			return false;
		}

		DLLGETCLASSOBJECTFUNC pFunc = (DLLGETCLASSOBJECTFUNC)GetProcAddress(image_ole_module_, "DllGetClassObject");
		if (pFunc == NULL)
		{
			FreeImageoleModule();
			return false;
		}
		CComPtr<IClassFactory> pFactory;
		HRESULT hr = (*pFunc)(CLSID_ImageOle/*������CLSID*/, IID_IClassFactory, (void**)&pFactory);
		if (hr == S_OK)
			hr = pFactory->CreateInstance(0, IID_IImageOle, ppv);

		if (hr != S_OK || *ppv == NULL)
		{
			FreeImageoleModule();
			return false;
		}
		return true;
	}

	String GetIconByFile(String file)
	{
		std::wstring app_w_path = dui::UIManager::GetInstancePath();
		std::wstring image_path = app_w_path + L"res\\icons\\";

		std::wstring file_exten;
		std::wstring ext;
		FilePathExtension(file, ext);
		if (ext.size() > 1)
		{
			file_exten = ext.substr(1);
		}
		if (file_exten == L"doc" || file_exten == L"docx")
		{
			image_path += L"file_doc.png";
		}
		else if (file_exten == L"ppt" || file_exten == L"pptx")
		{
			image_path += L"file_ppt.png";
		}
		else if (file_exten == L"xls" || file_exten == L"xlsx")
		{
			image_path += L"file_excel.png";
		}
		else if (file_exten == L"mp3")
		{
			image_path += L"file_mp3.png";
		}
		else if (file_exten == L"htm" || file_exten == L"html")
		{
			image_path += L"file_html.png";
		}
		else if (file_exten == L"txt" || file_exten == L"text")
		{
			image_path += L"file_txt.png";
		}
		else if (file_exten == L"pdf")
		{
			image_path += L"file_pdf.png";
		}
		else if (file_exten == L"zip")
		{
			image_path += L"file_zip.png";
		}
		else if (file_exten == L"rar")
		{
			image_path += L"file_rar.png";
		}
		else if (file_exten == L"zip")
		{
			image_path += L"file_zip.png";
		}
		else
		{
			image_path += L"file_default.png";
		}
		return image_path;
	}

	bool FilePathExtension(const String &filepath_in, String &extension_out)
	{
		if (filepath_in.size() == 0)
			return false;
		size_t pos = filepath_in.rfind(_T('.'));
		if (pos == String::npos)
			return false;
		extension_out = filepath_in.substr(pos, String::npos);
		return true;
	}

}
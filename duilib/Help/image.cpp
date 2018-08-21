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
		
		int     iBits;		//当前分辨率下每象素所占字节数         
		
		WORD     wBitCount;	//位图中每象素所占字节数         
		//定义调色板大小，     位图中像素字节大小     ，位图文件大小     ，     写入文件字节数             
		DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
		//位图属性结构             
		BITMAP     Bitmap;
		//位图文件头结构         
		BITMAPFILEHEADER     bmfHdr;
		//位图信息头结构             
		BITMAPINFOHEADER     bi;
		//指向位图信息头结构                 
		LPBITMAPINFOHEADER     lpbi;
		//定义文件，分配内存句柄，调色板句柄             
		HANDLE     fh, hDib, hPal, hOldPal = NULL;

		//计算位图文件每个像素所占字节数             
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

		//为位图内容分配内存             
		hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
		lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
		*lpbi = bi;

		//     处理调色板                 
		hPal = GetStockObject(DEFAULT_PALETTE);
		if (hPal)
		{
			hDC = ::GetDC(NULL);
			hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
			RealizePalette(hDC);
		}

		//     获取该调色板下新的像素值             
		GetDIBits(hDC, hbitmap, 0, (UINT)Bitmap.bmHeight,
			(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
			(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

		//恢复调色板                 
		if (hOldPal)
		{
			::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
			RealizePalette(hDC);
			::ReleaseDC(NULL, hDC);
		}

		//创建位图文件                 
		fh = CreateFile(save_path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

		if (fh == INVALID_HANDLE_VALUE)
		{
			GlobalUnlock(hDib);
			GlobalFree(hDib);
			return false;
		}
			

		//     设置位图文件头             
		bmfHdr.bfType = 0x4D42;     //     "BM"             
		dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
		bmfHdr.bfSize = dwDIBSize;
		bmfHdr.bfReserved1 = 0;
		bmfHdr.bfReserved2 = 0;
		bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
     
		::WriteFile(fh, (LPVOID)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);	//     写入位图文件头     
		::WriteFile(fh, (LPVOID)lpbi,	dwDIBSize, &dwWritten, NULL);			//     写入位图文件其余内容      

		//清除                 
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
		HRESULT hr = (*pFunc)(CLSID_ImageOle/*你的组件CLSID*/, IID_IClassFactory, (void**)&pFactory);
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
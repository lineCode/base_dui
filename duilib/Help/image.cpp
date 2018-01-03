#include "stdafx.h"
//#include "base/file/file_util.h"
//#include <memory>
//#include "base/util/string_util.h"
#include "image.h"

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


}
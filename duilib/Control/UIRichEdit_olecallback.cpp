#include "StdAfx.h"
//#include "UIRichEdit_olecallback.h"
#include "UIRichEdit.h"

//#include "richedit_util.h"
//#include "module/dragdrop/drag_drop.h"

namespace dui
{
IRichEditOleCallbackEx::IRichEditOleCallbackEx(RichEdit *re, std::function<void()> callback)
{
	pStorage = NULL;
	m_iNumStorages = 0;
	m_dwRef = 0;
	cf_nim_format_ = RegisterClipboardFormat(L"Nim_RichEdit_Format");
	cf_html_format_ = RegisterClipboardFormat(L"HTML Format");
	richedit_ = re;
	callback_ = callback;
	custom_mode_ = true;
}

IRichEditOleCallbackEx::~IRichEditOleCallbackEx()
{
	if (pStorage)
	{
		pStorage->Release();
	}
}
void IRichEditOleCallbackEx::SetCustomMode(bool custom)
{
	custom_mode_ = custom;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::GetNewStorage(LPSTORAGE* lplpstg)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;

	if ( iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback )
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}

ULONG STDMETHODCALLTYPE IRichEditOleCallbackEx::AddRef()
{
	return ++m_dwRef;
}

ULONG STDMETHODCALLTYPE IRichEditOleCallbackEx::Release()
{
	if ( --m_dwRef == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
	LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::ShowContainerUI(BOOL fShow)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
#if 0
	if (*lpclsid == CLSID_ImageOle)
		return S_OK;
	return E_NOTIMPL;
#endif
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
	DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	if (lpdataobj && fReally && (reco == RECO_DROP || reco == RECO_PASTE))
	{
		STGMEDIUM stgMedium;
		HRESULT ret;

		FORMATETC cFmt = { (CLIPFORMAT)CF_BITMAP, NULL, DVASPECT_CONTENT, -1, TYMED_GDI };
		ret = lpdataobj->GetData(&cFmt, &stgMedium);
		if (ret == S_OK)
		{
			HBITMAP hbmp = stgMedium.hBitmap;
			if (richedit_)
				richedit_->InsertHBitmap(InsertCustomItemErrorCallback(), hbmp, false);
			::ReleaseStgMedium(&stgMedium);
			return S_FALSE;
		}
		else
		{
			FORMATETC cFmt = { (CLIPFORMAT)cf_nim_format_, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			ret = lpdataobj->GetData(&cFmt, &stgMedium);
			if (ret == S_OK)
			{
				std::string fragment;
				fragment = (LPSTR)GlobalLock(stgMedium.hGlobal);
				GlobalUnlock(stgMedium.hGlobal);
				//Re_InsertNimTextInfo(text_services_, fragment, custom_mode_, callback_);
				::ReleaseStgMedium(&stgMedium);
				return S_FALSE;
			}
		}
	}
#if 0
	if (::OpenClipboard(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND()))
	{
		HDROP hDrop = (HDROP)::GetClipboardData(CF_HDROP);
		if (hDrop != NULL)
		{
			const int MAX_PATH_SIZE = 1024;

			wchar_t buff[MAX_PATH_SIZE] = { 0 };
			int iFile = DragQueryFileW(hDrop, 0xFFFFFFFF, buff, MAX_PATH_SIZE);
			for (int i = 0; i < iFile; i++)
			{
				int len = DragQueryFileW(hDrop, i, buff, MAX_PATH_SIZE);
				std::wstring path(buff, len);

				nim_comp::SessionManager::GetInstance()->SendFileImage("", path);
			}
		}
		::CloseClipboard();
	}
#endif
	return S_OK;  
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::ContextSensitiveHelp(BOOL fEnterMode)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	if (richedit_)
	{
#if 0
		std::wstring strText;
		std::wstring nim_format;
		bool bCustomObject = Re_GetNimTextInfo(text_services_, lpchrg, strText, nim_format);

		if (strText.empty() && nim_format.empty())
		{
			return E_NOTIMPL;
		}
		if (*lplpdataobj == NULL)
		{
			*lplpdataobj = new SdkDataObject;
		}
		SaveStr2DataObject(*lplpdataobj, strText, CF_TEXT);
		SaveStr2DataObject(*lplpdataobj, strText, CF_UNICODETEXT);
		if (bCustomObject)
		{
			SaveStr2DataObject(*lplpdataobj, nim_format, cf_nim_format_);
		}
#endif
		return S_OK;
	} 
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	//return S_OK;
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IRichEditOleCallbackEx::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
	HMENU FAR *lphmenu)
{
	return S_OK;
}
//HRESULT IRichEditOleCallbackEx::SaveStr2DataObject(LPDATAOBJECT pDataSource, std::wstring str, CLIPFORMAT cfFormat)
//{
//	HGLOBAL hG = NULL;
//	if (cfFormat == CF_UNICODETEXT)
//	{
//		int  strBytes=  str.length() * sizeof(wchar_t);  
//		wchar_t *temp = new wchar_t[str.length()+1];
//		ZeroMemory(temp, strBytes+sizeof(wchar_t));
//		memcpy(temp, str.c_str(), strBytes);
//		hG = GlobalAlloc(GMEM_DDESHARE, strBytes+sizeof(wchar_t));  
//		void* pBuffer = GlobalLock(hG);  
//		{  
//			memcpy(pBuffer, temp, strBytes+sizeof(wchar_t)); 
//			GlobalUnlock(hG);  
//		} 
//		delete[] temp;
//	} 
//	else
//	{
//		std::string tmp;
//		if (cfFormat == CF_TEXT)
//		{
//			int len = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
//			if (len > 0)
//			{
//				std::unique_ptr<char[]> gbk_text(new char[len]);
//				WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, gbk_text.get(), len, NULL, NULL);
//				tmp = gbk_text.get();
//			}
//
//		}
//		else
//		{
//			tmp = nbase::UTF16ToUTF8(str);
//		}
//		int  strBytes=  tmp.length() * sizeof(char);  
//		char *temp = new char[tmp.length()+1];
//		ZeroMemory(temp, strBytes+sizeof(char));
//		memcpy(temp, tmp.c_str(), strBytes);
//		hG = GlobalAlloc(GMEM_DDESHARE, strBytes+sizeof(char));  
//		void* pBuffer = GlobalLock(hG);  
//		{  
//			memcpy(pBuffer, temp, strBytes+sizeof(char)); 
//			GlobalUnlock(hG);  
//		} 
//		delete[] temp;
//	}
//
//	FORMATETC fmt;  
//	fmt.cfFormat = cfFormat;  
//	fmt.dwAspect = DVASPECT_CONTENT;  
//	fmt.lindex = -1;  
//	fmt.ptd = NULL;  
//	fmt.tymed = TYMED_HGLOBAL;  
//
//	STGMEDIUM stg;  
//	stg.tymed = TYMED_HGLOBAL;  
//	stg.hGlobal = hG;  
//	stg.pUnkForRelease = NULL;  
//
//	pDataSource->SetData(&fmt, &stg, FALSE);
//
//	return S_OK;
//}

}
#include "stdafx.h"
#include <atlbase.h>
#include <textserv.h>

#include "Utils/image_ole_i.h"

namespace dui {

	IRichEditOle* RichEdit::GetOleInterface()
	{
		assert(m_pTwh);
		if (!m_pTwh)
			return NULL;
		ITextServices *text_service = m_pTwh->GetTextServices();
		if (!text_service)
			return NULL;
		IRichEditOle *pRichEditOle = NULL;
		text_service->TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle, NULL);
		return pRichEditOle;
	}

	int RichEdit::GetCustomImageOleCount()
	{
		if (!m_pTwh)
			return 0;
		ITextServices *text_service = m_pTwh->GetTextServices();
		if (!text_service)
			return 0;

		int ret = 0;
		REOBJECT reobject;
		IRichEditOle * pRichEditOle = GetOleInterface();
		if (NULL == pRichEditOle)
			return ret;

		CHARRANGE chrg = { 0, GetTextLength() };

		for (LONG i = 0; i < chrg.cpMax; i++)
		{
			memset(&reobject, 0, sizeof(REOBJECT));
			reobject.cbStruct = sizeof(REOBJECT);
			reobject.cp = i;
			HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
			if (SUCCEEDED(hr))
			{
				i = reobject.cp;

				if (NULL == reobject.poleobj)
					continue;

				if (CLSID_ImageOle == reobject.clsid)
				{
					IImageOle * pImageOle = NULL;
					hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
					if (SUCCEEDED(hr))
					{
						long nImageIndex;
						pImageOle->GetFaceIndex(&nImageIndex);
						if (nImageIndex >= RE_OLE_TYPE_CUSTOM)
						{
							++ret;
						}
						pImageOle->Release();
					}
				}
				reobject.poleobj->Release();
			}
		}
		pRichEditOle->Release();

		return ret;
	}

	bool RichEdit::Re_FindFile(const std::wstring& file, bool check_sel/* = false*/)
	{
		bool ret = false;
		REOBJECT reobject;
		IRichEditOle * pRichEditOle = GetOleInterface();
		if (NULL == pRichEditOle)
			return ret;

		CHARRANGE chrg = { 0, GetTextLength() };
		long start_char = 0, end_char = 0;
		GetSel(start_char, end_char);

		for (LONG i = 0; i < chrg.cpMax && !ret; i++)
		{
			memset(&reobject, 0, sizeof(REOBJECT));
			reobject.cbStruct = sizeof(REOBJECT);
			reobject.cp = i;
			HRESULT hr = pRichEditOle->GetObject(REO_IOB_USE_CP, &reobject, REO_GETOBJ_POLEOBJ);
			if (SUCCEEDED(hr))
			{
				i = reobject.cp;

				if (!check_sel && i >= start_char && i<end_char)
				{
					continue;
				}

				if (NULL == reobject.poleobj)
					continue;
				if (CLSID_ImageOle == reobject.clsid)
				{
					IImageOle * pImageOle = NULL;
					hr = reobject.poleobj->QueryInterface(__uuidof(IImageOle), (void**)&pImageOle);
					if (SUCCEEDED(hr))
					{
						long nImageIndex;
						pImageOle->GetFaceIndex(&nImageIndex);
						if (nImageIndex == RE_OLE_TYPE_FILE)
						{
							wchar_t* image_info;
							pImageOle->GetFaceTag(&image_info);
							std::wstring image_tag = image_info;
							if (image_tag == file)
							{
								ret = true;
							}
						}
						if (pImageOle)
						{
							pImageOle->Release();
						}
					}
				}
				reobject.poleobj->Release();
			}
		}
		pRichEditOle->Release();

		return ret;
	}


	//--------------------------------------protected---------------------------------
	bool RichEdit::InsertCustomItem(InsertCustomItemErrorCallback callback, bool total_count_limit, const std::wstring& file, const std::wstring& face_tag, RE_OLE_TYPE ole_type
		, int face_id, bool scale, int scale_width/* = 0*/, int scale_height /*= 0*/, LONG cp/* = REO_CP_SELECTION*/)
	{
		ITextServices *text_service = m_pTwh->GetTextServices();
		assert(text_service);
		if (!text_service)
			return false;
		if (total_count_limit && ole_type >= RE_OLE_TYPE_CUSTOM && GetCustomImageOleCount() >= MAX_CUSTOM_ITEM_NUM)
		{
			if (callback)
			{
				callback();
			}
			return false;
		}
		if (text_service == NULL)
			return false;

		//m_RichEdit为您的RichEdit对象
		IRichEditOle* lpRichEditOle = GetOleInterface();

		if (lpRichEditOle == NULL)
			return false;

		//OLE对象
		IOleObject*  lpOleObject = NULL;
		IOleClientSite* lpOleClientSite = NULL;
		CComPtr<IImageOle> image_ole;
		CLSID   clsid;
		REOBJECT  reobject;
		HRESULT   hr = S_FALSE;
		do
		{
			{
				if (!CreateImageObject((LPVOID*)&image_ole))
				break;
			}

			//获得数据对象接口
			hr = image_ole->QueryInterface(IID_IOleObject, (void**)&lpOleObject);
			if (hr != S_OK)
				break;

			hr = lpOleObject->GetUserClassID(&clsid);
			if (hr != S_OK)
				break;

			hr = lpRichEditOle->GetClientSite(&lpOleClientSite);
			if (hr != S_OK)
				break;

			//初始化一个对象
			ZeroMemory(&reobject, sizeof(REOBJECT));
			reobject.cbStruct = sizeof(REOBJECT);
			reobject.clsid = clsid;
			reobject.cp = cp;
			reobject.dvaspect = DVASPECT_CONTENT;
			reobject.dwFlags = REO_BELOWBASELINE;
			reobject.poleobj = lpOleObject;
			reobject.polesite = lpOleClientSite;
			reobject.pstg = NULL;// lpStorage;

			lpOleObject->SetClientSite(lpOleClientSite);
			std::wstring image_path = file;
			image_ole->SetFaceTag((BSTR)(face_tag.c_str()));
			image_ole->SetFaceIndex(ole_type);
			image_ole->SetFaceId(face_id);
			if (ole_type == RE_OLE_TYPE_FILE)
			{
				OSVERSIONINFO os = { sizeof(OSVERSIONINFO) };
				::GetVersionEx(&os);
				std::wstring fontName = os.dwMajorVersion >= 6 ? L"微软雅黑" : L"新宋体";
				image_ole->SetFont((BSTR)(fontName.c_str()), 20, RGB(0x1a, 0x30, 0x47));
				if (image_path.size() == 0)
				{
					image_path = GetIconByFile(face_tag);
				}
				image_ole->SetBgColor(RGB(255, 255, 255));
			}
			else if (ole_type == RE_OLE_TYPE_IMAGELOADING)
			{
				if (image_path.size() == 0)
				{
					image_path = /*QPath::GetAppPath()*/UIManager::GetInstancePath();
					image_path.append(L"res\\icons\\loading.gif");
				}
			}
			image_ole->SetScaleSize(scale, scale_width, scale_height);
			image_ole->LoadFromFile((BSTR)(image_path.c_str()));
#if 0
			std::wstring guid = nbase::UTF8ToUTF16(QString::GetGUID());
			image_ole->SetGUID((BSTR)guid.c_str());
#endif
			OleSetContainedObject(lpOleObject, TRUE);

			hr = lpRichEditOle->InsertObject(&reobject);
		} while (0);

		if (lpOleObject != NULL)
			lpOleObject->Release();
		if (lpOleClientSite != NULL)
			lpOleClientSite->Release();

		lpRichEditOle->Release();

		return hr == S_OK;
	}

	











}
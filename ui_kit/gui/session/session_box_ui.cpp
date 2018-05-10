#include "stdafx.h"
#include "session_box.h"
#include "shared/threads.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#include "module/session/session_util.h"
#include "module/window/windows_manager.h"
#include "module/login/login_manager.h"
#include "module/db_manager/db_manager.h"
#include "gui/emoji/emoji_info.h"
#include "gui/emoji/emoji_form.h"
#include "gui/msgbox/msgbox.h"
#include "gui/image_view/image_view_form.h"
#include "gui/db_manager/db_manager_mainform.h"

using namespace dui;

namespace nim_comp
{

SessionBox::SessionBox(std::string id, NIMSessionType type) 
{
}

SessionBox::~SessionBox() 
{

}

void SessionBox::Init()
{
	printf("SessionBox::Init");

	CheckBtn *chk_face = (CheckBtn*)FindSubControl(L"chk_face");
	Button* btn_image = (Button*)FindSubControl(L"btn_image");
	Button* btn_file = (Button*)FindSubControl(L"btn_file");
	Button* btn_jsb = (Button*)FindSubControl(L"btn_jsb");
	Button* btn_clip = (Button*)FindSubControl(L"btn_clip");
	Button* btn_msg_record = (Button*)FindSubControl(L"btn_msg_record");
	Button* btn_send = (Button*)FindSubControl(L"btn_send");

	chk_face->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_image->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_file->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_jsb->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_clip->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_msg_record->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));
	btn_send->AttachClick(std::bind(&SessionBox::OnClicked, this, std::placeholders::_1));

	input_edit_ = (RichEdit*)FindSubControl(L"input_edit");

	InitRichEdit();
}

void SessionBox::InitRichEdit()
{
	if (!input_edit_)
	{
		return;
	}
	::RevokeDragDrop(m_pManager->GetPaintWindow());

	::RegisterDragDrop(m_pManager->GetPaintWindow(), this);
}

bool SessionBox::OnClicked(dui::TEvent *event)
{
	wstring name = event->pSender->GetName();
	if (event)
	{
		wprintf(L"SessionBox::OnClicked %s\n", name.c_str());
	}

	if (name == L"btn_clip")
	{
		/*if (CaptureManager::GetInstance()->IsCaptureTracking())
			return;
		StdClosure callback = nbase::Bind(&SessionBox::DoClip, this);
		nbase::ThreadManager::PostDelayedTask(shared::kThreadUI, callback, nbase::TimeDelta::FromMilliseconds(500));*/
		ImageViewForm *form = new ImageViewForm;
		form->Create(m_pManager->GetPaintWindow(), ImageViewForm::kClassName, WS_OVERLAPPED, 0, RECT());
		//form->WindowEx::Create(m_pManager->GetPaintWindow(), ImageViewForm::kClassName, WS_OVERLAPPED, 0, false, CDuiRect(0, 0, 700, 500));
		form->ShowWindow();
	}
	else if (name == L"chk_face")
	{
		CheckBtn *chk_face = (CheckBtn*)FindSubControl(L"chk_face");
		RECT rc = chk_face->GetPos();
		POINT pt_offset = { 150, 350 };
		//DpiManager::GetInstance()->ScalePoint(pt_offset);
		POINT pt = { rc.left - pt_offset.x, rc.top - pt_offset.y };
		::ClientToScreen(m_pManager->GetPaintWindow(), &pt);

		OnSelectEmotion sel = std::bind(&SessionBox::OnEmotionSelected, this, std::placeholders::_1);
		OnSelectSticker sel2 = std::bind(&SessionBox::OnEmotionSelectedSticker, this, std::placeholders::_1, std::placeholders::_2);
		OnEmotionClose  cls = std::bind(&SessionBox::OnEmotionClosed, this);

		EmojiForm* emoji_form = new EmojiForm;
		emoji_form->ShowEmoj(pt, sel, sel2, cls);
	}
	else if (name == L"btn_image")
	{
		std::wstring file_type = _T("图像文件");
		LPCTSTR filter = L"*.jpg;*.jpeg;*.png;*.bmp";
		std::wstring text = nbase::StringPrintf(L"%s(%s)", file_type.c_str(), filter);
		std::map<LPCTSTR, LPCTSTR> filters;
		filters[text.c_str()] = filter;

		//CFileDialogEx::FileDialogCallback2 cb = nbase::Bind(&SessionBox::OnImageSelected, this, false, std::placeholders::_1, std::placeholders::_2);
		CFileDialogEx::FileDialogCallback2 cb = std::bind(&SessionBox::OnImageSelected, this, false, std::placeholders::_1, std::placeholders::_2);

		CFileDialogEx* file_dlg = new CFileDialogEx();
		file_dlg->SetFilter(filters);
		file_dlg->SetParentWnd(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND());
		file_dlg->AyncShowOpenFileDlg(cb);
	}
	else if (name == L"btn_file")
	{
		std::wstring file_type = L"文件(*.*)";
		LPCTSTR filter = L"*.*";
		std::map<LPCTSTR, LPCTSTR> filters;
		filters[file_type.c_str()] = filter;

		//CFileDialogEx::FileDialogCallback2 cb = nbase::Bind(&SessionBox::OnFileSelected, this, std::placeholders::_1, std::placeholders::_2);
		CFileDialogEx::FileDialogCallback2 cb = std::bind(&SessionBox::OnFileSelected, this, std::placeholders::_1, std::placeholders::_2);

		CFileDialogEx* file_dlg = new CFileDialogEx();
		file_dlg->SetFilter(filters);
		file_dlg->SetMultiSel(TRUE);
		file_dlg->SetParentWnd(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND());
		file_dlg->AyncShowOpenFileDlg(cb);
	}
	else if (name == L"btn_jsb")
	{
		/*int jsb = (rand() % 3 + rand() % 4 + rand() % 5) % 3 + 1;

		Json::Value json;
		Json::FastWriter writer;
		json["type"] = CustomMsgType_Jsb;
		json["data"]["value"] = jsb;

		SendJsb(writer.write(json));*/
		DBManager::GetInstance()->ShowDBManagerMainForm("153", L"wd_common", 0xffff);
	}

	return false;
}

void SessionBox::OnEmotionSelected(std::wstring emo)
{
	std::wstring file;
	if (GetEmojiFileByTag(emo, file))
	{
		InsertFaceToEdit(input_edit_, file, emo);
	}
}

void SessionBox::OnEmotionSelectedSticker(const std::wstring &catalog, const std::wstring &name)
{
	//SendSticker(nbase::UTF16ToUTF8(catalog), nbase::UTF16ToUTF8(name));
}

void SessionBox::OnEmotionClosed()
{
	CheckBtn *chk_face = (CheckBtn*)FindSubControl(L"chk_face");
	chk_face->Selected(false, false);

	input_edit_->SetFocus();
}

void SessionBox::DoClip()
{
	/*std::wstring send_info;
	CaptureManager::CaptureCallback callback = nbase::Bind(&SessionBox::OnClipCallback, this, std::placeholders::_1, std::placeholders::_2);
	std::string acc = LoginManager::GetInstance()->GetAccount();
	assert(!acc.empty());
	std::wstring app_data_audio_path = QPath::GetUserAppDataDir(acc);
	if (CaptureManager::GetInstance()->StartCapture(callback, app_data_audio_path, send_info) == false)
	{
		OnClipCallback(FALSE, L"");
	}*/
}

void SessionBox::OnClipCallback(bool ret, const std::wstring& file_path)
{
	//if (ret)
	//{
	//	emoji::InsertImageToEdit(input_edit_, file_path);
	//	//	SendImage(file_path);
	//}
};

void SessionBox::OnImageSelected(bool is_snapchat, bool ret, std::wstring file_path)
{
	if (ret)
	{
		if (!nbase::FilePathIsExist(file_path, FALSE) || 0 == nbase::GetFileSize(file_path))
			return;

		std::wstring file_ext;
		nbase::FilePathExtension(file_path, file_ext);
		nbase::LowerString(file_ext);
		if (file_ext != L".jpg" && file_ext != L".jpeg" && file_ext != L".png" && file_ext != L".bmp")
			return;

		if (!is_snapchat)
		{
			InsertImageToEdit(input_edit_, file_path, false);
		}
		else 
		{
			/*SendSnapChat(file_path);*/
		}
	}
}

void SessionBox::OnFileSelected(BOOL ret, std::wstring file_path)
{
	if (ret)
	{
		if (CheckFileSize(file_path))
		{
			InsertFileToEdit(input_edit_, file_path);
		}
		else
		{
			ShowMsgBox(nim_comp::WindowsManager::GetInstance()->GetWindow(L"MainForm", L"MainForm")->GetHWND(), MsgboxCallback(), L"文件大小不能超过15M");
		}
	}
}

HRESULT SessionBox::QueryInterface(REFIID iid, void ** ppvObject)
{
	return S_OK;
}

ULONG SessionBox::AddRef(void)
{
	return S_OK;
}

ULONG SessionBox::Release(void)
{
	return S_OK;
}

HRESULT SessionBox::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (!CheckDropEnable(pt))
	{
		*pdwEffect = DROPEFFECT_NONE;
	}
	return S_OK;
}
HRESULT SessionBox::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	return S_OK;
}
HRESULT SessionBox::DragLeave(void)
{
	return S_OK;
}

HRESULT SessionBox::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
{
	if (pDataObj)
	{
		STGMEDIUM stgMedium;
		FORMATETC cFmt = { (CLIPFORMAT)CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		HRESULT hr = pDataObj->GetData(&cFmt, &stgMedium);
		if (hr == S_OK)
		{
			//拖曳文件
			HDROP file = NULL;
			file = (HDROP)GlobalLock(stgMedium.hGlobal);
			GlobalUnlock(stgMedium.hGlobal);
			OnDropFile(file);
			::ReleaseStgMedium(&stgMedium);
			input_edit_->SetFocus();
			return S_OK;
		}
	}
	return S_OK;
}

} // namespace nim_comp


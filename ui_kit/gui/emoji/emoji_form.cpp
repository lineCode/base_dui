#include "stdafx.h"
#include "shared/closure.h"
#include "emoji_info.h"
#include "emoji_form.h"
//#include "module/emoji/emoji_info.h


using namespace dui;
namespace nim_comp
{
const LPCTSTR EmojiForm::kClassName = L"EmojiForm";

EmojiForm::EmojiForm()
{
	only_emoj_ = false;
	is_closing_ = false;
}

EmojiForm::~EmojiForm()
{

}

std::wstring EmojiForm::GetSkinFolder()
{
	return L"emoji";
}

std::wstring EmojiForm::GetSkinFile()
{
	return L"emoji_form.xml";
}

LPCTSTR EmojiForm::GetWindowClassName() const
{
	return kClassName;
}

LPCTSTR EmojiForm::GetWindowId() const
{
	return kClassName;
}

UINT EmojiForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void EmojiForm::InitWindow()
{
	auto OnOptContainerClicked = [this](TEvent *event, int index){
		printf("EmojiForm OnOptContainerClicked, name:%s\n", event->pSender->GetName().c_str());
		TabLayout *list_tabbox = static_cast<TabLayout *>(m_PaintManager.FindControl(_T("list_tabbox")));
		list_tabbox->SelectItem(index);
		return true; };
	OptionContainer *sticker_op_emoji = dynamic_cast<OptionContainer *>(m_PaintManager.FindControl(_T("sticker_op_emoji")));
	OptionContainer *sticker_op_ajmd = dynamic_cast<OptionContainer *>(m_PaintManager.FindControl(_T("sticker_op_ajmd")));
	OptionContainer *sticker_op_lt = dynamic_cast<OptionContainer *>(m_PaintManager.FindControl(_T("sticker_op_lt")));
	OptionContainer *sticker_op_xxy = dynamic_cast<OptionContainer *>(m_PaintManager.FindControl(_T("sticker_op_xxy")));
	sticker_op_emoji->AttachClick(std::bind(OnOptContainerClicked, std::placeholders::_1, 0));
	sticker_op_ajmd->AttachClick(std::bind(OnOptContainerClicked, std::placeholders::_1, 1));
	sticker_op_lt->AttachClick(std::bind(OnOptContainerClicked, std::placeholders::_1, 2));
	sticker_op_xxy->AttachClick(std::bind(OnOptContainerClicked, std::placeholders::_1, 3));
	
	emoj_ = (TileLayout*)m_PaintManager.FindControl(L"tile_emoj");
	
	if (only_emoj_)
	{
		m_PaintManager.FindControl(L"sticker_vector_container")->SetVisible(false);
	}

	std::vector<Emoticon> vec;
	GetEmoticon(vec);
	if( vec.empty() )
		return;

	for(std::vector<Emoticon>::const_iterator it = vec.begin(); it != vec.end(); it++)
	{
		ButtonContainer* box = new ButtonContainer;
		m_PaintManager.FillBox(box, L"emoji_item.xml", nullptr, &m_PaintManager);
		emoj_->Add(box);

		box->SetKeyboardEnabled(false);
		box->AttachClick(std::bind(&EmojiForm::OnEmojiClicked, this, std::placeholders::_1));
		std::wstring tag = it->tag;
		assert(tag.size() > 2);
		box->SetToolTip(tag.substr(1, tag.size() - 2).c_str());

		Control* c = box->FindSubControl(L"ctrl_emoj");
		c->SetBkImage(it->path.c_str());
	}

	//AddSticker(L"ajmd", 48);
	//AddSticker(L"lt", 20);
}

LRESULT EmojiForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KILLFOCUS)
	{
		this->Close();
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT EmojiForm::OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled)
{
	if (close_cb_)
	{
		shared::Post2UI(nbase::Bind(close_cb_));
	}
	return __super::OnClose(u, w, l, bHandled);
}

//void EmojiForm::Close(UINT nRet /*= IDOK*/)
//{
//	is_closing_ = true;
//	__super::Close(nRet);
//}

void EmojiForm::ShowEmoj(POINT pt, OnSelectEmotion sel, OnSelectSticker sel_sticker, OnEmotionClose close_cb, bool only_emoj)
{
	sel_cb_ = sel;
	sel_sticker_cb_ = sel_sticker;
	close_cb_ = close_cb;
	only_emoj_ = only_emoj;

	HWND hwnd = WindowEx::Create(NULL, EmojiForm::kClassName, WS_POPUP, 0);
	if (hwnd == NULL)
		return;

	SetWindowPos(m_hWnd, HWND_TOPMOST, pt.x, pt.y, 0, 0, SWP_NOSIZE);
	ShowWindow();
}

void EmojiForm::AddSticker(std::wstring name, int num)
{
	/*std::wstring sticker_id = nbase::StringPrintf(L"sticker_%s", name.c_str());
	TileLayout* sticker = (TileLayout*)m_PaintManager.FindControl(sticker_id.c_str());
	for (int i = 1; i <= num; i++)
	{
		ButtonContainer* box = new ButtonContainer;
		GlobalManager::FillBoxWithCache(box, L"emoji/sticker.xml");
		sticker->Add(box);
		sticker->SetData(name);

		box->SetKeyboardEnabled(false);
		box->AttachClick(nbase::Bind(&EmojiForm::OnStickerClicked, this, std::placeholders::_1));

		Control* c = box->FindSubControl(L"sticker");
		std::wstring sticker_name = nbase::StringPrintf(L"%s%.3d", name.c_str(), i);

		box->SetDataID(sticker_name);
		std::wstring dir = QPath::GetAppPath() + L"res\\stickers\\" + name + L"\\";
		std::wstring path = dir + sticker_name + L".png";
		c->SetBkImage(path);
	}*/
}

bool EmojiForm::OnSelChanged(dui::TEvent* param)
{
	/*std::wstring name = param->pSender->GetName();
	std::wstring dataid = param->pSender->GetDataID();
	int pos = name.find(L"sticker_op_");
	if (pos != -1 && !dataid.empty())
	{
		int num = 0;
		nbase::StringToInt(dataid, &num);
		if (num > 0)
		{
			AddSticker(name.substr(11), num);
			param->pSender->SetDataID(L"");
		}
	}*/
	return true;
}

bool EmojiForm::OnEmojiClicked(dui::TEvent* arg)
{
	if (is_closing_)
		return false;

	std::wstring tip = arg->pSender->GetToolTip();
	if( tip.empty() )
	{
		this->Close();
		return false;
	}

	if( sel_cb_ )
	{
		std::wstring face_tag = L"[" + tip + L"]";
		shared::Post2UI( nbase::Bind(sel_cb_, face_tag) );
	}

	this->Close();
	return false;
}

bool EmojiForm::OnStickerClicked(dui::TEvent* arg)
{
	/*if (is_closing_)
		return false;

	std::wstring id = arg->pSender->GetDataID();
	if (id.empty())
	{
		this->Close();
		return false;
	}
	std::wstring sticker_name = arg->pSender->GetParent()->GetDataID();
	if (sticker_name.empty())
	{
		this->Close();
		return false;
	}

	if (sel_sticker_cb_)
	{
		Post2UI(nbase::Bind(sel_sticker_cb_, sticker_name, id));
	}

	this->Close();*/
	return false;
}
}
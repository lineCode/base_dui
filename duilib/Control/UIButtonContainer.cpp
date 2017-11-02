#include "StdAfx.h"
//#include "UIButtonContainer.h"

namespace dui{

	ButtonContainer::ButtonContainer() :m_uButtonState(0)
	{
		SetMouseChildEnabled(false);
	}
	ButtonContainer::~ButtonContainer()
	{

	}
	LPCTSTR ButtonContainer::GetClass() const
	{
		return DUI_CTR_BUTTONCONTAINER;
	}

	LPVOID ButtonContainer::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_BUTTONCONTAINER) == 0) return static_cast<ButtonContainer*>(this);
		return __super::GetInterface(pstrName);
	}

	void ButtonContainer::DoEvent(TEvent& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pParent != NULL) m_pParent->DoEvent(event);
			else __super::DoEvent(event);
			return;
		}
		if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
		{
			if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
				if (::PtInRect(&m_rcItem, event.ptMouse)) m_uButtonState |= UISTATE_PUSHED;
				else m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_BUTTONUP)
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
				if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) Activate();
				m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_CONTEXTMENU)
		{
			if (IsContextMenuUsed() && IsEnabled()) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			if (::PtInRect(&m_rcItem, event.ptMouse)) {
				if (IsEnabled()) {
					if ((m_uButtonState & UISTATE_HOT) == 0) {
						m_uButtonState |= UISTATE_HOT;
						Invalidate();
					}
				}
			}
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			if (!::PtInRect(&m_rcItem, event.ptMouse)) {
				if (IsEnabled()) {
					if ((m_uButtonState & UISTATE_HOT) != 0) {
						m_uButtonState &= ~UISTATE_HOT;
						Invalidate();
					}
				}
				if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
			}
			else {
				if (m_pManager) m_pManager->AddMouseLeaveNeeded(this);
				return;
			}
		}
		if (event.Type == UIEVENT_SETCURSOR)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		__super::DoEvent(event);
	}

	bool ButtonContainer::Activate()
	{
		if (!Control::Activate()) return false;
#if MODE_EVENTMAP
		if (OnEvent.find(UIEVENT_CLICK) != OnEvent.cend()){
			TEvent event;
			event.Type = UIEVENT_CLICK;
			event.pSender = this;
			if (!OnEvent.find(UIEVENT_CLICK)->second(&event)){
				return false;
			}
		}
#endif
		if (m_pManager != NULL) m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
		return true;
	}
}

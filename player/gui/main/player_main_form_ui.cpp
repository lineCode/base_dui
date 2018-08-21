#include "stdafx.h"
#include "player_main_form.h"

PlayerMainForm::PlayerMainForm()
{

}

PlayerMainForm::~PlayerMainForm()
{

}

void PlayerMainForm::Notify(dui::Event& msg)
{
	return __super::Notify(msg);
}


void PlayerMainForm::OnFinalMessage(HWND hWnd)
{
	return __super::OnFinalMessage(hWnd);
}

LRESULT PlayerMainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

void PlayerMainForm::OnEsc(BOOL &bHandled)
{
	return __super::OnEsc(bHandled);
}

void PlayerMainForm::InitWindow()
{

}

void PlayerMainForm::OnClick(dui::Event& msg)
{
	return __super::OnClick(msg);
}
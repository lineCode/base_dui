#ifndef _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_TEXT_EDIT_H_
#define _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_TEXT_EDIT_H_
#pragma once

#include "duilib/UIlib.h"

class CCaptureTextEdit : public dui::RichEdit
{
public:
	CCaptureTextEdit(const dui::CDuiRect &rect);
	~CCaptureTextEdit();
	
	void InitInfos();
	virtual void HandleMessage(dui::TEvent& event);
	virtual void Paint(dui::IRenderContext* pRender, const dui::CDuiRect& rcPaint) override;
	dui::CDuiRect GetCaptureTextEditRect(){ return rc_valid_; }
	void SetDotPenColor(DWORD color){ color_pen_ = color; }
	void SetTextFinishedCallback(const StdClosure& cb) { text_finished_cb_ = cb; }

private:
	void DrawRect(HDC hDC);

private:
	dui::CDuiRect		rc_valid_;			// ��Чλ��(���ڻ��Ƶ�λ��)
	DWORD			color_pen_;			// ������ɫ
	StdClosure		text_finished_cb_;	// ����д��ʱ��Ҫִ�е��ⲿ�ص�
};
#endif  // _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_TEXT_EDIT_H_
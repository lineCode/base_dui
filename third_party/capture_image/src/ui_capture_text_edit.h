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
	dui::CDuiRect		rc_valid_;			// 有效位置(窗口绘制的位置)
	DWORD			color_pen_;			// 虚框的颜色
	StdClosure		text_finished_cb_;	// 文字写完时需要执行的外部回调
};
#endif  // _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_TEXT_EDIT_H_
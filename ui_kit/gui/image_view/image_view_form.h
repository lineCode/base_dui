#ifndef IMAGE_VIEW_FORM_H__
#define IMAGE_VIEW_FORM_H__

#include "module/window/windows_manager.h"

namespace nim_comp
{
	class UiImageView;
	class ImageViewForm : public dui::WindowImplBase
	{
	public:
		enum CursorState
		{
			CURSOR_ARROW = 0,
			CURSOR_HAND_IMAGE,
			CURSOR_HAND_MOVE
		};

		ImageViewForm(void);
		virtual ~ImageViewForm(void);

		virtual String GetSkinFolder() override;
		virtual String GetSkinFile() override;
		virtual LPCTSTR GetWindowClassName() const override{ return kClassName; };
		//virtual LPCTSTR GetWindowId() const override{ return kClassName; };
		//virtual UINT GetClassStyle() const override;
		virtual void OnFinalMessage(HWND hWnd);
		virtual dui::Control* CreateControl(LPCTSTR pstrClass);

		virtual void InitWindow() override;
		virtual void Notify(dui::TEvent& msg) override;

		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		void SaveImageDlgCallback(BOOL ret, std::wstring file_path_save); //保存图片的回调
		void ActiveWindow();  //激活窗口
#if 0
	public:
		//窗口事件
		virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

		/*
		 *	@brief 显示某一个图片
		 *	@param size true:根据图片的尺寸自动设置窗体的初始大小
		 *			    false:使用窗体的默认大小，不随图片改动
		 *	@param nosave true:禁用保存按钮
		 *				  false:启用保存按钮
		 */
		bool StartViewPic(std::wstring path_pic, bool size, bool nosave);

		//显示/隐藏图片百分比提示
		void ShowImageScale(bool show);

		//启用/禁用工具栏按钮
		void SetToolbarEnable(bool eable);
		//调整工具条工具条内控件显示状态
		void AdjustToolbarStatus();

		// 根据HitTest结果设置鼠标光标
		void SetHitTestCursor(CursorState hit_test);

		//是否在其他控件上
		bool IsOnOtherControl(POINT pt);

		//检查上一张和下一张图片按钮的状态
		void CheckNextOrPreImageBtnStatus();
		void SetNextOrPreImageBtnVisible(bool show);

		//刷新当前显示的图片数
		void UpdateImagePage();

		void MaxWindowSize();
		void RestoredWindowSize();

		std::wstring CurrentImagePath() { return image_path_; }

	private:
		//外屏坐标调整
		void ScreenToClientEx(POINT &pt);

		//根据要显示的图片的尺寸，得到应该设置的窗体宽和高，bg_corner是窗体背景图片corner属性所需的偏移量
		bool GetDefaultWindowSize(int &wnd_width, int &wnd_height, int &bg_corner);

		//图片处理辅助函数
		void GetMimeType(const std::wstring& filepath, std::wstring& mime_type_out, std::wstring& exten_out);
		std::wstring GetMimeType(GUID& clsid);
#endif
	
		static LPCTSTR		kClassName;
	private:
		UiImageView *ui_image_view_;
#if 0
	private:
		static const int	kBackgroundCorner;	//非最大化窗体状态下背景图片的阴影效果的corner属性的值
		static LPCTSTR		kRestoredWindowBg;	//非最大化窗体状态的背景图
		static LPCTSTR		kMaxWindowBg;		//最大化窗体状态的背景图

		dui::Box* back_image_;
		dui::Control *gif_image_;		//正在下载图片时显示的wait动态图
		UiImageView *ui_image_view_;

		ui::Box	*scale_info_bg_;		
		ui::Label *scale_info_;			//缩放图片时中间显示的缩放信息
		nbase::WeakCallbackFlag hide_scale_timer_weakflag_;
		ui::Button *button_preimage_;
		ui::Button *button_nextimage_;
		ui::Button *button_close_;
		ui::Button *button_close_max_;

		ui::HBox* tool_bar_;
		ui::HBox* tool_bar_btns_;
		ui::Box *num_tip_bg_;
		ui::Label* num_info_;
		ui::Button *button_normal_;
		ui::Button *button_max_;
		ui::Button *button_restore_;
		ui::Button *button_magnify_;
		ui::Button *button_reduce_;
		ui::Button *button_rotate_;
		ui::Button *button_save_;


		bool pic_moving_;			//判断是否移动图片:true 表示移动图片
		POINT point_off_;			//记录鼠标变化之前的位置
		std::wstring image_path_;	//正在查看的图片的路径
		bool show_scale_;			//打开新的图片时是否要先显示一下当前的缩放比例	
		bool show_nextorpre_btn_;	//是否显示上一个/下一个按钮
#endif
	};

}

#endif //IMAGE_VIEW_FORM_H__
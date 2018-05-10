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
		void SaveImageDlgCallback(BOOL ret, std::wstring file_path_save); //����ͼƬ�Ļص�
		void ActiveWindow();  //�����
#if 0
	public:
		//�����¼�
		virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

		/*
		 *	@brief ��ʾĳһ��ͼƬ
		 *	@param size true:����ͼƬ�ĳߴ��Զ����ô���ĳ�ʼ��С
		 *			    false:ʹ�ô����Ĭ�ϴ�С������ͼƬ�Ķ�
		 *	@param nosave true:���ñ��水ť
		 *				  false:���ñ��水ť
		 */
		bool StartViewPic(std::wstring path_pic, bool size, bool nosave);

		//��ʾ/����ͼƬ�ٷֱ���ʾ
		void ShowImageScale(bool show);

		//����/���ù�������ť
		void SetToolbarEnable(bool eable);
		//�����������������ڿؼ���ʾ״̬
		void AdjustToolbarStatus();

		// ����HitTest������������
		void SetHitTestCursor(CursorState hit_test);

		//�Ƿ��������ؼ���
		bool IsOnOtherControl(POINT pt);

		//�����һ�ź���һ��ͼƬ��ť��״̬
		void CheckNextOrPreImageBtnStatus();
		void SetNextOrPreImageBtnVisible(bool show);

		//ˢ�µ�ǰ��ʾ��ͼƬ��
		void UpdateImagePage();

		void MaxWindowSize();
		void RestoredWindowSize();

		std::wstring CurrentImagePath() { return image_path_; }

	private:
		//�����������
		void ScreenToClientEx(POINT &pt);

		//����Ҫ��ʾ��ͼƬ�ĳߴ磬�õ�Ӧ�����õĴ����͸ߣ�bg_corner�Ǵ��屳��ͼƬcorner���������ƫ����
		bool GetDefaultWindowSize(int &wnd_width, int &wnd_height, int &bg_corner);

		//ͼƬ����������
		void GetMimeType(const std::wstring& filepath, std::wstring& mime_type_out, std::wstring& exten_out);
		std::wstring GetMimeType(GUID& clsid);
#endif
	
		static LPCTSTR		kClassName;
	private:
		UiImageView *ui_image_view_;
#if 0
	private:
		static const int	kBackgroundCorner;	//����󻯴���״̬�±���ͼƬ����ӰЧ����corner���Ե�ֵ
		static LPCTSTR		kRestoredWindowBg;	//����󻯴���״̬�ı���ͼ
		static LPCTSTR		kMaxWindowBg;		//��󻯴���״̬�ı���ͼ

		dui::Box* back_image_;
		dui::Control *gif_image_;		//��������ͼƬʱ��ʾ��wait��̬ͼ
		UiImageView *ui_image_view_;

		ui::Box	*scale_info_bg_;		
		ui::Label *scale_info_;			//����ͼƬʱ�м���ʾ��������Ϣ
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


		bool pic_moving_;			//�ж��Ƿ��ƶ�ͼƬ:true ��ʾ�ƶ�ͼƬ
		POINT point_off_;			//��¼���仯֮ǰ��λ��
		std::wstring image_path_;	//���ڲ鿴��ͼƬ��·��
		bool show_scale_;			//���µ�ͼƬʱ�Ƿ�Ҫ����ʾһ�µ�ǰ�����ű���	
		bool show_nextorpre_btn_;	//�Ƿ���ʾ��һ��/��һ����ť
#endif
	};

}

#endif //IMAGE_VIEW_FORM_H__
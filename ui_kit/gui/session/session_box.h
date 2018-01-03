#pragma once
#include "module/session/session_define.h"

namespace nim_comp
{
//class AtMeView;
	class SessionBox : public dui::VerticalLayout, public IDropTarget
	{
	public:
		SessionBox(std::string id, NIMSessionType type);
		~SessionBox();

		virtual void Init() override;
		void InitRichEdit();

		//virtual bool Notify(void* param) override;
		bool OnClicked(dui::TEvent *event);

		bool CheckFileSize(const std::wstring &src);
	private:
		/** 
		* 执行截图操作		
		* @return void 无返回值
		*/
		void DoClip();

		/** 
		* 截图完毕后的回调函数
		* @param[in] ret	是否截图成功 
		* @param[in] file_path	截图文件的保存路径		
		* @return void 无返回值
		*/
		void OnClipCallback(bool ret, const std::wstring& file_path);

		/**
		* 某个表情被选择后的回调函数
		* @param[in] emo		 选择的表情
		* @return void 无返回值
		*/
		void OnEmotionSelected(std::wstring emo);

		/**
		* 某个贴图表情被选择后的回调函数
		* @param[in] catalog	贴图所在目录
		* @param[in] name		贴图名字
		* @return void 无返回值
		*/
		void OnEmotionSelectedSticker(const std::wstring &catalog, const std::wstring &name);

		/**
		* 表情选择窗体关闭后的回调函数
		* @return void 无返回值
		*/
		void OnEmotionClosed();

		void OnImageSelected(bool is_snapchat, bool ret, std::wstring file_path);

		void OnFileSelected(BOOL ret, std::wstring file_path);

		void OnDropFile(HDROP hDrop);
	protected:
		// 实现系统的文件拖拽接口
		bool CheckDropEnable(POINTL pt);
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		ULONG STDMETHODCALLTYPE AddRef(void);
		ULONG STDMETHODCALLTYPE Release(void);
		HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
		HRESULT STDMETHODCALLTYPE DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
		HRESULT STDMETHODCALLTYPE DragLeave(void);
		HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);
	private:
		dui::RichEdit*	input_edit_;

		//IRichEditOleCallbackEx *richeditolecallback_;
	};

}
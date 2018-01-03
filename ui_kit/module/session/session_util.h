#pragma once
//#include "module/msglog/msglog.h"

//#include "module/login/login_manager.h"
//#include "module/user/user_manager.h"
//#include "gui/msgbox/msgbox.h"


//会话窗口提示类型

//消息顶部浮层：网络连接提示 
#define SESSION_TIP_LINK				L"SESSION_TIP_LINK"

//消息顶部浮层：不在群中提示 
#define SESSION_TIP_LEAVE				L"SESSION_TIP_LEAVE"

//消息顶部浮层：不是好友提示 
#define SESSION_TIP_NOT_FRIEND			L"SESSION_TIP_NOT_FRIEND"

//消息内容顶部嵌入：加载更多消息历史
#define CELL_LOAD_MORE_MSG				L"CELL_LOAD_MORE_MSG"
#define CELL_BTN_LOAD_MORE_MSG			L"CELL_BTN_LOAD_MORE_MSG"

//消息内容底部嵌入：正在输入状态
#define CELL_WRITING					L"CELL_WRITING"

enum SessionTipType
{
	STT_LINK,
	STT_LEAVE,
	STT_NOT_FRIEND,
	STT_LOAD_MORE_MSG,
	STT_WRITING,
	STT_DISMISS,
};

//会话类型
enum SessionType
{
	kSessionDouble = 0,
	kSessionTeam
};

namespace nim_comp
{
// 获取会话类型
//SessionType GetSessionType(const nim::IMMessage &msg);
//
//// 获取会话id
//std::string GetSessionId(const nim::IMMessage &msg);
//
//// 气泡显示位置，左 或 右
//bool IsBubbleRight(const nim::IMMessage &msg);
//
//// 附加可获取表情对应的字符串
//std::wstring GetRichText(dui::RichEdit* re);

// 计算消息时间：会话窗口abbreviate=false；会话列表abbreviate=true
std::wstring GetMessageTime(const long long t, bool abbreviate);

bool CheckIfShowTime(const long long old_timestamp, const long long new_timestamp);

// 计算文件md5
//std::string GetFileMD5(const std::wstring &file);
//
//// 生成上传图片
//void GenerateUploadImage(const std::wstring &src, const std::wstring &dest);
//
//// 打开地理位置
//void OpenMap(const std::string& title, const std::string& content,  const std::string& point);
//
//// 判断消息中对应的资源文件是否存在（图片、语音）
//bool IsResourceExist(const nim::IMMessage &msg);



/**
* 插入表情到RichEdit
* @param[in] edit RichEdit控件指针
* @param[in] file_name 表情文件名
* @param[in] tag 表情名称
* @return void 无返回值
*/
void InsertFaceToEdit(dui::RichEdit* edit, const std::wstring &file_name, const std::wstring &tag);

/**
* 将一段文字（其中可能包含表情）插入到RichEdit中
* @param[in] edit RichEdit控件指针
* @param[in] str 文字
* @return void 无返回值
*/
void InsertTextToEdit(dui::RichEdit* edit, const std::wstring &str);

/**
* 插入图片到RichEdit
* @param[in] edit RichEdit控件指针
* @param[in] image_src 图片路径
* @param[in] loading 是否显示正在加载中的图片外观
* @return void 无返回值
*/
void InsertImageToEdit(dui::RichEdit* edit, const std::wstring& image_src, bool loading);

/**
* 插入文件到RichEdit
* @param[in] edit RichEdit控件指针
* @param[in] file_path 文件路径
* @return void 无返回值
*/
void InsertFileToEdit(dui::RichEdit* edit, const std::wstring& file_path);
}
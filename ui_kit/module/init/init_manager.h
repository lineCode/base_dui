#pragma once

//#include "nim_ui_dll.h"


namespace nim_comp
{

/** @class InitManager
  * @brief 提供nim sdk初始化接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class InitManager
{
public:
	SINGLETON_DEFINE(InitManager);

public:
	/**
	* 初始化UI组件（应该在程序初始化时调用）
	* @param[in] enable_subscribe_event	是否启用事件订阅模块
	* @return void	无返回值
	*/
	void Init(LPCTSTR dui_res_dir = _T("res\\"));

	/**
	* 清理UI组件
	* @return void	无返回值
	*/
	void Clean();

private:
	InitManager(){};
	~InitManager(){};
};

}
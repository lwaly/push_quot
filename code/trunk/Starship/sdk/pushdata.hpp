/*******************************************************************************
* Project:  行情数据推送服务
* @file     pushdata.hpp
* @brief
* @author
* @date:    2018年6月27日
* @note
* Modify history:
******************************************************************************/
#ifndef SDK_DATA_PUSH_HPP_
#define SDK_DATA_PUSH_HPP_
#include <string>
using namespace std;

typedef void(*CallbackMethod)(char str[], int lengtn); //回调app接口

//启动服务
int StartSvr(string hostip, int port, CallbackMethod CallAppFunc);

//获取报错信息
string& gerError();


//停止服务
void StopSvr();

#endif /* SDK_DATA_PUSH_HPP_ */


/*******************************************************************************
* Project:  proto
* @file     BSW Cw.h
* @brief    BSW 业务命令字定义
* @author   wfh
* @date:    2015年10月12日
* @note
* Modify history:
******************************************************************************/
#ifndef _BSW_CW_H_
#define _BSW_CW_H_

namespace mg
{

/**
 * @brief BSW 业务命令字定义
 * @note BSW 业务命令字成对出现，从1001开始编号，并且遵从奇数表示请求命令字，
 * 偶数表示应答命令字，应答命令字 = 请求命令字 + 1
 */
enum E_BSW_COMMAND
{
	CMD_UNDEFINE                        = 0,        ///< 未定义
	CMD_REQ_SYS_ERROR					= 999,		///< 系统错误请求（无意义，不会被使用）
	CMD_RSQ_SYS_ERROR					= 1000,		///< 系统错误响应

	// 用户相关命令字，如用户注册、用户登录、修改用户资料等，号段：1001~2000
	CMD_REQ_USER_GET_CONFIG             = 1001,     ///< 用户取配置请求
	CMD_RSP_USER_GET_CONFIG             = 1002,     ///< 用户取配置应答

	CMD_REQ_USER_PRE_LOGIN              = 1003,     ///< 用户预登录请求
	CMD_RSP_USER_PRE_LOGIN              = 1004,     ///< 用户预登录应答

	CMD_REQ_USER_LOGIN                  = 1005,     ///< 用户登录请求
	CMD_RSP_USER_LOGIN                  = 1006,     ///< 用户登录应答

    CMD_REQ_USER_LOGOUT                 = 1007,     ///< 用户退出请求
    CMD_RSP_USER_LOGOUT                 = 1008,     ///< 用户退出应答

    CMD_RSP_USER_KICED_OFFLINE          = 1009,     ///< 用户被迫下线通知

	//////////////////////////////////////////////////////////////////////////
	//audio(2001-3000)
	CMD_REQ_GET_QUOT_CONFIG         = 2001, // 获取行情配置
    CMD_RSP_GET_QUOT_CONFIG         = 2002, // 获取行情配置

};

}   // end of namespace mg

#endif /* _BSW_CW_H_ */

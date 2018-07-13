/*******************************************************************************
 * Project:  DataProxyClient
 * @file     DataProxySession.hpp
 * @brief    Session 访问类
 * @author   wfh
 * @date:    2017年02月07日
 * @note     封装用户 session 的接口
 * Modify history:
 ******************************************************************************/

#ifndef _DATA_PROXY_SESSION_HPP_
#define _DATA_PROXY_SESSION_HPP_

#include "SessionUserInfo.hpp"

namespace bsw
{

class DataProxySession
{
public:
	DataProxySession();
	virtual ~DataProxySession() {}

public:
	bool SetLabor(oss::OssLabor* pLabor); //使用前应该先初始化
	SessionUserInfo* GetUserSession(uint32 uiUserID); //获取用户的 Session 存储

private:
	oss::OssLabor* m_pLabor; //底层进程指针
};

}

#endif //_DATA_PROXY_SESSION_HPP_
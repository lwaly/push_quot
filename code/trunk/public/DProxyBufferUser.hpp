/*******************************************************************************
 * Project:  Public
 * @file     DProxyBufferUser.hpp
 * @brief    封装用户信息存储协议代码。
 * @author   wfh
 * @date:    2016年11月27日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef _DPROXY_BUFFER_USER_HPP_
#define _DPROXY_BUFFER_USER_HPP_

#include "Public.hpp"

namespace mg
{

class DProxyBufferUser
{
public:
	DProxyBufferUser();
    virtual ~DProxyBufferUser() {}

public:
	std::string DPROXY_BUFFER_CHECK_ACCOUNT(const std::string strAccount, uint32 uiAccountType);
	std::string DPROXY_BUFFER_UPDATE_TOKEN(uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strToken);
	std::string DPROXY_BUFFER_UPDATE_USER_IDENTYFY(uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strIdentify);
	std::string DPROXY_BUFFER_GET_USER_INFO(uint32 uiUserID);
	std::string DPROXY_BUFFER_GET_USER_ONLINE_STATUS(uint32 uiAppType, uint32 uiUserID, uint32 uiClientType);
	std::string DPROXY_BUFFER_UPDATE_USER_ONLINE_STATUS_IDENTIFY_DEVICE(
		uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strIdentify, const std::string& strDevice);
};

} /* namespace mg */

#endif /* _DPROXY_BUFFER_USER_HPP_ */

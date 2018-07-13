/*******************************************************************************
 * Project:  Logic
 * @file     SessionUserInfo.hpp
 * @brief    用户数据区
 * @author   wfh
 * @date:    2016年10月31日
 * @note     Step 访问的一级数据缓存
 * Modify history:
 ******************************************************************************/
 
#ifndef _SESSION_USER_INFO_H_
#define _SESSION_USER_INFO_H_

#include "Public.hpp"
#include "session/Session.hpp"

namespace bsw
{

class SessionUserInfo : public oss::Session
{
public:
	SessionUserInfo(uint32 uiSessionID, ev_tstamp dSessionTimeout = 60.0);
	virtual ~SessionUserInfo();
	static std::string SessionClass() { return std::string("bsw::SessionUserInfo");}

	virtual oss::E_CMD_STATUS Timeout();

	const USER_INFO& GetUserInfo() { return m_tUserInfo; }
	void SetUserInfo(const USER_INFO& tInfo) { m_tUserInfo = tInfo; }
	bool IsUserInfoLoaded() const { return (oss::SESSION_LOADED == m_cLoadUserInfo); }
	void SetUserInfoLoaded() { m_cLoadUserInfo = oss::SESSION_LOADED; }

	bool GetUserStatusInfo(uint16 usAppType, uint16 usClientType, USER_STATUS_INFO& tInfo);
	bool SetUserStatusInfo(uint16 usAppType, uint16 usClientType, const USER_STATUS_INFO& tInfo);
	

private:
	oss::uint8 m_cLoadUserInfo;       //用户信息加载状态

	USER_INFO  m_tUserInfo;           //用户信息
	USER_STATUS_INFO m_tUserStatusInfo; //用户在线信息
	std::map<uint16, std::map<uint16, USER_STATUS_INFO> > m_mapUserStatusInfo; //<app type, <client type, info> >
};

} /* namespace bsw */

#endif //_SESSION_USER_INFO_H_
/*******************************************************************************
 * Project:  Logic
 * @file     SessionUserInfo.cpp
 * @brief    用户数据区
 * @author   wfh
 * @date:    2016年10月31日
 * @note     Step 访问的一级数据缓存
 * Modify history:
 ******************************************************************************/

#include "SessionUserInfo.hpp"
#include <enumeration.pb.h>

namespace mg
{

    SessionUserInfo::SessionUserInfo(uint32 ulSessionId, ev_tstamp dSessionTimeout/* = 60.*/)
        : Session(ulSessionId, dSessionTimeout, std::string("mg::SessionUserInfo")),
        m_cLoadUserInfo(oss::SESSION_UNLOAD)
    {
    }

    SessionUserInfo::~SessionUserInfo()
    {
    }

    bool SessionUserInfo::SetUserStatusInfo(uint16 usAppType, uint16 usClientType, const USER_STATUS_INFO& tInfo)
    {
        if (usClientType <= enumeration::TYPE_CLIENT_UNKNOWN
            || usClientType >= enumeration::TYPE_CLIENT_COUNT)
            return false;

        std::map<uint16, std::map<uint16, USER_STATUS_INFO> >::iterator itrMap = m_mapUserStatusInfo.find(usAppType);
        if (itrMap == m_mapUserStatusInfo.end())
        {
            std::map<uint16, USER_STATUS_INFO> mapInfo;
            mapInfo.insert(std::pair<uint16, USER_STATUS_INFO>(usClientType, tInfo));
            m_mapUserStatusInfo.insert(std::pair<uint16, std::map<uint16, USER_STATUS_INFO> >(usAppType, mapInfo));
        }
        else
        {
            std::map<uint16, USER_STATUS_INFO>::iterator itr = itrMap->second.find(usClientType);
            if (itr == itrMap->second.end())
            {
                itrMap->second.insert(std::pair<uint16, USER_STATUS_INFO>(usClientType, tInfo));
            }
            else
            {
                itr->second = tInfo;
            }
        }

        return true;
    }

    bool SessionUserInfo::GetUserStatusInfo(uint16 usAppType, uint16 usClientType, USER_STATUS_INFO& tInfo)
    {
        if (usClientType <= enumeration::TYPE_CLIENT_UNKNOWN
            || usClientType >= enumeration::TYPE_CLIENT_COUNT)
            return false;

        std::map<uint16, std::map<uint16, USER_STATUS_INFO> >::iterator itrMap = m_mapUserStatusInfo.find(usAppType);
        if (itrMap == m_mapUserStatusInfo.end())
            return false;

        std::map<uint16, USER_STATUS_INFO>::iterator itr = itrMap->second.find(usClientType);
        if (itr == itrMap->second.end())
            return false;

        tInfo = itr->second;
        return true;
    }

    oss::E_CMD_STATUS SessionUserInfo::Timeout()
    {
        return oss::STATUS_CMD_COMPLETED;
    }
} /* namespace mg */

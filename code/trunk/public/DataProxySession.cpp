/*******************************************************************************
 * Project:  DataProxyClient
 * @file     DataProxySession.hpp
 * @brief    Session 访问类
 * @author   wfh
 * @date:    2017年02月07日
 * @note     封装用户 session 的接口
 * Modify history:
 ******************************************************************************/

#define SESSION_VALID_TIME 90.0

#include "DataProxySession.hpp"

namespace bsw
{

    DataProxySession::DataProxySession() : m_pLabor(NULL)
    {
    }

    bool DataProxySession::SetLabor(oss::OssLabor* pLabor)
    {
        m_pLabor = pLabor;
        return (m_pLabor != NULL);
    }

    SessionUserInfo* DataProxySession::GetUserSession(uint32 uiUserID)
    {
        if (0 == uiUserID || NULL == m_pLabor)
            return NULL;

        SessionUserInfo* pSession = (SessionUserInfo*)m_pLabor->GetSession(uiUserID, SessionUserInfo::SessionClass());
        if (NULL == pSession)
        {
            pSession = new SessionUserInfo(uiUserID/*, SESSION_VALID_TIME*/);
            if (!m_pLabor->RegisterCallback(pSession))
            {
                SAFE_DELETE(pSession);
                return NULL;
            }
        }

        return pSession;
    }

}

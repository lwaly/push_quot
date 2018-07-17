/*******************************************************************************
* Project:  Public
* @file     StepQuotConfigGet.cpp
* @brief    获取行情推送配置
* @author   ly
* @date:    2018年7月12日
* @note
* Modify history:
******************************************************************************/


#include "StepQuotConfigGet.hpp"

enum E_STEP
{
    STEP_CHECK_PROTOCOL = 0,
    GET_QUOT_CONFIG_LIST,
    STEP_SEND_ACK
};

namespace mg
{

    StepQuotConfigGet::StepQuotConfigGet(const CContext& oInContext)
        : StepDataProxyEvent(oInContext), m_pAck(NULL)
    {
        SetClassName("StepQuotConfigGet");
    }

    StepQuotConfigGet::~StepQuotConfigGet()
    {
        LOG4_TRACE(__FUNCTION__);
        if (NULL != m_pAck)
        {
            delete m_pAck;
        }
    }

    uint32 StepQuotConfigGet::NextStep()
    {
        uint32 uiError = m_oErrInfo.error_code();
        if (ERR_OK == uiError)
        {
            switch (m_uiStep++)
            {
            case STEP_CHECK_PROTOCOL:
                uiError = StepCheckProtocol();
                break;
            case GET_QUOT_CONFIG_LIST:
                uiError = QuotConfigGet();
                break;
            case STEP_SEND_ACK:
                SendAck();
                break;
            default:
                break;
            }

            m_oErrInfo.set_error_code(uiError);
        }

        return HandleResult();
    }

    void StepQuotConfigGet::OnQuotConfigGet(const common::errorinfo& oErrInfo, const std::map<uint32, QUOT_CONFIG> mapInfo)
    {
        LOG4_TRACE(__FUNCTION__);
        m_pAck = new protocol::QuotConfigGetRes();
        for (std::map<uint32, QUOT_CONFIG>::const_iterator iter = mapInfo.begin(); iter != mapInfo.end(); iter++)
        {
            m_pAck->set_supported_resolutions(iter->second.szSupportedResolutions);
            m_pAck->set_supports_group_request(iter->second.iSupportsGroupRequest ? true : false);
            m_pAck->set_supports_marks(iter->second.iSupportsMarks ? true : false);
            m_pAck->set_supports_search(iter->second.iSupportsSearch ? true : false);
            m_pAck->set_supports_time(iter->second.iSupportsTime ? true : false);
        }
        common::errorinfo* pErrInfo = new common::errorinfo();
        pErrInfo->set_error_code(bsw_err_code(m_oErrInfo.error_code()));
        pErrInfo->set_error_info(bsw_err_msg(m_oErrInfo.error_code()));
        m_pAck->set_allocated_errinfo(pErrInfo);

        NextStep();
    }

    bool StepQuotConfigGet::SendAck()
    {
        LOG4_TRACE(__FUNCTION__);
        return ResponseToClient(m_pAck->SerializeAsString());;
    }

    uint32 StepQuotConfigGet::StepCheckProtocol()
    {
        LOG4_TRACE(__FUNCTION__);
        protocol::QuotConfigGetReq oRequest;
        do
        {
            if (!oRequest.ParseFromString(m_oInContext.m_oInMsgBody.body()))
            {
                m_oErrInfo.set_error_code(ERR_INVALID_PROTOCOL);
                m_oErrInfo.set_error_info("parse request failed!");
                LOG4_ERROR("parse request failed!");
                break;
            }

            //登录后调用才会有 uid 
            //         LOG4_INFO("get audio type list, userid = %u,", m_oInContext.m_oBasicInfo.uid());
            // 
            //         m_pSysSession = GetUserSession(SYSTEM_ID);
            // 
            //         if (NULL == m_pSysSession)
            //         {
            //             SET_ERR_INFO(ERR_SESSION_CREATE, "create session failed！");
            //             break;
            //         }

            NextStep();

        } while (0);

        return m_oErrInfo.error_code();
    }
    uint32 StepQuotConfigGet::QuotConfigGet()
    {
        LOG4_TRACE(__FUNCTION__);
        return m_oDpClient.QuotConfigGet();
    }
} /* namespace mg */

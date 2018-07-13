/*******************************************************************************
* Project:  Public
* @file     StepQuotConfigGet.cpp
* @brief    获取行情推送配置
* @author   ly
* @date:    2018年7月12日
* @note     
* Modify history:
******************************************************************************/

#include <protocol.pb.h>
#include "StepQuotConfigGet.hpp"

enum E_STEP
{
    STEP_CHECK_PROTOCOL = 0,
    GET_QUOT_CONFIG_LIST,
    STEP_SEND_ACK
};

namespace bsw
{

StepQuotConfigGet::StepQuotConfigGet(const CContext& oInContext) 
    : StepDataProxyEvent(oInContext)
{
    SetClassName("StepQuotConfigGet");
}

StepQuotConfigGet::~StepQuotConfigGet()
{
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
        default:
            break;
        }

        m_oErrInfo.set_error_code(uiError);
    }

    return HandleResult();
}

bool StepQuotConfigGet::SendAck()
{
    LOG4_TRACE(__FUNCTION__);

    protocol::QuotConfigGetRes oAck;
    oAck.set_supported_resolutions("[1,5,15,30,60]");
    oAck.set_supports_group_request(true);
    oAck.set_supports_marks(true);
    oAck.set_supports_search(true);
    oAck.set_supports_time(true);
    common::errorinfo* pErrInfo = new common::errorinfo();
    pErrInfo->set_error_code(bsw_err_code(m_oErrInfo.error_code()));
    pErrInfo->set_error_info(bsw_err_msg(m_oErrInfo.error_code()));
    oAck.set_allocated_errinfo(pErrInfo);

    return ResponseToClient(oAck.SerializeAsString());
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
    return NextStep();
}
} /* namespace bsw */

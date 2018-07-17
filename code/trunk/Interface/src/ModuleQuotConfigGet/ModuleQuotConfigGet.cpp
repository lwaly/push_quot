/*******************************************************************************
* Project:  InterfaceServer
* @file     StepQuotConfigGet.cpp
* @brief    获取上传音频信息记录步骤
* @author   ly
* @date:    2017年01月16日
* @note
* Modify history:
******************************************************************************/

#include "BswCw.h"
#include "BswMsg.hpp"
#include "BswError.h"
#include "BswErrorMapping.h"
#include "user_base_info.pb.h"
#include "enumeration.pb.h"
#include "ModuleQuotConfigGet.hpp"

//步骤枚举值
enum E_LOGIC_STEP
{
    STEP_CHEK_PROTOCOL = 0,
    STEP_RELAY_TO_LOGIC,
    STEP_SEND_ACK,
};

namespace mg
{

    StepQuotConfigGet::StepQuotConfigGet(const CHttpContext& oInContext) : StepDataProxyEvent(oInContext)
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
            case STEP_CHEK_PROTOCOL:
                uiError = StepCheckProtocol();
                break;

            case STEP_RELAY_TO_LOGIC:
                uiError = StepRelayToLogic();
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

    uint32 StepQuotConfigGet::StepCheckProtocol()
    {
        LOG4_TRACE(__FUNCTION__);

        do
        {
            //             loss::CJsonObject oJson;
            //             if (!ParseMsgBody(m_oInContext.m_oHttpMsg.body(), oJson))
            //             {
            //                 SET_ERR_INFO(ERR_HTTP_PARAM_DECODE, "parse json failed!");
            //                 break;
            //             }

            NextStep();

        } while (0);

        return m_oErrInfo.error_code();
    }

    uint32 StepQuotConfigGet::StepRelayToLogic()
    {
        MsgBody oMsgBody;
        oMsgBody.set_session_id(m_oInContext.m_oBasicInfo.uid()); //以 uid 作为路由
        oMsgBody.set_additional(m_oInContext.m_oBasicInfo.SerializeAsString());
        oMsgBody.set_body(m_oRequest.SerializeAsString());

        return m_oDpClient.AccessToLogic(CMD_REQ_GET_QUOT_CONFIG, oMsgBody);
    }

    void StepQuotConfigGet::OnAccessToLogic(const common::errorinfo& oErrInfo, const CContext& oOutContext)
    {
        LOG4_TRACE(__FUNCTION__);
        m_oErrInfo = oErrInfo;

        if (m_oErrInfo.error_code() == ERR_OK)
        {
            protocol::QuotConfigGetRes oAck;
            if (!oOutContext.m_oInMsgBody.has_body() || !oAck.ParseFromString(oOutContext.m_oInMsgBody.body()))
            {
                SET_ERR_INFO(ERR_HTTP_PARAM_DECODE, "parse ack failed!");
            }
            else
            {
                //logic 层返回数据包，有可能处理失败。
                if (ERR_OK == oAck.errinfo().error_code())
                {
                    m_oJson.Add("code", bsw_err_code(oErrInfo.error_code()));
                    m_oJson.Add("msg", bsw_err_msg(oErrInfo.error_code()));
                    m_oJson.Add("supports_search", oAck.supports_search());
                    m_oJson.Add("supports_group_request", oAck.supports_group_request());
                    m_oJson.Add("supported_resolutions", oAck.supported_resolutions());
                    m_oJson.Add("supports_marks", oAck.supports_marks());
                    m_oJson.Add("supports_time", oAck.supports_time());
                }
                else
                {
                    SET_ERR_INFO(oAck.errinfo().error_code(), oAck.errinfo().error_info().c_str());
                }
            }
        }

        NextStep();
    }

    bool StepQuotConfigGet::SendAck()
    {
        if (ERR_OK != m_oErrInfo.error_code())
        {
            m_oJson.Add("code", bsw_err_code(m_oErrInfo.error_code()));
            m_oJson.Add("msg", bsw_err_msg(m_oErrInfo.error_code()));
        }
        return ResponseToClient(m_oJson.ToString());
    }
} /* namespace mg */
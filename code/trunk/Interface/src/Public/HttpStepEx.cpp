/*******************************************************************************
 * Project:  Public
 * @file     HttpStepEx.cpp
 * @brief    封装部分原生代码
 * @author   wfh
 * @date:    2016年10月28日
 * @note
 * Modify history:
 ******************************************************************************/

#include "HttpStepEx.hpp"
#include "util/http/http_parser.h"

namespace mg
{

    HttpStepEx::HttpStepEx(const CHttpContext& oInContext) : m_oInContext(oInContext)
    {

    }

    HttpStepEx::~HttpStepEx()
    {

    }

    oss::E_CMD_STATUS HttpStepEx::Emit(common::errorinfo& oErrInfo)
    {
        return oss::STATUS_CMD_RUNNING;
    }

    oss::E_CMD_STATUS HttpStepEx::Emit(int iErrno, const std::string& strErrMsg, const std::string& strErrShow)
    {
        return oss::STATUS_CMD_RUNNING;
    }

    oss::E_CMD_STATUS HttpStepEx::Callback(
        const oss::tagMsgShell& stMsgShell,
        const MsgHead& oInMsgHead,
        const MsgBody& oInMsgBody,
        void* data)
    {
        return oss::STATUS_CMD_COMPLETED;
    }

    oss::E_CMD_STATUS HttpStepEx::Callback(
        const oss::tagMsgShell& stMsgShell,
        const HttpMsg& oHttpMsg,
        void* data)
    {
        return oss::STATUS_CMD_COMPLETED;
    }

    oss::E_CMD_STATUS HttpStepEx::Timeout()
    {
        return oss::STATUS_CMD_FAULT;
    }

    uint32 HttpStepEx::RegisterSyncStep(mg::HttpStepEx* pStep)
    {
        if (NULL == pStep)
        {
            LOG4_ERROR("new cmd = %s error!", ClassName().c_str());
            return oss::ERR_NEW;
        }

        if (!RegisterCallback(pStep))
        {
            LOG4_ERROR("register cmd = %s, step = %s failed!", ClassName().c_str(), pStep->ClassName().c_str());
            SAFE_DELETE(pStep);
            return ERR_REG_STEP_ERROR;
        }

        common::errorinfo oErrInfo;
        uint32 uiStatus = pStep->Emit(oErrInfo);
        if (uiStatus != oss::STATUS_CMD_RUNNING)
        {
            DeleteCallback(pStep); //DeleteCallback 已经实现了释放了pStep对象

            if (uiStatus == oss::STATUS_CMD_COMPLETED)
                return ERR_OK;

            return ERR_REG_STEP_ERROR;
        }

        return ERR_OK;
    }

    bool HttpStepEx::ParseMsgBody(const std::string& strBody, loss::CJsonObject& oJson)
    {
        if (strBody.empty())
            return false;

        std::string strJsonBody(strBody);
        std::size_t uiPos = strJsonBody.find("=");
        if (uiPos != std::string::npos) //去掉=前面的标示
            strJsonBody = strJsonBody.substr(uiPos + 1);

        if (!oJson.Parse(strJsonBody))
        {
            LOG4_DEBUG("parse body failed! body = %s", strJsonBody.c_str());
            return false;
        }

        return true;
    }

    bool HttpStepEx::ResponseToClient(const std::string& strBuf)
    {
        HttpMsg oOutHttpMsg;
        oOutHttpMsg.set_type(HTTP_RESPONSE);
        oOutHttpMsg.set_status_code(200);
        oOutHttpMsg.set_http_major(m_oInContext.m_oHttpMsg.http_major());
        oOutHttpMsg.set_http_minor(m_oInContext.m_oHttpMsg.http_minor());
        oOutHttpMsg.set_body(strBuf);

        if (!SendTo(m_oInContext.m_stMsgShell, oOutHttpMsg))
        {
            LOG4_ERROR("%s : sendto client failed", ClassName().c_str());
            return false;
        }

        LOG4_DEBUG("%s : sendto client success", ClassName().c_str());
        return true;
    }

} /* namespace mg */

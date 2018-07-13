/*******************************************************************************
 * Project:  AccessServer
 * @file     StepToClient.cpp
 * @brief 
 * @author   wfh
 * @date:    2016年11月06日
 * @note
 * Modify history:
 ******************************************************************************/
#include "BswError.h"
#include "StepToClient.hpp"

namespace bsw
{

StepToClient::StepToClient(
	const oss::tagMsgShell& stMsgShell,
	const MsgHead& oInMsgHead,
	const MsgBody& oInMsgBody)
    : oss::Step(stMsgShell, oInMsgHead, oInMsgBody)
{
}

StepToClient::~StepToClient()
{
}

oss::E_CMD_STATUS StepToClient::Emit(int iErrno, const std::string& strErrMsg, const std::string& strErrShow)
{
    bool bSendResult = false;
    MsgHead oOutMsgHead = m_oReqMsgHead;
    oOutMsgHead.set_seq(GetSequence());     // 更换消息头的seq后直接转发
    if (m_oReqMsgBody.has_session_id())
    {
        char szIdentify[32] = {0};
        snprintf(szIdentify, sizeof(szIdentify), "%u", m_oReqMsgBody.session_id());
        bSendResult = Step::SendTo(szIdentify, oOutMsgHead, m_oReqMsgBody);
    }
    else if (m_oReqMsgBody.has_session())
    {
        bSendResult = Step::SendTo(m_oReqMsgBody.session(), oOutMsgHead, m_oReqMsgBody);
    }
    else
    {
        MsgBody oOutMsgBody;
        OrdinaryResponse oRes;
        oRes.set_err_no(ERR_NO_SESSION_ID_IN_MSGBODY);
        oRes.set_err_msg("no session id!");
        oOutMsgBody.set_body(oRes.SerializeAsString());
        oOutMsgHead.set_cmd(oss::CMD_RSP_SYS_ERROR);
        oOutMsgHead.set_seq(m_oReqMsgHead.seq());
        oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
        Step::SendTo(m_stReqMsgShell, oOutMsgHead, oOutMsgBody);
        return oss::STATUS_CMD_COMPLETED;
    }

    if (bSendResult)
    {
        return oss::STATUS_CMD_RUNNING;
    }
    else
    {
        MsgBody oOutMsgBody;
        OrdinaryResponse oRes;
        oRes.set_err_no(ERR_USER_OFFLINE);
        oRes.set_err_msg("user offline!");
        oOutMsgBody.set_body(oRes.SerializeAsString());
        oOutMsgHead.set_cmd(oss::CMD_RSP_SYS_ERROR);
        oOutMsgHead.set_seq(m_oReqMsgHead.seq());
        oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
        Step::SendTo(m_stReqMsgShell, oOutMsgHead, oOutMsgBody);
        return oss::STATUS_CMD_COMPLETED;
    }
}

oss::E_CMD_STATUS StepToClient::Callback(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody,
                    void* data)
{
    m_oReqMsgHead.set_cmd(oInMsgHead.cmd());
    m_oReqMsgHead.set_msgbody_len(oInMsgBody.ByteSize());
    Step::SendTo(m_stReqMsgShell, m_oReqMsgHead, oInMsgBody);
    return oss::STATUS_CMD_COMPLETED;
}

oss::E_CMD_STATUS StepToClient::Timeout()
{
    return oss::STATUS_CMD_FAULT;
}

} /* namespace bsw */

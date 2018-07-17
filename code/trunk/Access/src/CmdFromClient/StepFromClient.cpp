/*******************************************************************************
 * Project:  AccessServer
 * @file     StepFromClient.cpp
 * @brief 
 * @author   lbh
 * @date:    2015年10月21日
 * @note
 * Modify history:
 ******************************************************************************/
#include "StepFromClient.hpp"

namespace mg
{

StepFromClient::StepFromClient(
                const oss::tagMsgShell& stMsgShell,
                const MsgHead& oInMsgHead,
                const MsgBody& oInMsgBody)
    : oss::Step(stMsgShell, oInMsgHead, oInMsgBody)
{
}

StepFromClient::~StepFromClient()
{
}

oss::E_CMD_STATUS StepFromClient::Emit(int iErrno, const std::string& strErrMsg, const std::string& strErrShow)
{
    MsgHead oOutMsgHead = m_oReqMsgHead;
    oOutMsgHead.set_seq(GetSequence());     // 更换消息头的seq后直接转发
    if (m_oReqMsgBody.has_session_id())
    {
        Step::SendToWithMod("LOGIC", m_oReqMsgBody.session_id(), oOutMsgHead, m_oReqMsgBody);
    }
    else if (m_oReqMsgBody.has_session())
    {
        unsigned int uiSessionFactor = 0;
        for (unsigned int i = 0; i < m_oReqMsgBody.session().size(); ++i)
        {
            uiSessionFactor += m_oReqMsgBody.session()[i];
        }
        Step::SendToWithMod("LOGIC", uiSessionFactor, oOutMsgHead, m_oReqMsgBody);
    }
    else
    {
        Step::SendToNext("LOGIC", oOutMsgHead, m_oReqMsgBody);
    }
    return(oss::STATUS_CMD_RUNNING);
}

oss::E_CMD_STATUS StepFromClient::Callback(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody,
                    void* data)
{
    m_oReqMsgHead.set_cmd(oInMsgHead.cmd());
    m_oReqMsgHead.set_msgbody_len(oInMsgBody.ByteSize());
    Step::SendTo(m_stReqMsgShell, m_oReqMsgHead, oInMsgBody);
    return(oss::STATUS_CMD_COMPLETED);
}

oss::E_CMD_STATUS StepFromClient::Timeout()
{
//    MsgHead oOutMsgHead = m_oMsgHead;
//    MsgBody oOutMsgBody;
//    OrdinaryResponse oRes;
//    oRes.set_err_no(ERR_LOGIC_SERVER_TIMEOUT);
//    oRes.set_err_msg("logic timeout!");
//    oOutMsgBody.set_body(oRes.SerializeAsString());
//    oOutMsgHead.set_cmd(m_oMsgHead.cmd() + 1);
//    oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
//    Step::SendTo(m_stMsgShell, oOutMsgHead, oOutMsgBody);
    LOG4CPLUS_WARN_FMT(GetLogger(), "cmd %u, seq %lu, logic timeout!", m_oReqMsgHead.cmd(), m_oReqMsgHead.seq());
    return(oss::STATUS_CMD_FAULT);
}

} /* namespace mg */

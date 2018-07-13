/*******************************************************************************
 * Project:  DataProxyClient
 * @file     StepDataProxyEvent.cpp
 * @brief    存储层访问回调事件处理类
 * @author   wfh
 * @date:    2016年12月22日
 * @note     其实整个步骤逻辑都是在一个 Step 里完成的，当子逻辑 NextStep 全部处理完，那么这个 Step 就可以结束了。 
 * Modify history:
 ******************************************************************************/

#include <BswMsg.hpp>
#include <BswError.h>
#include <BswErrorMapping.h>
#include "StepDataProxyEvent.hpp"
#include "ISyncDProxyProtocol.hpp"

namespace bsw
{

StepDataProxyEvent::StepDataProxyEvent(const CHttpContext& oInContext) 
	: HttpStepEx(oInContext), m_uiStep(0), m_bHandleError(false)
{
}

oss::E_CMD_STATUS StepDataProxyEvent::Emit(common::errorinfo& oErrInfo)
{
    LOG4_TRACE(__FUNCTION__);

    m_oErrInfo = oErrInfo;
    if (m_oErrInfo.error_code() == ERR_OK)
    {
        if (!InitDpClient()) //要初始化存储层接口才能调用。
        {
            SET_ERR_INFO(ERR_INVALID_DATA, "%s : init client failed!", ClassName().c_str());
            return oss::STATUS_CMD_FAULT;
        }
    }

    if (NextStep() != ERR_OK)
        return oss::STATUS_CMD_FAULT;

    return oss::STATUS_CMD_RUNNING;
}

uint32 StepDataProxyEvent::NextStep()
{
	return m_oErrInfo.error_code();
}

bool StepDataProxyEvent::SendAck()
{
	loss::CJsonObject oJson;
	oJson.Add("code", bsw_err_code(m_oErrInfo.error_code()));
	oJson.Add("msg", bsw_err_msg(m_oErrInfo.error_code()));
	return ResponseToClient(oJson.ToString());
}

uint32 StepDataProxyEvent::HandleResult()
{
    if (m_oErrInfo.error_code() != ERR_OK && !m_bHandleError)
    {
        m_bHandleError = true; //防止 NextStep 递归
        LOG4_ERROR("%s : step = %u, err code = %u, err msg = %s", 
            ClassName().c_str(), m_uiStep, m_oErrInfo.error_code(), m_oErrInfo.error_info().c_str());
        SendAck();
    }

    return m_oErrInfo.error_code();
}

bool StepDataProxyEvent::InitDpClient()
{
    /*
    其实将 Step 的部分成员指针传递到 m_oDpClient 中，方便系统接口的调用
    因为 Step 中很多接口都是 protected 的，传 Step 指针调用不了，但又不想修改 Step 的接口为 public
    因为这样会破坏系统原来的结构，所以把数据拿出来再进行简单封装。
    */
    return m_oDpClient.InitClient(this, GetLabor(), GetLoggerPtr(), GetSequence());
}

oss::E_CMD_STATUS StepDataProxyEvent::Callback(
    const oss::tagMsgShell& stMsgShell, const MsgHead& oInMsgHead, const MsgBody& oInMsgBody, void* data)
{
    common::errorinfo oErrInfo;

    do 
    {
        if (oss::CMD_RSP_SYS_ERROR == oInMsgHead.cmd())
        {
            SET_TMP_ERR_INFO(ERR_SYSTEM_ERROR, "%s : system error!", ClassName().c_str());
            break;
        }

    } while (0);

    CContext oOutContext(stMsgShell, oInMsgHead, oInMsgBody);
    return CallbackLogic(oErrInfo, oOutContext);
}

oss::E_CMD_STATUS StepDataProxyEvent::Timeout()
{
    LOG4_TRACE(__FUNCTION__);

    common::errorinfo oErrInfo;
    oErrInfo.set_error_code(ERR_ASYNC_TIMEOUT);
    oErrInfo.set_error_info("step timeout!");

    CContext oContext;
    return CallbackLogic(oErrInfo, oContext);
}

oss::E_CMD_STATUS StepDataProxyEvent::CallbackLogic(common::errorinfo& oErrInfo, const CContext& oContext)
{
    LOG4_TRACE(__FUNCTION__);

    //处理事件
    m_oDpClient.HandleEvent(m_oDpClient.GetDProxyReqCmd()+1, oErrInfo, oContext);

    if (oErrInfo.error_code() != ERR_OK)
        return oss::STATUS_CMD_FAULT;

    if (m_oDpClient.GetReqCount() <= 0) //如果步骤结束了，就正常退出
        return oss::STATUS_CMD_COMPLETED;

    return oss::STATUS_CMD_RUNNING;
}

} /* namespace bsw */
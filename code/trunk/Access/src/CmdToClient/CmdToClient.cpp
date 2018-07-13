/*******************************************************************************
 * Project:  AccessServer
 * @file     CmdToClient.cpp
 * @brief    转发数据给客户端。
 * @author   wfh
 * @date:    2016年11月06日
 * @note     除了预登录，登录，踢人，退出等协议，其它的业务协议都通过这个 so 转发给客户端。
 * Modify history:
 ******************************************************************************/
#include "BswError.h"
#include "Public.hpp"
#include "CmdToClient.hpp"

#ifdef __cplusplus
extern "C" {
#endif
oss::Cmd* create()
{
    oss::Cmd* pCmd = new bsw::CmdToClient();
    return(pCmd);
}
#ifdef __cplusplus
}
#endif

namespace bsw
{

CmdToClient::CmdToClient() : pStepToClient(NULL)
{
}

CmdToClient::~CmdToClient()
{
}

bool CmdToClient::AnyMessage(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody)
{
	LOG4_DEBUG(__FUNCTION__);
    pStepToClient = new StepToClient(stMsgShell, oInMsgHead, oInMsgBody);
    if (NULL == pStepToClient)
    {
        LOG4_ERROR("error %d: new StepToClient() error!", oss::ERR_NEW);
        return false;
    }

    if (RegisterCallback(pStepToClient))
    {
        if (oss::STATUS_CMD_RUNNING == pStepToClient->Emit(ERR_OK))
            return true;

        DeleteCallback(pStepToClient);
        return false;
    }
    else
    {
        SAFE_DELETE(pStepToClient);
        return false;
    }

	return true;
}

} /* namespace bsw */

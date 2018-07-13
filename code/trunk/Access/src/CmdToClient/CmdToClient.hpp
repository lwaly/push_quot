/*******************************************************************************
 * Project:  AccessServer
 * @file     CmdToClient.hpp
 * @brief    转发数据给客户端。
 * @author   wfh
 * @date:    2016年11月06日
 * @note     除了预登录，登录，踢人，退出等协议，其它的业务协议都通过这个 so 转发给客户端。
 * Modify history:
 ******************************************************************************/
#ifndef _CMD_TO_CLIENT_HPP_
#define _CMD_TO_CLIENT_HPP_

#include "cmd/Cmd.hpp"
#include "StepToClient.hpp"

#ifdef __cplusplus
extern "C" {
#endif
oss::Cmd* create();
#ifdef __cplusplus
}
#endif

namespace bsw
{

class CmdToClient: public oss::Cmd
{
public:
    CmdToClient();
    virtual ~CmdToClient();

    virtual bool AnyMessage(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody);

    StepToClient* pStepToClient;
};

} /* namespace bsw */

#endif /* _CMD_TO_CLIENT_HPP_ */

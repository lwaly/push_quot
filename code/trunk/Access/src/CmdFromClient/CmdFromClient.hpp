/*******************************************************************************
 * Project:  AccessServer
 * @file     CmdFromClient.hpp
 * @brief 
 * @author   lbh
 * @date:    2015年10月21日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CMDFROMCLIENT_CMDFROMCLIENT_HPP_
#define SRC_CMDFROMCLIENT_CMDFROMCLIENT_HPP_

#include "cmd/Cmd.hpp"
#include "StepFromClient.hpp"

#ifdef __cplusplus
extern "C" {
#endif
oss::Cmd* create();
#ifdef __cplusplus
}
#endif

namespace bsw
{

class CmdFromClient: public oss::Cmd
{
public:
    CmdFromClient();
    virtual ~CmdFromClient();

    virtual bool AnyMessage(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody);

    StepFromClient* pStepFromClient;
};

} /* namespace bsw */

#endif /* SRC_CMDFROMCLIENT_CMDFROMCLIENT_HPP_ */

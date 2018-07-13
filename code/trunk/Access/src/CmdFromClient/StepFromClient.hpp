/*******************************************************************************
 * Project:  AccessServer
 * @file     StepFromClient.hpp
 * @brief 
 * @author   lbh
 * @date:    2015年10月21日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_CMDFROMCLIENT_STEPFROMCLIENT_HPP_
#define SRC_CMDFROMCLIENT_STEPFROMCLIENT_HPP_

#include "BswError.h"
#include "step/Step.hpp"

namespace bsw
{

class StepFromClient: public oss::Step
{
public:
    StepFromClient(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody);
    virtual ~StepFromClient();

    virtual oss::E_CMD_STATUS Emit(int iErrno, const std::string& strErrMsg = "", const std::string& strErrShow = "");
    virtual oss::E_CMD_STATUS Callback(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody,
                    void* data = NULL);
    virtual oss::E_CMD_STATUS Timeout();
};

} /* namespace bsw */

#endif /* SRC_CMDFROMCLIENT_STEPFROMCLIENT_HPP_ */

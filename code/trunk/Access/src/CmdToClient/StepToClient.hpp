/*******************************************************************************
 * Project:  AccessServer
 * @file     StepToClient.hpp
 * @brief 
 * @author   wfh
 * @date:    2016年11月06日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef _STEP_TO_CLIENT_HPP_
#define _STEP_TO_CLIENT_HPP_

#include "BswError.h"
#include "step/Step.hpp"

namespace mg
{

class StepToClient: public oss::Step
{
public:
    StepToClient(
		const oss::tagMsgShell& stMsgShell,
		const MsgHead& oInMsgHead,
		const MsgBody& oInMsgBody);
    virtual ~StepToClient();

    virtual oss::E_CMD_STATUS Emit(int iErrno, const std::string& strErrMsg = "", const std::string& strErrShow = "");
    virtual oss::E_CMD_STATUS Callback(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody,
                    void* data = NULL);
    virtual oss::E_CMD_STATUS Timeout();
};

} /* namespace mg */

#endif /* _STEP_TO_CLIENT_HPP_ */

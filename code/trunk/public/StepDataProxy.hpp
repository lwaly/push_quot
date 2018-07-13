/*******************************************************************************
 * Project:  Public
 * @file     StepDataProxy.hpp
 * @brief    封装部分原生代码
 * @author   wfh
 * @date:    2016年10月28日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef _STEP_DATA_PROXY_HPP_
#define _STEP_DATA_PROXY_HPP_

#include "StepEx.hpp"

namespace bsw
{

class StepDataProxy : public StepEx
{
public:
	StepDataProxy(const CContext& oInContext);
    virtual ~StepDataProxy() {}

 	virtual oss::E_CMD_STATUS CallbackLogic(
 		common::errorinfo& oErrInfo, const DataMem::MemRsp& oRsp);

    virtual oss::E_CMD_STATUS Callback(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody,
                    void* data = NULL);
};

} /* namespace bsw */

#endif /* _STEP_DATA_PROXY_EX_HPP_ */

/*******************************************************************************
* Project:  Logic
* @file     CmdQuotConfigGet.cpp
* @brief    获取行情推送配置
* @author   ly
* @date:    2018年7月12日
* @note     
* Modify history:
******************************************************************************/

#include "CmdQuotConfigGet.hpp"
#include "StepQuotConfigGet.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    oss::Cmd* create()
    {
        oss::Cmd* pCmd = new bsw::CmdQuotConfigGet();
        return(pCmd);
    }
#ifdef __cplusplus
}
#endif

namespace bsw
{
bool CmdQuotConfigGet::AnyMessageLogic(const CContext& oInContext)
{
    LOG4_TRACE(__FUNCTION__);
    common::errorinfo oErrInfo;
    return (ERR_OK == RegisterSyncStep(oErrInfo, new StepQuotConfigGet(oInContext)));
}

} /* namespace bsw */

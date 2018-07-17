/*******************************************************************************
 * Project:  DataProxyClient
 * @file     StepDataProxyEventBase.cpp
 * @brief    存储层访问回调事件处理类
 * @author   wfh
 * @date:    2017年02月14日
 * @note     事件的默认处理步骤。 
 * Modify history:
 ******************************************************************************/

#include "StepDataProxyEventBase.hpp"

namespace mg
{

StepDataProxyEventBase::StepDataProxyEventBase(const CContext& oInContext) : StepEx(oInContext)
{

}

uint32 StepDataProxyEventBase::NextStep()
{
	return m_oErrInfo.error_code();
}


void StepDataProxyEventBase::OnQuotConfigGet(const common::errorinfo& oErrInfo, const std::map<uint32, QUOT_CONFIG> mapInfo)
{
    LOG4_TRACE("%s:%s", ClassName().c_str(), __FUNCTION__);
    m_oErrInfo = oErrInfo;
    NextStep();
}

} /* namespace mg */
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

namespace bsw
{

StepDataProxyEventBase::StepDataProxyEventBase(const CContext& oInContext) : StepEx(oInContext)
{

}

uint32 StepDataProxyEventBase::NextStep()
{
	return m_oErrInfo.error_code();
}

} /* namespace bsw */
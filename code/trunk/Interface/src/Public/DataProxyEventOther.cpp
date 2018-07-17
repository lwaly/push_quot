/*******************************************************************************
 * Project:  DataProxyClient
 * @file     DataProxyEventOther.cpp
 * @brief    音频存储事件回调处理类
 * @author   wfh
 * @date:    2016年12月31日
 * @note     
 * Modify history:
 ******************************************************************************/
#include <BswError.h>
#include "ISyncDProxyProtocol.hpp"
#include "DataProxyEventOther.hpp"

namespace mg
{

DataProxyEventOther::DataProxyEventOther() : m_pSyncEvent(NULL)
{
}

bool DataProxyEventOther::Init(ISyncEvent* pEvent)
{
	m_pSyncEvent = pEvent;
	return (m_pSyncEvent != NULL);
}

int DataProxyEventOther::HandleEvent(uint32 uiCmdRsp, common::errorinfo& oErrInfo, const CContext& oContext)
{
	if (NULL == m_pSyncEvent)
	{
		oErrInfo.set_error_code(ERR_INVALID_DATA);
		oErrInfo.set_error_info("event sink is null!");
		return HR_ERROR;
	}

	switch (uiCmdRsp)
	{
	case CMD_RSP_ACCESS_TO_LOGIC:
		return HANDLE_OTHER_ACCESS_TO_LOGIC_EVENT(oErrInfo, oContext);

	default:
		return HR_UNKOWN;
	}

	return 0;
}

int DataProxyEventOther::HANDLE_OTHER_ACCESS_TO_LOGIC_EVENT(common::errorinfo& oErrInfo, const CContext& oContext)
{
	m_pSyncEvent->OnAccessToLogic(oErrInfo, oContext);
	return HR_SUCCESS;
}

} /* namespace mg */

/*******************************************************************************
 * Project:  DataProxyClient
 * @file     DataProxyEventQuot.cpp
 * @brief    处理行情存储事件
 * @author   ly
 * @date:    2018年7月14日
 * @note
 * Modify history:
 ******************************************************************************/
#include <BswError.h>
#include "ISyncDProxyProtocol.hpp"
#include "DataProxyEventQuot.hpp"

namespace mg
{

    DataProxyEventQuot::DataProxyEventQuot() : m_pSyncEvent(NULL)
    {
    }

    bool DataProxyEventQuot::Init(ISyncEvent* pEvent, oss::OssLabor* pLabor)
    {
        if (NULL == pEvent || NULL == pLabor)
            return false;

        SetLabor(pLabor);
        m_pSyncEvent = pEvent;
        return true;
    }

    int DataProxyEventQuot::HandleEvent(uint32 uiCmdRsp, common::errorinfo& oErrInfo, const DataMem::MemRsp& oRsp)
    {
        if (NULL == m_pSyncEvent)
        {
            oErrInfo.set_error_code(ERR_INVALID_DATA);
            oErrInfo.set_error_info("event sink is null!");
            return HR_ERROR;
        }

        switch (uiCmdRsp)
        {
        case CMD_DP_RSP_QUOT_CONFIG:
            return HANDLE_QUOT_CONFIG_EVENT(oErrInfo, oRsp);
        default:
            return HR_UNKOWN;
        }

        return 0;
    }


    int DataProxyEventQuot::HANDLE_QUOT_CONFIG_EVENT(common::errorinfo& oErrInfo, const DataMem::MemRsp& oRsp)
    {
        std::map<uint32, QUOT_CONFIG> mapInfo;

        if (oErrInfo.error_code() == ERR_OK)
        {
            QUOT_CONFIG tInfo;
            int id;
            for (int i = 0; i < oRsp.record_data_size(); i++)
            {
                int iIndex = 0;
                const DataMem::Record& oRecord = oRsp.record_data(i);

                id = atol(oRecord.field_info(iIndex++).col_value().c_str());
                tInfo.iSupportsSearch = atol(oRecord.field_info(iIndex++).col_value().c_str());
                tInfo.iSupportsGroupRequest = atol(oRecord.field_info(iIndex++).col_value().c_str());
                snprintf(tInfo.szSupportedResolutions, sizeof(tInfo.szSupportedResolutions), oRecord.field_info(iIndex++).col_value().c_str());
                tInfo.iSupportsMarks = atol(oRecord.field_info(iIndex++).col_value().c_str());
                tInfo.iSupportsTime = atol(oRecord.field_info(iIndex++).col_value().c_str());

                mapInfo[id] = tInfo;
            }
        }

        m_pSyncEvent->OnQuotConfigGet(oErrInfo, mapInfo);
        return HR_SUCCESS;
    }

} /* namespace mg */

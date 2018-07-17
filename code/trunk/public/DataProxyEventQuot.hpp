/*******************************************************************************
 * Project:  DataProxyClient
 * @file     DataProxyEventQuot.hpp
 * @brief    处理行情存储事件
 * @author   ly
 * @date:    2018年7月14日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef _DATA_PROXY_EVENT_QUOT_HPP_
#define _DATA_PROXY_EVENT_QUOT_HPP_

#include "Public.hpp"
#include <common.pb.h>
#include "ISyncEvent.hpp"
#include "DataProxySession.hpp"

namespace mg
{

    class DataProxyEventQuot : public DataProxySession
    {
    public:
        DataProxyEventQuot();
        virtual ~DataProxyEventQuot() {}

    public:
        bool Init(ISyncEvent* pEvent, oss::OssLabor* pLabor);

    public: //处理事件回调入口
        int HandleEvent(uint32 uiCmdRsp, common::errorinfo& oErrInfo, const DataMem::MemRsp& oRsp);

    private: //处理具体事件

        int HANDLE_QUOT_CONFIG_EVENT(common::errorinfo& oErrInfo, const DataMem::MemRsp& oRsp);
    private:
        ISyncEvent* m_pSyncEvent; //事件基类，用于回调
    };

} /* namespace mg */

#endif /* _DATA_PROXY_EVENT_QUOT_HPP_ */

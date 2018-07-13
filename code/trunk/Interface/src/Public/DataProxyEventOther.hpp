/*******************************************************************************
 * Project:  DataProxyClient
 * @file     DataProxyEventOther.hpp
 * @brief    处理非存储事件
 * @author   wfh
 * @date:    2016年12月31日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef _DATA_PROXY_EVENT_OTHER_HPP_
#define _DATA_PROXY_EVENT_OTHER_HPP_

#include "ModulePublic.hpp"
#include <common.pb.h>
#include "ISyncEvent.hpp"

namespace bsw
{

class DataProxyEventOther
{
public:
	DataProxyEventOther();
	virtual ~DataProxyEventOther() {}

public:
	bool Init(ISyncEvent* pEvent);

public: //处理事件回调入口
	int HandleEvent(uint32 uiCmdRsp, common::errorinfo& oErrInfo, const CContext& oContext);

private: //处理具体事件
	int HANDLE_OTHER_ACCESS_TO_LOGIC_EVENT(common::errorinfo& oErrInfo, const CContext& oContext);

private:
	ISyncEvent* m_pSyncEvent; //事件基类，用于回调
};

} /* namespace bsw */

#endif /* _DATA_PROXY_EVENT_OTHER_HPP_ */

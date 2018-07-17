/*******************************************************************************
 * Project:  DataProxyClient
 * @file     StepDataProxyEventBase.hpp
 * @brief    存储层访问回调事件处理类
 * @author   wfh
 * @date:    2017年02月14日
 * @note     事件的默认处理步骤。
 * Modify history:
 ******************************************************************************/
#ifndef _STEP_DATA_PROXY_EVENT_BASE_HPP_
#define _STEP_DATA_PROXY_EVENT_BASE_HPP_

#include "StepEx.hpp"
#include "ISyncEvent.hpp"
#include <common.pb.h>

namespace mg
{

    class StepDataProxyEventBase : public StepEx, public ISyncEvent
    {
    public:
        StepDataProxyEventBase(const CContext& oInContext);
        virtual ~StepDataProxyEventBase() {}
        virtual uint32 NextStep(); //下一步。

    public:

        virtual void OnQuotConfigGet(const common::errorinfo& oErrInfo, const std::map<uint32, QUOT_CONFIG> mapInfo);
    protected: //供继承者使用变量
        common::errorinfo m_oErrInfo; //错误类
    };

} /* namespace mg */

#endif /* _STEP_DATA_PROXY_EVENT_BASE_HPP_ */

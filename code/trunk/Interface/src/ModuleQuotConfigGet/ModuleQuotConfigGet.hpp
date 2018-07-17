/*******************************************************************************
* Project:  InterfaceServer
* @file     StepQuotConfigGet.hpp
* @brief    获取上传音频信息记录步骤
* @author   ly
* @date:    2017年01月16日
* @note
* Modify history:
******************************************************************************/
#ifndef _STEP_AUDIO_UPLOAD_GET_HPP_
#define _STEP_AUDIO_UPLOAD_GET_HPP_

#include "ModuleEx.hpp"
#include "StepDataProxyEvent.hpp"
#include "protocol.pb.h"

namespace mg
{
    class StepQuotConfigGet : public StepDataProxyEvent
    {
    public:
        StepQuotConfigGet(const CHttpContext& oInContext);
        virtual ~StepQuotConfigGet();

    public:
        virtual void OnAccessToLogic(const common::errorinfo& oErrInfo, const CContext& oOutContext);
        virtual bool SendAck();
    public:
        virtual uint32 NextStep(); //异步执行下一步。

        //步骤
        uint32 StepCheckProtocol(); //检查协议步骤 
        uint32 StepRelayToLogic();  //转发到逻辑服务处理

    private:
        protocol::QuotConfigGetReq m_oRequest;
        loss::CJsonObject m_oJson;
    };
} /* namespace mg */

MODULE_CLASS_MAKE(ModuleQuotConfigGet, StepQuotConfigGet)
#endif /* _STEP_AUDIO_UPLOAD_GET_HPP_ */

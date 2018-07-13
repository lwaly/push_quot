/*******************************************************************************
* Project:  Public
* @file     StepQuotConfigGet.hpp
* @brief    获取行情推送配置
* @author   ly
* @date:    2018年7月12日
* @note
* Modify history:
******************************************************************************/
#ifndef _STEP_QUOT_CONFIG_GET_HPP_
#define _STEP_QUOT_CONFIG_GET_HPP_

#include "StepDataProxyEvent.hpp"

namespace bsw
{
class StepQuotConfigGet : public StepDataProxyEvent
{
public:
    StepQuotConfigGet(const CContext& oInContext);
    virtual ~StepQuotConfigGet();
private:
    bool SendAck();  //回包给客户端。
    virtual uint32 NextStep(); //多个步骤，执行完一个步骤进行下一个。

    //步骤
    uint32 QuotConfigGet();
    uint32 StepCheckProtocol();        //检查协议步骤
};
} /* namespace bsw */

#endif /* _STEP_QUOT_CONFIG_GET_HPP_ */

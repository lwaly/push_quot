/*******************************************************************************
* Project:  Logic
* @file     CmdQuotConfigGet.hpp
* @brief    获取行情推送配置
* @author   ly
* @date:    2018年7月12日
* @note     
* Modify history:
******************************************************************************/

#ifndef _LOGIC_CMD_QUOT_CONFIG_GET_HPP_
#define _LOGIC_CMD_QUOT_CONFIG_GET_HPP_

#include "CmdLogic.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    oss::Cmd* create();
#ifdef __cplusplus
}
#endif

namespace bsw
{
class CmdQuotConfigGet : public CmdEx
{
public:
    CmdQuotConfigGet() {}
    virtual ~CmdQuotConfigGet() {}
    virtual bool AnyMessageLogic(const CContext& oInContext);
};
} /* namespace bsw */

#endif /* _LOGIC_CMD_QUOT_CONFIG_GET_HPP_ */

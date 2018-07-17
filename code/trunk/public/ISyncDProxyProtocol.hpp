/*******************************************************************************
 * Project:  DataProxyClient
 * @file     ISyncDProxyProtocol.hpp
 * @brief    存储访问接口协议功能码
 * @author   wfh
 * @date:    2016年12月23日
 * @note     协议太多，全部放在一起容易混淆，所以应该归类，细分。同时命名前缀也应该一致，有条理也就方便管理和维护。
 * Modify history:
 ******************************************************************************/

#ifndef _I_SYNC_DATA_PROXY_PROTOCOL_HPP_
#define _I_SYNC_DATA_PROXY_PROTOCOL_HPP_

namespace mg
{

    //other(33001 - 34000)
    //////////////////////////////////////////////////////////////////////////
    enum
    {
        CMD_DP_REQ_QUOT_CONFIG = 20001,         //获取行情配置请求
        CMD_DP_RSP_QUOT_CONFIG,                 //获取行情配置回调
    };

    enum
    {
        CMD_REQ_ACCESS_TO_LOGIC = 40001, //接入层转发到逻辑层请求
        CMD_RSP_ACCESS_TO_LOGIC, //接入层转发到逻辑层回调
    };

} /* namespace mg */

#endif /* _I_SYNC_DATA_PROXY_PROTOCOL_HPP_ */

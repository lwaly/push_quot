/*******************************************************************************
 * Project:  DataProxyClient
 * @file     ISyncDProxyProtocol.hpp
 * @brief    存储访问接口协议功能码
 * @author   wfh
 * @date:    2016年12月23日
 * @note
 * Modify history:
 ******************************************************************************/

#ifndef _I_SYNC_DATA_PROXY_PROTOCOL_HPP_
#define _I_SYNC_DATA_PROXY_PROTOCOL_HPP_

namespace mg
{

enum
{
	CMD_DP_UNKNOWN = 0,
	
	//audio(30001 - 30100)

	//other(40001 - 40100)
	CMD_REQ_ACCESS_TO_LOGIC          = 40001, //接入层转发到逻辑层请求
	CMD_RSP_ACCESS_TO_LOGIC          = 40002, //接入层转发到逻辑层回调
};

} /* namespace mg */

#endif /* _I_SYNC_DATA_PROXY_PROTOCOL_HPP_ */

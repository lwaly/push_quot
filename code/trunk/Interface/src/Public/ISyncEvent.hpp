/*******************************************************************************
 * Project:  DataProxyClient
 * @file     ISyncEvent.hpp
 * @brief    存储访问接口回调事件
 * @author   wfh
 * @date:    2016年12月23日
 * @note
 * Modify history:
 ******************************************************************************/

#ifndef _I_SYNC_EVENT_HPP_
#define _I_SYNC_EVENT_HPP_

#include <Public.hpp>
#include "ModulePublic.hpp"

namespace mg
{

class ISyncEvent
{
public:
	/**
	 * @brief 命令转发回调
	 * @note  
	 * @param oErrInfo 错误码
	 * @param oOutContext 返回的包体
	 * @return 错误码
	 */
	virtual void OnAccessToLogic(const common::errorinfo& oErrInfo, const CContext& oOutContext) {}
};

} /* namespace mg */

#endif /* _I_SYNC_EVENT_HPP_ */

/*******************************************************************************
 * Project:  InterfaceServer
 * @file     StepDataProxyEvent.hpp
 * @brief    存储层访问回调事件处理类
 * @author   wfh
 * @date:    2016年12月22日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef _STEP_DATA_PROXY_EVENT_HPP_
#define _STEP_DATA_PROXY_EVENT_HPP_

#include "HttpStepEx.hpp"
#include "ISyncEvent.hpp"
#include "DataProxyClient.hpp"

namespace mg
{

class StepDataProxyEvent : public HttpStepEx, public ISyncEvent
{
public:
	StepDataProxyEvent(const CHttpContext& oInContext);
    virtual ~StepDataProxyEvent() {}

	virtual oss::E_CMD_STATUS Emit(common::errorinfo& oErrInfo);
	virtual oss::E_CMD_STATUS Callback(const oss::tagMsgShell& stMsgShell, 
		const MsgHead& oInMsgHead, const MsgBody& oInMsgBody, void* data = NULL);
	virtual oss::E_CMD_STATUS CallbackLogic(common::errorinfo& oErrInfo, const CContext& oContext);
	virtual oss::E_CMD_STATUS Timeout();

	virtual uint32 NextStep(); //下一步。
	virtual bool SendAck();   //回包给客户端。
	virtual uint32 HandleResult(); //处理错误信息 m_oErrInfo

	bool    InitDpClient(); //初始化存储层调用接口。

protected: //供继承者使用变量
	uint32 m_uiStep; //当前步骤
	bool m_bHandleError; //处理错误，避免 NextStep 递归。
	common::errorinfo m_oErrInfo; //错误类
	DataProxyClient m_oDpClient; //存储请求装封
};

} /* namespace mg */

#endif /* _STEP_DATA_PROXY_EVENT_HPP_ */

/*******************************************************************************
 * Project:  DataProxyClient
 * @file     StepDataProxyEvent.hpp
 * @brief    存储层访问回调事件处理类
 * @author   wfh
 * @date:    2016年12月22日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef _STEP_DATA_PROXY_EVENT_HPP_
#define _STEP_DATA_PROXY_EVENT_HPP_

#include "StepEx.hpp"
#include <common.pb.h>
#include "DataProxyClient.hpp"
#include "StepDataProxyEventBase.hpp"

namespace bsw
{

class StepDataProxyEvent : public StepDataProxyEventBase
{
public:
	StepDataProxyEvent(const CContext& oInContext);
    virtual ~StepDataProxyEvent() {}

	virtual oss::E_CMD_STATUS Emit(common::errorinfo& oErrInfo);
	virtual oss::E_CMD_STATUS Callback(const oss::tagMsgShell& stMsgShell, 
		const MsgHead& oInMsgHead, const MsgBody& oInMsgBody, void* data = NULL);
	virtual oss::E_CMD_STATUS CallbackLogic(common::errorinfo& oErrInfo, const CContext& oContext);
	virtual oss::E_CMD_STATUS Timeout();

public:
	virtual bool SendAck(); //回包给客户端。
	virtual uint32 HandleResult(); //处理错误信息 m_oErrInfo

public:
	bool    InitDpClient(); //初始化存储层调用接口。
	common::errorinfo* GetSysCode(const common::errorinfo& oErrInfo);
protected:
	uint32 m_uiStep;                  //当前步骤
	bool m_bHandleError;              //处理错误，避免 NextStep 递归。
	DataProxyClient m_oDpClient;      //存储请求装封
	SessionUserInfo* m_pMySession;    //用户 session
	SessionUserInfo* m_pOtherSession; //其它用户 session
	SessionUserInfo* m_pSysSession;   //系统用户 session
};

} /* namespace bsw */

#endif /* _STEP_DATA_PROXY_EVENT_HPP_ */

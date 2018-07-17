/*******************************************************************************
 * Project:  Public
 * @file     StepEx.hpp
 * @brief    封装部分原生代码
 * @author   wfh
 * @date:    2016年10月28日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef _STEP_EX_HPP_
#define _STEP_EX_HPP_

#include <BswError.h>
#include <step/Step.hpp>
#include "Public.hpp"
#include <common.pb.h>
#include "SessionUserInfo.hpp"

namespace mg
{

class StepEx : public oss::Step
{
public:
	StepEx(const CContext& oInContext);

    virtual ~StepEx();
	virtual oss::E_CMD_STATUS Emit(common::errorinfo& oErrInfo);
    virtual oss::E_CMD_STATUS Callback(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody,
                    void* data = NULL);
    virtual oss::E_CMD_STATUS Timeout();

	//获取用户内存缓存
	SessionUserInfo* GetUserSession(uint32 uiUserID); 

	//注册步骤
	uint32 RegisterSyncStep(mg::StepEx* pStep);

private:
	virtual oss::E_CMD_STATUS Emit(int iErrno, const std::string& strErrMsg = "", const std::string& strErrShow = "");

public:
	bool SendToCenter(const std::string& strBuf); //与中心服务通信
	bool SendToDataProxy(const std::string& strBuf); //与存储服务通信
	bool ResponseToClient( const std::string& strBuf); //回包给客户端

public:
	std::string IntToString(uint32 uiValue);
	std::string Int64ToString(uint64 uiValue);
	std::string FormatString(const char* pString, ...);

protected:
	CContext m_oInContext;
};

} /* namespace mg */

#endif /* _STEP_EX_HPP_ */

/*******************************************************************************
 * Project:  Public
 * @file     HttpStepEx.hpp
 * @brief    封装部分原生代码
 * @author   wfh
 * @date:    2016年10月28日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef _HTTP_STEP_EX_HPP_
#define _HTTP_STEP_EX_HPP_

#include <BswError.h>
#include <step/HttpStep.hpp>
#include "ModulePublic.hpp"
#include <common.pb.h>

namespace bsw
{

class HttpStepEx : public oss::HttpStep
{
public:
	HttpStepEx(const CHttpContext& oInContext);

    virtual ~HttpStepEx();
	virtual oss::E_CMD_STATUS Emit(common::errorinfo& oErrInfo);
	virtual oss::E_CMD_STATUS Emit(int iErrno, const std::string& strErrMsg, const std::string& strErrShow);
    virtual oss::E_CMD_STATUS Callback(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody,
                    void* data = NULL);
	virtual oss::E_CMD_STATUS Callback(
		const oss::tagMsgShell& stMsgShell,
		const HttpMsg& oHttpMsg,
		void* data = NULL);
    virtual oss::E_CMD_STATUS Timeout();

	//注册步骤
	uint32 RegisterSyncStep(bsw::HttpStepEx* pStep);

	//解析数据包
	bool ParseMsgBody(const std::string& strBody, loss::CJsonObject& oJson);

	//回包给客户端
	bool ResponseToClient( const std::string& strBuf); 
	

protected:
	CHttpContext m_oInContext;
};

} /* namespace bsw */

#endif /* _HTTP_STEP_EX_HPP_ */

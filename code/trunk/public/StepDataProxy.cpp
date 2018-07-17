/*******************************************************************************
 * Project:  Public
 * @file     StepDataProxy.cpp
 * @brief    封装部分原生代码 
 * @author   wfh
 * @date:    2016年10月31日
 * @note
 * Modify history:
 ******************************************************************************/

#include "StepDataProxy.hpp"

namespace mg
{

StepDataProxy::StepDataProxy(const CContext& oInContext) : StepEx(oInContext)
{
}

oss::E_CMD_STATUS StepDataProxy::CallbackLogic(
	common::errorinfo& oErrInfo, const DataMem::MemRsp& oRsp)
{
	return oss::STATUS_CMD_COMPLETED;
}

oss::E_CMD_STATUS StepDataProxy::Callback(
	const oss::tagMsgShell& stMsgShell,
	const MsgHead& oInMsgHead,
	const MsgBody& oInMsgBody,
	void* data)
{
	DataMem::MemRsp oRsp;
	common::errorinfo oErrInfo;

	do 
	{
		if (oss::CMD_RSP_SYS_ERROR == oInMsgHead.cmd())
		{
			LOG4_ERROR("system error, failed to set userid = %u token!");
			oErrInfo.set_error_code(ERR_SYSTEM_ERROR);
			oErrInfo.set_error_info("system error");
			break;
		}

		if (!oRsp.ParseFromString(oInMsgBody.body()))
		{
			LOG4_ERROR("parse protobuf data fault!");
			oErrInfo.set_error_code(ERR_PARASE_PROTOBUF);
			oErrInfo.set_error_info("parse protobuf data fault!");
			break;
		}

		//读存储出错
		if (oRsp.err_no() != ERR_OK)
		{
			LOG4_ERROR("dataproxy error %u : %s!", oRsp.err_no(), oRsp.err_msg().c_str());
			oErrInfo.set_error_code(ERR_DATA_PROXY_CALLBACK);
			oErrInfo.set_error_info("data proxy callback error!");
			break;
		}

	} while (0);

	return CallbackLogic(oErrInfo, oRsp);
}

} /* namespace mg */

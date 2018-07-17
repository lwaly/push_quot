/*******************************************************************************
 * Project:  Public
 * @file     CmdEx.cpp
 * @brief    为了封装底层一些细节和原生代码。
 * @author   wfh
 * @date:    2016年10月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#include "CmdEx.hpp"
#include <enumeration.pb.h>

namespace mg
{

CmdEx::CmdEx()
{
}

CmdEx::~CmdEx()
{
}

bool CmdEx::AnyMessage(
	const oss::tagMsgShell& stMsgShell,
	const MsgHead& oInMsgHead,
	const MsgBody& oInMsgBody)
{
	LOG4_DEBUG(__FUNCTION__);
	CContext oInContext(stMsgShell, oInMsgHead, oInMsgBody);
	return AnyMessageLogic(oInContext);
}

bool CmdEx::AnyMessageLogic(const CContext& oInContext)
{
	return true;
}

bool CmdEx::AnyMessageLogic(common::errorinfo& oErrInfo, const CContext& oInContext)
{
	return true;
}

uint32 CmdEx::RegisterSyncStep(mg::StepEx* pStep)
{
	if (NULL == pStep)
	{
		LOG4_ERROR("new cmd = %s error!", ClassName().c_str());
		return oss::ERR_NEW;
	}

	if (!RegisterCallback(pStep))
	{
		LOG4_ERROR("register cmd = %s, step = %s failed!", ClassName().c_str(), pStep->ClassName().c_str());
		SAFE_DELETE(pStep);
		return ERR_REG_STEP_ERROR;
	}

	common::errorinfo oErrInfo;
	if (pStep->Emit(oErrInfo) != oss::STATUS_CMD_RUNNING)
	{
		DeleteCallback(pStep); //DeleteCallback 已经实现了释放了pStep对象
		return ERR_REG_STEP_ERROR;
	}

	return ERR_OK;
}

uint32 CmdEx::RegisterSyncStep(common::errorinfo& oErrInfo, mg::StepEx* pStep)
{
	if (NULL == pStep)
	{
		LOG4_ERROR("new cmd = %s error!", ClassName().c_str());
		return oss::ERR_NEW;
	}

	if (!RegisterCallback(pStep))
	{
		LOG4_ERROR("register cmd = %s, step = %s failed!", ClassName().c_str(), pStep->ClassName().c_str());
		SAFE_DELETE(pStep);
		return ERR_REG_STEP_ERROR;
	}

	//错误需要处理，然后发回包，所以放在 Step 里面去处理。
	if (pStep->Emit(oErrInfo) != oss::STATUS_CMD_RUNNING)
	{
		DeleteCallback(pStep); //DeleteCallback 已经实现了释放了pStep对象
		return ERR_REG_STEP_ERROR;
	}

	return ERR_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CmdEx::ResponseToClient(const CContext& oInContext, const std::string& strBuf)
{
	MsgHead oOutMsgHead;
	MsgBody oOutMsgBody;
	oOutMsgBody.set_body(strBuf);
	oOutMsgHead.set_cmd(oInContext.m_oInMsgHead.cmd() + 1);
	oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
	oOutMsgHead.set_seq(oInContext.m_oInMsgHead.seq());

	return GetLabor()->SendTo(oInContext.m_stMsgShell, oOutMsgHead, oOutMsgBody);
}

std::string CmdEx::IntToString(uint32 uiValue)
{
	std::ostringstream ss;
	ss << uiValue;
	return ss.str();
}

std::string CmdEx::Int64ToString(uint64 uiValue)
{
	std::ostringstream ss;
	ss << uiValue;
	return ss.str();
}

std::string CmdEx::FormatString(const char* pString, ...)
{
	va_list argList;
	std::string strFormat;
	char szFormat[512] = {0};

	va_start(argList, pString);
	vsnprintf(szFormat, 512, pString, argList);
	va_end(argList);
	strFormat = szFormat;

	return strFormat;
}

} /* namespace mg */

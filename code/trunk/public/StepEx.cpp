/*******************************************************************************
 * Project:  Public
 * @file     StepEx.cpp
 * @brief    封装部分原生代码 
 * @author   wfh
 * @date:    2016年10月28日
 * @note
 * Modify history:
 ******************************************************************************/

#include "StepEx.hpp"

#define SESSION_VALID_TIME 300.0

namespace mg
{

StepEx::StepEx(const CContext& oInContext) 
	: oss::Step(oInContext.m_stMsgShell, oInContext.m_oInMsgHead, oInContext.m_oInMsgBody),
	m_oInContext(oInContext)
{

}

StepEx::~StepEx()
{
	
}

oss::E_CMD_STATUS StepEx::Emit(common::errorinfo& oErrInfo)
{
	return oss::STATUS_CMD_RUNNING;
}

oss::E_CMD_STATUS StepEx::Emit(int iErrno, const std::string& strErrMsg, const std::string& strErrShow)
{
	return oss::STATUS_CMD_RUNNING;
}

oss::E_CMD_STATUS StepEx::Callback(
	const oss::tagMsgShell& stMsgShell,
	const MsgHead& oInMsgHead,
	const MsgBody& oInMsgBody,
	void* data)
{
	return oss::STATUS_CMD_COMPLETED;
}

oss::E_CMD_STATUS StepEx::Timeout()
{
    return oss::STATUS_CMD_FAULT;
}

bool StepEx::SendToCenter(const std::string& strBuf)
{
	if (strBuf.empty()) 
	{
		LOG4_ERROR("send buffer is empty!");
		return false;
	}

	MsgHead oMsgHead;
	MsgBody oMsgBody;
	oMsgBody.set_body(strBuf);
	oMsgHead.set_cmd(oss::CMD_REQ_GET_LOAD_MIN_SERVER);
	oMsgHead.set_msgbody_len(oMsgBody.ByteSize());
	oMsgHead.set_seq(GetSequence());

	if (!SendToNext("CENTER", oMsgHead, oMsgBody))
		return false;

	return true;
}

SessionUserInfo* StepEx::GetUserSession(uint32 uiUserID)
{
	if (0 == uiUserID)
	{
		LOG4_DEBUG("invalid userid 0!");
		return NULL;
	}

	SessionUserInfo* pSession = (SessionUserInfo*)GetLabor()->GetSession(uiUserID, SessionUserInfo::SessionClass());
	if (NULL == pSession)
	{
		pSession = new SessionUserInfo(uiUserID/*, SESSION_VALID_TIME*/);
		if (!RegisterCallback(pSession))
		{
			LOG4_DEBUG("register session error!");
			SAFE_DELETE(pSession);
			return NULL;
		}
	}

	return pSession;
}

uint32 StepEx::RegisterSyncStep(mg::StepEx* pStep)
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
	uint32 uiStatus = pStep->Emit(oErrInfo);
	if (uiStatus != oss::STATUS_CMD_RUNNING)
	{
		DeleteCallback(pStep); //DeleteCallback 已经实现了释放了pStep对象

		if (uiStatus == oss::STATUS_CMD_COMPLETED)
			return ERR_OK;

		return ERR_REG_STEP_ERROR;
	}

	return ERR_OK;
}

bool StepEx::SendToDataProxy(const std::string& strBuf)
{
	if (strBuf.empty()) 
	{
		LOG4_ERROR("send buffer is empty!");
		return false;
	}

	MsgHead oMsgHead;
	MsgBody oMsgBody;
	oMsgBody.set_body(strBuf);
	oMsgHead.set_cmd(oss::CMD_REQ_STORATE);
	oMsgHead.set_msgbody_len(oMsgBody.ByteSize());
	oMsgHead.set_seq(GetSequence());

	if (!SendToNext("PROXY", oMsgHead, oMsgBody))
	{
		LOG4_ERROR("send to dataproxy error!");
		return false;
	}

	return true;
}

bool StepEx::ResponseToClient(const std::string& strBuf)
{
	MsgHead oOutMsgHead;
	MsgBody oOutMsgBody;
	oOutMsgBody.set_body(strBuf);
	oOutMsgHead.set_cmd(m_oInContext.m_oInMsgHead.cmd() + 1);
	oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
	oOutMsgHead.set_seq(m_oInContext.m_oInMsgHead.seq());

	return GetLabor()->SendTo(m_oInContext.m_stMsgShell, oOutMsgHead, oOutMsgBody);
}

std::string StepEx::IntToString(uint32 uiValue)
{
	std::ostringstream ss;
	ss << uiValue;
	return ss.str();
}

std::string StepEx::Int64ToString(uint64 uiValue)
{
	std::ostringstream ss;
	ss << uiValue;
	return ss.str();
}

std::string StepEx::FormatString(const char* pString, ...)
{
	va_list argList;
	std::string strFormat;
	char szFormat[1024] = {0};

	va_start(argList, pString);
	vsnprintf(szFormat, 512, pString, argList);
	va_end(argList);
	strFormat = szFormat;

	return strFormat;
}

} /* namespace mg */

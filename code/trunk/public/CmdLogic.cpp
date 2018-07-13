/*******************************************************************************
 * Project:  Public
 * @file     CmdLogic.cpp
 * @brief    为了封装底层一些细节和原生代码，封装了逻辑的部分用户信息。注意这是登录成功后才能调用的
 * @author   wfh
 * @date:    2016年10月21日
 * @note     
 * Modify history:
 ******************************************************************************/
#include "CmdLogic.hpp"
#include <enumeration.pb.h>
#include <user_base_info.pb.h>

namespace bsw
{

CmdLogic::CmdLogic()
{
}

CmdLogic::~CmdLogic()
{
}

bool CmdLogic::AnyMessage(
	const oss::tagMsgShell& stMsgShell,
	const MsgHead& oInMsgHead,
	const MsgBody& oInMsgBody)
{
	LOG4_DEBUG(__FUNCTION__);

	common::errorinfo oErrInfo;
	CContext oInContext(stMsgShell, oInMsgHead, oInMsgBody);
	
	do 
	{
		if (!oInMsgBody.has_additional())
		{
			std::string strErrMsg = FormatString("no additional data! cmd = %u", oInMsgHead.cmd());
			SET_TMP_ERR_INFO(ERR_NO_ADDITIONAL_IN_MSGBODY, strErrMsg.c_str());
			break;
		}

		//解析基础数据包
		user_base_info oUserBasicInfo;
		if (!oUserBasicInfo.ParseFromString(oInMsgBody.additional()))
		{
			std::string strErrMsg = FormatString("cmd = %u user basic info parse failed!", oInMsgHead.cmd());
			SET_TMP_ERR_INFO(ERR_MSG_BODY_DECODE, strErrMsg.c_str());
			break;
		}

		oInContext.m_oBasicInfo = oUserBasicInfo;

	} while (0);

	AnyMessageLogic(oInContext);
	return AnyMessageLogic(oErrInfo, oInContext);
}

} /* namespace bsw */
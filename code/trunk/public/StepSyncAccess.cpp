/*******************************************************************************
 * Project:  Public
 * @file     StepSyncAccess.cpp
 * @brief    用户信息存储访问步骤
 * @author   wfh
 * @date:    2016年11月20日
 * @note
 * Modify history:
 ******************************************************************************/
#include <enumeration.pb.h>
#include "StepSyncAccess.hpp"

enum E_SYNC_ACCESS_COMMAND
{
    DP_CMD_UNKOWN = 0,
    DP_CMD_CHECK_ACCOUNT,
    DP_CMD_UPDATE_TOKEN,
    DP_CMD_UPDATE_USER_IDENTIFY,
    DP_CMD_GET_USER_INFO,
    DP_CMD_GET_USER_ONLINE_STATUS,
    DP_CMD_UPDATE_ONLINE_STATUS_IDENTIFY_DEVICE,
    DP_CMD_COUNT,

    SYNC_CMD_UNKOWN = 100,
    SYNC_CMD_MIN_LOAD_ACCESS_SERVER,
    SYNC_CMD_ACCESS_TO_SERVER,
    SYNC_CMD_COUNT,
};

namespace bsw
{

StepSyncAccess::StepSyncAccess(const CContext& oInContext) 
	: StepEx(oInContext), m_uiDProxyCmd(0)
{
}
 
//兼容存储的，还有非存储的
oss::E_CMD_STATUS StepSyncAccess::Callback(
	const oss::tagMsgShell& stMsgShell, const MsgHead& oInMsgHead, const MsgBody& oInMsgBody, void* data)
{
	common::errorinfo oErrInfo;

	if (m_uiDProxyCmd > DP_CMD_UNKOWN && m_uiDProxyCmd < DP_CMD_COUNT)
	{
		DataMem::MemRsp oRsp;

		do 
		{
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
	else if (m_uiDProxyCmd > SYNC_CMD_UNKOWN && m_uiDProxyCmd < SYNC_CMD_COUNT)
	{
		return CallbackLogic(stMsgShell, oInMsgHead, oInMsgBody);
	}
	else
	{
		LOG4_ERROR("system error, failed to set userid = %u token!");
		oErrInfo.set_error_code(ERR_SYSTEM_ERROR);
		oErrInfo.set_error_info("system error");
		return oss::STATUS_CMD_FAULT;
	}

	return oss::STATUS_CMD_COMPLETED;
}

oss::E_CMD_STATUS StepSyncAccess::CallbackLogic(
	const oss::tagMsgShell& stMsgShell, const MsgHead& oInMsgHead, const MsgBody& oInMsgBody)
{
	common::errorinfo oErrInfo;
	uint32 uiCurDProxyCmd = m_uiDProxyCmd;
	m_uiDProxyCmd = SYNC_CMD_UNKOWN; //在回调中会执行 NextStep，导致 m_uiDProxyCmd 被修改。
	LOG4_DEBUG("sync cmd = %u", uiCurDProxyCmd);

	switch (uiCurDProxyCmd)
	{
	case SYNC_CMD_MIN_LOAD_ACCESS_SERVER:
		{
			int iPort = 0;
			int iError = 0;
			std::string strAccessIp;
			loss::CJsonObject oRspJson;

			do 
			{
				if (!oRspJson.Parse(oInMsgBody.body()))
				{
					LOG4_ERROR("parse center jason failed!");
					oErrInfo.set_error_code(ERR_PARSE_PACK_ERROR);
					oErrInfo.set_error_info("parse center jason failed!");
					break;
				}

				if (!oRspJson.Get("errcode", iError) 
					|| (iError != ERR_OK)
					|| !oRspJson.Get("outerport", iPort) 
					|| !oRspJson.Get("outerip", strAccessIp))
				{
					LOG4_ERROR("miss param from center!");
					oErrInfo.set_error_code(ERR_PARSE_PACK_ERROR);
					oErrInfo.set_error_info("miss param from center!");
					break;
				}

			} while (0);

			OnSyncGetMinLoadAccessServer(oErrInfo, strAccessIp, iPort);
		}
		break;
	case SYNC_CMD_ACCESS_TO_SERVER:
		{
			CContext oOutContext(stMsgShell, oInMsgHead, oInMsgBody);
			OnSyncAccessToLogic(oErrInfo, oOutContext);
		}
		break;
	default:
		{
			oErrInfo.set_error_code(ERR_STEP_RUNNING_ERROR);
			oErrInfo.set_error_info("undefine sysnc command!");
			LOG4_ERROR("undefine sync command!");
		}
		break;
	}

	return oss::STATUS_CMD_COMPLETED;
}

oss::E_CMD_STATUS StepSyncAccess::CallbackLogic(common::errorinfo& oErrInfo, const DataMem::MemRsp& oRsp)
{
	LOG4_TRACE(__FUNCTION__);

	uint32 uiCurDProxyCmd = m_uiDProxyCmd;
	m_uiDProxyCmd = DP_CMD_UNKOWN; //在回调中会执行 NextStep，导致 m_uiDProxyCmd 被修改。

	LOG4_DEBUG("data proxy cmd = %u", uiCurDProxyCmd);

	switch (uiCurDProxyCmd)
	{
	case DP_CMD_CHECK_ACCOUNT:
		{	
			uint32 uiUserID = 0;
			uint32 uiAccountID = 0;
			for (int i=0; i < oRsp.record_data_size(); i++)
			{
				const DataMem::Record& oRecord = oRsp.record_data(i);
				//uiUserID = strtoul(oRecord.field_info(0).col_value().c_str(), NULL, 10);
				uiAccountID = strtoul(oRecord.field_info(1).col_value().c_str(), NULL, 10);
                uiUserID = uiAccountID; //modify by wenfahua 2017-11-24
			}

			if (0 == uiUserID)
			{
				oErrInfo.set_error_code(ERR_USER_NOT_EXIST);
				oErrInfo.set_error_info("user not exist");
				LOG4_ERROR("user not exist!");
			}

 			OnSyncDProxyCheckAcount(oErrInfo, uiUserID, uiAccountID);
		}
		break;
	case DP_CMD_UPDATE_TOKEN:
		{	
			if (oErrInfo.error_code() == ERR_OK)
			{
			    USER_STATUS_INFO tInfo;
			    SessionUserInfo* pSession = GetUserSession(m_tStatusInfo.uiUserID);
			    if (pSession != NULL && pSession->GetUserStatusInfo(m_tStatusInfo.usAppType, m_tStatusInfo.usClientType, tInfo))
			    {
				    snprintf(tInfo.szToken, sizeof(tInfo.szToken), "%s", m_tStatusInfo.szToken);
				    pSession->SetUserStatusInfo(m_tStatusInfo.usAppType, m_tStatusInfo.usClientType, tInfo);
			    }
			}

 			OnSyncDProxyUpdateToken(oErrInfo, std::string(m_tStatusInfo.szToken));
		}
		break;
	case DP_CMD_UPDATE_USER_IDENTIFY:
		{	
			if (oErrInfo.error_code() == ERR_OK)
			{
			    USER_STATUS_INFO tInfo;
			    SessionUserInfo* pSession = GetUserSession(m_tStatusInfo.uiUserID);
			    if (pSession != NULL && pSession->GetUserStatusInfo(m_tStatusInfo.usAppType, m_tStatusInfo.usClientType, tInfo))
			    {
			        snprintf(tInfo.szIdentifyApp, sizeof(tInfo.szIdentifyApp), "%s", m_tStatusInfo.szIdentifyApp);
			        pSession->SetUserStatusInfo(m_tStatusInfo.usAppType, m_tStatusInfo.usClientType, tInfo);
			    }
			}

			OnSyncDProxyUpdateUserIdentify(oErrInfo);
		}
		break;
	case DP_CMD_GET_USER_INFO:
		{	
			USER_INFO tInfo;
			//解析从存储中读取到的数据
			for (int i=0; i < oRsp.record_data_size(); i++)
			{
				uint32 uiIndex = 0;
				const DataMem::Record& oRecord = oRsp.record_data(i);

				tInfo.uiUserID = strtoul(oRecord.field_info(uiIndex++).col_value().c_str(), NULL, 10);
				snprintf(tInfo.szAccount, sizeof(tInfo.szAccount), oRecord.field_info(uiIndex++).col_value().c_str());
				snprintf(tInfo.szNickName, sizeof(tInfo.szNickName), oRecord.field_info(uiIndex++).col_value().c_str());
				snprintf(tInfo.szPassWord, sizeof(tInfo.szPassWord), oRecord.field_info(uiIndex++).col_value().c_str());
				tInfo.cSex = strtoul(oRecord.field_info(uiIndex++).col_value().c_str(), NULL, 10);
				snprintf(tInfo.szMobile, sizeof(tInfo.szMobile), oRecord.field_info(uiIndex++).col_value().c_str());
				snprintf(tInfo.szSignature, sizeof(tInfo.szSignature), oRecord.field_info(uiIndex++).col_value().c_str());

                //modify by wenfahua 2017-11-24
                tInfo.uiUserID = atol(tInfo.szAccount);
				SessionUserInfo* pSession = GetUserSession(tInfo.uiUserID);
				if (pSession != NULL)
				{
					pSession->SetUserInfo(tInfo);
					pSession->SetUserInfoLoaded();
				}
			}

			OnSyncDProxyGetUserInfo(oErrInfo, tInfo);
		}
		break;
	case DP_CMD_GET_USER_ONLINE_STATUS:
		{	
			USER_STATUS_INFO tInfo = m_tStatusInfo;
			//解析从存储中读取到的数据
			for (int i=0; i < oRsp.record_data_size(); i++)
			{
				uint32 uiIndex = 0;
				const DataMem::Record& oRecord = oRsp.record_data(i);
				snprintf(tInfo.szToken, sizeof(tInfo.szToken), oRecord.field_info(uiIndex++).col_value().c_str());
				snprintf(tInfo.szIdentifyApp, sizeof(tInfo.szIdentifyApp), oRecord.field_info(uiIndex++).col_value().c_str());
				snprintf(tInfo.szDeviceInfo, sizeof(tInfo.szDeviceInfo), oRecord.field_info(uiIndex++).col_value().c_str());

				SessionUserInfo* pSession = GetUserSession(m_tStatusInfo.uiUserID);
				if (pSession != NULL) 
				{
					pSession->SetUserStatusInfo(m_tStatusInfo.usAppType, m_tStatusInfo.usClientType, tInfo);
				}
			}

			OnSyncDProxyGetUserOnlineStatus(oErrInfo, tInfo);
		}
		break;
	case DP_CMD_UPDATE_ONLINE_STATUS_IDENTIFY_DEVICE:
		{	
			if (oErrInfo.error_code() == ERR_OK)
			{
				USER_STATUS_INFO tInfo;
				SessionUserInfo* pSession = GetUserSession(m_tStatusInfo.uiUserID);
				if (pSession != NULL && pSession->GetUserStatusInfo(m_tStatusInfo.usAppType, m_tStatusInfo.usClientType, tInfo))
				{
					snprintf(tInfo.szDeviceInfo, sizeof(tInfo.szDeviceInfo), "%s", m_tStatusInfo.szDeviceInfo);
					snprintf(tInfo.szIdentifyApp, sizeof(tInfo.szIdentifyApp), "%s", m_tStatusInfo.szIdentifyApp);
					pSession->SetUserStatusInfo(m_tStatusInfo.usAppType, m_tStatusInfo.usClientType, tInfo);
				}
			}
			OnSyncDProxyUpdateUserOnlineStatusIdentifyAndDevice(oErrInfo);
		}
		break;
	default:
		{
			oErrInfo.set_error_code(ERR_STEP_RUNNING_ERROR);
			oErrInfo.set_error_info("undefine data proxy command!");
			LOG4_ERROR("undefine data proxy command!");
		}
		break;
	}

	if (oErrInfo.error_code() != ERR_OK)
		return oss::STATUS_CMD_FAULT;

	if (DP_CMD_UNKOWN == m_uiDProxyCmd) //如果步骤结束了，就正常退出
		return oss::STATUS_CMD_COMPLETED;

	return oss::STATUS_CMD_RUNNING;
}

oss::E_CMD_STATUS StepSyncAccess::Timeout()
{
	LOG4_TRACE(__FUNCTION__);

	common::errorinfo oErrInfo;
	oErrInfo.set_error_code(ERR_ASYNC_TIMEOUT);
	oErrInfo.set_error_info("step timeout!");

	switch (m_uiDProxyCmd)
	{
	case DP_CMD_CHECK_ACCOUNT:
		{
			OnSyncDProxyCheckAcount(oErrInfo, 0, 0);
		}
		break;
	case DP_CMD_UPDATE_TOKEN:
		{
			OnSyncDProxyUpdateToken(oErrInfo, std::string(m_tStatusInfo.szToken));
		}
		break;
	case DP_CMD_UPDATE_USER_IDENTIFY:
		{	
			OnSyncDProxyUpdateUserIdentify(oErrInfo);
		}
		break;
	case DP_CMD_GET_USER_INFO:
		{	
			USER_INFO tUserInfo;
			OnSyncDProxyGetUserInfo(oErrInfo, tUserInfo);
		}
		break;
	case DP_CMD_GET_USER_ONLINE_STATUS:
		{	
			USER_STATUS_INFO tInfo;
			OnSyncDProxyGetUserOnlineStatus(oErrInfo, tInfo);
		}
		break;
	case DP_CMD_UPDATE_ONLINE_STATUS_IDENTIFY_DEVICE:
		{	
			OnSyncDProxyUpdateUserOnlineStatusIdentifyAndDevice(oErrInfo);
		}
		break;
	case SYNC_CMD_MIN_LOAD_ACCESS_SERVER:
		{	
			OnSyncGetMinLoadAccessServer(oErrInfo, "", 0);
		}
		break;
	case SYNC_CMD_ACCESS_TO_SERVER:
		{	
			CContext oOutContext;
			OnSyncAccessToLogic(oErrInfo, oOutContext);
		}
		break;
	default:
		{
			LOG4_ERROR("undefine data proxy command!");
		}
		break;
	}

	return oss::STATUS_CMD_FAULT;
}

uint32 StepSyncAccess::SyncDProxyCheckAccount(const std::string strAccount, uint32 uiAccountType)
{
	LOG4_TRACE(__FUNCTION__);

	if (strAccount.empty() 
		|| uiAccountType <= enumeration::TYPE_LOGIN_BY_UNKNOWN 
		|| uiAccountType >= enumeration::TYPE_LOGIN_BY_COUNT)
	{
		LOG4_ERROR("invalid data!, account = %s, type = %u", strAccount.c_str(), uiAccountType);
		return ERR_INVALID_DATA;
	}

	std::string strDProxyBuffer = m_oDProxyBufUser.DPROXY_BUFFER_CHECK_ACCOUNT(strAccount, uiAccountType);
	if (!SendToDataProxy(strDProxyBuffer))
		return ERR_SEND_TO_DATAPROXY;

	m_uiDProxyCmd = DP_CMD_CHECK_ACCOUNT;
	return ERR_OK;
}

uint32 StepSyncAccess::SyncDProxyUpdateToken(
	uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strToken)
{
	if (0 == uiUserID || strToken.empty()
		|| uiClientType <= enumeration::TYPE_CLIENT_UNKNOWN 
		|| uiClientType >= enumeration::TYPE_CLIENT_COUNT)
	{
		LOG4_ERROR("invalid data!, userid = %u, app type = %u, client type = %u", 
			uiUserID, uiAppType, uiClientType);
		return ERR_INVALID_DATA;
	}

	m_tStatusInfo.Reset();
	m_tStatusInfo.uiUserID = uiUserID;
	m_tStatusInfo.usAppType = uiAppType;
	m_tStatusInfo.usClientType = uiClientType;
	snprintf(m_tStatusInfo.szToken, sizeof(m_tStatusInfo.szToken), "%s", strToken.c_str());

	std::string strDProxyBuffer = m_oDProxyBufUser.DPROXY_BUFFER_UPDATE_TOKEN(uiAppType, uiUserID, uiClientType, strToken);
	if (!SendToDataProxy(strDProxyBuffer))
		return ERR_SEND_TO_DATAPROXY;

	m_uiDProxyCmd = DP_CMD_UPDATE_TOKEN;
	return ERR_OK;
}

uint32 StepSyncAccess::SyncDProxyUpdateUserIdentify(
	uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strIdentify)
{
	if (0 == uiUserID
		|| uiClientType <= enumeration::TYPE_CLIENT_UNKNOWN 
		|| uiClientType >= enumeration::TYPE_CLIENT_COUNT)
	{
		LOG4_ERROR("invalid data!, userid = %u, app type = %u, client type = %u", 
			uiUserID, uiAppType, uiClientType);
		return ERR_INVALID_DATA;
	}

	m_tStatusInfo.Reset();
	m_tStatusInfo.uiUserID = uiUserID;
	m_tStatusInfo.usAppType = uiAppType;
	m_tStatusInfo.usClientType = uiClientType;
	snprintf(m_tStatusInfo.szIdentifyApp, sizeof(m_tStatusInfo.szIdentifyApp), "%s", strIdentify.c_str());

	std::string strDProxyBuffer = m_oDProxyBufUser.DPROXY_BUFFER_UPDATE_USER_IDENTYFY(uiAppType, uiUserID, uiClientType, strIdentify);
	if (!SendToDataProxy(strDProxyBuffer))
		return ERR_SEND_TO_DATAPROXY;

	m_uiDProxyCmd = DP_CMD_UPDATE_USER_IDENTIFY;
	return ERR_OK;
}

uint32 StepSyncAccess::SyncDProxyGetUserInfo(uint32 uiUserID)
{
	if (0 == uiUserID)
	{
		LOG4_ERROR("invalid data!, userid = %u", uiUserID);
		return ERR_INVALID_DATA;
	}

	m_tStatusInfo.Reset();
	m_tStatusInfo.uiUserID = uiUserID;

	std::string strDProxyBuffer = m_oDProxyBufUser.DPROXY_BUFFER_GET_USER_INFO(uiUserID);
	if (!SendToDataProxy(strDProxyBuffer))
		return ERR_SEND_TO_DATAPROXY;

	m_uiDProxyCmd = DP_CMD_GET_USER_INFO;
	return ERR_OK;
}

uint32 StepSyncAccess::SyncDProxyGetUserOnlineStatus(uint32 uiAppType, uint32 uiUserID, uint32 uiClientType)
{
	if (0 == uiUserID
		|| uiClientType <= enumeration::TYPE_CLIENT_UNKNOWN 
		|| uiClientType >= enumeration::TYPE_CLIENT_COUNT)
	{
		LOG4_ERROR("invalid data!, userid = %u, app type = %u, client type = %u", 
			uiUserID, uiAppType, uiClientType);
		return ERR_INVALID_DATA;
	}

	m_tStatusInfo.Reset();
	m_tStatusInfo.uiUserID = uiUserID;
	m_tStatusInfo.usAppType = uiAppType;
	m_tStatusInfo.usClientType = uiClientType;

	std::string strDProxyBuffer = m_oDProxyBufUser.DPROXY_BUFFER_GET_USER_ONLINE_STATUS(uiAppType, uiUserID, uiClientType);
	if (!SendToDataProxy(strDProxyBuffer))
		return ERR_SEND_TO_DATAPROXY;

	m_uiDProxyCmd = DP_CMD_GET_USER_ONLINE_STATUS;
	return ERR_OK;
}

uint32 StepSyncAccess::SyncDProxyUpdateUserOnlineStatusIdentifyAndDevice(
	uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strIdentify, const std::string& strDevice)
{
	if (0 == uiUserID || strIdentify.empty() || strDevice.empty()
		|| uiClientType <= enumeration::TYPE_CLIENT_UNKNOWN 
		|| uiClientType >= enumeration::TYPE_CLIENT_COUNT)
	{
		LOG4_ERROR("invalid data!, userid = %u, app type = %u, client type = %u", 
			uiUserID, uiAppType, uiClientType);
		return ERR_INVALID_DATA;
	}

	m_tStatusInfo.Reset();
	m_tStatusInfo.uiUserID = uiUserID;
	m_tStatusInfo.usAppType = uiAppType;
	m_tStatusInfo.usClientType = uiClientType;
	snprintf(m_tStatusInfo.szIdentifyApp, sizeof(m_tStatusInfo.szIdentifyApp), "%s", strIdentify.c_str());
	snprintf(m_tStatusInfo.szDeviceInfo, sizeof(m_tStatusInfo.szDeviceInfo), "%s", strDevice.c_str());

	std::string strDProxyBuffer = 
		m_oDProxyBufUser.DPROXY_BUFFER_UPDATE_USER_ONLINE_STATUS_IDENTIFY_DEVICE(uiAppType, uiUserID, uiClientType, strIdentify, strDevice);
	if (!SendToDataProxy(strDProxyBuffer))
		return ERR_SEND_TO_DATAPROXY;

	m_uiDProxyCmd = DP_CMD_UPDATE_ONLINE_STATUS_IDENTIFY_DEVICE;
	return ERR_OK;
}

uint32 StepSyncAccess::SyncGetMinLoadAccessServer()
{
	loss::CJsonObject oReqJson;
	oReqJson.Add("servertype", "ACCESS");
	if (!SendToCenter(oReqJson.ToString()))
	{
		LOG4_ERROR("send to center error!");
		return ERR_PRELOGIN_QRY_IDENTITY;
	}

	m_uiDProxyCmd = SYNC_CMD_MIN_LOAD_ACCESS_SERVER;
	return ERR_OK;
}

uint32 StepSyncAccess::SyncAccessToLogic()
{
	LOG4_TRACE(__FUNCTION__);

	MsgHead oOutMsgHead = m_oInContext.m_oInMsgHead;
	oOutMsgHead.set_seq(GetSequence());     // 更换消息头的seq后直接转发

	if (m_oInContext.m_oInMsgBody.has_session_id())
	{
		Step::SendToWithMod("LOGIC", m_oInContext.m_oInMsgBody.session_id(), oOutMsgHead, m_oInContext.m_oInMsgBody);
	}
	else if (m_oInContext.m_oInMsgBody.has_session())
	{
		unsigned int uiSessionFactor = 0;
		for (unsigned int i=0; i < m_oInContext.m_oInMsgBody.session().size(); i++)
			uiSessionFactor += m_oInContext.m_oInMsgBody.session()[i];

		Step::SendToWithMod("LOGIC", uiSessionFactor, oOutMsgHead, m_oInContext.m_oInMsgBody);
	}
	else
	{
		Step::SendToNext("LOGIC", oOutMsgHead, m_oInContext.m_oInMsgBody);
	}

	m_uiDProxyCmd = SYNC_CMD_ACCESS_TO_SERVER;
	return ERR_OK;
}

} /* namespace bsw */

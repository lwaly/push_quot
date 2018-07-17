/*******************************************************************************
 * Project:  InterfaceServer
 * @file     ModuleEx.cpp
 * @brief    为了封装HTTP逻辑底层一些细节和原生代码。
 * @author   wfh
 * @date:    2016年12月22日
 * @note     
 * Modify history:
 ******************************************************************************/
#include "ModuleEx.hpp"
#include <common.pb.h>
#include <enumeration.pb.h>
#include <user_base_info.pb.h>
#include "HttpParamCodec.hpp"
#include "Public.hpp"

namespace mg
{

ModuleEx::ModuleEx()
{
}

ModuleEx::~ModuleEx()
{
}

bool ModuleEx::AnyMessage(const oss::tagMsgShell& stMsgShell, const HttpMsg& oInHttpMsg)
{
	LOG4_DEBUG(__FUNCTION__);

	uint32 uiUserID = 0;
	std::string strToken;
	common::errorinfo oErrInfo;

	do 
	{
		if (!oInHttpMsg.has_method())
		{
			SET_TMP_ERR_INFO(ERR_HTTP_NO_METHOD, "no method");
			break;
		}

		if (oInHttpMsg.method() != HTTP_POST)
		{
			SET_TMP_ERR_INFO(ERR_HTTP_METHOD, "http method wrong");
			break;
		}

		if (!oInHttpMsg.has_body())//从oInHttpMsg.Body()中取参数
		{
			SET_TMP_ERR_INFO(ERR_HTTP_NO_BODY, "http param no body");
			break;
		}

		if (oInHttpMsg.headers_size() <= 0)
		{
			SET_TMP_ERR_INFO(ERR_HTTP_NO_HEADER, "http header is empty");
			break;
		}

		//验证 cookie 数据
		//if (!GetCookieInfo(oErrInfo, oInHttpMsg, strToken, uiUserID))
			//break;

		//官方的 ID 应该是统一过来的，不能传它们的管理员过来。
		//现在为了调试方便，统一将小于 SYSTEM_ID 的账号转化为 OFFICAL_ID
		//if (uiUserID < SYSTEM_ID) uiUserID = OFFICAL_ID;

	} while (0);

	CHttpContext oInContext(stMsgShell, oInHttpMsg);
	if (oErrInfo.error_code() == ERR_OK)
	{
		user_base_info oUserInfo;
		oUserInfo.set_uid(1000);
		oUserInfo.set_app_type(1);
		oUserInfo.set_client_type(enumeration::TYPE_CLIENT_WEB);
		oInContext.m_oBasicInfo = oUserInfo;
	}

	return AnyMessageLogic(oErrInfo, oInContext);
}

bool ModuleEx::AnyMessageLogic(common::errorinfo& oErrInfo, const CHttpContext& oInContext)
{
	return true;
}

uint32 ModuleEx::RegisterSyncStep(common::errorinfo& oErrInfo, mg::HttpStepEx* pStep)
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

bool ModuleEx::GetCookieInfo(
	common::errorinfo& oErrInfo, const HttpMsg& oInHttpMsg, std::string& strToken, uint32& uiUserID)
{
	const std::string& strMsgBody = oInHttpMsg.body();
	if (strMsgBody.empty())
	{
		SET_TMP_ERR_INFO(ERR_HTTP_NO_BODY, "msg body is empty!");
		return false;
	}

	std::string strJsonBody(strMsgBody);
	std::size_t uiPos = strMsgBody.find("=");

	if (uiPos != std::string::npos) //去掉=前面的标示
		strJsonBody = strMsgBody.substr(uiPos+1);

	LOG4_DEBUG("json body(%s)", strJsonBody.c_str());

	loss::CJsonObject oHttpJson;
	if (!oHttpJson.Parse(strJsonBody))
	{
		SET_TMP_ERR_INFO(ERR_JSON_PRASE_FAILED, "parse json body failed!");
		return false;
	}

	HttpParamCodec oHttpParse;
	//解析 cookie
	//////////////////////////////////////////////////////////////////////////
	for (int i=0; i < oInHttpMsg.headers_size(); i++)
	{
		if (oInHttpMsg.headers(i).header_name() != HTTP_KEY_COOKIE)
			continue;

		if (!oHttpParse.decodeCookie(oInHttpMsg.headers(i).header_value()))
		{
			SET_TMP_ERR_INFO(ERR_HTTP_NO_COOKIE, "no cookie!");
			return false;
		}
	}

	if (!oHttpParse.getCookieParam(HTTP_KEY_TOKEN, strToken))
	{
		SET_TMP_ERR_INFO(ERR_HTTP_COOKIE_PARAM_DECODE, "no token!");
		return false;
	}

	std::string strUserID;
	if (!oHttpParse.getCookieParam(HTTP_KEY_USERID, strUserID))
	{
		SET_TMP_ERR_INFO(ERR_HTTP_COOKIE_PARAM_DECODE, "no userid!");
		return false;
	}

	uiUserID = strtoul(strUserID.c_str(), NULL, 10);
	if (0 == uiUserID)
	{
		SET_TMP_ERR_INFO(ERR_HTTP_COOKIE_PARAM_DECODE, "userid is 0!");
		return false;
	}

	return true;
}

} /* namespace mg */

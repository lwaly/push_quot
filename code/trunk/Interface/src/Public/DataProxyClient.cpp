/*******************************************************************************
 * Project:  DataProxyClient
 * @file     DataProxyClient.hpp
 * @brief    封装存储访问接口的类
 * @author   wfh
 * @date:    2016年12月31日
 * @note     
 * Modify history:
 ******************************************************************************/

#include <BswError.h>
#include "DataProxyClient.hpp"
#include "ISyncDProxyProtocol.hpp"

namespace bsw
{

DataProxyClient::DataProxyClient() 
	: m_uiDProxyCmd(0), m_uiSequenece(0), m_pLabor(NULL), m_pLogger(NULL), m_bDataProxyEvent(true), m_iReqCount(0)
{

}

bool DataProxyClient::InitClient(ISyncEvent* pEvent, oss::OssLabor* pLabor, log4cplus::Logger* pLogger, uint32 uiSeq)
{
	if (NULL == pEvent || NULL == pLabor || NULL == pLogger || 0 == uiSeq)
		return false;

	m_pLabor = pLabor;
	m_pLogger = pLogger;
	m_uiSequenece = uiSeq;

	if (!m_oDpEventOther.Init(pEvent))
	{
		LOG4_ERROR("init data proxy other failed!");
		return false;
	}

	return true;
}

bool DataProxyClient::IsValid()
{
	if (NULL == m_pLabor || NULL == m_pLogger || 0 == m_uiSequenece)
		return false; //不能 log，因为 log 调用这些成员变量

	return true;
}

int DataProxyClient::HandleEvent(uint32 uiCmdRsp, common::errorinfo& oErrInfo, const CContext& oContext)
{
	int iResult = HR_UNKOWN;

	LOG4_DEBUG("cmd = %u", uiCmdRsp);

	m_iReqCount--;
	bool bDataProxyEvent = GetDataProxyEvent();
	SetDataProxyEvent(true);

	if (bDataProxyEvent)
	{
		do 
		{
			DataMem::MemRsp oRsp;

			if (oErrInfo.error_code() == ERR_OK)
			{
				if (!oContext.m_oInMsgBody.has_body() 
					|| !oRsp.ParseFromString(oContext.m_oInMsgBody.body()))
				{
					SET_TMP_ERR_INFO(ERR_PARASE_PROTOBUF, "parse protobuf data fault!");
					break;
				}

				//读存储出错
				if (oRsp.err_no() != ERR_OK)
				{
					SET_TMP_ERR_INFO(ERR_DATA_PROXY_CALLBACK, "data proxy callback error!");
					break;
				}
			}
			
			if (HR_UNKOWN != iResult)
				break;

		} while (0);
	}
	else
	{
		do 
		{
			iResult = m_oDpEventOther.HandleEvent(uiCmdRsp, oErrInfo, oContext);
			if (HR_UNKOWN != iResult)
				break;

		} while (0);
	}

	if (HR_UNKOWN == iResult) //找不到相关的协议
	{
		SET_TMP_ERR_INFO(ERR_STEP_RUNNING_ERROR, "undefine data proxy command!");
	}
	
	return iResult;
}

bool DataProxyClient::SendToNext(const std::string& strNodeType, const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
	return(m_pLabor->SendToNext(strNodeType, oMsgHead, oMsgBody));
}

bool DataProxyClient::SendToWithMod(const std::string& strNodeType, unsigned int uiModFactor, const MsgHead& oMsgHead, const MsgBody& oMsgBody)
{
    return(m_pLabor->SendToWithMod(strNodeType, uiModFactor, oMsgHead, oMsgBody));
}

bool DataProxyClient::SendToDataProxy(uint32 uiReqCmd, const std::string& strBuf)
{
	if (strBuf.empty()) 
	{
		LOG4_ERROR("send buffer is empty!");
		return false;
	}

	/*
	  每次只支持一种类型的命令（可以多个同类型的命令），因为所有的步骤其实都在一个 Step 里执行的，只是先后不一样而已。
	  在相同的 Step 里，那么这个step 的 seq 是一样的，所以无法同时执行多个不同类型的步骤。
	  请查看 StepDataProxyEvent::CallbackLogic 的实现。
	  否则多个不同类型的 Step 发出去回包有可能造成解包失败，导致崩溃，所以下面会对接口使用进行安全检查。
	*/
	if (m_iReqCount > 0 && m_uiDProxyCmd != uiReqCmd)
	{
		LOG4_ERROR("do not next until current step is completed!");
		return false;
	}

	MsgHead oMsgHead;
	MsgBody oMsgBody;
	oMsgBody.set_body(strBuf);
	oMsgHead.set_cmd(oss::CMD_REQ_STORATE);
	oMsgHead.set_msgbody_len(oMsgBody.ByteSize());
 	oMsgHead.set_seq(m_uiSequenece);
 
	if (!SendToNext("PROXY", oMsgHead, oMsgBody))
	{
		LOG4_ERROR("send to dataproxy error!");
		return false;
	}

	m_uiDProxyCmd = uiReqCmd;
	m_iReqCount++;
	return true;
}

uint32 DataProxyClient::AccessToLogic(uint32 uiReqCmd, const MsgBody& oMsgBody)
{
    LOG4_TRACE(__FUNCTION__);

    if (0 == uiReqCmd || 0 == oMsgBody.ByteSize())
    {
        LOG4_ERROR("invalid data!");
        return ERR_INVALID_DATA;
    }

    MsgHead oMsgHead;
    oMsgHead.set_cmd(uiReqCmd);
    oMsgHead.set_seq(m_uiSequenece);
    oMsgHead.set_msgbody_len(oMsgBody.ByteSize());

    bool bSendResult = false;

    if (oMsgBody.has_session_id())
    {
        bSendResult = SendToWithMod("LOGIC", oMsgBody.session_id(), oMsgHead, oMsgBody);
    }
    else if (oMsgBody.has_session())
    {
        unsigned int uiSessionFactor = 0;
        for (unsigned int i=0; i < oMsgBody.session().size(); i++)
            uiSessionFactor += oMsgBody.session()[i];

        bSendResult = SendToWithMod("LOGIC", uiSessionFactor, oMsgHead, oMsgBody);
    }
    else
    {
        bSendResult = SendToNext("LOGIC", oMsgHead, oMsgBody);
    }

    if (!bSendResult)
    {
        LOG4_ERROR("send to logic error!");
        return ERR_SEND_TO_LOGIG_MSG;
    }

    m_iReqCount++;
    SetDataProxyEvent(false); //这个不是存储的接口
    m_uiDProxyCmd = CMD_REQ_ACCESS_TO_LOGIC;
    return ERR_OK;
}

} /* namespace bsw */
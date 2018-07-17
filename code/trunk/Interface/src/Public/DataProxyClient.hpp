/*******************************************************************************
 * Project:  DataProxyClient
 * @file     DataProxyClient.hpp
 * @brief    封装存储访问接口的类
 * @author   wfh
 * @date:    2016年12月31日
 @note     
 * Modify history:
 ******************************************************************************/
#ifndef _DATA_PROXY_CLIENT_HPP_
#define _DATA_PROXY_CLIENT_HPP_

#include <Public.hpp>
#include "ModulePublic.hpp"
#include "DataProxyEventOther.hpp"
#include "step/Step.hpp"

namespace mg
{

class DataProxyClient
{
public:
	DataProxyClient();
	~DataProxyClient() {}

	//必须初始化成功才能使用。
	bool InitClient(ISyncEvent* pEvent, oss::OssLabor* pLabor, log4cplus::Logger* pLogger, uint32 uiSeq);
	log4cplus::Logger GetLogger() { return (*m_pLogger); }

public:
	int    HandleEvent(uint32 uiCmdRsp, common::errorinfo& oErrInfo, const CContext& oContext); //处理存储事件

public: 
	//access
	virtual uint32 AccessToLogic(uint32 uiReqCmd, const MsgBody& oMsgBody);

public:
	uint32 GetDProxyReqCmd() { return m_uiDProxyCmd; }
	int    GetReqCount() { return m_iReqCount; }

private:
	bool   IsValid(); //接口操作前，判断是否有效

	//设置事件类别
	void   SetDataProxyEvent(bool bDataProxy) { m_bDataProxyEvent = bDataProxy; }
	bool   GetDataProxyEvent() { return m_bDataProxyEvent; }

	//发送数据
	bool   SendToDataProxy(uint32 uiReqCmd, const std::string& strBuf); //发送到存储服务器
	bool   SendToNext(const std::string& strNodeType, const MsgHead& oMsgHead, const MsgBody& oMsgBody); //发送到下一个服务器结点
    bool   SendToWithMod(const std::string& strNodeType, unsigned int uiModFactor, const MsgHead& oMsgHead, const MsgBody& oMsgBody); //根据 sessionid 发送到指定的服务器结点

private:
	DataProxyEventOther m_oDpEventOther;	 //存储事件以外的逻辑

private:
	uint32 m_uiDProxyCmd; //存储读写自定义业务命令
	uint32 m_uiSequenece; //序列号
	oss::OssLabor* m_pLabor; //底层进程指针
	log4cplus::Logger* m_pLogger; //底层日志指针
	bool m_bDataProxyEvent; //存储服务器响应步骤。为了方便，会把小量（几个）的非存储访问步骤放进这个类。
	int m_iReqCount; //正在执行请求个数（支持同类型的多个请求同时进行）。
};

} /* namespace mg */

#endif /* _DATA_PROXY_CLIENT_HPP_ */

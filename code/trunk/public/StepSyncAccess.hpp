/*******************************************************************************
 * Project:  Public
 * @file     StepSyncAccess.hpp
 * @brief    用户信息存储访问步骤
 * @author   wfh
 * @date:    2016年11月27日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef _STEP_SYNC_ACCESS_HPP_
#define _STEP_SYNC_ACCESS_HPP_

#include "StepEx.hpp"
#include "DProxyBufferUser.hpp"

namespace bsw
{

class StepSyncAccess : public StepEx
{
public:
	StepSyncAccess(const CContext& oInContext);

    virtual ~StepSyncAccess() {}
	virtual oss::E_CMD_STATUS Timeout();
	virtual oss::E_CMD_STATUS Callback(const oss::tagMsgShell& stMsgShell, 
		const MsgHead& oInMsgHead, const MsgBody& oInMsgBody, void* data = NULL);
	virtual oss::E_CMD_STATUS CallbackLogic(common::errorinfo& oErrInfo, const DataMem::MemRsp& oRsp);
	virtual oss::E_CMD_STATUS CallbackLogic(
		const oss::tagMsgShell& stMsgShell, const MsgHead& oInMsgHead, const MsgBody& oInMsgBody);

public: //request
	/**
	 * @brief 检查用户账号
	 * @note  根据用户账号，从存储中找出相应的 uid
	 * @param strAccount 账号
	 * @param uiAccountType 客户端版本信息（enumeration.E_ACCOUNT_LOGIN_TYPE）
	 * @return 错误码
	 */
	virtual uint32 SyncDProxyCheckAccount(const std::string strAccount, uint32 uiAccountType);

	/**
	 * @brief 更新用户验证码
	 * @note  验证码在用户状态的 redis 数据里保存
	 * @param uiAppType 应用类型（enumeration.E_APP_TYPE）
	 * @param uiUserID 用户 id
	 * @param uiClientType 客户端类型（enumeration.E_CLIENT_TYPE）
	 * @param strToken 验证码
	 * @return 错误码
	 */
	virtual uint32 SyncDProxyUpdateToken(uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strToken);
	
	/**
	 * @brief 更新用户保存的接入服务器标识
	 * @note  每个用户登录成功后，都会将其登录的接入服务器写入 redis，方便数据跨进程跨机器路由
	 * @param uiAppType 应用类型（enumeration.E_APP_TYPE）
	 * @param uiUserID 用户 id
	 * @param uiClientType 客户端类型（enumeration.E_CLIENT_TYPE）
	 * @param strIdentify 接入标识
	 * @return 错误码
	 */
	virtual uint32 SyncDProxyUpdateUserIdentify(uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strIdentify);

	/**
	 * @brief 获取用户信息
	 * @note  
	 * @param uiUserID 用户 id
	 * @return 错误码
	 */
	virtual uint32 SyncDProxyGetUserInfo(uint32 uiUserID);
	
	/**
	 * @brief 获取用户在线状态信息
	 * @note  
	 * @param uiUserID 用户 id
	 * @return 错误码
	 */
	virtual uint32 SyncDProxyGetUserOnlineStatus(uint32 uiAppType, uint32 uiUserID, uint32 uiClientType);


	/**
	 * @brief 更新用户在线状态信息接入标识和物理设备
	 * @note  
	 * @param uiAppType 应用类型（enumeration.E_APP_TYPE）
	 * @param uiUserID 用户 id
	 * @param uiClientType 客户端类型（enumeration.E_CLIENT_TYPE）
	 * @param strIdentify 客户端连接接入服务标识
	 * @param strDevice 客户端物理设备
	 * @return 错误码
	 */
	virtual uint32 SyncDProxyUpdateUserOnlineStatusIdentifyAndDevice(
		uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strIdentify, const std::string& strDevice);

	//////////////////////////////////////////////////////////////////////////
	

	/**
	 * @brief 向中心获取负载最小的接入服务器
	 * @note  
	 * @return 错误码
	 */
	virtual uint32 SyncGetMinLoadAccessServer();

	/**
	 * @brief 接入服务器转到逻辑服务器
	 * @note  
	 * @return 错误码
	 */
	virtual uint32 SyncAccessToLogic();

public:
	/**
	 * @brief 检查用户账号回调
	 * @note  
	 * @param uiUserID 用户 id
	 * @param uiAccountID 哼哼唧唧号
	 * @return 错误码
	 */
	virtual void OnSyncDProxyCheckAcount(const common::errorinfo& oErrInfo, uint32 uiUserID, uint32 uiAccountID) {}

	/**
	 * @brief 更新用户验证码回调
	 * @note  
	 * @param oErrInfo 错误信息
	 * @return
	 */
	virtual void OnSyncDProxyUpdateToken(const common::errorinfo& oErrInfo, const std::string& strToken) {}

	/**
	 * @brief 更新用户保存的接入服务器标识回调
	 * @param oErrInfo 错误信息
	 * @return 
	 */
	virtual void OnSyncDProxyUpdateUserIdentify(const common::errorinfo& oErrInfo) {}

	/**
	 * @brief 获取用户在线信息回调
	 * @param oErrInfo 错误信息
	 * @param USER_STATUS_INFO 用户在线状态信息
	 * @return 
	 */
	virtual void OnSyncDProxyGetUserOnlineStatus(const common::errorinfo& oErrInfo, const USER_STATUS_INFO& tInfo) {}

	/**
	 * @brief 更新用户在线状态信息客户端标识和物理设备记录
	 * @note  
	 * @return 
	 */
	virtual void OnSyncDProxyUpdateUserOnlineStatusIdentifyAndDevice(const common::errorinfo& oErrInfo) {}




	//////////////////////////////////////////////////////////////////////////
	

	/**
	 * @brief 向中心获取负载最小的接入服务器回调
	 * @note  
	 * @param oErrInfo 错误信息
	 * @return
	 */
	virtual void OnSyncGetMinLoadAccessServer(const common::errorinfo& oErrInfo, const std::string& strIp, uint32 uiPort) {}

	/**
	 * @brief 接入服务器转到逻辑服务器回调
	 * @note  
	 * @param oErrInfo 错误信息
	 * @return
	 */
	virtual void OnSyncAccessToLogic(const common::errorinfo& oErrInfo, const CContext& oOutContext) {}

	/**
	 * @brief 接入服务器转到逻辑服务器回调
	 * @note 
	 * @param oErrInfo 错误信息
	 * @param oUserInfo 用户信息结构体
	 * @return
	 */
	virtual void OnSyncDProxyGetUserInfo(const common::errorinfo& oErrInfo, const USER_INFO& tUserInfo) {}

private:
	uint32 m_uiDProxyCmd; //存储读写自定义业务命令
	DProxyBufferUser m_oDProxyBufUser; //业务存储部分代码封装。
	USER_STATUS_INFO m_tStatusInfo; //用户在线信息
};

} /* namespace bsw */

#endif /* _STEP_SYNC_ACCESS_HPP_ */

#ifndef _BSW_PUBLIC_H_
#define _BSW_PUBLIC_H_

#include <stdio.h>
#include <string>
#include <string.h>
#include <OssDefine.hpp>
#include <util/UnixTime.hpp>
#include <storage/dataproxy.pb.h>
#include <storage/MemOperator.hpp>
#include "../Starship/src/protocol/msg.pb.h"
#include <user_base_info.pb.h>
#include <enumeration.pb.h>
#include <BswMsg.hpp>
#include <BswError.h>
#include <BswErrorMapping.h>

namespace bsw
{
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int int64;
typedef unsigned long long int uint64;

#define SAFE_DELETE(x) { if (x) delete x; x = NULL; }
#define SAFE_ARRAY_DELETE(x) { if (x) delete [] x; x = NULL; }

#define SYSTEM_ID  1000
#define OFFICAL_ID 999
#define MAX_PAGE_SIZE 20

enum 
{
	HR_ERROR = -1,
	HR_UNKOWN = 0,
	HR_SUCCESS = 1,
};

//简化错误码的调用
#define SET_ERR_INFO(x, args...)           \
    char szFormat[1024] = {0};                  \
    snprintf(szFormat, sizeof(szFormat), args); \
    m_oErrInfo.set_error_code(x);               \
    m_oErrInfo.set_error_info(szFormat);        \
    LOG4_ERROR(args); 

#define SET_TMP_ERR_INFO(x, args...)           \
    char szFormat[1024] = {0};                  \
    snprintf(szFormat, sizeof(szFormat), args); \
    oErrInfo.set_error_code(x);               \
    oErrInfo.set_error_info(szFormat);        \
    LOG4_ERROR(args); 

class CContext
{
public:
	CContext() {}
	CContext(const oss::tagMsgShell& stMsgShell, const MsgHead& oInMsgHead, const MsgBody& oInMsgBody) 
		: m_oInMsgHead(oInMsgHead), m_oInMsgBody(oInMsgBody), m_stMsgShell(stMsgShell)
	{
	}

	CContext(const CContext& obj) 
	{
		*this = obj;
	}

	virtual ~CContext() {}

	CContext& operator= (const CContext &obj)
	{
		if (this == &obj)
			return *this;

		m_stMsgShell = obj.m_stMsgShell;
		m_oInMsgHead = obj.m_oInMsgHead;
		m_oInMsgBody = obj.m_oInMsgBody;
		m_oBasicInfo = obj.m_oBasicInfo;
		return *this;
	}

public:
	MsgHead m_oInMsgHead;
	MsgBody m_oInMsgBody;
	oss::tagMsgShell m_stMsgShell;
	user_base_info m_oBasicInfo;
};

//用户个人信息
typedef struct USER_INFO
{
	uint32 uiUserID;					//用户ID
	char   szAccount[13];               //用户账号
	char   szNickName[31];              //用户昵称
	char   szPassWord[33];              //用户密码
	uint8  cSex;                        //用户性别
	char   szMobile[14];				//手机号
	char   szSignature[241];            //个性签名
	char   szAvatar[101];               //头像

	USER_INFO()
	{
		uiUserID = 0;
		memset(szAccount, 0, sizeof(szAccount));
		memset(szNickName, 0, sizeof(szNickName));
		memset(szPassWord, 0, sizeof(szPassWord));
		cSex = 0;
		memset(szMobile, 0, sizeof(szMobile));
		memset(szSignature, 0, sizeof(szSignature));
		memset(szAvatar, 0, sizeof(szAvatar));
	}

	USER_INFO(const USER_INFO& obj)
	{
		uiUserID = obj.uiUserID;
		snprintf(szAccount, sizeof(szAccount), obj.szAccount);
		snprintf(szNickName, sizeof(szNickName), obj.szNickName);
		snprintf(szPassWord, sizeof(szPassWord), obj.szPassWord);
		cSex = obj.cSex;
		snprintf(szMobile, sizeof(szMobile), obj.szMobile);
		snprintf(szSignature, sizeof(szSignature), obj.szSignature);
		snprintf(szAvatar, sizeof(szAvatar), obj.szAvatar);
	}

	USER_INFO& operator = (const USER_INFO &obj)
	{
		if (this == &obj) return *this;

		uiUserID = obj.uiUserID;
		snprintf(szAccount, sizeof(szAccount), obj.szAccount);
		snprintf(szNickName, sizeof(szNickName), obj.szNickName);
		snprintf(szPassWord, sizeof(szPassWord), obj.szPassWord);
		cSex = obj.cSex;
		snprintf(szMobile, sizeof(szMobile), obj.szMobile);
		snprintf(szSignature, sizeof(szSignature), obj.szSignature);
		snprintf(szAvatar, sizeof(szAvatar), obj.szAvatar);
		return *this;
	}

}_USER_INFO, *LPUSER_INFO;

typedef struct USER_STATUS_INFO
{
	uint32 uiUserID;           //用户 ID
	uint16 usAppType;          //应用类型
	uint16 usClientType;       //用户终端类型
	char szToken[64];          //token
	char szIdentifyApp[40];    //APP类型客户端连接标识
	char szIdentifyWeb[40];    //Web类型客户端连接标识
	char szDeviceInfo[40];     //客户端设备号

	USER_STATUS_INFO()
	{
		Reset();
	}

	void Reset()
	{
		uiUserID = 0;
		usAppType = 0;
		usClientType = 0;
		memset(szToken, 0, sizeof(szToken));
		memset(szIdentifyApp, 0, sizeof(szIdentifyApp));
		memset(szIdentifyWeb, 0, sizeof(szIdentifyWeb));
		memset(szDeviceInfo, 0, sizeof(szDeviceInfo));
	}

	USER_STATUS_INFO(const USER_STATUS_INFO& obj)
	{
		*this = obj;
	}

	USER_STATUS_INFO& operator = (const USER_STATUS_INFO& obj)
	{
		uiUserID = obj.uiUserID;
		usAppType = obj.usAppType;
		usClientType = obj.usClientType;
		snprintf(szToken, sizeof(szToken), obj.szToken);
		snprintf(szIdentifyApp, sizeof(szIdentifyApp), obj.szIdentifyApp);
		snprintf(szIdentifyWeb, sizeof(szIdentifyWeb), obj.szIdentifyWeb);
		snprintf(szDeviceInfo, sizeof(szDeviceInfo), obj.szDeviceInfo);
		return *this;
	}

}_USER_STATUS_INFO, *LPUSER_STATUS_INFO;

}
#endif //_BSW_PUBLIC_H_

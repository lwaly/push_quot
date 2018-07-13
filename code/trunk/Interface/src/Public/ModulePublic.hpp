

#ifndef _BSW_MODULE_PUBLIC_H_
#define _BSW_MODULE_PUBLIC_H_

#include <stdio.h>
#include <string>
#include <string.h>
#include <OssDefine.hpp>
#include <util/UnixTime.hpp>
#include <storage/dataproxy.pb.h>
#include <storage/MemOperator.hpp>
#include "../../Starship/src/protocol/http.pb.h"
#include "../Starship/src/protocol/msg.pb.h"
#include <user_base_info.pb.h>


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

#define SYSTEM_ID 1000
#define MAX_PAGE_SIZE 20

#define HTTP_KEY_COOKIE      "Cookie"
#define HTTP_KEY_TOKEN       "token"
#define HTTP_KEY_USERID      "userid"

// enum 
// {
	// HR_ERROR = -1,
	// HR_UNKOWN = 0,
	// HR_SUCCESS = 1,
// };

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

//http数据包上下文
class CHttpContext
{
public:
	CHttpContext() {}
	CHttpContext(const oss::tagMsgShell& stMsgShell, const HttpMsg& oInHttpMsg) 
		: m_stMsgShell(stMsgShell), m_oHttpMsg(oInHttpMsg)
	{
	}

	CHttpContext(const CHttpContext& obj) 
	{
		*this = obj;
	}

	virtual ~CHttpContext() {}

	CHttpContext& operator= (const CHttpContext &obj)
	{
		if (this == &obj)
			return *this;

		m_stMsgShell = obj.m_stMsgShell;
		m_oHttpMsg = obj.m_oHttpMsg;
		m_oBasicInfo = obj.m_oBasicInfo;
		return *this;
	}

public:
	oss::tagMsgShell m_stMsgShell;
	HttpMsg m_oHttpMsg;
	user_base_info m_oBasicInfo;
};

}
#endif //_BSW_MODULE_PUBLIC_H_

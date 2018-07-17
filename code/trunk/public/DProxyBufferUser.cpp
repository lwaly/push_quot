/*******************************************************************************
* Project:  Public
* @file     DProxyBufferUser.cpp
* @brief    封装用户信息存储协议代码。
* @author   wfh
* @date:    2016年11月27日
* @note
* Modify history:
******************************************************************************/
#include "ImTableName.h"
#include "ImRedisProto.h"
#include "DProxyBufferUser.hpp"
#include "PublicFunc.hpp"
#include <enumeration.pb.h>

namespace mg
{

DProxyBufferUser::DProxyBufferUser()
{
}

std::string DProxyBufferUser::DPROXY_BUFFER_CHECK_ACCOUNT(const std::string strAccount, uint32 uiAccountType)
{
    //直接从数据库中查询数据，目前 redis 还没有建立好手机，邮箱，对 uid 的映射关系，所以暂时不能通过 redis 去查询。
    oss::DbOperator oDbOperate(0, STR_TABLE_USERINFO, DataMem::MemOperate::DbOperate::SELECT);
    oDbOperate.AddDbField("u_id");
    oDbOperate.AddDbField("u_xox_account"); //暂时还没有用上，先取过来，以后有可能会用上。

    //根据上传的不同类型账号，查询数据库表中的不同字段。
    switch (uiAccountType)
    {
    case enumeration::TYPE_LOGIN_BY_UID:
        oDbOperate.AddCondition(DataMem::MemOperate::DbOperate::Condition::EQ, 
            "u_xox_account", strAccount, DataMem::STRING);
        break;

    case enumeration::TYPE_LOGIN_BY_PHONE:
        oDbOperate.AddCondition(DataMem::MemOperate::DbOperate::Condition::EQ, 
            "u_mobile", strAccount, DataMem::STRING);
        break;

    case enumeration::TYPE_LOGIN_BY_EMAIL:
        oDbOperate.AddCondition(DataMem::MemOperate::DbOperate::Condition::EQ, 
            "u_email", strAccount, DataMem::STRING);
        break;

    case enumeration::TYPE_LOGIN_BY_QQ:
        oDbOperate.AddCondition(DataMem::MemOperate::DbOperate::Condition::EQ, 
            "u_qq", strAccount, DataMem::STRING);
        break;

    default:
        return "";
    }

    return oDbOperate.MakeMemOperate()->SerializeAsString();
}

std::string DProxyBufferUser::DPROXY_BUFFER_UPDATE_TOKEN(
    uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strToken)
{
    char szRedisKey[64] = {0};
    snprintf(szRedisKey, sizeof(szRedisKey), "%u:%u:%u:%u", REDIS_T_HASH, BSW_DATA_USER_ONLINE, uiAppType, uiUserID);
    oss::RedisOperator oRedisOper(uiUserID, szRedisKey, "hset");

    switch (uiClientType)
    {
    case enumeration::TYPE_CLIENT_PC:
        oRedisOper.AddRedisField("pc_token", strToken);
        break;
    case enumeration::TYPE_CLIENT_ANDROID:
        oRedisOper.AddRedisField("android_token", strToken);
        break;
    case enumeration::TYPE_CLIENT_IOS:
        oRedisOper.AddRedisField("iphone_token", strToken);
        break;
    case enumeration::TYPE_CLIENT_IPAD:
        oRedisOper.AddRedisField("pad_token", strToken);
        break;
    case enumeration::TYPE_CLIENT_WEB:
        oRedisOper.AddRedisField("web_token", strToken);
        break;
    default:
        return "";
    }

    return oRedisOper.MakeMemOperate()->SerializeAsString();
}

std::string DProxyBufferUser::DPROXY_BUFFER_UPDATE_USER_IDENTYFY(
    uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strIdentify)
{
    char szRedisKey[64] = {0};
    snprintf(szRedisKey, sizeof(szRedisKey), "%u:%u:%u:%u", REDIS_T_HASH, BSW_DATA_USER_ONLINE, uiAppType, uiUserID);
    oss::RedisOperator oRedisOper(uiUserID, szRedisKey, "hmset", "");

    switch (uiClientType)
    {
    case enumeration::TYPE_CLIENT_PC:
        {
            oRedisOper.AddRedisField("pc_access", strIdentify);
        }
        break;
    case enumeration::TYPE_CLIENT_ANDROID:
        {
            oRedisOper.AddRedisField("android_access", strIdentify);
        }
        break;
    case enumeration::TYPE_CLIENT_IOS:
        {
            oRedisOper.AddRedisField("iphone_access", strIdentify);
        }
        break;
    case enumeration::TYPE_CLIENT_IPAD:
        {
            oRedisOper.AddRedisField("pad_access", strIdentify);
        }
        break;
    case enumeration::TYPE_CLIENT_WEB:
        {
            oRedisOper.AddRedisField("web_access", strIdentify);
        }
        break;
    default:
        return "";
    }

    return oRedisOper.MakeMemOperate()->SerializeAsString();
}

std::string DProxyBufferUser::DPROXY_BUFFER_GET_USER_INFO(uint32 uiUserID)
{
    //用户数据由其它业务系统维护，所以不能保证 redis 和 数据库数据同步。暂时从数据库读取数据。
    /*
    char szRedisKey[64] = {0};
    snprintf(szRedisKey, sizeof(szRedisKey), "%u:%u:%u", REDIS_T_HASH, BSW_DATA_USER_INFO, uiUserID);

    oss::MemOperator oMemOper(uiUserID, STR_TABLE_USERINFO, 
    DataMem::MemOperate::DbOperate::SELECT, szRedisKey, "hmset", "hmget");

    //获得用户的信息。
    oMemOper.AddField("u_id");
    oMemOper.AddField("u_xox_account");
    oMemOper.AddField("u_nickname");
    oMemOper.AddField("u_password");
    oMemOper.AddField("u_sex");
    oMemOper.AddField("u_mobile");
    oMemOper.AddField("u_birthday");
    oMemOper.AddField("u_signature");

    oMemOper.AddCondition(DataMem::MemOperate::DbOperate::Condition::EQ, 
    "u_id", IntToString(uiUserID), DataMem::INT);

    return oMemOper.MakeMemOperate()->SerializeAsString();
    */

    //直接从数据库中查询数据，目前 redis 还没有建立好手机，邮箱，对 uid 的映射关系，所以暂时不能通过 redis 去查询。
    oss::DbOperator oDbOperate(uiUserID, STR_TABLE_USERINFO, DataMem::MemOperate::DbOperate::SELECT);
    oDbOperate.AddDbField("u_id");
    oDbOperate.AddDbField("u_xox_account");
    oDbOperate.AddDbField("u_nickname");
    oDbOperate.AddDbField("u_password");
    oDbOperate.AddDbField("u_sex");
    oDbOperate.AddDbField("u_mobile");
    oDbOperate.AddDbField("u_birthday"); 
    oDbOperate.AddDbField("u_signature");

    oDbOperate.AddCondition(DataMem::MemOperate::DbOperate::Condition::EQ, 
        "u_xox_account", IntToString(uiUserID), DataMem::STRING);

    return oDbOperate.MakeMemOperate()->SerializeAsString();
}

std::string DProxyBufferUser::DPROXY_BUFFER_GET_USER_ONLINE_STATUS(uint32 uiAppType, uint32 uiUserID, uint32 uiClientType)
{
    char szRedisKey[64] = {0};
    snprintf(szRedisKey, sizeof(szRedisKey), "%u:%u:%u:%u", REDIS_T_HASH, BSW_DATA_USER_ONLINE, uiAppType, uiUserID);
    oss::RedisOperator oRedisOper(uiUserID, szRedisKey, "", "hmget");

    switch (uiClientType)
    {
    case enumeration::TYPE_CLIENT_PC:
        {
            oRedisOper.AddRedisField("pc_token");
            oRedisOper.AddRedisField("pc_access");
            oRedisOper.AddRedisField("pc_device");
        }
        break;
    case enumeration::TYPE_CLIENT_ANDROID:
        {
            oRedisOper.AddRedisField("android_token");
            oRedisOper.AddRedisField("android_access");
            oRedisOper.AddRedisField("android_device");
        }
        break;
    case enumeration::TYPE_CLIENT_IOS:
        {
            oRedisOper.AddRedisField("iphone_token");
            oRedisOper.AddRedisField("iphone_access");
            oRedisOper.AddRedisField("iphone_device");
        }
        break;
    case enumeration::TYPE_CLIENT_IPAD:
        {
            oRedisOper.AddRedisField("pad_token");
            oRedisOper.AddRedisField("pad_access");
            oRedisOper.AddRedisField("pad_device");
        }
        break;
    case enumeration::TYPE_CLIENT_WEB:
        {
            oRedisOper.AddRedisField("web_token");
            oRedisOper.AddRedisField("web_access");
            oRedisOper.AddRedisField("web_device");
        }
        break;
    default:
        return "";
    }

    return oRedisOper.MakeMemOperate()->SerializeAsString();
}

std::string DProxyBufferUser::DPROXY_BUFFER_UPDATE_USER_ONLINE_STATUS_IDENTIFY_DEVICE(
    uint32 uiAppType, uint32 uiUserID, uint32 uiClientType, const std::string& strIdentify, const std::string& strDevice)
{
    char szRedisKey[64] = {0};
    snprintf(szRedisKey, sizeof(szRedisKey), "%u:%u:%u:%u", REDIS_T_HASH, BSW_DATA_USER_ONLINE, uiAppType, uiUserID);
    oss::RedisOperator oRedisOper(uiUserID, szRedisKey, "hmset", "");

    switch (uiClientType)
    {
    case enumeration::TYPE_CLIENT_PC:
        {
            oRedisOper.AddRedisField("pc_access", strIdentify);
            oRedisOper.AddRedisField("pc_device", strDevice);
        }
        break;
    case enumeration::TYPE_CLIENT_ANDROID:
        {
            oRedisOper.AddRedisField("android_access", strIdentify);
            oRedisOper.AddRedisField("android_device", strDevice);
        }
        break;
    case enumeration::TYPE_CLIENT_IOS:
        {
            oRedisOper.AddRedisField("iphone_access", strIdentify);
            oRedisOper.AddRedisField("iphone_device", strDevice);
        }
        break;
    case enumeration::TYPE_CLIENT_IPAD:
        {
            oRedisOper.AddRedisField("pad_access", strIdentify);
            oRedisOper.AddRedisField("pad_device", strDevice);
        }
        break;
    case enumeration::TYPE_CLIENT_WEB:
        {
            oRedisOper.AddRedisField("web_access", strIdentify);
            oRedisOper.AddRedisField("web_device", strDevice);
        }
        break;
    default:
        return "";
    }

    return oRedisOper.MakeMemOperate()->SerializeAsString();
}

} /* namespace mg */


#include "BswErrorMapping.h"

namespace bsw
{

int bsw_err_code(int code)
{
    switch (code)
    {
        case oss::ERR_OK:                               return ERR_OK;
        case oss::ERR_PARASE_PROTOBUF:                  return ERR_PARASE_PROTOBUF;
        case oss::ERR_NO_SUCH_WORKER_INDEX:             return ERR_NO_SUCH_WORKER_INDEX;
        case oss::ERR_UNKNOWN_CMD:                      return ERR_UNKNOWN_CMD;
        case oss::ERR_NEW:                              return ERR_NEW;
        case oss::ERR_REDIS_CONN_CLOSE:                 return ERR_REDIS_CONN_CLOSE;
        case oss::ERR_DISCONNECT:                       return ERR_DISCONNECT;
        case oss::ERR_NO_CALLBACK:                      return ERR_NO_CALLBACK;
        case oss::ERR_DATA_TRANSFER:                    return ERR_DATA_TRANSFER;
        case oss::ERR_REPEAT_REGISTERED:                return ERR_REPEAT_REGISTERED;
        case oss::ERR_SERVERINFO:                       return ERR_SERVERINFO;
        case oss::ERR_SESSION:                          return ERR_SESSION;
        case oss::ERR_BODY_JSON:                        return ERR_BODY_JSON;
        case oss::ERR_SERVERINFO_RECORD:                return ERR_SERVERINFO_RECORD;
        case oss::ERR_INCOMPLET_DATAPROXY_DATA:         return ERR_INCOMPLET_DATAPROXY_DATA;
        case oss::ERR_INVALID_REDIS_ROUTE:              return ERR_INVALID_REDIS_ROUTE;
        case oss::ERR_REDIS_NODE_NOT_FOUND:             return ERR_REDIS_NODE_NOT_FOUND;
        case oss::ERR_REGISTERCALLBACK_REDIS:           return ERR_REGISTERCALLBACK_REDIS;
        case oss::ERR_REDIS_CMD:                        return ERR_REDIS_CMD;
        case oss::ERR_UNEXPECTED_REDIS_REPLY:           return ERR_UNEXPECTED_REDIS_REPLY;
        case oss::ERR_RESULTSET_EXCEED:                 return ERR_RESULTSET_EXCEED;
        case oss::ERR_LACK_CLUSTER_INFO:                return ERR_LACK_CLUSTER_INFO;
        case oss::ERR_TIMEOUT:                          return ERR_TIMEOUT;
        case oss::ERR_REDIS_AND_DB_CMD_NOT_MATCH:       return ERR_REDIS_AND_DB_CMD_NOT_MATCH;
        case oss::ERR_REDIS_NIL_AND_DB_FAILED:          return ERR_REDIS_NIL_AND_DB_FAILED;
        case oss::ERR_NO_RIGHT:                         return ERR_NO_RIGHT;
        case oss::ERR_QUERY:                            return ERR_QUERY;
        case oss::ERR_REDIS_STRUCTURE_WITH_DATASET:     return ERR_REDIS_STRUCTURE_WITH_DATASET;
        case oss::ERR_REDIS_STRUCTURE_WITHOUT_DATASET:  return ERR_REDIS_STRUCTURE_WITHOUT_DATASET;
        case oss::ERR_DB_FIELD_NUM:                     return ERR_DB_FIELD_NUM;
        case oss::ERR_DB_FIELD_ORDER_OR_FIELD_NAME:     return ERR_DB_FIELD_ORDER_OR_FIELD_NAME;
        case oss::ERR_KEY_FIELD:                        return ERR_KEY_FIELD;
        case oss::ERR_KEY_FIELD_VALUE:                  return ERR_KEY_FIELD_VALUE;
        case oss::ERR_JOIN_FIELDS:                      return ERR_JOIN_FIELDS;
        case oss::ERR_LACK_JOIN_FIELDS:                 return ERR_LACK_JOIN_FIELDS;
        case oss::ERR_REDIS_STRUCTURE_NOT_DEFINE:       return ERR_REDIS_STRUCTURE_NOT_DEFINE;
        case oss::ERR_INVALID_CMD_FOR_HASH_DATASET:     return ERR_INVALID_CMD_FOR_HASH_DATASET;
        case oss::ERR_DB_TABLE_NOT_DEFINE:              return ERR_DB_TABLE_NOT_DEFINE;
        case oss::ERR_DB_OPERATE_MISSING:               return ERR_DB_OPERATE_MISSING;
        case oss::ERR_REQ_MISS_PARAM:                   return ERR_REQ_MISS_PARAM;
        case oss::ERR_LACK_TABLE_FIELD:                 return ERR_LACK_TABLE_FIELD;
        case oss::ERR_TABLE_FIELD_NAME_EMPTY:           return ERR_TABLE_FIELD_NAME_EMPTY;
        case oss::ERR_UNDEFINE_REDIS_OPERATE:           return ERR_UNDEFINE_REDIS_OPERATE;
    }

    if (code < 10000)
    {
        return ERR_SYSTEM_ERROR;
    }
    else if (code >= 10000 && code < 20000)  // 此种情况应在上面的switch里处理
    {
        return ERR_SERVER_ERROR;
    }
    else if (code >= 20000 && code < 30000)
    {
        return code;
    }
	
    return ERR_UNKNOWN;
}

const char * bsw_err_msg(int code)
{
    switch (code)
    {
        case oss::ERR_OK:                               return "成功";
        case oss::ERR_PARASE_PROTOBUF:                  return "协议错误[20000]";
        case oss::ERR_NO_SUCH_WORKER_INDEX:             return "系统错误[20001]";
        case oss::ERR_UNKNOWN_CMD:                      return "未知的命令字[20002]";
        case oss::ERR_NEW:                              return "系统繁忙[20003]";
        case oss::ERR_REDIS_CONN_CLOSE:                 return "系统错误[20004]";
        case oss::ERR_DISCONNECT:                       return "系统错误[20005]";
        case oss::ERR_NO_CALLBACK:                      return "系统错误[20006]";
        case oss::ERR_DATA_TRANSFER:                    return "系统错误[20007]";
        case oss::ERR_REPEAT_REGISTERED:                return "系统繁忙[20008]";
        case oss::ERR_SERVERINFO:                       return "系统错误[20009]";
        case oss::ERR_SESSION:                          return "系统错误[20010]";
        case oss::ERR_BODY_JSON:                        return "协议错误[20011]";
        case oss::ERR_SERVERINFO_RECORD:                return "系统错误[20012]";
        case oss::ERR_INCOMPLET_DATAPROXY_DATA:         return "数据错误[20013]";
        case oss::ERR_INVALID_REDIS_ROUTE:              return "未知服务[20014]";
        case oss::ERR_REDIS_NODE_NOT_FOUND:             return "系统错误[20015]";
        case oss::ERR_REGISTERCALLBACK_REDIS:           return "系统错误[20016]";
        case oss::ERR_REDIS_CMD:                        return "数据错误[20017]";
        case oss::ERR_UNEXPECTED_REDIS_REPLY:           return "数据错误[20018]";
        case oss::ERR_RESULTSET_EXCEED:                 return "数据错误[20019]";
        case oss::ERR_LACK_CLUSTER_INFO:                return "数据错误[20020]";
        case oss::ERR_TIMEOUT:                          return "数据错误[20021]";
        case oss::ERR_REDIS_AND_DB_CMD_NOT_MATCH:       return "数据错误[20022]";
        case oss::ERR_REDIS_NIL_AND_DB_FAILED:          return "数据错误[20023]";
        case oss::ERR_NO_RIGHT:                         return "数据错误[20024]";
        case oss::ERR_QUERY:                            return "数据错误[20025]";
        case oss::ERR_REDIS_STRUCTURE_WITH_DATASET:     return "数据错误[20026]";
        case oss::ERR_REDIS_STRUCTURE_WITHOUT_DATASET:  return "数据错误[20027]";
        case oss::ERR_DB_FIELD_NUM:                     return "数据错误[20028]";
        case oss::ERR_DB_FIELD_ORDER_OR_FIELD_NAME:     return "数据错误[20029]";
        case oss::ERR_KEY_FIELD:                        return "数据错误[20030]";
        case oss::ERR_KEY_FIELD_VALUE:                  return "数据错误[20031]";
        case oss::ERR_JOIN_FIELDS:                      return "数据错误[20032]";
        case oss::ERR_LACK_JOIN_FIELDS:                 return "数据错误[20033]";
        case oss::ERR_REDIS_STRUCTURE_NOT_DEFINE:       return "数据错误[20034]";
        case oss::ERR_INVALID_CMD_FOR_HASH_DATASET:     return "数据错误[20035]";
        case oss::ERR_DB_TABLE_NOT_DEFINE:              return "数据错误[20036]";
        case oss::ERR_DB_OPERATE_MISSING:               return "数据错误[20037]";
        case oss::ERR_REQ_MISS_PARAM:                   return "数据错误[20038]";
        case oss::ERR_LACK_TABLE_FIELD:                 return "数据错误[20039]";
        case oss::ERR_TABLE_FIELD_NAME_EMPTY:           return "数据错误[20040]";
        case oss::ERR_UNDEFINE_REDIS_OPERATE:           return "数据错误[20041]";
        case oss::ERR_REDIS_READ_WRITE_CMD_NOT_MATCH:   return "数据错误[20042]";

        // bsw error
        case bsw::ERR_PARASE_PROTOBUF:                  return "协议错误[20000]";
        case bsw::ERR_NO_SUCH_WORKER_INDEX:             return "系统错误[20001]";
        case bsw::ERR_UNKNOWN_CMD:                      return "未知的命令字[20002]";
        case bsw::ERR_NEW:                              return "系统繁忙[20003]";
        case bsw::ERR_REDIS_CONN_CLOSE:                 return "系统错误[20004]";
        case bsw::ERR_DISCONNECT:                       return "系统错误[20005]";
        case bsw::ERR_NO_CALLBACK:                      return "系统错误[20006]";
        case bsw::ERR_DATA_TRANSFER:                    return "系统错误[20007]";
        case bsw::ERR_REPEAT_REGISTERED:                return "系统繁忙[20008]";
        case bsw::ERR_SERVERINFO:                       return "系统错误[20009]";
        case bsw::ERR_SESSION:                          return "系统错误[20010]";
        case bsw::ERR_BODY_JSON:                        return "协议错误[20011]";
        case bsw::ERR_SERVERINFO_RECORD:                return "系统错误[20012]";
        case bsw::ERR_INCOMPLET_DATAPROXY_DATA:         return "数据错误[20013]";
        case bsw::ERR_INVALID_REDIS_ROUTE:              return "未知服务[20014]";
        case bsw::ERR_REDIS_NODE_NOT_FOUND:             return "系统错误[20015]";
        case bsw::ERR_REGISTERCALLBACK_REDIS:           return "系统错误[20016]";
        case bsw::ERR_REDIS_CMD:                        return "数据错误[20017]";
        case bsw::ERR_UNEXPECTED_REDIS_REPLY:           return "数据错误[20018]";
        case bsw::ERR_RESULTSET_EXCEED:                 return "数据错误[20019]";
        case bsw::ERR_LACK_CLUSTER_INFO:                return "数据错误[20020]";
        case bsw::ERR_TIMEOUT:                          return "数据错误[20021]";
        case bsw::ERR_REDIS_AND_DB_CMD_NOT_MATCH:       return "数据错误[20022]";
        case bsw::ERR_REDIS_NIL_AND_DB_FAILED:          return "数据错误[20023]";
        case bsw::ERR_NO_RIGHT:                         return "数据错误[20024]";
        case bsw::ERR_QUERY:                            return "数据错误[20025]";
        case bsw::ERR_REDIS_STRUCTURE_WITH_DATASET:     return "数据错误[20026]";
        case bsw::ERR_REDIS_STRUCTURE_WITHOUT_DATASET:  return "数据错误[20027]";
        case bsw::ERR_DB_FIELD_NUM:                     return "数据错误[20028]";
        case bsw::ERR_DB_FIELD_ORDER_OR_FIELD_NAME:     return "数据错误[20029]";
        case bsw::ERR_KEY_FIELD:                        return "数据错误[20030]";
        case bsw::ERR_KEY_FIELD_VALUE:                  return "数据错误[20031]";
        case bsw::ERR_JOIN_FIELDS:                      return "数据错误[20032]";
        case bsw::ERR_LACK_JOIN_FIELDS:                 return "数据错误[20033]";
        case bsw::ERR_REDIS_STRUCTURE_NOT_DEFINE:       return "数据错误[20034]";
        case bsw::ERR_INVALID_CMD_FOR_HASH_DATASET:     return "数据错误[20035]";
        case bsw::ERR_DB_TABLE_NOT_DEFINE:              return "数据错误[20036]";
        case bsw::ERR_DB_OPERATE_MISSING:               return "数据错误[20037]";
        case bsw::ERR_REQ_MISS_PARAM:                   return "数据错误[20038]";
        case bsw::ERR_LACK_TABLE_FIELD:                 return "数据错误[20039]";
        case bsw::ERR_TABLE_FIELD_NAME_EMPTY:           return "数据错误[20040]";
        case bsw::ERR_UNDEFINE_REDIS_OPERATE:           return "数据错误[20041]";
        case bsw::ERR_REDIS_READ_WRITE_CMD_NOT_MATCH:   return "数据错误[20042]";

        case ERR_UNKNOWN:                               return "未知错误[20200]";
        case ERR_UNKNOW_CMD:                            return "未知命令字[20201]";
        case ERR_SYSTEM_ERROR:                          return "系统错误[20202]";          ///< 系统错误（数据库错误等，通过查询后台Server日志可以找到更具体错误码）
        case ERR_SERVER_BUSY:                           return "系统繁忙[20203]";
        case ERR_SERVER_ERROR:                          return "服务器错误[20204]";
        case ERR_SERVER_TIMEOUT:                        return "系统繁忙，请稍后再尝试[20205]";
        case ERR_SENDTONEXT_FAILED:                     return "服务器错误[20206]";
        case ERR_UNKNOWN_RESPONSE_CMD:                  return "未知响应命令字[20207]";    ///< 未知响应命令字，通常为对端没有找到对应Cmd处理者，由框架层返回错误
        case ERR_ASYNC_TIMEOUT:                         return "系统繁忙，请稍后再尝试[20208]";
        case ERR_LOAD_CONFIGFILE:                       return "系统错误[20209]";          ///< 模块加载配置文件失败
        case ERR_STEP_RUNNING_ERROR:                    return "系统错误[20210]";          ///< 注册的step不在运行态
        case ERR_REG_STEP_ERROR:                        return "系统繁忙，请稍后再尝试[20211]";    ///< 注册Step失败
        case ERR_STEP_EMIT_LIMIT:                       return "系统繁忙，请稍后再尝试[20212]";    ///< STEP emit 次数超过限制
        case ERR_LOGIC_SERVER_TIMEOUT:                  return "系统繁忙，请稍后再尝试[20213]";    ///< 逻辑Server处理超时
        case ERR_PROC_RELAY:                            return "系统繁忙，请稍后再尝试[20214]";    ///< 数据转发到其它进程失败
        case ERR_SEND_TO_LOGIG_MSG:                     return "系统繁忙，请稍后再尝试[20215]";   ///< 发送消息到逻辑服务器失败
        case ERR_REG_SESSION:                           return "系统繁忙，请稍后再尝试[20216]";   ///< session注册失败
        case ERR_SESSION_CREATE:                        return "系统繁忙，请稍后再尝试[20217]";   ///< 创建 session 失败
        case ERR_MALLOC_FAILED:                         return "系统繁忙，请稍后再尝试[20218]";   ///< new对象失败
        case ERR_SEQUENCE:                              return "系统繁忙，请稍后再尝试[20219]";   ///< 错误序列号
        case ERR_RC5ENCRYPT_ERROR:                      return "加密失败[20220]";                ///< Rc5Encrypt 加密失败
        case ERR_RC5DECRYPT_ERROR:                      return "解密失败[20221]";                ///< Rc5Decrypt 解密失败
        case ERR_REQ_FREQUENCY:                         return "请求过于频繁[20222]";            ///< 请求过于频繁（未避免被攻击而做的系统保护）
        case ERR_NO_SESSION_ID_IN_MSGBODY:              return "数据错误[20301]";                ///< MsgBody缺少session_id
        case ERR_NO_ADDITIONAL_IN_MSGBODY:              return "数据错误[20302]";                ///< MsgBody缺少additional
        case ERR_MSG_BODY_DECODE:                       return "数据错误[20303]";                ///< msg body解析出错
        case ERR_INVALID_PROTOCOL:                      return "协议错误[20304]";                ///< 协议错误
        case ERR_PACK_INFO_ERROR:                       return "数据错误[20305]";                ///< 部分信息打成PB协议包
        case ERR_PARSE_PACK_ERROR:                      return "协议错误[20306]";                ///< 解析PB协议包
        case ERR_REQ_MISS_PB_PARAM:                     return "参数缺失[20307]";                 ///< 请求缺少参数(pb中带的参数不全)
        case ERR_PROTOCOL_FORMAT:                       return "协议格式错误[20308]";             ///< 协议格式错误
        case ERR_LIST_INCOMPLETE:                       return "参数缺失[20309]";                 ///< 参数列表缺失或不全
        case ERR_BEYOND_RANGE:                          return "参数值错误[20310]";               ///< 传入的参数值超过规定范围
        case ERR_PARMS_VALUES_MISSING:                  return "参数缺失[20311]";                  ///< 传入的参数在程序传递或解析过程中丢失
        case ERR_JSON_PRASE_FAILED:                     return "协议格式错误[20312]";              ///< JSON 数据解析失败
        case ERR_INVALID_DATA:                          return "数据错误[20313]";                 ///< 错误数据
        case ERR_INVALID_SESSION_ID:                    return "协议错误[20314]";                 ///< 错误的session路由信息
        case ERR_HTTP_SESSION_GET:                      return "数据错误[20401]";                 ///< HTTP SESSION 获取失败
        case ERR_HTTP_NO_HEADER:                        return "数据错误[20402]";                 ///< http请求没有header
        case ERR_HTTP_NO_BODY:                          return "数据错误[20403]";                 ///< http请求没有body
        case ERR_HTTP_NO_COOKIE:                        return "Cookie缺失[20404]";               ///< http请求缺少cookie
        case ERR_HTTP_NO_METHOD:                        return "协议格式错误[20405]";             ///< http请求缺少请求类型(GET/POST)
        case ERR_HTTP_TYPE:                             return "协议格式错误[20406]";             ///< http类型错误(请求/响应)
        case ERR_HTTP_METHOD:                           return "数据错误[20407]";                 ///< http接口错误(GET/POST)
        case ERR_HTTP_PARAM_DECODE:                     return "数据错误[20408]";                 ///< http参数解析失败
        case ERR_HTTP_PARAM_MISSING:                    return "参数缺失[20409]";                 ///< http请求缺少参数
        case ERR_HTTP_COOKIE_PARAM_DECODE:              return "Cookie数据错误[20410]";           ///< http请求cookie参数解析失败
        case ERR_HTTP_COOKIE_PARAM_MISSING:             return "Cookie参数缺失[20411]";           ///< http请求缺少cookie参数
        case ERR_HTTP_PARAM_ERROR:                      return "参数格式错误[20412]";             ///< http参数格式错误
        case ERR_HTTP_COOKIE_PARAM_ERROR:               return "Cookie数据错误[20413]";           ///< http请求Cookie参数错误
        case ERR_HTTP_POST_TO_JAVA:                     return "系统繁忙，请稍后再尝试[20414]";    ///< http post数据给java服务时出错
        case ERR_SENDUSESET_MSG:                        return "系统繁忙，请稍后再尝试[20501]";    ///< 发送存储userset失败
        case ERR_STOREUSERSET_MSG:                      return "系统繁忙，请稍后再尝试[20502]";    ///< 存储userset 消息失败
        case ERR_STOREUSERSET_MSGLIST:                  return "系统繁忙，请稍后再尝试[20503]";    ///< 存储userset消息列表失败
        case ERR_LOGIC_NO_DATA:                         return "数据不存在[20504]";               ///< LOGIC 没有查询数据
        case ERR_NO_DATA:                               return "数据不存在[20505]";               ///< dataproxy没有查询到数据
        case ERR_SEND_TO_DATAPROXY:                     return "系统繁忙，请稍后再尝试[20506]";    ///< 发送请求包给dataproxy失败
        case ERR_OPERATOR_MONGO_ERROR:                  return "系统繁忙，请稍后再尝试[20507]";    ///< 操作芒果数据库失败
        case ERR_DATA_PROXY_CALLBACK:                   return "系统繁忙，请稍后再尝试[20508]";    ///< data proxy 返回数据失败
        case ERR_DATA_LOAD_USER_INFO:                   return "系统繁忙，请稍后再尝试[20509]";    ///< 加载用户信息失败
        case ERR_DATA_LOAD_FRIENDS:                     return "系统繁忙，请稍后再尝试[20510]";    ///< 加载好友列表失败
        case ERR_DATA_LOAD_USER_ALL_GROUP:              return "系统繁忙，请稍后再尝试[20511]";    ///< 加载用户拥有群列表失败
        case ERR_DATA_LOAD_USER_ONLINE_STATUS:          return "系统繁忙，请稍后再尝试[20512]";    ///< 加载用户状态失败
        case ERR_DATA_LOAD_AUDIO_TYPE_LIST:             return "系统繁忙，请稍后再尝试[20513]";    ///< 加载音频类型列表失败
        case ERR_SERVER_AUTHCODE:                       return "身份信息错误，请重新登录[21001]";  ///< 验证码错误，请重新登录
        case ERR_USER_OFFLINE:                          return "用户不在线[21002]";               ///< 用户处于离线状态
        case ERR_USER_KICED_OFFLINE:                    return "您的账号在其他设备登录，请重新登录[21003]";    ///< 你的帐号已经别处登录，您被迫下线
        case ERR_PARSE_PRELOGIN_PKT:                    return "数据错误[21004]";               ///< 解预登录请求包错误
        case ERR_PRELOGIN_QRY_IDENTITY:                 return "系统繁忙，请稍后再尝试[21005]";    ///< 预登录从center查询ip,port出错
        case ERR_QUERY_UINFO_FROM_BUSINESS:             return "系统繁忙，请稍后再尝试[21006]";    ///< 从业务查询用户信息失败
        case ERR_CHECK_PRE_LOGIN_TOKEN:                 return "身份信息错误，请重新登录[21007]";  ///< 校验预登录token失败
        case ERR_DEVICE_KICED_OFFLINE:                  return "您的账号在其他设备登录，请重新登录[21008]";    ///< 你的帐号已经在同一时间，同一设备多次登录，您被迫下线!
        case ERR_USER_NOT_EXIST:                        return "用户不存在[21009]";             ///< 用户不存在
        case ERR_USER_INFO_REGISTER:                    return "用户信息注册失败[21010]";        ///< 用户信息注册失败
        case ERR_USER_ALREADY_EXIST:                    return "用户已经存在[21011]";           ///< 用户已经存在
        case ERR_MOBILE_ALREADY_EXIST:                  return "手机号已经被使用[21012]";        ///< 电话已经存在
		case ERR_NICKNAME_ALREADY_EXIST:                return "昵称已被使用[21013]";           ///< 昵称已经存在
		case ERR_USER_INCORRECT_PASSWORD:               return "密码错误[21014]";              ///< 密码错误
    }

    if (code < 10000)
    {
        return "系统错误[20202]";
    }
    else if (code >= 10000 && code < 20000)  // 此种情况应在上面的switch里处理
    {
        return "系统错误[20199]";
    }
    else if (code >= 20000 && code < 30000)  // 此种情况应在上面的switch里处理
    {
        return "逻辑错误[20200]";
    }
    return "未知错误[20200]";
}

}

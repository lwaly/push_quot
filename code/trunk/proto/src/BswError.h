/*******************************************************************************
* Project:  proto
* @file     BswError.h
* @brief    BSW 系统错误定义
* @author   wfh
* @date:    2016年10月20日
* @note
* Modify history:
******************************************************************************/
#ifndef _BSW_ERROR_H_
#define _BSW_ERROR_H_

namespace bsw
{

/**
 * @brief BSW 系统错误码定义
 * @note BSW 系统错误码从20001开始编号
 */
enum E_ERROR_NO
{
/////////////////////////////////<错误代码按功能协议划分:
///<20000-21000 模块公共错误代码
    ERR_OK                                  = 0,        ///< 正确
    // 20000~20199 系统错误码转业务错误码
    ERR_PARASE_PROTOBUF                     = 20000,    ///< 解析Protobuf出错
    ERR_NO_SUCH_WORKER_INDEX                = 20001,    ///< 未知的Worker进程编号
    ERR_UNKNOWN_CMD                         = 20002,    ///< 未知命令字
    ERR_NEW                                 = 20003,    ///< new出错（无法分配堆内存）
    ERR_REDIS_CONN_CLOSE                    = 20004,    ///< redis连接已关闭
    ERR_DISCONNECT                          = 20005,    ///< 已存在的连接发生错误
    ERR_NO_CALLBACK                         = 20006,    ///< 回调不存在或已超时
    ERR_DATA_TRANSFER                       = 20007,    ///< 数据传输出错
    ERR_REPEAT_REGISTERED                   = 20008,    ///< 重复注册
    ERR_SERVERINFO                          = 20009,    ///< 服务器信息错误
    ERR_SESSION                             = 20010,    ///< 获取会话错误
    ERR_BODY_JSON                           = 20011,    ///< 消息体json解析错误
    ERR_SERVERINFO_RECORD                   = 20012,    ///< 存档服务器信息错误
    ERR_INCOMPLET_DATAPROXY_DATA            = 20013,    ///< DataProxy请求数据包不完整
    ERR_INVALID_REDIS_ROUTE                 = 20014,    ///< 无效的redis路由信息
    ERR_REDIS_NODE_NOT_FOUND                = 20015,    ///< 未找到合适的redis节点
    ERR_REGISTERCALLBACK_REDIS              = 20016,    ///< 注册RedisStep错误
    ERR_REDIS_CMD                           = 20017,    ///< redis命令执行出错
    ERR_UNEXPECTED_REDIS_REPLY              = 20018,    ///< 不符合预期的redis结果
    ERR_RESULTSET_EXCEED                    = 20019,    ///< 数据包超过protobuf最大限制
    ERR_LACK_CLUSTER_INFO                   = 20020,    ///< 缺少集群信息
    ERR_TIMEOUT                             = 20021,    ///< 超时
    ERR_REDIS_AND_DB_CMD_NOT_MATCH          = 20022,    ///< redis读写操作与DB读写操作不匹配
    ERR_REDIS_NIL_AND_DB_FAILED             = 20023,    ///< redis结果集为空，但发送DB操作失败
    ERR_NO_RIGHT                            = 20024,    ///< 数据操作权限不足
    ERR_QUERY                               = 20025,    ///< 查询出错，如拼写SQL错误
    ERR_REDIS_STRUCTURE_WITH_DATASET        = 20026,    ///< redis数据结构由DB的各字段值序列化（或串联）而成，请求与存储不符
    ERR_REDIS_STRUCTURE_WITHOUT_DATASET     = 20027,    ///< redis数据结构并非由DB的各字段值序列化（或串联）而成，请求与存储不符
    ERR_DB_FIELD_NUM                        = 20028,    ///< redis数据结构由DB的各字段值序列化（或串联）而成，请求的字段数量错误
    ERR_DB_FIELD_ORDER_OR_FIELD_NAME        = 20029,    ///< redis数据结构由DB的各字段值序列化（或串联）而成，请求字段顺序或字段名错误
    ERR_KEY_FIELD                           = 20030,    ///< redis数据结构由DB的各字段值序列化（或串联）而成，指定的key_field错误或未指定，或未在对应表的数据字典中找到该字段
    ERR_KEY_FIELD_VALUE                     = 20031,    ///< redis数据结构指定的key_field所对应的值缺失或值为空
    ERR_JOIN_FIELDS                         = 20032,    ///< redis数据结构由DB字段串联而成，串联的字段错误
    ERR_LACK_JOIN_FIELDS                    = 20033,    ///< redis数据结构由DB字段串联而成，缺失串联字段
    ERR_REDIS_STRUCTURE_NOT_DEFINE          = 20034,    ///< redis数据结构未在DataProxy的配置中定义
    ERR_INVALID_CMD_FOR_HASH_DATASET        = 20035,    ///< redis hash数据结构由DB的各字段值序列化（或串联）而成，而请求中的hash命令不当
    ERR_DB_TABLE_NOT_DEFINE                 = 20036,    ///< 表未在DataProxy的配置中定义
    ERR_DB_OPERATE_MISSING                  = 20037,    ///< redis数据结构存在对应的DB表，但数据请求缺失对数据库表操作
    ERR_REQ_MISS_PARAM                      = 20038,    ///< 请求参数缺失
    ERR_LACK_TABLE_FIELD                    = 20039,    ///< 数据库表字段缺失
    ERR_TABLE_FIELD_NAME_EMPTY              = 20040,    ///< 数据库表字段名为空
    ERR_UNDEFINE_REDIS_OPERATE              = 20041,    ///< 未定义或不支持的redis数据操作（只支持string和hash的dataset update操作）
    ERR_REDIS_READ_WRITE_CMD_NOT_MATCH      = 20042,    ///< redis读命令与写命令不对称

    ///<20200-20299 系统类错误代码
    ERR_UNKNOWN                              = 20200,    ///< 未知错误
    ERR_UNKNOW_CMD                           = 20201,    ///< 未知命令字
    ERR_SYSTEM_ERROR                         = 20202,    ///< 系统错误（数据库错误等，通过查询后台Server日志可以找到更具体错误码）
    ERR_SERVER_BUSY             	         = 20203,    ///< 服务器繁忙
    ERR_SERVER_ERROR			             = 20204,    ///< 服务器错误（给客户端的错误响应）
    ERR_SERVER_TIMEOUT                       = 20205,    ///< 服务器处理超时
    ERR_SENDTONEXT_FAILED		             = 20206,    ///< SendToNext 发送失败
    ERR_UNKNOWN_RESPONSE_CMD                 = 20207,    ///< 未知响应命令字，通常为对端没有找到对应Cmd处理者，由框架层返回错误
    ERR_ASYNC_TIMEOUT                        = 20208,    ///< 异步操作超时
    ERR_LOAD_CONFIGFILE		                 = 20209,    ///< 模块加载配置文件失败
    ERR_STEP_RUNNING_ERROR		             = 20210,    ///< 注册的step不在运行态
    ERR_REG_STEP_ERROR                       = 20211,    ///< 注册Step失败
    ERR_STEP_EMIT_LIMIT                      = 20212,    ///< STEP emit 次数超过限制
    ERR_LOGIC_SERVER_TIMEOUT                 = 20213,    ///< 逻辑Server处理超时
    ERR_PROC_RELAY                           = 20214,    ///< 数据转发到其它进程失败
    ERR_SEND_TO_LOGIG_MSG                    = 20215,    ///< 发送消息到逻辑服务器失败
    ERR_REG_SESSION                          = 20216,    ///< session注册失败
	ERR_SESSION_CREATE                       = 20217,    ///< 创建 session 失败
    ERR_MALLOC_FAILED		                 = 20218,    ///< new对象失败
    ERR_SEQUENCE                             = 20219,    ///< 错误序列号
    ERR_RC5ENCRYPT_ERROR		             = 20220,    ///< Rc5Encrypt 加密失败
    ERR_RC5DECRYPT_ERROR		             = 20221,    ///< Rc5Decrypt 解密失败
    ERR_REQ_FREQUENCY                        = 20222,    ///< 请求过于频繁（未避免被攻击而做的系统保护）


    ///<20300-20399 协议类错误代码
    ERR_NO_SESSION_ID_IN_MSGBODY             = 20301,    ///< MsgBody缺少session_id
    ERR_NO_ADDITIONAL_IN_MSGBODY	         = 20302,    ///< MsgBody缺少additional
    ERR_MSG_BODY_DECODE		                 = 20303,    ///< msg body解析出错
    ERR_INVALID_PROTOCOL                     = 20304,    ///< 协议错误
    ERR_PACK_INFO_ERROR			             = 20305,    ///< 部分信息打成PB协议包
    ERR_PARSE_PACK_ERROR		             = 20306,    ///< 解析PB协议包
    ERR_REQ_MISS_PB_PARAM		             = 20307,    ///< 请求缺少参数(pb中带的参数不全)
    ERR_PROTOCOL_FORMAT                      = 20308,    ///< 协议格式错误
    ERR_LIST_INCOMPLETE 		             = 20309,    ///< 参数列表缺失或不全
    ERR_BEYOND_RANGE		                 = 20310,    ///< 传入的参数值超过规定范围
    ERR_PARMS_VALUES_MISSING	             = 20311,    ///< 传入的参数在程序传递或解析过程中丢失
    ERR_JSON_PRASE_FAILED		             = 20312,    ///< JSON 数据解析失败
    ERR_INVALID_DATA                         = 20313,    ///< 错误数据
    ERR_INVALID_SESSION_ID                   = 20314,    ///< 错误的session路由信息

    ///<20400-20499 HTTP类错误代码
    ERR_HTTP_SESSION_GET		             = 20401,    ///< HTTP SESSION 获取失败
    ERR_HTTP_NO_HEADER                       = 20402,    ///< http请求没有header
    ERR_HTTP_NO_BODY                         = 20403,    ///< http请求没有body
    ERR_HTTP_NO_COOKIE                       = 20404,    ///< http请求缺少cookie
    ERR_HTTP_NO_METHOD                       = 20405,    ///< http请求缺少请求类型(GET/POST)
    ERR_HTTP_TYPE                            = 20406,    ///< http类型错误(请求/响应)
    ERR_HTTP_METHOD                          = 20407,    ///< http接口错误(GET/POST)
    ERR_HTTP_PARAM_DECODE                    = 20408,    ///< http参数解析失败
    ERR_HTTP_PARAM_MISSING                   = 20409,    ///< http请求缺少参数
    ERR_HTTP_COOKIE_PARAM_DECODE             = 20410,    ///< http请求cookie参数解析失败
    ERR_HTTP_COOKIE_PARAM_MISSING            = 20411,    ///< http请求缺少cookie参数
    ERR_HTTP_PARAM_ERROR                     = 20412,    ///< http参数格式错误
    ERR_HTTP_COOKIE_PARAM_ERROR              = 20413,    ///< http请求Cookie参数错误
    ERR_HTTP_POST_TO_JAVA                    = 20414,    ///< http post数据给java服务时出错

   ///<20500-20599 存储类错误代码
    ERR_SENDUSESET_MSG			             = 20501,    ///< 发送存储userset失败
    ERR_STOREUSERSET_MSG		             = 20502,    ///< 存储userset 消息失败
    ERR_STOREUSERSET_MSGLIST		         = 20503,    ///< 存储userset消息列表失败
    ERR_LOGIC_NO_DATA		                 = 20504,    ///< LOGIC 没有查询数据
    ERR_NO_DATA                              = 20505,    ///< dataproxy没有查询到数据
    ERR_SEND_TO_DATAPROXY                    = 20506,    ///< 发送请求包给dataproxy失败
    ERR_OPERATOR_MONGO_ERROR                 = 20507,    ///< 操作芒果数据库失败
    ERR_DATA_PROXY_CALLBACK                  = 20508,    ///< data proxy 返回数据失败
    ERR_DATA_LOAD_USER_INFO                  = 20509,    ///< 加载用户信息失败
    ERR_DATA_LOAD_FRIENDS                    = 20510,    ///< 加载好友列表失败
    ERR_DATA_LOAD_USER_ALL_GROUP             = 20511,    ///< 加载用户拥有群列表失败
    ERR_DATA_LOAD_USER_ONLINE_STATUS         = 20512,    ///< 加载用户状态失败
    ERR_DATA_LOAD_AUDIO_TYPE_LIST            = 20513,    ///< 加载音频类型列表失败
	ERR_DATA_AUDIO_ADD_UPLOAD_INFO           = 20514,    ///< 音频信息上传失败
	ERR_DATA_AUDIO_ULOAD_ADD_USER_TIMING_LIST     = 20515, ///< 添加个人音频按时间排序最新列表失败
	ERR_DATA_AUDIO_ULOAD_ADD_SYS_TIMING_LIST      = 20516, ///< 添加系统音频按时间排序最新列表失败
	ERR_DATA_AUDIO_ULOAD_ADD_SYS_TYPE_TIMING_LIST = 20517, ///< 添加系统类型音频按时间排序最新列表失败
	ERR_DATA_AUDIO_DELETE_UPLOAD			 = 20518,	 ///< 删除上传音频失败


    ///<21000~21999	用户相关命令字，如用户注册、用户登录、修改用户资料等	用户行为
    ERR_SERVER_AUTHCODE            	         = 21001,    ///< 验证码错误，请重新登录
    ERR_USER_OFFLINE                         = 21002,    ///< 用户处于离线状态
    ERR_USER_KICED_OFFLINE                   = 21003,    ///< 你的帐号已经别处登录，您被迫下线
    ERR_PARSE_PRELOGIN_PKT                   = 21004,    ///< 解预登录请求包错误
    ERR_PRELOGIN_QRY_IDENTITY                = 21005,    ///< 预登录从center查询ip,port出错
    ERR_QUERY_UINFO_FROM_BUSINESS            = 21006,    ///< 从业务查询用户信息失败
    ERR_CHECK_PRE_LOGIN_TOKEN                = 21007,    ///< 校验预登录token失败
    ERR_DEVICE_KICED_OFFLINE                 = 21008,    ///< 你的帐号已经在同一时间，同一设备多次登录，您被迫下线!
    ERR_USER_NOT_EXIST                       = 21009,    ///< 用户不存在
    ERR_USER_INFO_REGISTER                   = 21010,    ///< 用户信息注册失败
    ERR_USER_ALREADY_EXIST                   = 21011,    ///< 用户已经存在
    ERR_MOBILE_ALREADY_EXIST                 = 21012,    ///< 电话已经存在
    ERR_NICKNAME_ALREADY_EXIST               = 21013,    ///< 昵称已经存在
	ERR_USER_INCORRECT_PASSWORD              = 21014,    ///< 密码错误
};


}

#endif /* _BSW_ERROR_H_ */

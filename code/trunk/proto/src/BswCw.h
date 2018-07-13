/*******************************************************************************
* Project:  proto
* @file     BSW Cw.h
* @brief    BSW 业务命令字定义
* @author   wfh
* @date:    2015年10月12日
* @note
* Modify history:
******************************************************************************/
#ifndef _BSW_CW_H_
#define _BSW_CW_H_

namespace bsw
{

/**
 * @brief BSW 业务命令字定义
 * @note BSW 业务命令字成对出现，从1001开始编号，并且遵从奇数表示请求命令字，
 * 偶数表示应答命令字，应答命令字 = 请求命令字 + 1
 */
enum E_BSW_COMMAND
{
	CMD_UNDEFINE                        = 0,        ///< 未定义
	CMD_REQ_SYS_ERROR					= 999,		///< 系统错误请求（无意义，不会被使用）
	CMD_RSQ_SYS_ERROR					= 1000,		///< 系统错误响应

	// 用户相关命令字，如用户注册、用户登录、修改用户资料等，号段：1001~2000
	CMD_REQ_USER_GET_CONFIG             = 1001,     ///< 用户取配置请求
	CMD_RSP_USER_GET_CONFIG             = 1002,     ///< 用户取配置应答

	CMD_REQ_USER_PRE_LOGIN              = 1003,     ///< 用户预登录请求
	CMD_RSP_USER_PRE_LOGIN              = 1004,     ///< 用户预登录应答

	CMD_REQ_USER_LOGIN                  = 1005,     ///< 用户登录请求
	CMD_RSP_USER_LOGIN                  = 1006,     ///< 用户登录应答

    CMD_REQ_USER_LOGOUT                 = 1007,     ///< 用户退出请求
    CMD_RSP_USER_LOGOUT                 = 1008,     ///< 用户退出应答

    CMD_RSP_USER_KICED_OFFLINE          = 1009,     ///< 用户被迫下线通知

	//////////////////////////////////////////////////////////////////////////
	//audio(2001-3000)
	CMD_REQ_GET_AUDIO_TYPE_LIST         = 2001, // 获取音频类型列表
	CMD_RSP_GET_AUDIO_TYPE_LIST         = 2002, // 获取音频类型列表响应

	CMD_REQ_AUDIO_UPLOAD                = 2003, // 上传音频信息记录请求
	CMD_RSP_AUDIO_UPLOAD                = 2004, // 上传音频信息记录回执

	CMD_REQ_DEL_UPLOAD_AUDIO			= 2007, // 删除上传音频
	CMD_RSP_DEL_UPLOAD_AUDIO			= 2008,	// 删除上传音频响应

	CMD_REQ_RENAME_UPLOAD_AUDIO			= 2011, // 重命名上传音频
	CMD_RSP_RENAME_UPLOAD_AUDIO			= 2012,	// 重命名上传音频响应

	CMD_REQ_GET_UPLOADED_RECORDS        = 2015, // 获取已上传音频记录协议
	CMD_RSP_GET_UPLOADED_RECORDS        = 2016, // 获取已上传音频记录协议响应

	CMD_REQ_AUDIO_DOWNLOAD              = 2005, // 下载音频信息记录请求
	CMD_RSP_AUDIO_DOWNLOAD              = 2006, // 下载音频信息记录回执

	CMD_REQ_DEL_DOWNLOAD_AUDIO			= 2009, // 删除下载音频
	CMD_RSP_DEL_DOWNLOAD_AUDIO			= 2010,	// 删除下载音频响应

	CMD_REQ_RENAME_DOWNLOAD_AUDIO		= 2013, // 重命名下载音频
	CMD_RSP_RENAME_DOWNLOAD_AUDIO		= 2014,	// 重命名下载音频响应

	CMD_REQ_GET_DOWNLOADED_RECORDS      = 2017, // 获取已下载音频记录协议
	CMD_RSP_GET_DOWNLOADED_RECORDS      = 2018, // 获取已下载音频记录协议响应

    CMD_REQ_GET_AUDIO_AUDIT_RECORDS     = 2023, // 获取审核音频记录协议
    CMD_RSP_GET_AUDIO_AUDIT_RECORDS     = 2024, // 获取审核音频记录协议响应

    CMD_REQ_MODIFY_AUDIO_AUDIT_RECORD   = 2025, // 修改审核音频记录协议
    CMD_RSP_MODIFY_AUDIO_AUDIT_RECORD   = 2026, // 修改审核音频记录协议响应

	//////////////////////////////////////////////////////////////////////////
	//picture(3001-4000)
	CMD_REQ_GET_PIC_TYPE_LIST			        = 3001, // 获取画质类型列表
	CMD_RSP_GET_PIC_TYPE_LIST			        = 3002, // 获取画质类型列表响应

	CMD_REQ_PIC_UPLOAD					        = 3003, // 上传画质信息记录请求
	CMD_RSP_PIC_UPLOAD					        = 3004, // 上传画质信息记录回执

	CMD_REQ_PIC_DLOAD_ADD				        = 3005, // 下载画质请求
	CMD_RSP_PIC_DLOAD_ADD				        = 3006, // 下载画质回执

	CMD_REQ_PIC_DLOAD_DEL				        = 3009, //删除下载画质请求
	CMD_RSP_PIC_DLOAD_DEL				        = 3010, //删除下载画质回执

	CMD_REQ_DEL_UPLOAD_PIC			            = 3007, // 删除上传画质
	CMD_RSP_DEL_UPLOAD_PIC			            = 3008,	// 删除上传画质响应

	CMD_REQ_RENAME_UPLOAD_PIC			        = 3011, // 重命名上传画质
	CMD_RSP_RENAME_UPLOAD_PIC			        = 3012,	// 重命名上传画质响应

	CMD_REQ_RENAME_DLOAD_PIC                    = 3013, // 重命名下载画质
	CMD_RSP_RENAME_DLOAD_PIC                    = 3014, // 重命名下载画质响应

	CMD_REQ_GET_UPLOADED_RECORDS_PIC            = 3015, // 获取已上传画质记录协议
	CMD_RSP_GET_UPLOADED_RECORDS_PIC            = 3016, // 获取已上传画质记录协议响应

	CMD_REQ_GET_DOWNLOADED_RECORDS_PIC          = 3017, // 获取已下载画质记录协议
	CMD_RSP_GET_DOWNLOADED_RECORDS_PIC          = 3018, // 获取已下载画质记录协议响应

    CMD_REQ_PIC_UPLOAD_AUDIT_STATUS_MODIFY      = 3019, // 修改审核画质记录协议
    CMD_RSP_PIC_UPLOAD_AUDIT_STATUS_MODIFY      = 3020, // 修改审核画质记录协议响应

    CMD_REQ_PIC_UPLOAD_AUDIT_STATUS_GET         = 3021, // 修改审核画质记录协议
    CMD_RSP_PIC_UPLOAD_AUDIT_STATUS_GET         = 3022, // 修改审核画质记录协议响应
	//////////////////////////////////////////////////////////////////////////
	//effect(4001-5000)
	CMD_REQ_EFFECT_DLOAD_ADD                    = 4001, // 下载画质请求
	CMD_RSP_EFFECT_DLOAD_ADD                    = 4002, // 下载画质回执

	CMD_REQ_EFFECT_DLOAD_DEL                    = 4003, // 删除下载画质请求
	CMD_RSP_EFFECT_DLOAD_DEL                    = 4004, // 删除下载画质回执

	CMD_REQ_EFFECT_RENAME                       = 4005, // 重命名特效
	CMD_RSP_EFFECT_RENAME                       = 4006, // 重命名特效响应

	CMD_REQ_EFFECT_GET                          = 4007, // 获取已下载特效记录协议
	CMD_RSP_EFFECT_GET                          = 4008, // 获取已下载特效记录协议响应

    CMD_REQ_EFFECT_UPLOAD                       = 4009, // 上传特效信息记录请求
    CMD_RSP_EFFECT_UPLOAD                       = 4010, // 上传特效信息记录回执

    CMD_REQ_DEL_UPLOAD_EFFECT                   = 4011, // 删除上传特效
    CMD_RSP_DEL_UPLOAD_EFFECT                   = 4012, // 删除上传特效响应

	CMD_REQ_RENAME_UPLOAD_EFFECT                = 4013, // 重命名下载特效
	CMD_RSP_RENAME_UPLOAD_EFFECT                = 4014, // 重命名下载特效响应

	CMD_REQ_GET_UPLOADED_RECORDS_EFFECT         = 4015, // 获取单个上传特效记录协议
	CMD_RSP_GET_UPLOADED_RECORDS_EFFECT         = 4016, // 获取单个上传特效记录协议响应

    CMD_REQ_GET_UPLOAD_PAGE_RECORDS_EFFECT      = 4017, // 获取已上传特效记录协议
    CMD_RSP_GET_UPLOAD_PAGE_RECORDS_EFFECT      = 4018, // 获取已上传特效记录协议响应
	//////////////////////////////////////////////////////////////////////////
	//audio_type(5001-6000)
	CMD_REQ_AUDIO_TYPE_ADD				= 5001, //音频类型添加请求
	CMD_RSP_AUDIO_TYPE_ADD				= 5002, //音频类型添加回执

	CMD_REQ_AUDIO_TYPE_DEL				= 5003, //音频类型删除请求
	CMD_RSP_AUDIO_TYPE_DEL				= 5004, //音频类型删除回执

	CMD_REQ_AUDIO_TYPE_UPDATE			= 5005, //音频类型更新请求
	CMD_RSP_AUDIO_TYPE_UPDATE			= 5006, //音频类型更新回执

	CMD_REQ_AUDIO_TYPE_FIND				= 5007, //音频类型查询请求
	CMD_RSP_AUDIO_TYPE_FIND				= 5008, //音频类型查询回执

	CMD_REQ_AUDIO_TYPE_GET_RECORD		= 5009, //音频类型分页获取请求
	CMD_RSP_AUDIO_TYPE_GET_RECORD		= 5010, //音频类型分页获取回执

	//////////////////////////////////////////////////////////////////////////
	//camera_type(6001-7000)
	CMD_REQ_CAMERA_TYPE_ADD				= 6001, //摄像头类型添加请求
	CMD_RSP_CAMERA_TYPE_ADD				= 6002, //摄像头类型添加回执

	CMD_REQ_CAMERA_TYPE_DEL				= 6003, //摄像头类型删除请求
	CMD_RSP_CAMERA_TYPE_DEL				= 6004, //摄像头类型删除回执

	CMD_REQ_CAMERA_TYPE_UPDATE			= 6005, //摄像头类型更新请求
	CMD_RSP_CAMERA_TYPE_UPDATE			= 6006, //摄像头类型更新回执

	CMD_REQ_CAMERA_TYPE_FIND			= 6007, //摄像头类型查询请求
	CMD_RSP_CAMERA_TYPE_FIND			= 6008, //摄像头类型查询回执

	CMD_REQ_CAMERA_TYPE_GET_RECORD		= 6009, //摄像头类型分页获取请求
	CMD_RSP_CAMERA_TYPE_GET_RECORD		= 6010, //摄像头类型分页获取回执


	//////////////////////////////////////////////////////////////////////////
	//data transfer(7001-8000)
	CMD_REQ_DT_AUDIO_UPLOAD				= 7001, //音频上传记录数据迁移请求
	CMD_RSP_DT_AUDIO_UPLOAD				= 7002, //音频上传记录数据迁移回调

    CMD_REQ_DT_AUDIO_DOWNLOAD			= 7003, //音频下载记录数据迁移请求
    CMD_RSP_DT_AUDIO_DOWNLOAD			= 7004, //音频下载记录数据迁移回调

    CMD_REQ_DT_PIC_UPLOAD				= 7005, //画质上传记录数据迁移请求
    CMD_RSP_DT_PIC_UPLOAD				= 7006, //画质上传记录数据迁移回调

    CMD_REQ_DT_PIC_DOWNLOAD				= 7007, //画质下载记录数据迁移请求
    CMD_RSP_DT_PIC_DOWNLOAD				= 7008, //画质下载记录数据迁移回调

	CMD_REQ_DT_AUDIO_TYPE				= 7011, //音频类型记录数据迁移请求
	CMD_RSP_DT_AUDIO_TYPE				= 7012, //音频类型记录数据迁移回调

	CMD_REQ_DT_CAMERA_TYPE				= 7013, //摄像头类型记录数据迁移请求
	CMD_RSP_DT_CAMERA_TYPE				= 7014, //摄像头类型记录数据迁移回调
};

}   // end of namespace bsw

#endif /* _BSW_CW_H_ */

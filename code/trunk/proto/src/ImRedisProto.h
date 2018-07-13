/*******************************************************************************
* Project:  proto
* @file     RedisData.hpp
* @brief    Redis数据结构定义
* @author   wfh
* @date:    2016年11月15日
* @note
* Modify history:
******************************************************************************/
#ifndef _REDIS_DATA_HPP_
#define _REDIS_DATA_HPP_

enum E_REDIS_TYPE
{
    REDIS_T_HASH                = 1,    ///< redis hash
    REDIS_T_SET                 = 2,    ///< redis set
    REDIS_T_KEYS                = 3,    ///< redis keys
    REDIS_T_STRING              = 4,    ///< redis string
    REDIS_T_LIST                = 5,    ///< redis list
    REDIS_T_SORT_SET            = 6,    ///< redis sort set
};

enum E_REDIS_IM_DATA
{
    /* 属性 */
    BSW_DATA_USER_INFO          = 1,    ///< 用户基础属性数据，对应数据库中db_userinfo.tb_userinfo_xx
    BSW_DATA_USER_ONLINE        = 2,    ///< 用户在线状态

    //音频
    BSW_DATA_SYS_AUDIO_TYPE                    = 120,   ///< 音频类型列表
    BSW_DATA_AUDIO_UPLOAD_USER_RECORD          = 101,   ///< 用户上传音频信息记录
    BSW_DATA_AUDIO_UPLOAD_USER_TIMING_LIST     = 102,   ///< 用户根据时间排序(个人最新)的上传音频列表
    BSW_DATA_USER_DOWNLOAD_AUDIO               = 111,   ///< 用户下载音频信息记录
    BSW_DATA_USER_DOWNLOAD_AUDIO_TIMING_LIST   = 112,   ///< 用户根据时间排序(个人最新)的下载音频列表
    BSW_DATA_SYS_AUDIO_HOT_LIST                = 121,   ///< 音频下载次数
    BSW_DATA_AUDIO_UPLOAD_SYS_TIMING_LIST      = 122,   ///< 系统音频最新(根据上传时间)
    BSW_DATA_SYS_TYPE_AUDIO_HOT_LIST           = 123,   ///< 音频分类型下载次数
    BSW_DATA_AUDIO_UPLOAD_SYS_TYPE_TIMING_LIST = 124,   ///< 系统根据类型音频最新(根据上传时间)
    BSW_DATA_AUDIO_UNAUDIT_LIST                = 125,   ///< 未审核列表
    BSW_DATA_AUDIO_AUDIT_NOT_LIST              = 127,   ///< 审核未通过列表

    //画质
    BSW_DATA_USER_UPLOAD_PICTURE                = 201,  //用户上传画质列表信息
    BSW_DATA_USER_UPLOAD_PICTURE_TIMING_LIST    = 202,  //用户根据时间排序的上传画质列表
    BSW_DATA_USER_DOWNLOAD_PICTURE              = 211,  //用户下载画质列表信息
    BSW_DATA_USER_DOWNLOAD_PICTURE_TIMING_LIST  = 212,  //用户根据时间排序的下载画质列表
    BSW_DATA_SYS_PICTURE_TYPE                   = 220,  //系统画质类型数据。
    BSW_DATA_SYS_PICTURE_HOT_LIST               = 221,  //系统画质最热(根据下载次数)
    BSW_DATA_SYS_PICTURE_TIMING_LIST            = 222,  //系统画质最新(根据上传时间)
    BSW_DATA_SYS_TYPE_PICTURE_HOT_LIST          = 223,  //系统根据类型画质最热(根据下载次数)
    BSW_DATA_SYS_TYPE_PICTURE_TIMING_LIST       = 224,  //系统根据类型画质最新(根据上传时间)

    //特效
	BSW_DATA_EFFECT_UPLOAD_USER_RECORD          = 301,   ///< 用户上传特效信息记录
    BSW_DATA_EFFECT_UPLOAD_USER_TIMING_LIST     = 302,   ///< 用户上传特效最新(根据上传时间)
    BSW_DATA_USER_DOWNLOAD_EFFECT               = 311,   ///< 用户下载特效信息记录
    BSW_DATA_USER_DOWNLOAD_EFFECT_TIMING_LIST   = 312,   ///< 用户根据时间排序(个人最新)的下载特效列表
    BSW_DATA_SYS_EFFECT_HOT_LIST                = 321,   ///< 特效下载次数
    BSW_DATA_EFFECT_UPLOAD_SYS_TIMING_LIST      = 322,   ///< 系统特效最新(根据上传时间)
};

#endif /* _REDIS_DATA_HPP_ */

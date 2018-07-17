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

    DATA_QUOT_CONFIG = 201,   ///< 行情配置
};

#endif /* _REDIS_DATA_HPP_ */

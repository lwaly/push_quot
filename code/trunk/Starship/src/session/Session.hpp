/*******************************************************************************
 * Project:  AsyncServer
 * @file     Session.hpp
 * @brief    会话基类
 * @author   lbh
 * @date:    2015年7月28日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SESSION_HPP_
#define SESSION_HPP_

#include "log4cplus/logger.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/loggingmacros.h"
#include "ev.h"         // need ev_tstamp
#include "OssDefine.hpp"
#include "labor/OssLabor.hpp"

namespace oss
{

enum SESSION_LOAD_STATUS
{
    SESSION_UNLOAD      = 0,
    SESSION_LOADING     = 1,
    SESSION_LOADED      = 2,
};

class OssWorker;

class Session
{
public:
    Session(uint32 ulSessionId, ev_tstamp dSessionTimeout = 60.0, const std::string& strSessionClass = "oss::Session");
    Session(const std::string& strSessionId, ev_tstamp dSessionTimeout = 60.0, const std::string& strSessionClass = "oss::Session");
    virtual ~Session();

    /**
     * @brief 会话超时回调
     */
    virtual E_CMD_STATUS Timeout() = 0;

protected:
    bool RegisterCallback(Session* pSession);

    /**
     * @brief 删除回调步骤
     * @param pSession 回调步骤实例
     */
    void DeleteCallback(Session* pSession);

    bool RegisterCallback(Step* pStep);

    /**
     * @brief 删除回调步骤
     * @param pStep 回调步骤实例
     */
    void DeleteCallback(Step* pStep);

    uint32 GetNodeId();
    uint32 GetWorkerIndex();

    /**
     * @brief 获取当前节点类型
     * @return 当前节点类型
     */
    const std::string& GetNodeType() const;

    /**
     * @brief 获取Server自定义配置
     * @return Server自定义配置
     */
    const loss::CJsonObject& GetCustomConf() const;

    /**
     * @brief 获取当前时间
     * @note 获取当前时间，比time(NULL)速度快消耗小，不过没有time(NULL)精准，如果对时间精度
     * 要求不是特别高，建议调用GetNowTime()替代time(NULL)
     * @return 当前时间
     */
    time_t GetNowTime() const;

    /**
     * @brief 预处理
     * @note 预处理用于将等待预处理对象与框架建立弱联系，使被处理的对象可以获得框架一些工具，如写日志指针
     * @param pStep 等待预处理的Step
     * @return 预处理结果
     */
    bool Pretreat(Step* pStep);

    log4cplus::Logger GetLogger()
    {
        return (*m_pLogger);
    }

    OssLabor* GetLabor()
    {
        return(m_pLabor);
    }

public:
    const std::string& GetSessionId() const
    {
        return(m_strSessionId);
    }

    const std::string& GetSessionClass() const
    {
        return(m_strSessionClassName);
    }

    /**
     * @brief 设置会话最近刷新时间
     */
    void SetActiveTime(ev_tstamp activeTime)
    {
        m_activeTime = activeTime;
    }

    /**
     * @brief 获取会话刷新时间
     */
    ev_tstamp GetActiveTime() const
    {
        return(m_activeTime);
    }

    /**
     * @brief 获取会话刷新时间
     */
    ev_tstamp GetTimeout() const
    {
        return(m_dSessionTimeout);
    }

    /**
     * @brief 是否注册了回调
     * @return 注册结果
     */
    bool IsRegistered() const
    {
        return(m_bRegistered);
    }

private:
    void SetLabor(OssLabor* pLabor)
    {
        m_pLabor = pLabor;
    }

    void SetLogger(log4cplus::Logger* pLogger)
    {
        m_pLogger = pLogger;
    }

    /**
     * @brief 设置为已注册状态
     */
    void SetRegistered()
    {
        m_bRegistered = true;
    }

private:
    bool m_bRegistered;
    ev_tstamp m_dSessionTimeout;
    ev_tstamp m_activeTime;
    std::string m_strSessionId;
    std::string m_strSessionClassName;
    OssLabor* m_pLabor;
    log4cplus::Logger* m_pLogger;
    ev_timer* m_pTimeoutWatcher;

    friend class OssWorker;
};

} /* namespace oss */

#endif /* SESSION_HPP_ */

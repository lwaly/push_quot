/*******************************************************************************
 * Project:  PluginServer
 * @file     Cmd.cpp
 * @brief 
 * @author   bwarliao
 * @date:    2015年3月6日
 * @note
 * Modify history:
 ******************************************************************************/
#include "Cmd.hpp"

namespace oss
{

Cmd::Cmd()
    : m_pErrBuff(NULL), m_pLabor(0), m_pLogger(0), m_iCmd(0)
{
    m_pErrBuff = new char[gc_iErrBuffLen];
}

Cmd::~Cmd()
{
    if (m_pErrBuff != NULL)
    {
        delete[] m_pErrBuff;
        m_pErrBuff = NULL;
    }
}

uint32 Cmd::GetNodeId()
{
    return(m_pLabor->GetNodeId());
}

uint32 Cmd::GetWorkerIndex()
{
    return(m_pLabor->GetWorkerIndex());
}

bool Cmd::RegisterCallback(Step* pStep)
{
    return(m_pLabor->RegisterCallback(pStep));
}

void Cmd::DeleteCallback(Step* pStep)
{
    m_pLabor->DeleteCallback(pStep);
}

bool Cmd::Pretreat(Step* pStep)
{
    return(m_pLabor->Pretreat(pStep));
}

bool Cmd::RegisterCallback(Session* pSession)
{
    return(m_pLabor->RegisterCallback(pSession));
}

void Cmd::DeleteCallback(Session* pSession)
{
    return(m_pLabor->DeleteCallback(pSession));
}

Session* Cmd::GetSession(uint32 uiSessionId, const std::string& strSessionClass)
{
    return(m_pLabor->GetSession(uiSessionId, strSessionClass));
}

Session* Cmd::GetSession(const std::string& strSessionId, const std::string& strSessionClass)
{
    return(m_pLabor->GetSession(strSessionId, strSessionClass));
}

bool Cmd::RegisterCallback(const std::string& strRedisNodeType, RedisStep* pRedisStep)
{
    return(GetLabor()->RegisterCallback(strRedisNodeType, pRedisStep));
}

bool Cmd::RegisterCallback(const std::string& strHost, int iPort, RedisStep* pRedisStep)
{
    return(GetLabor()->RegisterCallback(strHost, iPort, pRedisStep));
}

} /* namespace oss */

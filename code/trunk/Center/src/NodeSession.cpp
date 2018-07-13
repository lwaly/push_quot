/*
 * NodeSession.cpp
 *
 *  Created on: 2015年11月6日
 *      Author: xxx
 */
#include "NodeSession.h"

namespace oss {
#define LOAD_CENTER_CMD(centerconf,name,member) if (!centerconf.Get(name, member))  \
                { char errStr[64];snprintf(errStr,sizeof(errStr),"center cmd load(%s) failed",name);err = errStr;return false;}

bool NodeSession::Init(const loss::CJsonObject& conf, std::string& err) {
    if (boInit) {
        return true;
    }
    std::string dbip;
    LOAD_CENTER_CMD(conf, "dbip", dbip); //连接db地址
    LOAD_CENTER_CMD(conf, "nodereport_timeinterval", m_nodereport_timeinterval); //节点上报时间间隔(目前未使用，暂时设置为10)
    LOAD_CENTER_CMD(conf, "nodestatuscheck_timeinterval", m_nodestatuscheck_timeinterval); //节点状态检查时间间隔
    LOAD_CENTER_CMD(conf, "nodeoffline_timeinterval", m_nodeoffline_timeinterval); //检查节点下线时间间隔
    LOAD_CENTER_CMD(conf, "deleteofflinenode_timeinterval", m_deleteofflinenode_timeinterval); //检查下线节点信息删除时间间隔
    LOAD_CENTER_CMD(conf, "nodeloadlog_timeinterval", m_nodeloadlog_timeinterval); //节点负载日志写入时间间隔
    LOAD_CENTER_CMD(conf, "nodeloadlog_overdue", m_nodeloadlog_overdue); //节点负载日志过时时间
    LOAD_CENTER_CMD(conf, "nodeloadstatistics_timeinterval", m_nodeloadstatistics_timeinterval); //节点负载统计时间间隔
    LOAD_CENTER_CMD(conf, "nodeloadstatistics_overdue", m_nodeloadstatistics_overdue); //节点负载统计过时时间
    LOAD_CENTER_CMD(conf, "nodeloadcheck_timeinterval", m_nodeloadcheck_timeinterval); //节点负载检查时间(检查节点负载的日志和统计的时间间隔)
    LOAD_CENTER_CMD(conf, "serverdataloadstatuslog_overdue", m_serverdataloadstatuslog_overdue); //服务器数据负载日志过时时间
    LOAD_CENTER_CMD(conf, "serverdataloadstatuslogcheck_timeinterval", m_serverDataLoadCheck_timeinterval); //服务器数据负载日志检查时间间隔
    LOAD_CENTER_CMD(conf, "center_inner_host", m_center_inner_host); //中心服务器内网地址
    LOAD_CENTER_CMD(conf, "center_inner_port", m_center_inner_port); //中心服务器内网端口
    LOAD_CENTER_CMD(conf, "center_node_type", m_center_node_type); //中心服务器节点类型
    LOAD_CENTER_CMD(conf, "center_process_num", m_center_process_num); //中心服务器工作进程数
    //连接db用户、db密码、db库名、db字符集、db端口
    m_pMysqlDbi = new loss::CMysqlDbi(dbip.c_str(), conf("dbuser").c_str(),
                                      conf("dbpwd").c_str(), conf("dbname").c_str(),
                                      conf("dbcharacterset").c_str(),
                                      ::atoi(conf("dbport").c_str()));
    if (NULL == m_pMysqlDbi) {
        err = "center cmd load DB failed";
        return false;
    }
    boInit = true;
    return true;
}

bool NodeSession::loadNodeTypes() {
    nodeTypesVec.clear();
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1, "select * from %s", NODE_TYPE_TABLE);
    loss::T_vecResultSet vecRes;
    if (0 != m_pMysqlDbi->ExecSql(szSql, vecRes)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "loadNodeTypes error,%d:%s",
                            m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    if (vecRes.empty()) { //没有节点类型信息
        LOG4CPLUS_ERROR_FMT(GetLogger(), "loadNodeTypes empty,%d:%s",
                            m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    NodeType nodeType;
    for (loss::T_vecResultSet::iterator it = vecRes.begin(); it != vecRes.end();
            ++it) {
        nodeType.clear();
        loss::T_mapRow& valmap = *it;
        //服务器类型
        loss::T_mapRow::iterator mapit = valmap.find("nodetype");
        if (valmap.end() == mapit) {
            continue;
        }
        nodeType.nodetype = valmap["nodetype"];
        //服务器loadso
        mapit = valmap.find("loadso");
        if (valmap.end() == mapit) {
            continue;
        }
        nodeType.loadso = valmap["loadso"];
        //需要获取的服务器信息
        mapit = valmap.find("noticeservers");
        if (valmap.end() == mapit) {
            continue;
        }
        const std::string& noticeservers = valmap["noticeservers"];
        LOG4CPLUS_DEBUG_FMT(GetLogger(),
                            "loadNodeTypes nodetype(%s),noticeservers(%s)",
                            nodeType.nodetype.c_str(), noticeservers.c_str());
        if (!noticeservers.empty()) {
            loss::CJsonObject jParse;
            if (jParse.Parse(noticeservers) && jParse.IsArray()) {
                for (int i = 0; i < jParse.GetArraySize(); ++i) {
                    std::string nodeTypeStr = jParse[i].ToString();
                    LOG4CPLUS_DEBUG_FMT(GetLogger(),
                                        "loadNodeTypes parse noticeservers(%s)",
                                        nodeTypeStr.c_str());
                    std::string::iterator nodeTypeStrIt = std::remove(
                            nodeTypeStr.begin(), nodeTypeStr.end(),
                            '\"');
                    nodeTypeStr.erase(nodeTypeStrIt, nodeTypeStr.end());
                    LOG4CPLUS_DEBUG_FMT(GetLogger(),
                                        "loadNodeTypes parse noticeservers(%s)",
                                        nodeTypeStr.c_str());
                    nodeType.neededServers.push_back(nodeTypeStr);
                }
            } else {
                LOG4CPLUS_WARN_FMT(GetLogger(),
                                   "loadNodeTypes parse noticeservers(%s) failed",
                                   noticeservers.c_str());
            }
        }
        nodeTypesVec.push_back(nodeType);
    }
    return (true);
}
//检查节点类型
bool NodeSession::checkNodeType(const std::string& node_type) {
    for (NodeTypesVec::iterator it = nodeTypesVec.begin();
            it != nodeTypesVec.end(); ++it) {
        if (node_type == it->nodetype) {
            return true;
        }
    }
    return false;
}

oss::uint32 NodeSession::getNewNodeID() {
    uint32 tmpID(0);
    for (NodeInfoMapIT it = m_mapNodeInfo.begin(); it != m_mapNodeInfo.end();
            ++it) {
        if (it->second.node_id > (int) tmpID) {
            tmpID = it->second.node_id;
        }
    }
    ++tmpID;
    m_nodeid = tmpID;
    return m_nodeid;
}
void NodeSession::AddNodeInfo(const std::string& NodeKey,
                              const st_NodeInfo& Info) {
    NodeInfoMapIT iter = m_mapNodeInfo.find(NodeKey);
    if (iter == m_mapNodeInfo.end()) {
        m_mapNodeInfo.insert(make_pair(NodeKey, Info));
    }
}
bool NodeSession::DelNodeInfo(const std::string& NodeKey) {
    std::map<std::string, st_NodeInfo>::iterator iter = m_mapNodeInfo.find(
                NodeKey);
    if (iter != m_mapNodeInfo.end()) {
        m_mapNodeInfo.erase(iter);
        return true;
    }
    return false;
}
bool NodeSession::GetNodeInfo(const std::string& NodeKey, st_NodeInfo& nInfo) {
    st_NodeInfo* pInfo = NULL;
    std::map<std::string, st_NodeInfo>::iterator iter = m_mapNodeInfo.find(
                NodeKey);
    if (iter != m_mapNodeInfo.end()) {
        pInfo = &iter->second;
        nInfo = *pInfo;
        return true;
    }
    return false;
}
st_NodeInfo* NodeSession::GetNodeInfo(const std::string& NodeKey) {
    std::map<std::string, st_NodeInfo>::iterator iter = m_mapNodeInfo.find(
                NodeKey);
    if (iter != m_mapNodeInfo.end()) {
        return &iter->second;
    }
    return NULL;
}
//写节点到数据库，如果是报告信息需要设置boReport = true
bool NodeSession::WriteNodeDataToDB(const st_NodeInfo& nodeInfo, bool boReport) {
    setCurrentTime();
    if (!WriteNodeStatus(nodeInfo)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "WriteNodeStatus failed");
        return (false);
    }
    if (boReport) {
        if (!NodeloadCheck()) {
            LOG4CPLUS_WARN_FMT(GetLogger(), "NodeloadCheck failed");
            return (false);
        }
        if (!WriteNodeLog()) {
            LOG4CPLUS_WARN_FMT(GetLogger(), "WriteNodeLog failed");
            return (false);
        }
        if (!WriteNodeStatistics(nodeInfo)) {
            LOG4CPLUS_WARN_FMT(GetLogger(), "WriteNodeStatistics failed");
            return (false);
        }
    }
    return (true);
}
bool NodeSession::SetNodeDataOfflineToDBByNodeId(int node_id) {
    setCurrentTime();
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "update %s set serverstatus=%d WHERE nodeid=%d",
             NODE_LOAD_STATUS_TABLE, eNodeStatus_Offline, node_id);
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_WARN_FMT(GetLogger(),
                           "SetNodeDataOfflineToDBByNodeId failed %d:%s",
                           m_pMysqlDbi->GetErrno(),
                           m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    return (true);
}
const NodeSession::NodeType* NodeSession::getNodeTypeServerInfo(
    const std::string& regservertype) {
    NodeTypesVec::iterator it = nodeTypesVec.begin();
    NodeTypesVec::iterator itEnd = nodeTypesVec.end();
    for (; it != itEnd; ++it) {
        const NodeType& nodetype = *it;
        if (regservertype == nodetype.nodetype) {
            return (&nodetype);
        }
    }
    return (NULL);
}

//删除超时的下线节点状态到数据库
bool NodeSession::ClearOverdueOfflineNodeStatusToDB() {
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "delete from %s WHERE activetime <= %llu and serverstatus=%d",
             NODE_LOAD_STATUS_TABLE,
             (uint64)(m_currenttime - m_deleteofflinenode_timeinterval),
             eNodeStatus_Offline);
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "ClearOverdueOfflineNodeStatusToDB error %d:%s",
                            m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    return (true);
}
bool NodeSession::CheckOfflineNodeStatusToDB() {
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "update %s set serverstatus=%d WHERE activetime <= %llu",
             NODE_LOAD_STATUS_TABLE, eNodeStatus_Offline,
             (uint64)(m_currenttime - m_nodeoffline_timeinterval));
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "CheckOfflineNodeStatusToDB error,mysql error %d:%s",
                            m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    //    if (m_pMysqlDbi->AffectRows() > 0)
    //    {
    //        LOG4CPLUS_INFO_FMT(GetLogger(),
    //                        "OfflineNodeStatusToDB update nodes(%d),now(%llu),nodeofflinetime(%llu),check condition(%llu)",
    //                        m_pMysqlDbi->AffectRows(), m_currenttime,
    //                        m_nodeoffline_timeinterval,
    //                        (uint64)(m_currenttime - m_nodeoffline_timeinterval));
    //    }
    return (true);
}
bool NodeSession::ReplaceNodeStatusToDB(const st_NodeInfo& nodeInfo) {
    char szSql[3096];
    snprintf(szSql, sizeof(szSql) - 1,
             "replace into %s values(%d,'%s',%d,'%s',%d,'%s',%d,%d,%llu,%d,%d,%d,%d,%d,%d,%d,'%s')",
             NODE_LOAD_STATUS_TABLE, nodeInfo.node_id,
             nodeInfo.node_type.c_str(), nodeInfo.node_port,
             nodeInfo.node_ip.c_str(), nodeInfo.node_access_port,
             nodeInfo.node_access_ip.c_str(), eNodeStatus_Online,
             nodeInfo.worker_num, m_currenttime, nodeInfo.load,
             nodeInfo.connect, nodeInfo.client, nodeInfo.recvNum,
             nodeInfo.sendNum, nodeInfo.recvByte, nodeInfo.sendByte,
             nodeInfo.worker.c_str()); //时间以服务器时间为准
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "ReplaceNodeStatusToDB node(%d,%s) error %d:%s",
                            nodeInfo.node_id, nodeInfo.node_type.c_str(),
                            m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    //    LOG4CPLUS_INFO_FMT(GetLogger(),
    //                    "ReplaceNodeStatusToDB nodes node_type(%s) now(%llu) port(%d) ip(%s)",
    //                    nodeInfo.node_type.c_str(), m_currenttime,
    //                    nodeInfo.node_port, nodeInfo.node_ip.c_str());
    return (true);
}
bool NodeSession::UpdateNodeStatusToDB(const st_NodeInfo& nodeInfo) {
    if (nodeInfo.node_id == 0) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "UpdateNodeStatusToDB node ,node id is zero(%d,%s)",
                            nodeInfo.node_id, nodeInfo.node_type.c_str());
        return (false);
    }
    char szSql[3096];
    snprintf(szSql, sizeof(szSql) - 1,
             "update %s set serverstatus=%d,workernum=%d,activetime=%llu,connect=%d,client=%d,serverload=%d,recvnum=%d,sendnum=%d,recvbyte=%d,sendbyte=%d,worker='%s' WHERE innerport=%d and innerip='%s'",
             NODE_LOAD_STATUS_TABLE, eNodeStatus_Online,
             nodeInfo.worker_num, nodeInfo.active_time, nodeInfo.connect,
             nodeInfo.client, nodeInfo.load, nodeInfo.recvNum,
             nodeInfo.sendNum, nodeInfo.recvByte, nodeInfo.sendByte,
             nodeInfo.worker.c_str(), nodeInfo.node_port,
             nodeInfo.node_ip.c_str());
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "UpdateNodeStatusToDB node(%d,%s)error %d:%s",
                            nodeInfo.node_id, nodeInfo.node_type.c_str(),
                            m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    return (true);
}
bool NodeSession::CheckNodeStatusToDB(const st_NodeInfo& nodeInfo) {
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "select nodeid from %s WHERE innerport=%d and innerip='%s'",
             NODE_LOAD_STATUS_TABLE, nodeInfo.node_port,
             nodeInfo.node_ip.c_str());
    loss::T_vecResultSet vecRes;
    if (0 != m_pMysqlDbi->ExecSql(szSql, vecRes)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "CheckNodeStatusToDB error,%d:%s",
                            m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    if (vecRes.empty()) { //没有该节点信息
        return (false);
    }
    return (true);
}
bool NodeSession::ClearNodeStatusToDB(const st_NodeInfo& nodeInfo) {
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "delete from %s WHERE innerport=%d and innerip='%s'",
             NODE_LOAD_STATUS_TABLE, nodeInfo.node_port,
             nodeInfo.node_ip.c_str());
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "ClearNodeStatusToDBByNodeId nodetype=%s and innerport=%d and innerip=%s",
                            nodeInfo.node_type.c_str(), nodeInfo.node_port,
                            nodeInfo.node_ip.c_str());
        return (false);
    }
    return (true);
}
bool NodeSession::ClearNodeStatusToDBByNodeID(const st_NodeInfo& nodeInfo) {
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "delete from %s WHERE nodeid=%d and serverstatus=%d",
             NODE_LOAD_STATUS_TABLE, nodeInfo.node_id,
             eNodeStatus_Offline);
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "ClearNodeStatusToDBByNodeId nodetype=%s and innerport=%d and innerip=%s",
                            nodeInfo.node_type.c_str(), nodeInfo.node_port,
                            nodeInfo.node_ip.c_str());
        return (false);
    }
    return (true);
}
bool NodeSession::GetNodeStatusByNodeType(const std::string& nodetype,
        std::vector<NodeLoadStatus>& nodeStatusList) {
    nodeStatusList.clear();
    NodeLoadStatus nodeStatus;
    std::map<std::string, st_NodeInfo>::iterator iter = m_mapNodeInfo.begin();
    for (; iter != m_mapNodeInfo.end(); ++iter) {
        if (nodetype == iter->second.node_type) {
            nodeStatus.clear();
            nodeStatus = iter->second;
            LOG4CPLUS_DEBUG_FMT(GetLogger(),
                                "GetNodeStatusByNodeType(innerip:%s,innerport:%d,outerip:%s,outerport:%d)",
                                nodeStatus.innerip.c_str(), nodeStatus.innerport,
                                nodeStatus.outerip.c_str(), nodeStatus.outerport);
            nodeStatusList.push_back(nodeStatus);
        }
    }
    if (nodeStatusList.empty()) {
        return (false);
    }
    return (true);
}

bool NodeSession::GetNodeStatusByNodeTypeFromDB(const std::string& nodetype,
        std::vector<NodeLoadStatus>& nodeStatusList) {
    nodeStatusList.clear();
    if (!checkNodeType(nodetype)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "no such nodetype(%s)",
                           nodetype.c_str());
        return (false);
    }
    char szSql[3096];
    snprintf(szSql, sizeof(szSql) - 1,
             "select * from %s where nodetype ='%s' and serverstatus=%d",
             NODE_LOAD_STATUS_TABLE, nodetype.c_str(),
             eNodeStatus_Online);
    loss::T_vecResultSet vecRes;
    if (0 != m_pMysqlDbi->ExecSql(szSql, vecRes)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "get NodeStatus error,nodetype(%s),,%d:%s",
                            nodetype.c_str(), m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    if (vecRes.empty()) { //没有节点状态信息
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "get NodeStatus empty,nodetype(%s),%d:%s",
                            nodetype.c_str(), m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    NodeLoadStatus nodeStatus;
    for (loss::T_vecResultSet::iterator it = vecRes.begin(); it != vecRes.end();
            ++it) {
        nodeStatus.clear();
        loss::T_mapRow& valmap = *it;
        //服务器nodetype
        loss::T_mapRow::iterator mapit = valmap.find("nodetype");
        if (valmap.end() == mapit) {
            continue;
        }
        nodeStatus.nodetype = valmap["nodetype"];
        //服务器serverload
        mapit = valmap.find("serverload");
        if (valmap.end() == mapit) {
            continue;
        }
        nodeStatus.serverload = atoi(valmap["serverload"].c_str());
        //服务器innerport
        mapit = valmap.find("innerport");
        if (valmap.end() == mapit) {
            continue;
        }
        nodeStatus.innerport = atoi(valmap["innerport"].c_str());
        //服务器innerip
        mapit = valmap.find("innerip");
        if (valmap.end() == mapit) {
            continue;
        }
        nodeStatus.innerip = valmap["innerip"];
        //服务器outerport
        mapit = valmap.find("outerport");
        if (valmap.end() == mapit) {
            continue;
        }
        nodeStatus.outerport = atoi(valmap["outerport"].c_str());
        //服务器outerip
        mapit = valmap.find("outerip");
        if (valmap.end() == mapit) {
            continue;
        }
        nodeStatus.outerip = valmap["outerip"];
        nodeStatusList.push_back(nodeStatus);
    }
    return (true);
}

//写当前的节点状态
bool NodeSession::WriteNodeStatus(const st_NodeInfo& nodeInfo) {
    if (!ReplaceNodeStatusToDB(nodeInfo)) {
        return (false);
    }
    CheckNodeStatus();
    return (true);
}
//检查节点状态
bool NodeSession::CheckNodeStatus() {
    if (m_currenttime
            >= m_nodestatuscheck_lasttime
            + m_nodestatuscheck_timeinterval) {
        ClearOverdueOfflineNodeStatusToDB(); //删除长时间下线的节点信息
        m_nodestatuscheck_lasttime = m_currenttime;
    }
    CheckOfflineNodeStatusToDB(); //检查一段时间不活跃的节点信息，设置其为下线
    return true;
}
//节点日志操作
bool NodeSession::WriteNodeLog() {
    if (m_currenttime >= m_nodeloadlog_lasttime + m_nodeloadlog_timeinterval) {
        return InsertNodeLogToDB();
    }
    return (true);
}
bool NodeSession::InsertNodeLogToDB() {
    m_nodeloadlog_lasttime = m_currenttime;
    char szSql[3096];
    NodeInfoMapIT it = m_mapNodeInfo.begin();
    NodeInfoMapIT itEnd = m_mapNodeInfo.end();
    for (; it != itEnd; ++it) {
        const st_NodeInfo& nodeInfo = it->second;
        snprintf(szSql, sizeof(szSql) - 1,
                 "insert into %s values(%d,'%s',%d,'%s',%d,'%s',%d,%llu,%d,%d,%d,%d,%d,%d,%d,'%s')",
                 NODE_LOAD_LOG_TABLE, nodeInfo.node_id,
                 nodeInfo.node_type.c_str(), nodeInfo.node_port,
                 nodeInfo.node_ip.c_str(), nodeInfo.node_access_port,
                 nodeInfo.node_access_ip.c_str(), nodeInfo.worker_num,
                 m_currenttime, nodeInfo.load, nodeInfo.connect,
                 nodeInfo.client, nodeInfo.recvNum, nodeInfo.sendNum,
                 nodeInfo.recvByte, nodeInfo.sendByte,
                 nodeInfo.worker.c_str());
        if (0 != m_pMysqlDbi->ExecSql(szSql)) {
            LOG4CPLUS_ERROR_FMT(GetLogger(),
                                "InsertNodeLogToDB node(%d,%s)error %d:%s",
                                nodeInfo.node_id, nodeInfo.node_type.c_str(),
                                m_pMysqlDbi->GetErrno(),
                                m_pMysqlDbi->GetError().c_str());
            return (false);
        }
    }
    return (true);
}
//节点统计操作
bool NodeSession::WriteNodeStatistics(const st_NodeInfo& nodeInfo) {
    //统计信息
    NodeStatisticsInfoMapIT it = m_mapNodeStatisticsInfo.find(
                                     nodeInfo.getNodeKey());
    if (it == m_mapNodeStatisticsInfo.end()) {
        st_NodeStaisticsInfo statisticsinfo(nodeInfo);
        m_mapNodeStatisticsInfo.insert(
            make_pair(nodeInfo.getNodeKey(), statisticsinfo));
    } else {
        it->second += nodeInfo;
    }
    if (m_currenttime
            >= m_nodeloadstatistics_lasttime
            + m_nodeloadstatistics_timeinterval) {
        return InsertNodeStatisticsToDB();
    }
    return (true);
}
bool NodeSession::InsertNodeStatisticsToDB() {
    m_nodeloadstatistics_lasttime = m_currenttime;
    char szSql[256];
    NodeStatisticsInfoMapIT it = m_mapNodeStatisticsInfo.begin();
    NodeStatisticsInfoMapIT itEnd = m_mapNodeStatisticsInfo.end();
    for (; it != itEnd; ++it) {
        st_NodeStaisticsInfo& nodeInfo = it->second;
        snprintf(szSql, sizeof(szSql) - 1,
                 "insert into %s values(%d,'%s',%d,'%s',%d,'%s',%d,%llu,%d,%d,%d,%d,%d,%d,%d)",
                 NODE_LOAD_STATISTICS_TABLE, nodeInfo.node_id,
                 nodeInfo.node_type.c_str(), nodeInfo.node_port,
                 nodeInfo.node_ip.c_str(), nodeInfo.node_access_port,
                 nodeInfo.node_access_ip.c_str(), nodeInfo.worker_num,
                 m_currenttime, nodeInfo.load, nodeInfo.client,
                 nodeInfo.connect, nodeInfo.recvNum, nodeInfo.sendNum,
                 nodeInfo.recvByte, nodeInfo.sendByte);
        if (0 != m_pMysqlDbi->ExecSql(szSql)) {
            LOG4CPLUS_ERROR_FMT(GetLogger(),
                                "InsertNodeStatisticsToDB error %d:%s",
                                m_pMysqlDbi->GetErrno(),
                                m_pMysqlDbi->GetError().c_str());
            return (false);
        }
        nodeInfo.clearLoad();
    }
    return (true);
}
bool NodeSession::NodeloadCheck() {
    if (m_currenttime
            >= m_nodeloadcheck_lasttime + m_nodeloadcheck_timeinterval) {
        m_nodeloadcheck_lasttime = m_currenttime;
        ClearOverdueOfflineNodeLogToDB();
        ClearOverdueOfflineNodeStatisticsToDB();
    }
    return (true);
}
bool NodeSession::ClearOverdueOfflineNodeLogToDB() {
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "delete from %s WHERE activetime <= %llu",
             NODE_LOAD_LOG_TABLE,
             (uint64)(m_currenttime - m_nodeloadlog_overdue));
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_WARN_FMT(GetLogger(),
                           "ClearOverdueOfflineNodeLogToDB failed,error %d:%s",
                           m_pMysqlDbi->GetErrno(),
                           m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "ClearOverdueOfflineNodeLogToDB ok");
    return (true);
}
bool NodeSession::ClearOverdueOfflineNodeStatisticsToDB() {
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "delete from %s WHERE currenttime <= %llu",
             NODE_LOAD_STATISTICS_TABLE,
             (uint64)(m_currenttime - m_nodeloadstatistics_overdue));
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_WARN_FMT(GetLogger(),
                           "ClearOverdueOfflineNodeStatisticsToDB failed,error %d:%s",
                           m_pMysqlDbi->GetErrno(),
                           m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "ClearOverdueOfflineNodeStatisticsToDB ok");
    return (true);
}

bool NodeSession::WriteServerDataLoadToDB(const char* nodetype, int innerport,
        const char* innerip, int outerport, const char* outerip,
        const char* status) {
    setCurrentTime();
    ServerDataLoadCheck();
    if (!ReplaceServerDataLoadStatusToDB(nodetype, innerport, innerip, outerport,
                                         outerip, status)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "failed to ReplaceServerDataLoadStatusToDB nodetype:%s", nodetype);
        return false;
    }
    if (!WriteServerDataLoadLogToDB(nodetype, innerport, innerip, outerport, outerip,
                                    status)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "failed to WriteServerDataLoadLogToDB nodetype:%s", nodetype);
        return false;
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "succeeded to WriteServerDataLoadToDB nodetype:%s", nodetype);
    return (true);
}
bool NodeSession::ServerDataLoadCheck() {
    if (m_currenttime
            >= m_serverdataloadcheck_lasttime
            + m_serverDataLoadCheck_timeinterval) {
        m_serverdataloadcheck_lasttime = m_currenttime;
        ClearOverdueServerDataLogToDB();
    }
    return (true);
}
bool NodeSession::ReplaceServerDataLoadStatusToDB(const char* nodetype,
        int innerport, const char* innerip, int outerport,
        const char* outerip, const char* status) {
    char szSql[4096];
    snprintf(szSql, sizeof(szSql) - 1,
             "replace into %s values('%s',%d,'%s',%d,'%s','%s','%s')",
             NODE_SERVER_DATA_STATUS_TABLE, nodetype, innerport, innerip,
             outerport, outerip, status, loss::time_t2TimeStr(m_currenttime).c_str());
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "ReplaceServerDataLoadStatusToDB  error %d:%s",
                            m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    return (true);
}
bool NodeSession::WriteServerDataLoadLogToDB(const char* nodetype,
        int innerport, const char* innerip, int outerport,
        const char* outerip, const char* status) {
    char szSql[4096];
    snprintf(szSql, sizeof(szSql) - 1,
             "insert into %s values('%s',%d,'%s',%d,'%s','%s','%s')",
             NODE_SERVER_DATA_LOG_TABLE, nodetype, innerport, innerip,
             outerport, outerip, status, loss::time_t2TimeStr(m_currenttime).c_str());
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "WriteServerDataLoadLogToDB error %d:%s",
                            m_pMysqlDbi->GetErrno(),
                            m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    return (true);
}

bool NodeSession::ClearOverdueServerDataLogToDB() {
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1, "delete from %s WHERE time <= %llu",
             NODE_SERVER_DATA_LOG_TABLE,
             (uint64)(m_currenttime - m_nodeloadlog_overdue));
    if (0 != m_pMysqlDbi->ExecSql(szSql)) {
        LOG4CPLUS_WARN_FMT(GetLogger(),
                           "ClearOverdueOfflineNodeLogToDB failed,error %d:%s",
                           m_pMysqlDbi->GetErrno(),
                           m_pMysqlDbi->GetError().c_str());
        return (false);
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "ClearOverdueOfflineNodeLogToDB ok");
    return (true);
}

}
;
//name space im

/*
 * NodeSession.h
 *
 *  Created on: 2015年10月21日
 *      Author: xxx
 */

#ifndef CODE_CENTERSERVER_SRC_NODESESSION_H_
#define CODE_CENTERSERVER_SRC_NODESESSION_H_
#include <string>
#include <map>
#include "dbi/MysqlDbi.hpp"
#include "session/Session.hpp"
#include "Comm.hpp"
#include "NodeRegMgr.hpp"

//需要常住内存，超时时间一年，使用后继续延长
#define NODE_SESSION_TIME (3600 * 24 * 30 * 12)
#define NODE_SESSIN_ID (20000)
#define NODE_LOAD_STATUS_TABLE "tb_nodeload_status"
#define NODE_LOAD_LOG_TABLE "tb_nodeload_log"
#define NODE_LOAD_STATISTICS_TABLE "tb_nodeload_statistics"
#define NODE_SERVER_CONFIG_TABLE "tb_serverconfig"
#define NODE_MANAGE_TABLE "tb_node_manage"
#define NODE_TYPE_TABLE "tb_nodetype"
#define NODE_SERVER_DATA_STATUS_TABLE "tb_serverdata_status"
#define NODE_SERVER_DATA_LOG_TABLE "tb_serverdata_log"

enum NodeStatus {
    eNodeStatus_Online = 1, eNodeStatus_Offline = 2,
};
namespace oss {

class NodeSession: public Session {
    friend class NodeRegMgr;
public:
    NodeSession()
        : Session(NODE_SESSIN_ID, NODE_SESSION_TIME), m_nodeRegMgr(
              this), m_center_inner_port(0), m_center_process_num(
              0), boInit(false), m_nodeid(0), m_pMysqlDbi(
              NULL), m_currenttime(0), m_nodeoffline_timeinterval(0), m_deleteofflinenode_timeinterval(
              0), m_nodereport_timeinterval(0), m_nodestatuscheck_timeinterval(
              0), m_nodeloadlog_timeinterval(0), m_nodeloadlog_overdue(
              0), m_nodeloadstatistics_timeinterval(0), m_nodeloadstatistics_overdue(
              0), m_nodeloadcheck_timeinterval(0), m_serverDataLoadCheck_timeinterval(
              0), m_serverdataloadstatuslog_overdue(0), m_nodeloadlog_lasttime(
              0), m_nodeloadstatistics_lasttime(0), m_nodeloadcheck_lasttime(
              0), m_nodestatuscheck_lasttime(0), m_serverdataloadcheck_lasttime(
              0) {
    }
    virtual ~NodeSession() {
        if (m_pMysqlDbi) {
            delete m_pMysqlDbi;
        }
    }
    bool Init(const loss::CJsonObject& conf, std::string& err);
    //加载节点类型
    bool loadNodeTypes();
    //检查节点类型
    bool checkNodeType(const std::string& node_type);
    //会话超时
    E_CMD_STATUS Timeout() {
        return STATUS_CMD_RUNNING;
    }
    //获取新的节点id
    oss::uint32 getNewNodeID();
    // 添加标识的节点信息到在线节点管理器中去
    void AddNodeInfo(const std::string& NodeKey, const st_NodeInfo& Info);
    //从在线节点管理器删除节点信息
    bool DelNodeInfo(const std::string& NodeKey);
    //从在线节点管理器中获取节点信息
    bool GetNodeInfo(const std::string& NodeKey, st_NodeInfo& nInfo);
    //从在线节点管理器中获取节点信息
    st_NodeInfo* GetNodeInfo(const std::string& NodeKey);
    //获取节点数量
    inline oss::uint32 GetMapNodeInfoSize() {
        return m_mapNodeInfo.size();
    }
    //获取mysql last error
    inline const std::string& GetLastMysqlError() const {
        return m_pMysqlDbi->GetError();
    }
    //获取 mysql连接对象
    inline loss::CMysqlDbi* GetMysqlDbi() const {
        return m_pMysqlDbi;
    }
    //写节点到数据库，如果是报告信息需要设置boReport = true
    bool WriteNodeDataToDB(const st_NodeInfo& nodeInfo, bool boReport = false);
    //设置服务器为下线
    bool SetNodeDataOfflineToDBByNodeId(int node_id);
    struct NodeType {
        std::string nodetype;
        std::string loadso;
        std::vector<std::string> neededServers;
        void clear() {
            nodetype.clear();
            loadso.clear();
            neededServers.clear();
        }
    };
    typedef std::vector<NodeType> NodeTypesVec;
    //节点类型
    NodeTypesVec nodeTypesVec;
    //根据节点类型获取所有节点状态
    bool GetNodeStatusByNodeType(const std::string& nodetype,
                                 std::vector<NodeLoadStatus>& nodeStatusList);
    //根据节点类型获取所有节点状态(从DB)
    bool GetNodeStatusByNodeTypeFromDB(const std::string& nodetype, std::vector<NodeLoadStatus>& nodeStatusList);
    //获取注册服务器需要的服务器类型
    const NodeType* getNodeTypeServerInfo(const std::string& regservertype);
    //注册节点管理器
    NodeRegMgr m_nodeRegMgr;
    typedef std::map<std::string, st_NodeInfo> NodeInfoMap;
    typedef NodeInfoMap::iterator NodeInfoMapIT;
    typedef NodeInfoMap::const_iterator NodeInfoMapCIT;

    typedef std::map<std::string, st_NodeStaisticsInfo> NodeStatisticsInfoMap;
    typedef NodeStatisticsInfoMap::iterator NodeStatisticsInfoMapIT;
    typedef NodeStatisticsInfoMap::const_iterator NodeStatisticsInfoMapCIT;
    //在线节点管理器（key为节点类型：IP：端口，value为节点信息）
    NodeInfoMap m_mapNodeInfo;
    //在线节点统计管理器（key为节点类型：IP：端口，value为节点统计信息）
    NodeStatisticsInfoMap m_mapNodeStatisticsInfo;
    //写server负载到db
    bool WriteServerDataLoadToDB(const char* nodetype, int innerport,
                                 const char* innerip, int outerport, const char* outerip, const char* status);
    std::string m_center_inner_host;//中心服务器内网地址
    int m_center_inner_port;//中心服务器内网端口
    std::string m_center_node_type;//中心服务器节点类型
    int m_center_process_num;//中心服务器工作进程数
private:
    void setCurrentTime() {
        m_currenttime = ::time(NULL);
    }
    //删除超时的下线节点状态到数据库
    bool ClearOverdueOfflineNodeStatusToDB();
    //检查设置超时节点为下线到数据库
    bool CheckOfflineNodeStatusToDB();
    //替换插入节点的状态（为上线）到数据库
    bool ReplaceNodeStatusToDB(const st_NodeInfo& nodeInfo);
    //更新节点的状态（为上线）到数据库
    bool UpdateNodeStatusToDB(const st_NodeInfo& nodeInfo);
    //(根据节点ip和端口)检查指定节点在数据库中是否存在
    bool CheckNodeStatusToDB(const st_NodeInfo& nodeInfo);
    //(根据节点ip和端口)删除指定节点的状态到数据库
    bool ClearNodeStatusToDB(const st_NodeInfo& nodeInfo);
    //(根据节点id)删除指定节点的状态信息到数据库（需下线的节点）
    bool ClearNodeStatusToDBByNodeID(const st_NodeInfo& nodeInfo);
    //写当前的节点状态
    bool WriteNodeStatus(const st_NodeInfo& nodeInfo);
    //检查节点状态
    bool CheckNodeStatus();
    //写入节点日志到数据库
    bool WriteNodeLog();
    //插入节点日志到数据库
    bool InsertNodeLogToDB();
    //写入节点统计到数据库
    bool WriteNodeStatistics(const st_NodeInfo& nodeInfo);
    //插入数据到节点统计表
    bool InsertNodeStatisticsToDB();
    //节点负载检查
    bool NodeloadCheck();
    //清理数据库中的节点日志数据中的超时信息
    bool ClearOverdueOfflineNodeLogToDB();
    //清理数据库中的统计节点数据中的超时信息
    bool ClearOverdueOfflineNodeStatisticsToDB();
    //redis负载检查
    bool ServerDataLoadCheck();
    //写服务器数据负载状态
    bool ReplaceServerDataLoadStatusToDB(const char* nodetype, int innerport,
                                         const char* innerip, int outerport, const char* outerip, const char* status);
    //写服务器数据负载日志
    bool WriteServerDataLoadLogToDB(const char* nodetype, int innerport,
                                    const char* innerip, int outerport, const char* outerip, const char* status);
    //清除过期Dataproxy日志
    bool ClearOverdueServerDataLogToDB();
private:
    bool boInit;
    oss::uint32 m_nodeid; //节点分配器
    loss::CMysqlDbi* m_pMysqlDbi; //数据库连接
    oss::uint64 m_currenttime; //当前时间
    int m_nodeoffline_timeinterval; //检查节点下线时间间隔
    int m_deleteofflinenode_timeinterval; //检查下线节点信息删除时间间隔
    int m_nodereport_timeinterval; //节点上报时间间隔
    int m_nodestatuscheck_timeinterval; //节点状态检查时间间隔
    int m_nodeloadlog_timeinterval; //节点负载日志写入时间间隔
    int m_nodeloadlog_overdue; //节点负载日志过时时间
    int m_nodeloadstatistics_timeinterval; //节点负载统计时间间隔
    int m_nodeloadstatistics_overdue; //节点负载统计过时时间
    int m_nodeloadcheck_timeinterval; //节点负载检查时间(检查节点负载的日志和统计的时间间隔)
    int m_serverDataLoadCheck_timeinterval; //服务器数据负载日志检查时间间隔
    int m_serverdataloadstatuslog_overdue; //服务器数据负载日志过时时间
    oss::uint64 m_nodeloadlog_lasttime; //节点负载最后日志时间
    oss::uint64 m_nodeloadstatistics_lasttime; //节点负载最后统计时间
    oss::uint64 m_nodeloadcheck_lasttime; //节点负载最后检查时间
    oss::uint64 m_nodestatuscheck_lasttime; //服务器状态检查时间
    oss::uint64 m_serverdataloadcheck_lasttime; //dataproxy状态检查时间
};

}
;

#endif /* CODE_CENTERSERVER_SRC_NODESESSION_H_ */

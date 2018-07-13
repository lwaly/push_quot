/*******************************************************************************
 * Project:  CenterServer
 * @file     NodeRegMgr.cpp
 * @brief    告知对端己方Worker进程信息
 * @author   xxx
 * @date:    2015年8月13日
 * @note
 * Modify history:
 ******************************************************************************/
#include "NodeRegMgr.hpp"
#include "NodeSession.h"

namespace oss {

NodeRegMgr::NodeRegMgr(NodeSession* pSess)
    : m_pSess(pSess) {
}
NodeRegMgr::~NodeRegMgr() {
}
int NodeRegMgr::regNode(const tagMsgShell& stMsgShell,
                        const MsgHead& oInMsgHead, const MsgBody& oInMsgBody,
                        st_NodeInfo& nodeinfo) {
    m_regMsgShell = stMsgShell;
    m_regNodeInfoMsg = nodeinfo;
    st_NodeInfo* pNodeInfo = m_pSess->GetNodeInfo(nodeinfo.getNodeKey());
    if (pNodeInfo) { //注册过的只需要更新节点信息
        if (!pNodeInfo->update(nodeinfo)) {
            LOG4CPLUS_WARN_FMT(m_pSess->GetLogger(),
                               "pNodeInfo(%d) update failed", nodeinfo.node_id);
        }
        LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(),
                            "regNode register already!");
        m_regNodeInfoMsg = *pNodeInfo; //以服务器注册服务器信息为准
    } else { //没有注册过的需要检查,然后分配节点并注册
        LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(), "check node(%d,%s,%d)",
                            m_regNodeInfoMsg.node_id, m_regNodeInfoMsg.node_ip.c_str(), m_regNodeInfoMsg.node_port);
        //获取注册的服务器配置
        if (!GetServerDataFromDB(m_regNodeInfoMsg.node_type.c_str(), m_regNodeInfoMsg.node_ip.c_str())) {
            LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(), "GetServerDataFromDB error");
            return ERR_SERVERINFO;
        }
        NodeInfoDBVerifyRet verifyRet = m_regNodeInfoDB.verify(m_regNodeInfoMsg);
        if (eNodeInfoDBVerifyRet_OK != verifyRet) {
            LOG4CPLUS_WARN_FMT(m_pSess->GetLogger(),
                               "m_regNodeInfoDB  verify error(%s)",
                               m_regNodeInfoDB.getError(verifyRet));
            return ERR_SERVERINFO;
        }
        if (!m_centerNodeInfoDB.isLoaded()) {
            //获取中心服务器配置
            if (!GetCenterDataFromDB()) {
                LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(), "GetCenterDataFromDB error");
                return ERR_SERVERINFO;
            }
        }
        m_regNodeInfoMsg.node_id = nodeinfo.node_id = m_pSess->getNewNodeID(); //需用节点id分配器来分配节点ID
        if (!m_pSess->WriteNodeDataToDB(nodeinfo)) { //记录到数据库
            LOG4CPLUS_WARN_FMT(m_pSess->GetLogger(), "WriteNodeDataToDB false(%s)",
                               m_pSess->GetLastMysqlError().c_str());
            return ERR_SERVERINFO_RECORD;
        }
        LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(), "(%s):before AddNodeInfo size(%u)",
                            __FUNCTION__, m_pSess->GetMapNodeInfoSize());
        //加入到节点管理MAP
        m_pSess->AddNodeInfo(m_regNodeInfoMsg.getNodeKey(), m_regNodeInfoMsg);
        LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(), "(%s):after AddNodeInfo size(%u)",
                            __FUNCTION__, m_pSess->GetMapNodeInfoSize());
    }
    //返回注册响应,会分配节点id
    ResponseNodeReg(stMsgShell, oInMsgHead, oInMsgBody, ERR_OK);
    //发送注册服务器的配置信息(注册成功后才调用)
    SendServerConfig(stMsgShell, oInMsgHead, oInMsgBody);
    //给注册者发其他服务器通知、注册者给其它服务发通知
    SendNodeRegNotice(oInMsgHead, oInMsgBody);
    return (ERR_OK);
}
bool NodeRegMgr::loadServerConfig(const std::string& nodetype,
                                  std::string& serverconfig, short configtype) {  //加载服务器配置
    if (nodetype.empty()) {
        LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(),
                            "nodetype empty when loadServerConfig");
        return (false);
    }
    serverconfig.clear();
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "SELECT * from  %s WHERE nodetype='%s' AND configtype=%d",
             NODE_SERVER_CONFIG_TABLE, nodetype.c_str(), configtype);
    loss::T_vecResultSet vecRes;
    if (0 != m_pSess->GetMysqlDbi()->ExecSql(szSql, vecRes)) {
        LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(),
                            "SELECT from %s error,errno(%d:%s),szSql(%s)",
                            NODE_SERVER_CONFIG_TABLE,
                            m_pSess->GetMysqlDbi()->GetErrno(),
                            m_pSess->GetMysqlDbi()->GetError().c_str(),
                            szSql);
        return (false);
    }
    if (vecRes.empty()) {
        LOG4CPLUS_INFO_FMT(m_pSess->GetLogger(),
                           "SELECT from %s empty,check config table,nodetype(%s),configtype(%d),szSql(%s)",
                           NODE_SERVER_CONFIG_TABLE, nodetype.c_str(), configtype,
                           szSql);
        return (false);
    }
    loss::T_mapRow& valmap = *vecRes.begin();
    if (valmap.end() == valmap.find("config")) {
        LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(),
                            "%s loadFromMapRow don't find config segment",
                            NODE_SERVER_CONFIG_TABLE);
        return (false);
    }
    serverconfig = valmap["config"];
    return (true);
}

bool NodeRegMgr::ResponseNodeReg(const tagMsgShell& stMsgShell,
                                 const MsgHead& oInMsgHead, const MsgBody& oInMsgBody, int iRet) {
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    oOutMsgHead.set_cmd(CMD_RSP_NODE_REGISTER);//注册节点应答
    oOutMsgHead.set_seq(oInMsgHead.seq());
    loss::CJsonObject jObjReturn;
    jObjReturn.Add("errcode", iRet);
    jObjReturn.Add("node_id", iRet ? 0 : m_regNodeInfoMsg.node_id);
    oOutMsgBody.set_body(jObjReturn.ToString());
    oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
    if (iRet) {
        LOG4CPLUS_WARN_FMT(m_pSess->GetLogger(), "register node(%s)false",
                           m_regNodeInfoDB.nodetype);
    } else {
        LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(), "register node(%s) ok nodeid(%d)",
                            m_regNodeInfoDB.nodetype, m_regNodeInfoMsg.node_id);
    }
    return m_pSess->GetLabor()->SendTo(stMsgShell, oOutMsgHead, oOutMsgBody);
}
bool NodeRegMgr::SendServerConfig(const tagMsgShell& stMsgShell,
                                  const MsgHead& oInMsgHead, const MsgBody& oInMsgBody) {
    const std::string& reg_node_type = m_regNodeInfoMsg.node_type;
    if (!reg_node_type.empty()) {
        //发送server配置
        std::string serverconfig;
        if (!loadServerConfig(reg_node_type, serverconfig)) { //从DB加载server配置
            LOG4CPLUS_INFO_FMT(m_pSess->GetLogger(),
                               "CmdNodeReg::AnyMessage,register node type(%s) don't has configuration",
                               serverconfig.c_str());
            return (false);
        }
        MsgHead oServerConfigOutMsgHead;
        MsgBody oServerConfigOutMsgBody;
        oServerConfigOutMsgHead.set_cmd(CMD_REQ_SERVER_CONFIG);
        oServerConfigOutMsgHead.set_seq(m_pSess->GetLabor()->GetSequence());
        loss::CJsonObject jParse;
        jParse.Clear();
        if (!jParse.Parse(serverconfig)) {
            LOG4CPLUS_WARN_FMT(m_pSess->GetLogger(),
                               "CmdNodeReg::AnyMessage,(%s)load server coinfig from DB,it 's not json,check serverconfig: %s",
                               reg_node_type.c_str(), serverconfig.c_str());
        }
        LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(),
                            "send server config out configuration format[%s]",
                            serverconfig.c_str());
        oServerConfigOutMsgBody.set_body(serverconfig);
        oServerConfigOutMsgHead.set_msgbody_len(
            oServerConfigOutMsgBody.ByteSize());
        if (!m_pSess->GetLabor()->SendTo(stMsgShell, oServerConfigOutMsgHead,
                                         oServerConfigOutMsgBody)) {
            LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(),
                                "fail to send to send server data config");
            return (false);
        }
    } else {
        LOG4CPLUS_WARN_FMT(m_pSess->GetLogger(), "register node_type is empty");
        return (false);
    }
    return (true);
}

//获取需要的已注册的服务器的节点信息(json格式)
bool NodeRegMgr::GetNeededRegisteredNodes(
    const std::vector<std::string>& neededServers,
    loss::CJsonObject& jObj) {
    const st_NodeInfo* pInfo = NULL;
    jObj.AddEmptySubArray("node_arry_reg");
    loss::CJsonObject tmember;
    for (std::map<std::string, st_NodeInfo>::const_iterator it_iter =
                m_pSess->m_mapNodeInfo.begin();
            it_iter != m_pSess->m_mapNodeInfo.end(); ++it_iter) { //已注册服务器
        pInfo = &it_iter->second;
        LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(),
                            "NodeRegMgr::GetNeededRegisteredNodes node_type[%s]",
                            pInfo->node_type.c_str());

        for (std::vector<std::string>::const_iterator it =
                    neededServers.begin(); it != neededServers.end(); ++it) {

            if (*it == pInfo->node_type) { //判断是否是需要的服务器类型
                tmember.Clear();
                tmember.Add("node_type", pInfo->node_type);
                tmember.Add("node_ip", pInfo->node_ip);
                tmember.Add("node_port", pInfo->node_port);
                tmember.Add("worker_num", pInfo->worker_num);
                jObj["node_arry_reg"].Add(tmember);
            }
        }
    }
    //中心服务器信息
    tmember.Clear();
    tmember.Add("node_type", m_pSess->m_center_node_type);
    tmember.Add("node_ip", m_pSess->m_center_inner_host);
    tmember.Add("node_port", m_pSess->m_center_inner_port);
    tmember.Add("worker_num", m_pSess->m_center_process_num);
    jObj["node_arry_reg"].Add(tmember);
    LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(),
                        "GetNeededRegisteredNodes json[%s],center_node_type(%s)",
                        tmember.ToString().c_str(),
                        m_pSess->m_center_node_type.c_str());
    return (true);
}

//给注册者发其他服务器通知、注册者给其它服务发通知
int NodeRegMgr::SendNodeRegNotice(const MsgHead& oInMsgHead,
                                  const MsgBody& oInMsgBody) {
    const std::string& strRegNodeType = m_regNodeInfoMsg.node_type;
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    //节点注册通知(目前发送通知并不处理应答)
    oOutMsgHead.set_cmd(CMD_REQ_NODE_REG_NOTICE);
    oOutMsgHead.set_seq(oInMsgHead.seq());
    //给注册者发通知
    //发送格式{\"node_arry_reg\":[{\"node_type\":\"LOGIC\",\"node_ip\":\"192.168.18.22\",\"node_port\":40120,\"worker_num\":2}]}
    {
        const NodeSession::NodeType* pNodeType = m_pSess->getNodeTypeServerInfo(
                    strRegNodeType);
        if (pNodeType) {
            loss::CJsonObject jRegisteredNodesObj;
            GetNeededRegisteredNodes(pNodeType->neededServers, jRegisteredNodesObj);//获取需要的已注册的服务
            oOutMsgBody.set_body(jRegisteredNodesObj.ToString());
            oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
            LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(),
                                "NodeRegMgr::SendNodeRegNotice! to from out json[%s]",
                                jRegisteredNodesObj.ToString().c_str());
            oOutMsgHead.set_seq(m_pSess->GetLabor()->GetSequence());
            bool bRet = m_pSess->GetLabor()->SendTo(m_regMsgShell, oOutMsgHead,
                                                    oOutMsgBody);
            if (bRet) {
                LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(),
                                    "NodeRegMgr::SendNodeRegNotice!reg node stMsgShell[%u,%llu] Send cmd[%u] seq[%u] send ok,jRegisteredNodesObj(%s),register type(%s),send to node type(%s)",
                                    m_regMsgShell.iFd, m_regMsgShell.ulSeq,
                                    oOutMsgHead.cmd(), oOutMsgHead.seq(),
                                    jRegisteredNodesObj.ToString().c_str(), strRegNodeType.c_str(),
                                    strRegNodeType.c_str());
            } else {
                LOG4CPLUS_WARN_FMT(m_pSess->GetLogger(), "NodeRegMgr::SendNodeRegNotice!reg node stMsgShell[%u,%llu] Send cmd[%u] seq[%u] failed to send to node type(%s)",
                                   m_regMsgShell.iFd, m_regMsgShell.ulSeq, oOutMsgHead.cmd(), oOutMsgHead.seq(), strRegNodeType.c_str());
            }
        } else {
            LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(), "node type(%s) don't need other server routes", strRegNodeType.c_str());
        }
    }
    //新注册的服务给其它服务发通知
    //发送格式 {\"node_arry_reg\":[{\"node_type\":\"ACCESS\",\"node_ip\":\"192.168.18.22\",\"node_port\":40111,\"worker_num\":2}]}
    {
        loss::CJsonObject jRegNodeObj;
        oOutMsgBody.Clear();
        oOutMsgHead.Clear();
        oOutMsgHead.set_cmd(CMD_REQ_NODE_REG_NOTICE);
        oOutMsgHead.set_seq(m_pSess->GetLabor()->GetSequence());
        jRegNodeObj.AddEmptySubArray("node_arry_reg");
        loss::CJsonObject tmember;
        tmember.Add("node_type", m_regNodeInfoMsg.node_type);
        tmember.Add("node_ip", m_regNodeInfoMsg.node_ip);
        tmember.Add("node_port", m_regNodeInfoMsg.node_port);
        tmember.Add("worker_num", m_regNodeInfoMsg.worker_num);
        jRegNodeObj["node_arry_reg"].Add(tmember);
        oOutMsgBody.set_body(jRegNodeObj.ToString());
        oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
        const NodeSession::NodeInfoMap& mapNodesInfo = m_pSess->m_mapNodeInfo;
        for (std::map<std::string, st_NodeInfo>::const_iterator it_iter =
                    mapNodesInfo.begin();
                it_iter != mapNodesInfo.end(); ++it_iter) { //已注册服务器
            const st_NodeInfo& info = it_iter->second;
            //如果是同一个节点的就不发通知了
            //        if (info.getNodeKey() == m_regNodeInfoMsg.getNodeKey())
            //        {
            //            continue;
            //        }
            //给其他服务发送通知
            const NodeSession::NodeType* pNodeType =
                m_pSess->getNodeTypeServerInfo(info.node_type);
            if (pNodeType) {  //已注册的节点需要的节点类型
                const std::vector<std::string>& registeredNodeNeededServers = pNodeType->neededServers;
                if (!registeredNodeNeededServers.empty()) {
                    for (std::vector<std::string>::const_iterator it =
                                registeredNodeNeededServers.begin();
                            it != registeredNodeNeededServers.end(); ++it) {
                        if (strRegNodeType == *it) { //刚注册的服务是已注册服务需要的节点类型，则发送通知
                            LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(),
                                                "NodeRegMgr::SendNodeRegNotice!jRegNodeObj[%s]",
                                                jRegNodeObj.ToString().c_str());
                            //通知已注册的服务
                            bool bRet = m_pSess->GetLabor()->SendTo(
                                            info.getNodeKey(), oOutMsgHead,
                                            oOutMsgBody);
                            if (bRet) {
                                LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(),
                                                    "StepSendNotice::Start! stMsgShell[%u,%llu] Send cmd[%u] seq[%u] send ok,jRegNodeObj(%s),register type(%s),send to node(%s,%s)",
                                                    m_regMsgShell.iFd,
                                                    m_regMsgShell.ulSeq,
                                                    oOutMsgHead.cmd(),
                                                    oOutMsgHead.seq(),
                                                    jRegNodeObj.ToString().c_str(),
                                                    strRegNodeType.c_str(),
                                                    info.node_type.c_str(),
                                                    info.getNodeKey().c_str());
                            } else {
                                LOG4CPLUS_WARN_FMT(m_pSess->GetLogger(), "StepSendNotice::Start!reg stMsgShell[%u,%llu] Send cmd[%u] seq[%u].failed to send to node(%s)",
                                                   m_regMsgShell.iFd, m_regMsgShell.ulSeq, oOutMsgHead.cmd(), oOutMsgHead.seq(), info.getNodeKey().c_str());
                            }
                        }
                    }
                } else {
                    LOG4CPLUS_DEBUG_FMT(m_pSess->GetLogger(),
                                        "NodeRegMgr::SendNodeRegNotice!registed node type(%s) don't need other node routes",
                                        info.node_type.c_str());
                }
            } else {
                LOG4CPLUS_WARN_FMT(m_pSess->GetLogger(),
                                   "NodeRegMgr::SendNodeRegNotice!node type(%s) don't have server,please check table tb_nodetype",
                                   info.node_type.c_str());
            }
        }
    }
    return (ERR_OK);
}
bool NodeRegMgr::GetCenterDataFromDB() {
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "SELECT * from  %s WHERE nodetype = \'CENTER\'",
             NODE_MANAGE_TABLE);
    loss::T_vecResultSet vecRes;
    if (0 != m_pSess->GetMysqlDbi()->ExecSql(szSql, vecRes)) {
        LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(),
                            "SELECT from %s error:(%d,%s)", NODE_MANAGE_TABLE,
                            m_pSess->GetMysqlDbi()->GetErrno(),
                            m_pSess->GetMysqlDbi()->GetError().c_str());
        return false;
    }
    m_pSess->GetMysqlDbi()->FreeResult();
    if (vecRes.empty()) {
        LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(), "SELECT %s empty",
                            NODE_MANAGE_TABLE);
        return false;
    }
    loss::T_mapRow& valmap = *vecRes.begin();
    if (!m_centerNodeInfoDB.loadFromMapRow(valmap)) {
        LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(),
                            "SELECT from %s ok,but centerNodeInfoDB loadFromMapRow failed,check fields",
                            NODE_MANAGE_TABLE);
        return false;
    }
    return (true);
}
bool NodeRegMgr::GetServerDataFromDB(const char* servertype,
                                     const char* serverip) {
    char szSql[128];
    snprintf(szSql, sizeof(szSql) - 1,
             "SELECT * from  %s WHERE nodetype = \'%s\' and innerip = \'%s\'",
             NODE_MANAGE_TABLE, servertype, serverip);
    loss::T_vecResultSet vecRes;
    if (0 != m_pSess->GetMysqlDbi()->ExecSql(szSql, vecRes)) {
        LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(),
                            "SELECT from %s error:(%d,%s),servertype(%s),serverip(%s)",
                            NODE_MANAGE_TABLE,
                            m_pSess->GetMysqlDbi()->GetErrno(),
                            m_pSess->GetMysqlDbi()->GetError().c_str(),
                            servertype, serverip);
        return false;
    }
    m_pSess->GetMysqlDbi()->FreeResult();
    if (vecRes.empty()) {
        LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(),
                            "SELECT %s empty,servertype(%s),serverip(%s)",
                            NODE_MANAGE_TABLE, servertype, serverip);
        return false;
    }
    loss::T_mapRow& valmap = *vecRes.begin();
    if (!m_regNodeInfoDB.loadFromMapRow(valmap)) {
        LOG4CPLUS_ERROR_FMT(m_pSess->GetLogger(),
                            "SELECT from %s ,loadFromMapRow failed,check fileds!servertype(%s),serverip(%s)",
                            NODE_MANAGE_TABLE, servertype, serverip);
        return false;
    }
    return (true);
}

} /* namespace oss*/

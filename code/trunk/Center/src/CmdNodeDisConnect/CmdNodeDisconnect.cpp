/*******************************************************************************
 * Project:  CenterServer
 * @file     CmdNodeDisconnect.cpp
 * @brief
 * @author   xxx
 * @date:    2015年8月9日
 * @note
 * Modify history:
 ******************************************************************************/
#include "CmdNodeDisconnect.hpp"

#ifdef __cplusplus
extern "C" {
#endif

oss::Cmd* create() {
    oss::Cmd* pCmd = new oss::CmdNodeDisconnect();
    return (pCmd);
}

#ifdef __cplusplus
}
#endif

namespace oss {
CmdNodeDisconnect::CmdNodeDisconnect(): boInit(false) {
}

CmdNodeDisconnect::~CmdNodeDisconnect() {

}
bool CmdNodeDisconnect::Init() {
    if (boInit) {
        return true;
    }
    loss::CJsonObject oCenterConfJson;
    //配置文件路径查找
    std::string strConfFile = GetConfigPath()
                              + std::string("/CenterCmd.json");
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "CONF FILE = %s.",
                        strConfFile.c_str());

    std::ifstream fin(strConfFile.c_str());
    //配置信息输入流
    if (fin.good()) {
        //解析配置信息 JSON格式
        std::stringstream ssContent;
        ssContent << fin.rdbuf();
        if (!m_oCurrentConf.Parse(ssContent.str())) {
            //配置文件解析失败
            LOG4CPLUS_ERROR_FMT(GetLogger(),
                                "Read conf (%s) error,it's maybe not a json file!",
                                strConfFile.c_str());
            ssContent.str("");
            fin.close();
            return false;
        }
    } else {
        //配置信息流读取失败
        LOG4CPLUS_ERROR_FMT(GetLogger(), "Open conf (%s) error!",
                            strConfFile.c_str());
        return false;
    }
    boInit = true;
    return true;
}

NodeSession* CmdNodeDisconnect::getSession() {
    NodeSession* pSess = (NodeSession*) GetLabor()->GetSession(NODE_SESSIN_ID);
    if (pSess) {
        return (pSess);
    }
    //注册节点会话
    pSess = new NodeSession();
    if (pSess == NULL) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "error %d: new NodeSession() error!", ERR_NEW);
        return (NULL);
    }
    std::string err;
    if (!pSess->Init(m_oCurrentConf, err)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "NodeSession init error(%s)!", err.c_str());
        delete pSess;
        pSess = NULL;
        return (NULL);
    }
    if (GetLabor()->RegisterCallback(pSess)) {
        LOG4CPLUS_DEBUG_FMT(GetLogger(), "register NodeSession ok!");
        pSess->loadNodeTypes();
        return (pSess);
    } else {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "register NodeSession error!");
        delete pSess;
        pSess = NULL;
    }
    return (NULL);
}
bool CmdNodeDisconnect::AnyMessage(
    const tagMsgShell& stMsgShell,
    const MsgHead& oInMsgHead,
    const MsgBody& oInMsgBody) {
    if (!DelNode(stMsgShell, oInMsgHead, oInMsgBody)) {
        //目前为同步处理，没有断开连接应答。
        LOG4CPLUS_WARN_FMT(GetLogger(), "failed to del node,body(%s)", oInMsgBody.body().c_str());
    } else {
        LOG4CPLUS_DEBUG_FMT(GetLogger(), "del node ok,node identity(%s)", oInMsgBody.body().c_str());
    }
    return true;
}


bool CmdNodeDisconnect::DelNode(const tagMsgShell& stMsgShell,
                                const MsgHead& oInMsgHead,
                                const MsgBody& oInMsgBody) {
    const std::string& delNodeIdentify = oInMsgBody.body();
    NodeSession* pSess = getSession();
    if (!pSess) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "StepNodeDisConnect pSess none!");
        return false;
    }
    Clear();
    //获取对应的节点信息
    if (!pSess->GetNodeInfo(delNodeIdentify, m_delNodeInfo)) {
        LOG4CPLUS_WARN_FMT(GetLogger(),
                           "StepNodeDisConnect No such node.del node identity(%s)!",
                           delNodeIdentify.c_str());
        return false;
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "(%s):before DelNodeInfo size(%u),disconnect server type(%s)",
                        __FUNCTION__, pSess->GetMapNodeInfoSize(), m_delNodeInfo.node_type.c_str());
    //从map中删除节点信息
    if (!pSess->DelNodeInfo(delNodeIdentify)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "failed to del node.node id(%d)!",
                           m_delNodeInfo.node_id);
        return false;
    }
    if (!pSess->SetNodeDataOfflineToDBByNodeId(m_delNodeInfo.node_id)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "SetNodeDataOfflineToDBByNodeId false(%d)!",
                           m_delNodeInfo.node_id);
        return false;
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "(%s):after DelNodeInfo size(%u),disconnect server type(%s)",
                        __FUNCTION__, pSess->GetMapNodeInfoSize(), m_delNodeInfo.node_type.c_str());
    //给其它模块发下线通知
    SendDisConnectToOthers(pSess, oInMsgHead, oInMsgBody);
    return true;
}

//发送连接断开通知到其它服务
bool CmdNodeDisconnect::SendDisConnectToOthers(NodeSession* pSess, const MsgHead& oInMsgHead, const MsgBody& oInMsgBody) {
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    oOutMsgHead.set_cmd(CMD_REQ_NODE_REG_NOTICE);
    oOutMsgHead.set_seq(oInMsgHead.seq());
    loss::CJsonObject jNodeExitObj, tmember;
    jNodeExitObj.AddEmptySubArray("node_arry_exit");
    tmember.Add("node_type", m_delNodeInfo.node_type);
    tmember.Add("node_ip", m_delNodeInfo.node_ip);
    tmember.Add("node_port", m_delNodeInfo.node_port);
    tmember.Add("worker_num", m_delNodeInfo.worker_num);
    jNodeExitObj["node_arry_exit"].Add(tmember);
    oOutMsgBody.set_body(jNodeExitObj.ToString());
    oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
    LOG4CPLUS_DEBUG_FMT(GetLogger(),
                        "StepNodeDisConnect::SendDisConnectToOthers!jNodeExitObj[%s]",
                        jNodeExitObj.ToString().c_str());
    bool boSendedNotice(false);
    //遍历管理器内存的node列表,如果断开连接的服务是它们需要的服务,则通知它们注销该断开连接的服务
    {
        //注销的服务类型
        const std::string& unRegServerType = m_delNodeInfo.node_type;
        //在线节点管理器（key为节点类型：IP：端口，value为节点信息）
        const NodeSession::NodeInfoMap& mapNodeInfo = pSess->m_mapNodeInfo;
        for (NodeSession::NodeInfoMapCIT it_iter =
                    mapNodeInfo.begin();
                it_iter != mapNodeInfo.end(); ++it_iter) { //已注册的服务器
            const st_NodeInfo& nodeInfo = it_iter->second;
            //如果是同一个节点的就不发通知了
            //            if (nodeInfo.getNodeKey() == m_st_NodeInfo.getNodeKey())
            //            {
            //                continue;
            //            }
            //获取其他服务的配置
            const NodeSession::NodeType* pNodeType = pSess->getNodeTypeServerInfo(nodeInfo.node_type);
            if (pNodeType) {
                //其他服务需要的服务
                const std::vector<std::string>& neededServers = pNodeType->neededServers;
                for (std::vector<std::string>::const_iterator it = neededServers.begin();
                        it != neededServers.end(); ++it) { //该类服务器需要的服务器类型
                    if (unRegServerType == *it) { //注销的服务器是该类服务器需要的服务器,则通知该类服务器注销
                        LOG4CPLUS_DEBUG_FMT(GetLogger(),
                                            "StepNodeDisConnect::SendDisConnectToOthers!jNodeExitObj[%s]",
                                            jNodeExitObj.ToString().c_str());
                        bool bRet = GetLabor()->SendTo(nodeInfo.getNodeKey(), oOutMsgHead, oOutMsgBody);
                        if (bRet) {
                            LOG4CPLUS_DEBUG_FMT(GetLogger(),
                                                "StepNodeDisConnect::SendDisConnectToOthers!Send cmd[%u] seq[%u] send ok",
                                                oOutMsgHead.cmd(), oOutMsgHead.seq());
                            boSendedNotice = true;
                        } else {
                            LOG4CPLUS_DEBUG_FMT(GetLogger(),
                                                "StepNodeDisConnect::SendDisConnectToOthers!Send cmd[%u] seq[%u] send fail",
                                                oOutMsgHead.cmd(), oOutMsgHead.seq());
                        }
                    }
                }
            } else {
                LOG4CPLUS_WARN_FMT(GetLogger(),
                                   "StepNodeDisConnect::SendDisConnectToOthers!node type(%s) don't have server config,please check table tb_nodetype",
                                   nodeInfo.node_type.c_str());
            }
        }
    }
    if (!boSendedNotice) {
        LOG4CPLUS_DEBUG_FMT(GetLogger(), "did not send any notices to unregister node.jNodeExitObj(%s)", jNodeExitObj.ToString().c_str());
    }
    return (true);
}



} /* namespace oss */

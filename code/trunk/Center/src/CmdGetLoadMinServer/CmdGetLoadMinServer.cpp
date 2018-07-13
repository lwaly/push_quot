/*******************************************************************************
 * Project:  CenterServer
 * @file     CmdGetLoadMinServer.cpp
 * @brief
 * @author   xxx
 * @date:    2015年8月9日
 * @note
 * Modify history:
 ******************************************************************************/
#include <iostream>
#include "CmdGetLoadMinServer.hpp"
#include "util/json/CJsonObject.hpp"
#include "../NodeSession.h"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

oss::Cmd* create() {
    oss::Cmd* pCmd = new oss::CmdGetLoadMinServer();
    return (pCmd);
}

#ifdef __cplusplus
}
#endif

namespace oss {

CmdGetLoadMinServer::CmdGetLoadMinServer()
    : boInit(false) {
}

CmdGetLoadMinServer::~CmdGetLoadMinServer() {
}

bool CmdGetLoadMinServer::Init() {
    if (boInit) {
        return true;
    }
    loss::CJsonObject oCenterConfJson;
    //配置文件路径查找
    std::string strConfFile = GetConfigPath()
                              + std::string("CenterCmd.json");
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "CONF FILE = %s.", strConfFile.c_str());

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

NodeSession* CmdGetLoadMinServer::getSession() {
    NodeSession* pSess = (NodeSession*) GetLabor()->GetSession(NODE_SESSIN_ID);
    if (pSess) {
        return (pSess);
    }
    //注册节点会话
    pSess = new NodeSession();
    if (pSess == NULL) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "error %d: new NodeSession() error!",
                            ERR_NEW);
        return (NULL);
    }
    std::string err;
    if (!pSess->Init(m_oCurrentConf, err)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "NodeSession init error!(%s)", err.c_str());
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
bool CmdGetLoadMinServer::AnyMessage(const tagMsgShell& stMsgShell,
                                     const MsgHead& oInMsgHead, const MsgBody& oInMsgBody) {
    //请求格式
    // {
    //    "servertype":"ACCESS"
    // }
    //返回格式
    // {
    //     "errcode":0,
    //     "outerport":1000,
    //     "outerip":"192.168.18.22",
    //     "innerport":2000,
    //     "innerip":"192.168.18.22"
    // }
    loss::CJsonObject jParseObj;
    NodeLoadStatus nodeLoadStatus;
    if (GetCmd() != (int) oInMsgHead.cmd()) {
        LOG4CPLUS_DEBUG_FMT(GetLogger(),
                            "CmdGetLoadMinServer::AnyMessage,Cmd not valid(%d,%d)", GetCmd(),
                            (int)oInMsgHead.cmd());
        return Response(stMsgShell, oInMsgHead, nodeLoadStatus, ERR_UNKNOWN_CMD);
    }
    NodeSession* pSess = getSession();
    if (!pSess) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "get session error");
        return Response(stMsgShell, oInMsgHead, nodeLoadStatus, ERR_SESSION);
    }
    if (!jParseObj.Parse(oInMsgBody.body())) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "error jsonParse error! json[%s]",
                            oInMsgBody.body().c_str());
        return Response(stMsgShell, oInMsgHead, nodeLoadStatus, ERR_BODY_JSON);
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(),
                        "CmdGetLoadMinServer jsonbuf[%s] Parse is ok",
                        oInMsgBody.body().c_str());
    //解析JSON数据
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "CmdGetLoadMinServer::AnyMessage (%s)", jParseObj.ToString().c_str());
    std::string servertype;
    if (!jParseObj.Get("servertype", servertype)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "error jsonParse error! json[%s]",
                            oInMsgBody.body().c_str());
        return Response(stMsgShell, oInMsgHead, nodeLoadStatus, ERR_BODY_JSON);
    }
    if (!pSess->checkNodeType(servertype)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "no such nodetype(%s)", servertype.c_str());
        return Response(stMsgShell, oInMsgHead, nodeLoadStatus, ERR_SERVERINFO);
    }
    //获取指定类型的节点状态列表
    std::vector<NodeLoadStatus> nodeStatusList;
    if (!pSess->GetNodeStatusByNodeType(servertype, nodeStatusList)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "get server node failed(%s)",
                            servertype.c_str());
        return Response(stMsgShell, oInMsgHead, nodeLoadStatus, ERR_SERVERINFO);
    }
    if (nodeStatusList.empty()) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "nodeStatusList empty! json[%s]",
                            oInMsgBody.body().c_str());
        return Response(stMsgShell, oInMsgHead, nodeLoadStatus, ERR_SERVERINFO);
    }
    //获取节点状态列表中负载最小的节点
    {
        std::vector<NodeLoadStatus>::iterator itNode = nodeStatusList.begin();
        std::vector<NodeLoadStatus>::iterator itEndNode = nodeStatusList.end();
        nodeLoadStatus = *itNode;
        ++itNode;
        for (; itNode != itEndNode; ++itNode) {
            if (nodeLoadStatus.serverload > itNode->serverload) { //获取负载最小的节点
                nodeLoadStatus = *itNode;
            }
        }
    }
    return Response(stMsgShell, oInMsgHead, nodeLoadStatus, ERR_OK);
}

bool CmdGetLoadMinServer::Response(const tagMsgShell& stMsgShell,
                                   const MsgHead& oInMsgHead, const NodeLoadStatus& nodeLoadStatus, int iRet) {
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    oOutMsgHead.set_cmd(oInMsgHead.cmd() + 1);
    oOutMsgHead.set_seq(oInMsgHead.seq());
    loss::CJsonObject jObjReturn;
    jObjReturn.Add("outerport", nodeLoadStatus.outerport);
    jObjReturn.Add("outerip", nodeLoadStatus.outerip);
    jObjReturn.Add("innerport", nodeLoadStatus.innerport);
    jObjReturn.Add("innerip", nodeLoadStatus.innerip);
    jObjReturn.Add("errcode", iRet);
    LOG4CPLUS_DEBUG_FMT(GetLogger(),
                        "CmdGetLoadMinServer return json[%s]",
                        jObjReturn.ToString().c_str());
    oOutMsgBody.set_body(jObjReturn.ToString());
    oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
    GetLabor()->SendTo(stMsgShell, oOutMsgHead, oOutMsgBody);
    return (iRet ? false : true);
}


} /* namespace oss */

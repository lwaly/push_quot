/*******************************************************************************
 * Project:  CenterServer
 * @file     CmdNodeReport.cpp
 * @brief
 * @author   xxx
 * @date:    2015年8月9日
 * @note
 * Modify history:
 ******************************************************************************/
#include "CmdNodeReport.hpp"
#include "util/json/CJsonObject.hpp"
#include <iostream>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

oss::Cmd* create() {
    oss::Cmd* pCmd = new oss::CmdNodeReport();
    return (pCmd);
}

#ifdef __cplusplus
}
#endif

namespace oss {

CmdNodeReport::CmdNodeReport()
    : boInit(false) {
}

CmdNodeReport::~CmdNodeReport() {
}
bool CmdNodeReport::Init() {
    if (boInit) {
        return true;
    }
    loss::CJsonObject oCenterConfJson;
    //配置文件路径查找
    std::string strConfFile = GetConfigPath() + std::string("/CenterCmd.json");
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

NodeSession* CmdNodeReport::getSession() {
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
        LOG4CPLUS_ERROR_FMT(GetLogger(), "NodeSession init error!%s", err.c_str());
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
bool CmdNodeReport::AnyMessage(const tagMsgShell& stMsgShell,
                               const MsgHead& oInMsgHead, const MsgBody& oInMsgBody) {
    if (GetCmd() != (int) oInMsgHead.cmd()) {
        LOG4CPLUS_DEBUG_FMT(GetLogger(),
                            "CmdNodeReg::AnyMessage,Cmd not valid(%d,%d)", GetCmd(),
                            (int)oInMsgHead.cmd());
        return Response(stMsgShell, oInMsgHead, ERR_UNKNOWN_CMD);
    }
    //上报信息
    loss::CJsonObject jParseObj;
    if (!jParseObj.Parse(oInMsgBody.body())) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "error jsonParse error! json[%s]",
                            oInMsgBody.body().c_str());
        return Response(stMsgShell, oInMsgHead, ERR_BODY_JSON);
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(),
                        "CmdNodeReport jsonbuf[%s] Parse is ok",
                        oInMsgBody.body().c_str());
    //解析节点数据
    st_NodeInfo nodeinfo;
    if (!nodeinfo.pareJsonData(jParseObj)) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "nodeinfo.pareJsonDat error jsonbuf[%s] Parse is ok",
                            oInMsgBody.body().c_str());
        return Response(stMsgShell, oInMsgHead, ERR_BODY_JSON);
    }
    NodeSession* pSess = getSession();
    if (!pSess) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "getSession none");
        return Response(stMsgShell, oInMsgHead, ERR_SESSION);
    }
    if (!pSess->checkNodeType(nodeinfo.node_type)) {
        LOG4CPLUS_ERROR_FMT(GetLabor()->GetLogger(),
                            "NodeRegMgr checkNodeType error(%s)",
                            nodeinfo.node_type.c_str());
        return Response(stMsgShell, oInMsgHead, ERR_SERVERINFO);
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(),
                        "report node(%s,%s) connect:%d,client:%d",
                        nodeinfo.node_ip.c_str(),
                        nodeinfo.node_type.c_str(), nodeinfo.connect,
                        nodeinfo.client);

    st_NodeInfo* pNodeInfo = pSess->GetNodeInfo(nodeinfo.getNodeKey());
    if (!pNodeInfo) { //没有该节点则注册
        int iRet = pSess->m_nodeRegMgr.regNode(stMsgShell, oInMsgHead,
                                               oInMsgBody, nodeinfo);
        if (iRet) {
            LOG4CPLUS_ERROR_FMT(GetLogger(),
                                "CmdNodeReport regNode msg jsonbuf[%s] is wrong,error code(%d)",
                                oInMsgBody.body().c_str(), iRet);
            return Response(stMsgShell, oInMsgHead, iRet);
        }
    } else { //更新上报状态
        nodeinfo.node_id = pNodeInfo->node_id; //以中心服务器的nodeid为准
        if (!pNodeInfo->update(nodeinfo)) {
            LOG4CPLUS_WARN_FMT(GetLogger(),
                               "pNodeInfo(%s,%d,%s,%d) update failed",
                               nodeinfo.node_type.c_str(),
                               nodeinfo.node_id, nodeinfo.node_ip.c_str(), nodeinfo.node_port);
            return Response(stMsgShell, oInMsgHead, ERR_SERVERINFO);
        }
        if (!pSess->WriteNodeDataToDB(nodeinfo, true)) {
            LOG4CPLUS_WARN_FMT(GetLogger(),
                               "WriteNodeDataToDB nodeid(%d) false,mysql error(%s)",
                               nodeinfo.node_id,
                               pSess->GetLastMysqlError().c_str());
            return Response(stMsgShell, oInMsgHead, ERR_SERVERINFO_RECORD);
        }
    }
    return Response(stMsgShell, oInMsgHead, ERR_OK);
}

bool CmdNodeReport::Response(const tagMsgShell& stMsgShell,
                             const MsgHead& oInMsgHead, int iRet) {
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    oOutMsgHead.set_cmd(oInMsgHead.cmd() + 1);
    oOutMsgHead.set_seq(oInMsgHead.seq());
    loss::CJsonObject jObj, jObjReturn;
    jObjReturn.Add("errcode", iRet);
    oOutMsgBody.set_body(jObjReturn.ToString());
    oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
    GetLabor()->SendTo(stMsgShell, oOutMsgHead, oOutMsgBody);
    return (iRet ? false : true);
}


} /* namespace oss */

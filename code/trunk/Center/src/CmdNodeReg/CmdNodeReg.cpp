/*******************************************************************************
 * Project:  CenterServer
 * @file     CmdNodeReg.cpp
 * @brief
 * @author   xxx
 * @date:    2015年8月9日
 * @note
 * Modify history:
 ******************************************************************************/
#include <iostream>
#include "CmdNodeReg.hpp"

#include "../NodeRegMgr.hpp"
#include "util/json/CJsonObject.hpp"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

oss::Cmd* create() {
    oss::Cmd* pCmd = new oss::CmdNodeReg();
    return (pCmd);
}

#ifdef __cplusplus
}
#endif

namespace oss {

CmdNodeReg::CmdNodeReg()
    : boInit(false) {
}
CmdNodeReg::~CmdNodeReg() {
}
bool CmdNodeReg::Init() {
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

NodeSession* CmdNodeReg::getSession() {
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

bool CmdNodeReg::AnyMessage(const tagMsgShell& stMsgShell,
                            const MsgHead& oInMsgHead, const MsgBody& oInMsgBody) {
    if (GetCmd() != (int) oInMsgHead.cmd()) {
        LOG4CPLUS_DEBUG_FMT(GetLogger(),
                            "CmdNodeReg::AnyMessage,Cmd not valid(%d,%d)", GetCmd(),
                            (int)oInMsgHead.cmd());
        return Response(stMsgShell, oInMsgHead, ERR_UNKNOWN_CMD, 0);
    }
    NodeSession* pSess = getSession();
    if (!pSess) {
        LOG4CPLUS_DEBUG_FMT(GetLogger(), "CmdNodeReg::AnyMessage pSess none!");
        return Response(stMsgShell, oInMsgHead, ERR_SESSION, 0);
    }
    loss::CJsonObject jParseObj;
    if (!jParseObj.Parse(oInMsgBody.body())) {
        LOG4CPLUS_DEBUG_FMT(GetLogger(), "failed to parse json body");
        return Response(stMsgShell, oInMsgHead, ERR_BODY_JSON, 0);
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(),
                        "CmdNodeReg jsonbuf[%s] Parse is ok",
                        oInMsgBody.body().c_str());
    //解析节点数据
    st_NodeInfo nodeinfo;
    if (!nodeinfo.pareJsonData(jParseObj)) {
        LOG4CPLUS_ERROR_FMT(GetLabor()->GetLogger(),
                            "NodeRegMgr nodeinfo.pareJsonData error");
        return Response(stMsgShell, oInMsgHead, ERR_BODY_JSON, 0);
    }
    if (!pSess->checkNodeType(nodeinfo.node_type)) {
        LOG4CPLUS_ERROR_FMT(GetLabor()->GetLogger(),
                            "NodeRegMgr checkNodeType error(%s)",
                            nodeinfo.node_type.c_str());
        return Response(stMsgShell, oInMsgHead, ERR_SERVERINFO, 0);
    }
    int regNodeRet = pSess->m_nodeRegMgr.regNode(stMsgShell, oInMsgHead,
                     oInMsgBody, nodeinfo);
    if (regNodeRet) { //注册失败返回注册应答，否则已在注册函数中发送
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "CmdNodeReg msg jsonbuf[%s] is wrong,error code(%d)",
                            oInMsgBody.body().c_str(), regNodeRet);
        return Response(stMsgShell, oInMsgHead, ERR_SERVERINFO, 0);
    }
    GetLabor()->AddMsgShell(nodeinfo.getNodeKey(), stMsgShell);//加入到该节点的路由信息
    return true;
}

bool CmdNodeReg::Response(const tagMsgShell& stMsgShell,
                          const MsgHead& oInMsgHead, int iRet, int node_id) {
    /*
     * 返回结构
     * {
     *  "errcode":0,
     *  "node_id":1
     * }
     * */
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "CmdNodeReg::Response iRet(%d),node_id(%d)", iRet, node_id);
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    oOutMsgHead.set_cmd(oInMsgHead.cmd() + 1);
    oOutMsgHead.set_seq(oInMsgHead.seq());
    loss::CJsonObject jObjReturn;
    jObjReturn.Add("errcode", iRet);
    jObjReturn.Add("node_id", iRet ? 0 : node_id);
    oOutMsgBody.set_body(jObjReturn.ToString());
    oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
    GetLabor()->SendTo(stMsgShell, oOutMsgHead, oOutMsgBody);
    return (iRet ? false : true);
}


} /* namespace oss */

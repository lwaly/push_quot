/*******************************************************************************
 * Project:  CenterServer
 * @file     CmdServerReport.cpp
 * @brief
 * @author   xxx
 * @date:    2015年8月9日
 * @note
 * Modify history:
 ******************************************************************************/
#include "CmdServerDataStatusReport.hpp"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

oss::Cmd* create() {
    oss::Cmd* pCmd = new oss::CmdServerReport();
    return (pCmd);
}

#ifdef __cplusplus
}
#endif

namespace oss {

CmdServerReport::CmdServerReport()
    : boInit(false) {
}

CmdServerReport::~CmdServerReport() {
}

bool CmdServerReport::Init() {
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

void CmdServerReport::ClearStatus() {
    nodetype.clear();
    innerport = 0;
    innerip.clear();
    outerport = 0;
    outerip.clear();
    status.clear();
}

NodeSession* CmdServerReport::getSession() {
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

/*消息体格式如下
 * 定时上报
 * 上报的数据如下
 * 请求格式
 {
 "nodetype":"PROXY",
 "innerport":10000,
 "innerip":"192.168.18.22",
 "outerport":20000,
 "outerip":"192.168.18.22",
 "status":
 {
 "proxy_nodes": [
 {
 "192.168.18.78:22125": [
 {
 "master": "192.168.18.78:6379",
 "slaver": [
 "192.168.18.78:6380"
 ]
 },
 {
 "volMaxUsedRate": 0.000919,
 "volAvrUsedRate": 0.000684,
 "volUsed": 2939544,
 "volTotal": 4294967296
 }
 ]
 }
 ],
 "redis_nodes": [
 {
 "192.168.18.78:6379": {
 "useVol": 1972920,
 "totalVol": 2147483648,
 "useRate": 0.000919,
 "role": "master"
 }
 },
 {
 "192.168.18.78:6380": {
 "useVol": 966624,
 "totalVol": 2147483648,
 "useRate": 0.00045,
 "role": "slaver"
 }
 }
 ],
 "no_frag_volusedrate": {
 "log_uid": 0.000919,
 "log_gid": 0.000919,
 "property_uid": 0.000919,
 "property_gid": 0.000919
 }
 }

 }
 返回格式
 {
 "errcode"：0
 }
 * */
bool CmdServerReport::AnyMessage(const tagMsgShell& stMsgShell,
                                 const MsgHead& oInMsgHead, const MsgBody& oInMsgBody) {
    if (GetCmd() != (int) oInMsgHead.cmd()) {
        LOG4CPLUS_DEBUG_FMT(GetLogger(), "CmdServerReport::AnyMessage,Cmd not valid(%d,%d)", GetCmd(),
                            (int)oInMsgHead.cmd());
        return Response(stMsgShell, oInMsgHead, ERR_UNKNOWN_CMD);
    }
    NodeSession* pSess = getSession();
    if (!pSess) {
        LOG4CPLUS_ERROR_FMT(GetLogger(), "error jsonParse error! json[%s]",
                            oInMsgBody.body().c_str());
        return Response(stMsgShell, oInMsgHead, ERR_SESSION);
    }
    loss::CJsonObject jParseObj;
    if (!jParseObj.Parse(oInMsgBody.body())) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "error jsonParse error! body[%s]",
                           oInMsgBody.body().c_str());
        return Response(stMsgShell, oInMsgHead, ERR_BODY_JSON);
    }
    LOG4CPLUS_DEBUG_FMT(GetLogger(), "server report json data(%s)", jParseObj.ToString().c_str());
    ClearStatus();//清除本地缓存,再解析消息数据
    if (!jParseObj.Get("nodetype", nodetype)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "CmdServerReport::AnyMessage miss nodetype");
        return Response(stMsgShell, oInMsgHead, ERR_REQ_MISS_PARAM);
    }
    if (!jParseObj.Get("innerport", innerport)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "CmdServerReport::AnyMessage miss innerport");
        return Response(stMsgShell, oInMsgHead, ERR_REQ_MISS_PARAM);
    }
    if (!jParseObj.Get("innerip", innerip)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "CmdServerReport::AnyMessage miss innerip");
        return Response(stMsgShell, oInMsgHead, ERR_REQ_MISS_PARAM);
    }
    jParseObj.Get("outerport", outerport);
    jParseObj.Get("outerip", outerip);
    if (!jParseObj.Get("status", status)) {
        LOG4CPLUS_WARN_FMT(GetLogger(), "CmdServerReport::AnyMessage miss status");
        return Response(stMsgShell, oInMsgHead, ERR_REQ_MISS_PARAM);
    }
    if (!pSess->WriteServerDataLoadToDB(nodetype.c_str(), innerport,
                                        innerip.c_str(), outerport, outerip.c_str(),
                                        status.c_str())) {
        LOG4CPLUS_ERROR_FMT(GetLogger(),
                            "WriteServerDataLoadToDB error,nodetype(%s),innerport(%d),innerip(%s),outerport(%d),outerip(%s),status(%s)",
                            nodetype.c_str(), innerport, innerip.c_str(),
                            outerport, outerip.c_str(), status.c_str());
        return Response(stMsgShell, oInMsgHead, ERR_SERVERINFO_RECORD);
    }
    return Response(stMsgShell, oInMsgHead, ERR_OK);
}

bool CmdServerReport::Response(const tagMsgShell& stMsgShell,
                               const MsgHead& oInMsgHead, int iRet) {
    MsgHead oOutMsgHead;
    MsgBody oOutMsgBody;
    oOutMsgHead.set_cmd(oInMsgHead.cmd() + 1);
    oOutMsgHead.set_seq(oInMsgHead.seq());
    loss::CJsonObject jObjReturn;
    jObjReturn.Add("errcode", iRet);
    oOutMsgBody.set_body(jObjReturn.ToString());
    oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
    GetLabor()->SendTo(stMsgShell, oOutMsgHead, oOutMsgBody);
    return (iRet ? false : true);
}

} /* namespace oss */

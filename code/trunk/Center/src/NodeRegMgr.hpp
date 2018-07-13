/*******************************************************************************
 * Project:  CenterServer
 * @file     StepNodeRegMgr.hpp
 * @brief    处理节点注册
 * @author   xxx
 * @date:    2015年9月16日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef SRC_STEP_SYS_STEP_NODEREG_HPP_
#define SRC_STEP_SYS_STEP_NODEREG_HPP_
#include "protocol/oss_sys.pb.h"
#include "util/json/CJsonObject.hpp"
#include "step/Step.hpp"
#include "cmd/Cmd.hpp"
#include "cmd/CW.hpp"
#include "dbi/MysqlDbi.hpp"
#include "Comm.hpp"

namespace oss {
class NodeSession;
class Cmd;

enum NodeInfoDBVerifyRet {
    eNodeInfoDBVerifyRet_OK, eNodeInfoDBVerifyRet_IP_ERROR, //ip验证有误
    eNodeInfoDBVerifyRet_SERVERTYPE_ERROR, //服务器类型验证有误
    eNodeInfoDBVerifyRet_NODE_CANUSE_ERROR, //服务器节点不可用
};

//节点的db管理信息(本结构成员为原子类型)
struct NodeInfoDB {
    char loaded; //标识从db加载
    char canuse; //节点是否能使用(1:能使用,0:不能使用)
    char nodetype[32]; //节点类型
    char innerip[64]; //内网IP
    char outerip[64]; //外网IP
    NodeInfoDB() {
        clear();
    }
    void clear() {
        bzero(this, sizeof(*this));
    }
    bool isLoaded() const {
        return (0 != loaded);
    }
    bool loadFromMapRow(loss::T_mapRow& valmap) {
        if (valmap.end() == valmap.find("nodetype")) {
            return false;
        }
        if (valmap.end() == valmap.find("innerip")) {
            return false;
        }
        if (valmap.end() == valmap.find("outerip")) {
            return false;
        }
        if (valmap.end() == valmap.find("canuse")) {
            return false;
        }
        loaded = 1;
        if (!valmap["nodetype"].empty()) {
            snprintf(nodetype, sizeof(nodetype) - 1,
                     valmap["nodetype"].c_str());
        }
        if (!valmap["innerip"].empty()) {
            snprintf(innerip, sizeof(innerip) - 1, valmap["innerip"].c_str());
        }
        if (!valmap["outerip"].empty()) {
            snprintf(outerip, sizeof(outerip) - 1, valmap["outerip"].c_str());
        }
        if (!valmap["canuse"].empty()) {
            canuse = (char) atoi(valmap["canuse"].c_str());
        }
        return true;
    }
    NodeInfoDBVerifyRet verify(const st_NodeInfo& nodeInfo) {
        if (nodeInfo.node_ip != innerip) {
            return eNodeInfoDBVerifyRet_IP_ERROR;
        }
        if (nodeInfo.node_type != nodetype) {
            return eNodeInfoDBVerifyRet_SERVERTYPE_ERROR;
        }
        if (canuse != 1) {
            return eNodeInfoDBVerifyRet_NODE_CANUSE_ERROR;
        }
        return eNodeInfoDBVerifyRet_OK;
    }
    const char* getError(NodeInfoDBVerifyRet eError) {
        if (eNodeInfoDBVerifyRet_IP_ERROR == eError) {
            return "fail to verify ip";
        } else if (eNodeInfoDBVerifyRet_SERVERTYPE_ERROR == eError) {
            return "fail to verify servertype";
        } else if (eNodeInfoDBVerifyRet_NODE_CANUSE_ERROR == eError) {
            return "fail to verify canuse,it 's disabled";
        }
        return "unknown error";
    }
};

//节点注册管理类
class NodeRegMgr {
public:
    NodeRegMgr(NodeSession* pSess);
    //注册节点(已有节点会更新).注册失败返回错误码
    //注册成功会发送以下消息：
    //(1)返回注册响应,会分配节点id
    //(2)发送注册服务器的配置信息
    //(3)给注册者发需要的其他服务器的通知、注册者给其它需要的服务发通知
    int regNode(const tagMsgShell& stMsgShell, const MsgHead& oInMsgHead,
                const MsgBody& oInMsgBody, st_NodeInfo& nodeinfo);
    ~NodeRegMgr();
    NodeInfoDB m_centerNodeInfoDB; //中心服务器的节点信息
    //临时缓存数据
    NodeInfoDB m_regNodeInfoDB;    //注册服务器的节点信息
    st_NodeInfo m_regNodeInfoMsg;    //注册来的节点消息的数据
    tagMsgShell m_regMsgShell;    //注册来的连接地址
    //注册返回信息：（1）注册响应,会分配节点id（2）发送注册服务器的配置信息(注册成功后才调用)（3）给注册者发其他服务器通知、注册者的给其它服务发通知
    //返回注册响应,会分配节点id
    bool ResponseNodeReg(const tagMsgShell& stMsgShell,
                         const MsgHead& oInMsgHead, const MsgBody& oInMsgBody,
                         int iRet);
private:
    //发送注册服务器的配置信息(注册成功后才调用)
    bool SendServerConfig(const tagMsgShell& stMsgShell,
                          const MsgHead& oInMsgHead, const MsgBody& oInMsgBody);
    //给注册者发其他服务器通知、注册者给其它服务发通知
    int SendNodeRegNotice(const MsgHead& oInMsgHead, const MsgBody& oInMsgBody);
private:
    //获取中心服务器数据信息
    bool GetCenterDataFromDB();
    //获取指定类型的服务器数据信息
    bool GetServerDataFromDB(const char* servertype, const char* serverip);
    //加载服务器配置,configtype:服务器类型 （0为初始化配置）
    bool loadServerConfig(const std::string& nodetype,
                          std::string& serverconfig, short configtype = 0);
    //插入节点负载信息
    bool ReplaceNodeStatusToDB(const st_NodeInfo& nodeInfo);
    bool UpdateNodeStatusToDB(const st_NodeInfo& nodeInfo);
    bool ClearOverdueNodeDataToDB();
    //获取需要的已注册的服务器的节点信息(json格式)
    bool GetNeededRegisteredNodes(const std::vector<std::string>& neededServers,
                                  loss::CJsonObject& jObj);
    NodeSession* m_pSess;
};

} /* namespace oss */

#endif /* SRC_STEP_SYS_STEP_STEPTELLWORKER_HPP_ */

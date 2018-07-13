/*******************************************************************************
 * Project:  CenterServer
 * @file     CmdNodeDisconnect.hpp
 * @brief    节点连接断开
 * @author   xxx
 * @date:    2015年9月18日
 * @note     各人节点注册到CENTER，后面断开，CENTER处理节点断开
 * Modify history:
 ******************************************************************************/
#ifndef CMD_NODE_DISCONNECT_HPP_
#define CMD_NODE_DISCONNECT_HPP_
#include <iostream>
#include "cmd/Cmd.hpp"
#include "protocol/oss_sys.pb.h"
#include "util/json/CJsonObject.hpp"
#include "../NodeSession.h"

namespace oss {
class CmdNodeDisconnect : public oss::Cmd {
public:
    CmdNodeDisconnect();
    virtual ~CmdNodeDisconnect();
    virtual bool Init();
    NodeSession* getSession();
    virtual bool AnyMessage(
        const tagMsgShell& stMsgShell,
        const MsgHead& oInMsgHead,
        const MsgBody& oInMsgBody);
private:
    bool DelNode(const tagMsgShell& stMsgShell,
                 const MsgHead& oInMsgHead,
                 const MsgBody& oInMsgBody);
    bool SendDisConnectToOthers(NodeSession* pSess, const MsgHead& oInMsgHead, const MsgBody& oInMsgBody);
    void Clear() {
        m_delNodeInfo.clean();
    }
    st_NodeInfo m_delNodeInfo;//节点信息
    bool boInit;
    loss::CJsonObject m_oCurrentConf;       ///< 当前加载的配置
};

} /* namespace oss */

#endif /* CMDTOLDWORKER_HPP_ */

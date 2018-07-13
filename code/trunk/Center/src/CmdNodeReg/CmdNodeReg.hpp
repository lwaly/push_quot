/*******************************************************************************
 * Project:  CenterServer
 * @file     CmdNodeReg.hpp
 * @brief    被告知Worker信息
 * @author   xxx
 * @date:    2015年8月9日
 * @note     A连接B成功后，A将己方的Worker信息A_attr（节点类型和Worker唯一标记）告知B，
 * B收到A后将A_attr登记起来，并将己方的Worker信息B_attr回复A
 * Modify history:
 ******************************************************************************/
#ifndef CMD_NODE_REG_HPP_
#define CMD_NODE_REG_HPP_
#include "protocol/oss_sys.pb.h"
#include "OssError.hpp"
#include "cmd/Cmd.hpp"
#include "../Comm.hpp"
#include "../NodeSession.h"

namespace oss {
class CmdNodeReg: public oss::Cmd {
public:
    CmdNodeReg();
    virtual ~CmdNodeReg();
    virtual bool Init();
    NodeSession* getSession();
    virtual bool AnyMessage(const tagMsgShell& stMsgShell,
                            const MsgHead& oInMsgHead, const MsgBody& oInMsgBody);
    //注册节点应答。返回分配节点id
    bool Response(const tagMsgShell& stMsgShell,
                  const MsgHead& oInMsgHead, int iRet, int node_id);
private:
    bool boInit;
    loss::CJsonObject m_oCurrentConf;       ///< 当前加载的配置
};
} /* namespace oss */

#endif /* CMDTOLDWORKER_HPP_ */

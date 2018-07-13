/*******************************************************************************
 * Project:  CenterServer
 * @file     CmdNodeReg.hpp
 * @brief    节点上报
 * @author   xxx
 * @date:    2015年9月17日
 * @note     其它模块向CENTER报告相关信息
 * Modify history:
 ******************************************************************************/
#ifndef CMD_NODE_REPORT_HPP_
#define CMD_NODE_REPORT_HPP_

#include "protocol/oss_sys.pb.h"
#include "cmd/Cmd.hpp"
#include "dbi/MysqlDbi.hpp"
#include "../Comm.hpp"
#include "../NodeSession.h"

namespace oss {
/**
 * @brief   获取access
 * @author  xxx
 * @date    2015年12月2日
 * @note    获取低负载的access server
 */
class CmdGetLoadMinServer : public oss::Cmd {
public:
    CmdGetLoadMinServer();
    virtual ~CmdGetLoadMinServer();
    virtual bool Init();
    NodeSession* getSession();
    virtual bool AnyMessage(
        const tagMsgShell& stMsgShell,
        const MsgHead& oInMsgHead,
        const MsgBody& oInMsgBody);
private:
    bool Response(const tagMsgShell& stMsgShell,
                  const MsgHead& oInMsgHead, const NodeLoadStatus& nodeLoadStatus, int iRet);
    loss::CJsonObject m_oCurrentConf;       ///< 当前加载的配置
    bool boInit;
};

} /* namespace oss */

#endif /* CMD_NODE_REPORT_HPP_ */

/*******************************************************************************
 * Project:  CenterServer
 * @file     CmdNodeReg.hpp
 * @brief    节点上报
 * @author   HSC
 * @date:    2015年9月17日
 * @note     其它模块向CENTER报告相关信息
 * Modify history:
 ******************************************************************************/
#ifndef CMD_NODE_REPORT_HPP_
#define CMD_NODE_REPORT_HPP_
#include <iostream>
#include "util/json/CJsonObject.hpp"
#include "protocol/oss_sys.pb.h"
#include "cmd/Cmd.hpp"
#include "dbi/MysqlDbi.hpp"
#include "Comm.hpp"
#include "NodeSession.h"

namespace oss {
class CmdServerReport : public oss::Cmd {
public:
    CmdServerReport();
    virtual ~CmdServerReport();
    virtual bool Init();
    void ClearStatus();
    NodeSession* getSession();
    virtual bool AnyMessage(
        const tagMsgShell& stMsgShell,
        const MsgHead& oInMsgHead,
        const MsgBody& oInMsgBody);
private:
    bool Response(const tagMsgShell& stMsgShell,
                  const MsgHead& oInMsgHead, int iRet);
    loss::CJsonObject m_oCurrentConf;       ///< 当前加载的配置
    std::string nodetype;
    int innerport;
    std::string innerip;
    int outerport;
    std::string outerip;
    std::string status;
    bool boInit;
};

} /* namespace oss */

#endif /* CMD_NODE_REPORT_HPP_ */

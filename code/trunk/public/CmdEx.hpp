/*******************************************************************************
 * Project:  Public
 * @file     CmdEx.hpp
 * @brief    为了封装底层一些细节和原生代码。
 * @author   wfh
 * @date:    2016年10月19日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef _CMD_EX_H_
#define _CMD_EX_H_

#include "cmd/Cmd.hpp"
#include "Public.hpp"
#include "StepEx.hpp"

namespace bsw
{

class CmdEx : public oss::Cmd
{
public:
    CmdEx();
    virtual ~CmdEx();

    virtual bool AnyMessage(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody);
	virtual bool AnyMessageLogic(const CContext& oInContext);
	virtual bool AnyMessageLogic(common::errorinfo& oErrInfo, const CContext& oInContext);

	uint32 RegisterSyncStep(bsw::StepEx* pStep);
	uint32 RegisterSyncStep(common::errorinfo& oErrInfo, bsw::StepEx* pStep);

public:
	std::string IntToString(uint32 uiValue);
	std::string Int64ToString(uint64 uiValue);
	std::string FormatString(const char* pString, ...);

public:
	bool ResponseToClient( //回包给客户端
		const CContext& oInContext, const std::string& strBuf);
};

} /* namespace bsw */

#endif /* _CMD_EX_H_ */

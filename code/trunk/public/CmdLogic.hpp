/*******************************************************************************
 * Project:  Public
 * @file     CmdLogic.hpp
 * @brief    为了封装底层一些细节和原生代码，封装了逻辑的部分用户信息。注意这是登录成功后才能调用的
 * @author   wfh
 * @date:    2016年10月19日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef _CMD_LOGIC_H_
#define _CMD_LOGIC_H_

#include "CmdEx.hpp"
#include "Public.hpp"

namespace bsw
{

class CmdLogic : public CmdEx
{
public:
    CmdLogic();
    virtual ~CmdLogic();

    virtual bool AnyMessage(
                    const oss::tagMsgShell& stMsgShell,
                    const MsgHead& oInMsgHead,
                    const MsgBody& oInMsgBody);
};

} /* namespace bsw */

#define CMD_CLASS_MAKE(CmdClass, StepClass)			  \
													  \
namespace bsw										  \
{													  \
class CmdClass : public CmdLogic					  \
{													  \
public:                                               \
	CmdClass() { SetClassName(""#CmdClass""); }       \
	virtual ~CmdClass() {}						      \
	virtual bool AnyMessageLogic(common::errorinfo& oErrInfo, const CContext& oInContext)\
	{												  \
		LOG4_TRACE(__FUNCTION__);					  \
		StepClass* pStep = new StepClass(oInContext); \
		return RegisterSyncStep(oErrInfo, pStep);	  \
	}												  \
};													  \
													  \
extern "C"											  \
{													  \
	oss::Cmd* create()								  \
	{												  \
		oss::Cmd* pCmd = new CmdClass();			  \
		return(pCmd);								  \
	}												  \
}													  \
}

#endif /* _CMD_LOGIC_H_ */

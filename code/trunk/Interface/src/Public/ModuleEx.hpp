/*******************************************************************************
 * Project:  InterfaceServer
 * @file     ModuleEx.hpp
 * @brief    为了封装HTTP逻辑底层一些细节和原生代码。
 * @author   wfh
 * @date:    2016年12月22日
 * @note     
 * Modify history:
 ******************************************************************************/
#ifndef _MODULE_EX_H_
#define _MODULE_EX_H_

#include "ModulePublic.hpp"
#include <cmd/Module.hpp>
#include "HttpStepEx.hpp"

namespace bsw
{

class ModuleEx : public oss::Module
{
public:
    ModuleEx();
    virtual ~ModuleEx();

    virtual bool AnyMessage(const oss::tagMsgShell& stMsgShell, const HttpMsg& oInHttpMsg);
	virtual bool AnyMessageLogic(common::errorinfo& oErrInfo, const CHttpContext& oInContext);

	uint32 RegisterSyncStep(common::errorinfo& oErrInfo, bsw::HttpStepEx* pStep);

	bool GetCookieInfo(common::errorinfo& oErrInfo, const HttpMsg& oInHttpMsg, std::string& strToken, uint32& uiUserID);
};

} /* namespace bsw */

#define MODULE_CLASS_MAKE(ModuleClass, StepClass)	  \
													  \
namespace bsw										  \
{													  \
class ModuleClass : public ModuleEx					  \
{													  \
public:                                               \
	ModuleClass() { SetClassName(""#ModuleClass""); } \
	virtual ~ModuleClass() {}						  \
	virtual bool AnyMessageLogic(common::errorinfo& oErrInfo, const CHttpContext& oInContext)\
{													  \
	LOG4_TRACE(__FUNCTION__);						  \
	StepClass* pStep = new StepClass(oInContext);	  \
	return RegisterSyncStep(oErrInfo, pStep);		  \
}													  \
};													  \
													  \
extern "C"											  \
{													  \
	oss::Cmd* create()								  \
	{												  \
		oss::Cmd* pCmd = new ModuleClass();			  \
		return(pCmd);								  \
	}												  \
}													  \
}
								
#endif /* _MODULE_EX_H_ */

/*******************************************************************************
 * Project:  Public
 * @file     PublicFunc.hpp
 * @brief    公共函数
 * @author   wfh
 * @date:    2016年11月20日
 * @note
 * Modify history:
 ******************************************************************************/
#ifndef _PUBLIC_FUNC_HPP_
#define _PUBLIC_FUNC_HPP_

#include "ModulePublic.hpp"

namespace bsw
{

std::string IntToString(uint32 uiValue);
std::string Int64ToString(uint64 uiValue);
std::string FormatString(const char* pString, ...);

} /* namespace bsw */

#endif /* _PUBLIC_FUNC_HPP_ */

/*******************************************************************************
 * Project:  Public
 * @file     PublicFunc.cpp
 * @brief    公共函数
 * @author   wfh
 * @date:    2016年11月20日
 * @note
 * Modify history:
 ******************************************************************************/
#include <stdarg.h>
#include <sstream>
#include "ModulePublic.hpp"

namespace mg
{

std::string FormatString(const char* pString, ...)
{
	va_list argList;
	std::string strFormat;
	char szFormat[1024] = {0};

	va_start(argList, pString);
	vsnprintf(szFormat, 512, pString, argList);
	va_end(argList);
	strFormat = szFormat;

	return strFormat;
}

std::string IntToString(uint32 uiValue)
{
	std::ostringstream ss;
	ss << uiValue;
	return ss.str();
}

std::string Int64ToString(uint64 uiValue)
{
	std::ostringstream ss;
	ss << uiValue;
	return ss.str();
}

} /* namespace mg */

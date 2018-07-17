/*******************************************************************************
* Project:  proto
* @file     BswErrorMapping.h
* @brief    mg错误与系统错误映射
* @author   wfh
* @date:    2016年4月9日
* @note
* Modify history:
******************************************************************************/
#ifndef SRC_IMERRORMAPPING_H_
#define SRC_IMERRORMAPPING_H_

#include "OssError.hpp"
#include "BswError.h"

namespace mg
{

int bsw_err_code(int code);
const char * bsw_err_msg(int code);

}

#endif /* SRC_IMERRORMAPPING_H_ */

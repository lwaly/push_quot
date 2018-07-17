/*******************************************************************************
* Project:  proto
* @file     BswMsg.hpp
* @brief    xox消息相关处理
* @author   wfh
* @date:    2016年10月27日
* @note
* Modify history:
******************************************************************************/
#ifndef _BSW_MSG_H_
#define _BSW_MSG_H_

#include <time.h>

namespace mg
{

const unsigned long long gc_ullSecondBit = 0xFFFFFFFF80000000ULL;
const unsigned long long gc_ullNodeIdBit = 0x7FC00000;
const unsigned long long gc_ullWorkerIndexBit = 0x3C0000;
const unsigned long long gc_ullSequenceBit = 0x3FFFF;

unsigned long long GetMsgId(unsigned short uiNodeId, unsigned char ucWorkerIndex);

}

#endif /* _BSW_MSG_H_ */

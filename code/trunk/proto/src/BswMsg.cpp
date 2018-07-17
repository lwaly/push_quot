/*******************************************************************************
* Project:  proto
* @file     BswMsg.cpp
* @brief    xox消息相关处理
* @author   wfh
* @date:    2016年10月27日
* @note
* Modify history:
******************************************************************************/

#include "BswMsg.hpp"

namespace mg
{

unsigned long long GetMsgId(unsigned short uiNodeId, unsigned char ucWorkerIndex)
{
    static unsigned int uiSequence = 0;
    static unsigned long long ullSecond = 0;
    unsigned long long ullMsgId = time(NULL);
    unsigned int uiNodeIdBit = ((unsigned int)uiNodeId << 22) & gc_ullNodeIdBit;
    unsigned int uiWorkerIndexBit = ((unsigned int)ucWorkerIndex << 18) & gc_ullWorkerIndexBit;
    if (ullMsgId > ullSecond)
    {
        ullSecond = ullMsgId;
        uiSequence = 0;
    }
    else
    {
        ++uiSequence;
    }
    uiSequence &= gc_ullSequenceBit;
    ullMsgId <<= 31;
    ullMsgId |= (uiNodeIdBit | uiWorkerIndexBit | uiSequence);
    return(ullMsgId);
}

}

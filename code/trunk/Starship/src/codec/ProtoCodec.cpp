/*******************************************************************************
 * Project:  Starship
 * @file     ProtoCodec.cpp
 * @brief 
 * @author   lbh
 * @date:    2015年10月6日
 * @note
 * Modify history:
 ******************************************************************************/
#include "ProtoCodec.hpp"

namespace oss
{

ProtoCodec::ProtoCodec(loss::E_CODEC_TYPE eCodecType, const std::string& strKey)
    : StarshipCodec(eCodecType, strKey)
{
}

ProtoCodec::~ProtoCodec()
{
}

E_CODEC_STATUS ProtoCodec::Encode(const MsgHead& oMsgHead, const MsgBody& oMsgBody, loss::CBuffer* pBuff)
{
    LOG4_TRACE("%s() pBuff->ReadableBytes()=%u", __FUNCTION__, pBuff->ReadableBytes());
    int iErrno = 0;
    int iHadWriteLen = 0;
    int iWriteLen = 0;
    int iNeedWriteLen = oMsgHead.ByteSize();
    iWriteLen = pBuff->Write(oMsgHead.SerializeAsString().c_str(), oMsgHead.ByteSize());
    if (iWriteLen != iNeedWriteLen)
    {
        LOG4_ERROR("buff write head iWriteLen != iNeedWriteLen!");
        pBuff->SetWriteIndex(pBuff->GetWriteIndex() - iHadWriteLen);
        return(CODEC_STATUS_ERR);
    }
    iHadWriteLen += iWriteLen;
    if (oMsgHead.msgbody_len() == 0)    // 无包体（心跳包等）
    {
        pBuff->Compact(8192);
        return(CODEC_STATUS_OK);
    }
    iNeedWriteLen = oMsgBody.ByteSize();
    iWriteLen = pBuff->Write(oMsgBody.SerializeAsString().c_str(), oMsgBody.ByteSize());
    LOG4_TRACE("pBuff->ReadableBytes()=%u", pBuff->ReadableBytes());
    if (iWriteLen == iNeedWriteLen)
    {
        pBuff->Compact(8192);      // 超过32KB则重新分配内存
        return(CODEC_STATUS_OK);
    }
    else
    {
        LOG4_ERROR("buff write body iWriteLen != iNeedWriteLen!");
        pBuff->SetWriteIndex(pBuff->GetWriteIndex() - iHadWriteLen);
        return(CODEC_STATUS_ERR);
    }
}

E_CODEC_STATUS ProtoCodec::Decode(loss::CBuffer* pBuff, MsgHead& oMsgHead, MsgBody& oMsgBody)
{
    LOG4_TRACE("%s() pBuff->ReadableBytes()=%d, pBuff->GetReadIndex()=%d",
                    __FUNCTION__, pBuff->ReadableBytes(), pBuff->GetReadIndex());
    oMsgHead.set_cmd(0);
    oMsgHead.set_msgbody_len(0);
    oMsgHead.set_seq(0);
    int iHeadSize = oMsgHead.ByteSize();
    if ((int)(pBuff->ReadableBytes()) >= iHeadSize)
    {
        bool bResult = oMsgHead.ParseFromArray(pBuff->GetRawReadBuffer(), iHeadSize);
        if (bResult)
        {
            if (0 == oMsgHead.msgbody_len())      // 无包体（心跳包等）
            {
                pBuff->SkipBytes(iHeadSize);
                pBuff->Compact(8192);
                return(CODEC_STATUS_OK);
            }
            if (pBuff->ReadableBytes() >= iHeadSize + oMsgHead.msgbody_len())
            {
                bResult = oMsgBody.ParseFromArray(
                                pBuff->GetRawReadBuffer() + iHeadSize, oMsgHead.msgbody_len());
                if (bResult)
                {
                    pBuff->SkipBytes(oMsgHead.ByteSize() + oMsgBody.ByteSize());
                    pBuff->Compact(8192);      // 超过32KB则重新分配内存
                    return(CODEC_STATUS_OK);
                }
                else
                {
                    LOG4_ERROR("cmd[%u], seq[%lu] oMsgBody.ParseFromArray() error!", oMsgHead.cmd(), oMsgHead.seq());
                    return(CODEC_STATUS_ERR);
                }
            }
            else
            {
                return(CODEC_STATUS_PAUSE);
            }
        }
        else
        {
            LOG4_ERROR("oMsgHead.ParseFromArray() error!");
            return(CODEC_STATUS_ERR);
        }
    }
    else
    {
        return(CODEC_STATUS_PAUSE);
    }
}

} /* namespace oss */

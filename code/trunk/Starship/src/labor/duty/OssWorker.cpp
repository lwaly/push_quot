/*******************************************************************************
 * Project:  AsyncServer
 * @file     OssWorker.cpp
 * @brief
 * @author   lbh
 * @date:    2015年7月27日
 * @note
 * Modify history:
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
#include "hiredis/async.h"
#include "hiredis/adapters/libev.h"
#include "unix_util/process_helper.h"
#include "unix_util/proctitle_helper.h"
#ifdef __cplusplus
}
#endif
#include "OssWorker.hpp"
#include "codec/ProtoCodec.hpp"
#include "codec/ClientMsgCodec.hpp"
#include "codec/HttpCodec.hpp"
#include "codec/CodecWsExtentJson.hpp"
#include "step/Step.hpp"
#include "step/RedisStep.hpp"
#include "step/HttpStep.hpp"
#include "session/Session.hpp"
#include "cmd/Cmd.hpp"
#include "cmd/Module.hpp"
#include "cmd/sys_cmd/CmdConnectWorker.hpp"
#include "cmd/sys_cmd/CmdToldWorker.hpp"
#include "cmd/sys_cmd/CmdUpdateNodeId.hpp"
#include "cmd/sys_cmd/CmdNodeNotice.hpp"
#include "cmd/sys_cmd/CmdBeat.hpp"
#include "cmd/sys_cmd/CmdUpdateConfig.hpp"
#include "step/sys_step/StepIoTimeout.hpp"

namespace oss
{

    tagSo::tagSo() : pSoHandle(NULL), pCmd(NULL), iVersion(0)
    {
    }

    tagSo::~tagSo()
    {
        if (pCmd != NULL)
        {
            delete pCmd;
            pCmd = NULL;
        }
        if (pSoHandle != NULL)
        {
            dlclose(pSoHandle);
            pSoHandle = NULL;
        }
    }

    tagModule::tagModule() : pSoHandle(NULL), pModule(NULL), iVersion(0)
    {
    }

    tagModule::~tagModule()
    {
        if (pModule != NULL)
        {
            delete pModule;
            pModule = NULL;
        }
        if (pSoHandle != NULL)
        {
            dlclose(pSoHandle);
            pSoHandle = NULL;
        }
    }


    void OssWorker::TerminatedCallback(struct ev_loop* loop, struct ev_signal* watcher, int revents)
    {
        if (watcher->data != NULL)
        {
            OssWorker* pWorker = (OssWorker*)watcher->data;
            pWorker->Terminated(watcher);  // timeout，worker进程无响应或与Manager通信通道异常，被manager进程终止时返回
        }
    }

    void OssWorker::IdleCallback(struct ev_loop* loop, struct ev_idle* watcher, int revents)
    {
        if (watcher->data != NULL)
        {
            OssWorker* pWorker = (OssWorker*)watcher->data;
            pWorker->CheckParent();
        }
    }

    void OssWorker::IoCallback(struct ev_loop* loop, struct ev_io* watcher, int revents)
    {
        if (watcher->data != NULL)
        {
            tagIoWatcherData* pData = (tagIoWatcherData*)watcher->data;
            OssWorker* pWorker = (OssWorker*)pData->pWorker;
            if (revents & EV_READ)
            {
                pWorker->IoRead(pData, watcher);
            }
            if (revents & EV_WRITE)
            {
                pWorker->IoWrite(pData, watcher);
            }
            if (revents & EV_ERROR)
            {
                pWorker->IoError(pData, watcher);
            }
        }
    }

    void OssWorker::IoTimeoutCallback(struct ev_loop* loop, struct ev_timer* watcher, int revents)
    {
        if (watcher->data != NULL)
        {
            tagIoWatcherData* pData = (tagIoWatcherData*)watcher->data;
            OssWorker* pWorker = pData->pWorker;
            pWorker->IoTimeout(watcher);
        }
    }

    void OssWorker::PeriodicTaskCallback(struct ev_loop* loop, struct ev_timer* watcher, int revents)
    {
        if (watcher->data != NULL)
        {
            OssWorker* pWorker = (OssWorker*)(watcher->data);

            pWorker->CheckParent();

        }
        ev_timer_stop(loop, watcher);
        ev_timer_set(watcher, NODE_BEAT + ev_time() - ev_now(loop), 0);
        ev_timer_start(loop, watcher);
    }

    void OssWorker::StepTimeoutCallback(struct ev_loop* loop, struct ev_timer* watcher, int revents)
    {
        if (watcher->data != NULL)
        {
            Step* pStep = (Step*)watcher->data;
            ((OssWorker*)(pStep->GetLabor()))->StepTimeout(pStep, watcher);
        }
    }

    void OssWorker::SessionTimeoutCallback(struct ev_loop* loop, struct ev_timer* watcher, int revents)
    {
        if (watcher->data != NULL)
        {
            Session* pSession = (Session*)watcher->data;
            ((OssWorker*)pSession->GetLabor())->SessionTimeout(pSession, watcher);
        }
    }

    void OssWorker::RedisConnectCallback(const redisAsyncContext *c, int status)
    {
        if (c->data != NULL)
        {
            OssWorker* pWorker = (OssWorker*)c->data;
            pWorker->RedisConnect(c, status);
        }
    }

    void OssWorker::RedisDisconnectCallback(const redisAsyncContext *c, int status)
    {
        if (c->data != NULL)
        {
            OssWorker* pWorker = (OssWorker*)c->data;
            pWorker->RedisDisconnect(c, status);
        }
    }

    void OssWorker::RedisCmdCallback(redisAsyncContext *c, void *reply, void *privdata)
    {
        if (c->data != NULL)
        {
            OssWorker* pWorker = (OssWorker*)c->data;
            pWorker->RedisCmdResult(c, reply, privdata);
        }
    }

    OssWorker::OssWorker(const std::string& strWorkPath, int iControlFd, int iDataFd, int iWorkerIndex, loss::CJsonObject& oJsonConf)
        : m_pErrBuff(NULL), m_ulSequence(0), m_bInitLogger(false), m_dIoTimeout(480.0), m_strWorkPath(strWorkPath), m_uiNodeId(0),
        m_iManagerControlFd(iControlFd), m_iManagerDataFd(iDataFd), m_iWorkerIndex(iWorkerIndex), m_iWorkerPid(0),
        m_dMsgStatInterval(60.0), m_iMsgPermitNum(60),
        m_iRecvNum(0), m_iRecvByte(0), m_iSendNum(0), m_iSendByte(0),
        m_loop(NULL), m_pCmdConnect(NULL)
    {
        m_iWorkerPid = getpid();
        m_pErrBuff = new char[gc_iErrBuffLen];
        if (!Init(oJsonConf))
        {
            exit(-1);
        }
        if (!CreateEvents())
        {
            exit(-2);
        }
        PreloadCmd();
        LoadSo(oJsonConf["so"]);
        LoadModule(oJsonConf["module"]);
    }

    OssWorker::~OssWorker()
    {
        Destroy();
    }

    void OssWorker::Run()
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        ev_run(m_loop, 0);
    }

    void OssWorker::Terminated(struct ev_signal* watcher)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        int iSignum = watcher->signum;
        delete watcher;
        //Destroy();
        LOG4_FATAL("terminated by signal %d!", iSignum);
        exit(iSignum);
    }

    bool OssWorker::CheckParent()
    {
        //LOG4_TRACE("%s()", __FUNCTION__);
        pid_t iParentPid = getppid();
        if (iParentPid == 1)    // manager进程已不存在
        {
            LOG4_INFO("no manager process exist, worker %d exit.", m_iWorkerIndex);
            //Destroy();
            exit(0);
        }
        MsgHead oMsgHead;
        MsgBody oMsgBody;
        loss::CJsonObject oJsonLoad;
        oJsonLoad.Add("load", int32(m_mapCallbackStep.size()));
        oJsonLoad.Add("connect", int32(m_mapFdAttr.size()));
        oJsonLoad.Add("recv_num", m_iRecvNum);
        oJsonLoad.Add("recv_byte", m_iRecvByte);
        oJsonLoad.Add("send_num", m_iSendNum);
        oJsonLoad.Add("send_byte", m_iSendByte);
        oJsonLoad.Add("client", int32(m_mapFdAttr.size() - m_mapInnerFd.size()));
        // LOG4_TRACE("%s", oJsonLoad.ToString().c_str());
        oMsgBody.set_body(oJsonLoad.ToString());
        oMsgHead.set_cmd(CMD_REQ_UPDATE_WORKER_LOAD);
        oMsgHead.set_seq(GetSequence());
        oMsgHead.set_msgbody_len(oMsgBody.ByteSize());
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(m_iManagerControlFd);
        if (iter != m_mapFdAttr.end())
        {
            int iErrno = 0;
            iter->second->pSendBuff->Write(oMsgHead.SerializeAsString().c_str(), oMsgHead.ByteSize());
            iter->second->pSendBuff->Write(oMsgBody.SerializeAsString().c_str(), oMsgBody.ByteSize());
            iter->second->pSendBuff->WriteFD(m_iManagerControlFd, iErrno);
            iter->second->pSendBuff->Compact(8192);
        }
        m_iRecvNum = 0;
        m_iRecvByte = 0;
        m_iSendNum = 0;
        m_iSendByte = 0;
        //    uint32 uiLoad = m_mapFdAttr.size() + m_mapCallbackStep.size();
        //    write(m_iManagerControlFd, &uiLoad, sizeof(uiLoad));    // 向父进程上报当前进程负载
        return(true);
    }

    bool OssWorker::SendToParent(const MsgHead& oMsgHead, const MsgBody& oMsgBody)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(m_iManagerControlFd);
        if (iter != m_mapFdAttr.end())
        {
            int iErrno = 0;
            iter->second->pSendBuff->Write(oMsgHead.SerializeAsString().c_str(), oMsgHead.ByteSize());
            iter->second->pSendBuff->Write(oMsgBody.SerializeAsString().c_str(), oMsgBody.ByteSize());
            iter->second->pSendBuff->WriteFD(m_iManagerControlFd, iErrno);
            iter->second->pSendBuff->Compact(8192);
        }
        m_iRecvNum = 0;
        m_iRecvByte = 0;
        m_iSendNum = 0;
        m_iSendByte = 0;
        return(true);
    }

    bool OssWorker::IoRead(tagIoWatcherData* pData, struct ev_io* watcher)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        if (watcher->fd == m_iManagerDataFd)
        {
            return(FdTransfer());
        }
        else
        {
            return(RecvDataAndDispose(pData, watcher));
        }
    }

    bool OssWorker::RecvDataAndDispose(tagIoWatcherData* pData, struct ev_io* watcher)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        int iErrno = 0;
        int iReadLen = 0;
        std::map<int, tagConnectionAttr*>::iterator conn_iter;
        conn_iter = m_mapFdAttr.find(pData->iFd);
        if (conn_iter == m_mapFdAttr.end())
        {
            LOG4_ERROR("no fd attr for %d!", pData->iFd);
        }
        else
        {
            conn_iter->second->dActiveTime = ev_now(m_loop);
            if (pData->ulSeq != conn_iter->second->ulSeq)
            {
                LOG4_DEBUG("callback seq %lu not match the conn attr seq %lu",
                    pData->ulSeq, conn_iter->second->ulSeq);
                ev_io_stop(m_loop, watcher);
                pData->pWorker = NULL;
                delete pData;
                watcher->data = NULL;
                delete watcher;
                watcher = NULL;
                return(false);
            }
            conn_iter->second->pRecvBuff->Compact(8192);
            LOG4_DEBUG("iReadLen1 is %d", iReadLen);
        read_again:
            iReadLen = conn_iter->second->pRecvBuff->ReadFD(pData->iFd, iErrno);
            LOG4_DEBUG("iReadLen2 is %d", iReadLen);
            if (NULL == conn_iter->second->pRemoteAddr)
            {
                LOG4_TRACE("recv from fd %d identify %s, data len %d codec %d",
                    pData->iFd, conn_iter->second->strIdentify.c_str(),
                    iReadLen, conn_iter->second->eCodecType);
            }
            else
            {
                LOG4_TRACE("recv from fd %d ip %s identify %s, data len %d codec %d",
                    pData->iFd, conn_iter->second->pRemoteAddr,
                    conn_iter->second->strIdentify.c_str(),
                    iReadLen, conn_iter->second->eCodecType);
            }
            if (iReadLen > 0)
            {
                m_iRecvByte += iReadLen;
                conn_iter->second->ulMsgNumUnitTime++;      // TODO 这里要做发送消息频率限制，有待补充
                conn_iter->second->ulMsgNum++;
                MsgHead oInMsgHead, oOutMsgHead;
                MsgBody oInMsgBody, oOutMsgBody;
                StarshipCodec* pCodec = NULL;
                std::map<loss::E_CODEC_TYPE, StarshipCodec*>::iterator codec_iter = m_mapCodec.find(conn_iter->second->eCodecType);
                if (codec_iter == m_mapCodec.end())
                {
                    LOG4_ERROR("no codec found for %d!", conn_iter->second->eCodecType);
                    if (pData->iFd != m_iManagerControlFd && pData->iFd != m_iManagerDataFd)
                    {
                        LOG4_DEBUG("if (pData->iFd != m_iManagerControlFd && pData->iFd != m_iManagerDataFd)");
                        DestroyConnect(conn_iter);
                    }
                    return(false);
                }
                while (conn_iter->second->pRecvBuff->ReadableBytes() >= gc_uiClientMsgHeadSize)
                {
                    oInMsgHead.Clear();
                    oInMsgBody.Clear();
                    E_CODEC_STATUS eCodecStatus = codec_iter->second->Decode(conn_iter->second->pRecvBuff, oInMsgHead, oInMsgBody);
                    if (CODEC_STATUS_OK == eCodecStatus)
                    {
                        if (loss::CODEC_PROTOBUF == conn_iter->second->eCodecType)   // 连接尚未完成
                        {
                            if (eConnectStatus_ok != conn_iter->second->ucConnectStatus)
                            {
                                LOG4_DEBUG("oInMsgHead.cmd(%u),ucConnectStatus(%u)",
                                    oInMsgHead.cmd(), conn_iter->second->ucConnectStatus);
                                if (CMD_RSP_TELL_WORKER == oInMsgHead.cmd())
                                {
                                    conn_iter->second->ucConnectStatus = eConnectStatus_ok;
                                }
                                else if (CMD_RSP_TELL_WORKER > oInMsgHead.cmd())
                                {
                                    conn_iter->second->ucConnectStatus = eConnectStatus_connecting;
                                }
                            }
                        }
                        ++m_iRecvNum;
                        conn_iter->second->dActiveTime = ev_now(m_loop);
                        bool bDisposeResult = false;
                        tagMsgShell stMsgShell;
                        stMsgShell.iFd = pData->iFd;
                        stMsgShell.ulSeq = conn_iter->second->ulSeq;
                        if (oInMsgHead.has_cmd() && oInMsgHead.has_seq() && oInMsgHead.cmd() != 0)  // 基于TCP的自定义协议请求或带cmd、seq自定义头域的http请求
                        {
#ifdef NODE_TYPE_ACCESS
                            if (conn_iter->second->pClientData != NULL)
                            {
                                if (conn_iter->second->pClientData->ReadableBytes() > 0)
                                {
                                    oInMsgBody.set_additional(conn_iter->second->pClientData->GetRawReadBuffer(),
                                        conn_iter->second->pClientData->ReadableBytes());
                                    oInMsgHead.set_msgbody_len(oInMsgBody.ByteSize());
                                }
                                else// 如果是http短连接，则不会含oInMsgHead.cmd()，也不需要验证
                                {
                                    // 如果是websocket连接，则需要验证连接
                                    if (loss::CODEC_WEBSOCKET_JSON == conn_iter->second->eCodecType)
                                    {
                                        std::map<int32, std::list<uint32> >::iterator http_iter = m_mapHttpAttr.find(stMsgShell.iFd);
                                        if (http_iter == m_mapHttpAttr.end())   // 未经握手的websocket客户端连接发送数据过来，直接断开
                                        {
                                            LOG4_DEBUG("invalid request, please handshake first!");
                                            DestroyConnect(conn_iter);
                                            return(false);
                                        }
                                    }
                                    else
                                    {
                                        std::map<int, uint32>::iterator inner_iter = m_mapInnerFd.find(stMsgShell.iFd);
                                        if (inner_iter == m_mapInnerFd.end() && conn_iter->second->ulMsgNum > 1)   // 未经账号验证的客户端连接发送数据过来，直接断开
                                        {
                                            LOG4_DEBUG("invalid request, please login first!");
                                            DestroyConnect(conn_iter);
                                            return(false);
                                        }
                                    }
                                }
                            }
                            else// 如果是http短连接，则不会含oInMsgHead.cmd()，也不需要验证
                            {
                                if (loss::CODEC_WEBSOCKET_JSON == conn_iter->second->eCodecType) // 如果是websocket连接，则需要验证连接
                                {
                                    std::map<int32, std::list<uint32> >::iterator http_iter = m_mapHttpAttr.find(stMsgShell.iFd);
                                    if (http_iter == m_mapHttpAttr.end())   // 未经握手的websocket客户端连接发送数据过来，直接断开
                                    {
                                        LOG4_DEBUG("invalid request, please handshake first!");
                                        DestroyConnect(conn_iter);
                                        return(false);
                                    }
                                }
                                else
                                {
                                    std::map<int, uint32>::iterator inner_iter = m_mapInnerFd.find(stMsgShell.iFd);
                                    if (inner_iter == m_mapInnerFd.end() && conn_iter->second->ulMsgNum > 1)   // 未经账号验证的客户端连接发送数据过来，直接断开
                                    {
                                        LOG4_DEBUG("invalid request, please login first!");
                                        DestroyConnect(conn_iter);
                                        return(false);
                                    }
                                }
                            }
#endif
                            if ((gc_uiCmdReq & oInMsgHead.cmd()) && oInMsgHead.has_seq())
                            {
                                conn_iter->second->ulForeignSeq = oInMsgHead.seq();
                            }
                            bDisposeResult = Dispose(stMsgShell, oInMsgHead, oInMsgBody, oOutMsgHead, oOutMsgBody); // 处理过程有可能会断开连接，所以下面要做连接是否存在检查
                            std::map<int, tagConnectionAttr*>::iterator dispose_conn_iter = m_mapFdAttr.find(pData->iFd);
                            if (dispose_conn_iter == m_mapFdAttr.end())     // 连接已断开，资源已回收
                            {
                                return(true);
                            }
                            else
                            {
                                if (pData->ulSeq != dispose_conn_iter->second->ulSeq)     // 连接已断开，资源已回收
                                {
                                    return(true);
                                }
                            }
                            if (oOutMsgHead.ByteSize() > 0)
                            {
                                eCodecStatus = codec_iter->second->Encode(oOutMsgHead, oOutMsgBody, conn_iter->second->pSendBuff);
                                if (CODEC_STATUS_OK == eCodecStatus)
                                {
                                    conn_iter->second->pSendBuff->WriteFD(pData->iFd, iErrno);
                                    conn_iter->second->pSendBuff->Compact(8192);
                                    if (iErrno != 0)
                                    {
                                        LOG4_ERROR("error %d %s!",
                                            iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                                    }
                                }
                                else if (CODEC_STATUS_ERR == eCodecStatus)
                                {
                                    LOG4_ERROR("faild to Encode");
                                }
                            }
                        }
                        else  // url方式的http请求
                        {
                            HttpMsg oInHttpMsg;
                            HttpMsg oOutHttpMsg;
                            if (oInHttpMsg.ParseFromString(oInMsgBody.body()))
                            {
                                conn_iter->second->dKeepAlive = 10;   // 未带KeepAlive参数，默认10秒钟关闭
                                for (int i = 0; i < oInHttpMsg.headers_size(); ++i)
                                {
                                    if (std::string("Keep-Alive") == oInHttpMsg.headers(i).header_name())
                                    {
                                        conn_iter->second->dKeepAlive = strtoul(oInHttpMsg.headers(i).header_value().c_str(), NULL, 10);
                                        AddIoTimeout(conn_iter->first, conn_iter->second->ulSeq, conn_iter->second, conn_iter->second->dKeepAlive);
                                        break;
                                    }
                                    else if (std::string("Connection") == oInHttpMsg.headers(i).header_name())
                                    {
                                        if (std::string("keep-alive") == oInHttpMsg.headers(i).header_value())
                                        {
                                            conn_iter->second->dKeepAlive = 65.0;
                                            AddIoTimeout(conn_iter->first, conn_iter->second->ulSeq, conn_iter->second, 65.0);
                                            break;
                                        }
                                        else if (std::string("close") == oInHttpMsg.headers(i).header_value()
                                            && HTTP_RESPONSE == oInHttpMsg.type())
                                        {   // 作为客户端请求远端http服务器，对方response后要求客户端关闭连接
                                            conn_iter->second->dKeepAlive = -1;
                                            LOG4_DEBUG("std::string(\"close\") == oInHttpMsg.headers(i).header_value()");
                                            DestroyConnect(conn_iter);
                                            break;
                                        }
                                        else
                                        {
                                            AddIoTimeout(conn_iter->first, conn_iter->second->ulSeq, conn_iter->second, conn_iter->second->dKeepAlive);
                                        }
                                    }
                                }
                                bDisposeResult = Dispose(stMsgShell, oInHttpMsg, oOutHttpMsg);  // 处理过程有可能会断开连接，所以下面要做连接是否存在检查
                                std::map<int, tagConnectionAttr*>::iterator dispose_conn_iter = m_mapFdAttr.find(pData->iFd);
                                if (dispose_conn_iter == m_mapFdAttr.end())     // 连接已断开，资源已回收
                                {
                                    return(true);
                                }
                                else
                                {
                                    if (pData->ulSeq != dispose_conn_iter->second->ulSeq)     // 连接已断开，资源已回收
                                    {
                                        return(true);
                                    }
                                }
                                if (conn_iter->second->dKeepAlive < 0)
                                {
                                    if (HTTP_RESPONSE == oInHttpMsg.type())
                                    {
                                        LOG4_DEBUG("if (HTTP_RESPONSE == oInHttpMsg.type())");
                                        DestroyConnect(conn_iter);
                                    }
                                    else
                                    {
                                        ((HttpCodec*)codec_iter->second)->AddHttpHeader("Connection", "close");
                                    }
                                }
                                if (oOutHttpMsg.ByteSize() > 0)
                                {
                                    eCodecStatus = ((HttpCodec*)codec_iter->second)->Encode(oOutHttpMsg, conn_iter->second->pSendBuff);
                                    if (CODEC_STATUS_OK == eCodecStatus)
                                    {
                                        conn_iter->second->pSendBuff->WriteFD(pData->iFd, iErrno);
                                        conn_iter->second->pSendBuff->Compact(8192);
                                        if (iErrno != 0)
                                        {
                                            LOG4_ERROR("error %d %s!",
                                                iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                                        }
                                    }
                                    else if (CODEC_STATUS_ERR == eCodecStatus)
                                    {
                                        LOG4_ERROR("faild to Encode");
                                    }

                                }
                            }
                            else
                            {
                                LOG4_ERROR("oInHttpMsg.ParseFromString() error!");
                            }
                        }
                        if (!bDisposeResult)
                        {
                            break;
                        }
                    }
                    else if (CODEC_STATUS_ERR == eCodecStatus)
                    {
                        if (pData->iFd != m_iManagerControlFd && pData->iFd != m_iManagerDataFd)
                        {
                            LOG4_DEBUG("if (pData->iFd != m_iManagerControlFd && pData->iFd != m_iManagerDataFd)");
                            DestroyConnect(conn_iter);
                        }
                        return(false);
                    }
                    else
                    {
                        break;  // 数据尚未接收完整
                    }
                }
                return(true);
            }
            else if (iReadLen == 0)
            {
                LOG4_DEBUG("fd %d closed by peer, errno %d %s!",
                    pData->iFd, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                if (pData->iFd != m_iManagerControlFd && pData->iFd != m_iManagerDataFd)
                {
                    LOG4_DEBUG("if (pData->iFd != m_iManagerControlFd && pData->iFd != m_iManagerDataFd)");
                    DestroyConnect(conn_iter);
                }
            }
            else
            {
                LOG4_TRACE("recv from fd %d errno %d: %s",
                    pData->iFd, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                if (EAGAIN == iErrno)    // 对非阻塞socket而言，EAGAIN不是一种错误;EINTR即errno为4，错误描述Interrupted system call，操作也应该继续。
                {
                    ;
                }
                else if (EINTR == iErrno)
                {
                    goto read_again;
                }
                else
                {
                    LOG4_ERROR("recv from fd %d errno %d: %s",
                        pData->iFd, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                    if (pData->iFd != m_iManagerControlFd && pData->iFd != m_iManagerDataFd)
                    {
                        LOG4_DEBUG("if (pData->iFd != m_iManagerControlFd && pData->iFd != m_iManagerDataFd), pData->iFd = %d,  m_iManagerDataFd = %d , m_iManagerControlFd  = %d\n", pData->iFd, m_iManagerDataFd, m_iManagerControlFd);
                        DestroyConnect(conn_iter);
                    }
                }
            }
        }
        return(false);
    }

    bool OssWorker::FdTransfer()
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        char szIpAddr[16] = { 0 };
        int iCodec = 0;
        // int iAcceptFd = recv_fd(m_iManagerDataFd);
        int iAcceptFd = recv_fd_with_attr(m_iManagerDataFd, szIpAddr, 16, &iCodec);
        if (iAcceptFd <= 0)
        {
            if (iAcceptFd == 0)
            {
                LOG4_ERROR("recv_fd from m_iManagerDataFd %d len %d", m_iManagerDataFd, iAcceptFd);
                exit(2); // manager与worker通信fd已关闭，worker进程退出
            }
            else if (errno != EAGAIN)
            {
                LOG4_ERROR("recv_fd from m_iManagerDataFd %d error %d", m_iManagerDataFd, errno);
                //Destroy();
                exit(2); // manager与worker通信fd已关闭，worker进程退出
            }
        }
        else
        {
            uint32 ulSeq = GetSequence();
            tagConnectionAttr* pConnAttr = CreateFdAttr(iAcceptFd, ulSeq, loss::E_CODEC_TYPE(iCodec));
            x_sock_set_block(iAcceptFd, 0);
            if (pConnAttr)
            {
                snprintf(pConnAttr->pRemoteAddr, 32, szIpAddr);
                LOG4_DEBUG("pConnAttr->pClientAddr = %s, iCodec = %d", pConnAttr->pRemoteAddr, iCodec);
                std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(iAcceptFd);
                if (AddIoTimeout(iAcceptFd, ulSeq, iter->second, 1.5))     // 为了防止大量连接攻击，初始化连接只有一秒即超时，在第一次超时检查（或正常发送第一个http数据包）之后才采用正常配置的网络IO超时检查
                {
                    if (!AddIoReadEvent(iter))
                    {
                        LOG4_DEBUG("if (!AddIoReadEvent(iter))");
                        DestroyConnect(iter);
                        return(false);
                    }
                    //                if (!AddIoErrorEvent(iAcceptFd))
                    //                {
                    //                    DestroyConnect(iter);
                    //                    return(false);
                    //                }
                    return(true);
                }
                else
                {
                    LOG4_DEBUG("if(AddIoTimeout(iAcceptFd, ulSeq, iter->second, 1.5)) else");
                    DestroyConnect(iter);
                    return(false);
                }
            }
            else    // 没有足够资源分配给新连接，直接close掉
            {
                close(iAcceptFd);
            }
        }
        return(false);
    }

    bool OssWorker::IoWrite(tagIoWatcherData* pData, struct ev_io* watcher)
    {
        //LOG4_TRACE("%s()", __FUNCTION__);
        std::map<int, tagConnectionAttr*>::iterator attr_iter = m_mapFdAttr.find(pData->iFd);
        if (attr_iter == m_mapFdAttr.end())
        {
            return(false);
        }
        else
        {
            if (pData->ulSeq != attr_iter->second->ulSeq)
            {
                LOG4_DEBUG("callback seq %lu not match the conn attr seq %lu",
                    pData->ulSeq, attr_iter->second->ulSeq);
                ev_io_stop(m_loop, watcher);
                pData->pWorker = NULL;
                delete pData;
                watcher->data = NULL;
                delete watcher;
                watcher = NULL;
                return(false);
            }
            int iErrno = 0;
            int iNeedWriteLen = (int)attr_iter->second->pSendBuff->ReadableBytes();
            int iWriteLen = 0;
            iWriteLen = attr_iter->second->pSendBuff->WriteFD(pData->iFd, iErrno);
            attr_iter->second->pSendBuff->Compact(8192);
            if (iWriteLen < 0)
            {
                if (EAGAIN != iErrno && EINTR != iErrno)    // 对非阻塞socket而言，EAGAIN不是一种错误;EINTR即errno为4，错误描述Interrupted system call，操作也应该继续。
                {
                    LOG4_TRACE("if (EAGAIN != iErrno && EINTR != iErrno)");
                    LOG4_ERROR("send to fd %d error %d: %s",
                        pData->iFd, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                    DestroyConnect(attr_iter);
                }
                else if (EAGAIN == iErrno)  // 内容未写完，添加或保持监听fd写事件
                {
                    attr_iter->second->dActiveTime = ev_now(m_loop);
                    AddIoWriteEvent(attr_iter);
                }
            }
            else if (iWriteLen > 0)
            {
                m_iSendByte += iWriteLen;
                attr_iter->second->dActiveTime = ev_now(m_loop);
                if (iWriteLen == iNeedWriteLen)  // 已无内容可写，取消监听fd写事件
                {
                    RemoveIoWriteEvent(attr_iter);
                }
                else    // 内容未写完，添加或保持监听fd写事件
                {
                    AddIoWriteEvent(attr_iter);
                }
            }
            else    // iWriteLen == 0 写缓冲区为空
            {
                //            LOG4_TRACE("pData->iFd %d, watcher->fd %d, iter->second->pWaitForSendBuff->ReadableBytes()=%d",
                //                            pData->iFd, watcher->fd, attr_iter->second->pWaitForSendBuff->ReadableBytes());
                if (attr_iter->second->pWaitForSendBuff->ReadableBytes() > 0)    // 存在等待发送的数据，说明本次写事件是connect之后的第一个写事件
                {
                    std::map<uint32, int>::iterator index_iter = m_mapSeq2WorkerIndex.find(attr_iter->second->ulSeq);
                    if (index_iter != m_mapSeq2WorkerIndex.end())   // 系统内部Server间通信需由OssManager转发
                    {
                        tagMsgShell stMsgShell;
                        stMsgShell.iFd = pData->iFd;
                        stMsgShell.ulSeq = attr_iter->second->ulSeq;
                        AddInnerFd(stMsgShell);
                        if (loss::CODEC_PROTOBUF == attr_iter->second->eCodecType)  // 系统内部Server间通信
                        {
                            m_pCmdConnect->Start(stMsgShell, index_iter->second);
                        }
                        else        // 与系统外部Server通信，连接成功后直接将数据发送
                        {
                            SendTo(stMsgShell);
                        }
                        m_mapSeq2WorkerIndex.erase(index_iter);
                        LOG4_TRACE("RemoveIoWriteEvent(%d)", pData->iFd);
                        RemoveIoWriteEvent(attr_iter);    // 在m_pCmdConnect的两个回调之后再把等待发送的数据发送出去
                    }
                    else // 与系统外部Server通信，连接成功后直接将数据发送
                    {
                        tagMsgShell stMsgShell;
                        stMsgShell.iFd = pData->iFd;
                        stMsgShell.ulSeq = attr_iter->second->ulSeq;
                        SendTo(stMsgShell);
                    }
                }
            }
            return(true);
        }
    }

    bool OssWorker::IoError(tagIoWatcherData* pData, struct ev_io* watcher)
    {
        //LOG4_TRACE("%s()", __FUNCTION__);
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(pData->iFd);
        if (iter == m_mapFdAttr.end())
        {
            return(false);
        }
        else
        {
            LOG4_TRACE("if (iter == m_mapFdAttr.end()) else");
            if (pData->ulSeq != iter->second->ulSeq)
            {
                LOG4_DEBUG("callback seq %lu not match the conn attr seq %lu",
                    pData->ulSeq, iter->second->ulSeq);
                ev_io_stop(m_loop, watcher);
                pData->pWorker = NULL;
                delete pData;
                watcher->data = NULL;
                delete watcher;
                watcher = NULL;
                return(false);
            }
            DestroyConnect(iter);
            return(true);
        }
    }

    bool OssWorker::IoTimeout(struct ev_timer* watcher, bool bCheckBeat)
    {
        bool bRes = false;
        tagIoWatcherData* pData = (tagIoWatcherData*)watcher->data;
        if (pData == NULL)
        {
            LOG4_ERROR("pData is null in %s()", __FUNCTION__);
            ev_timer_stop(m_loop, watcher);
            pData->pWorker = NULL;
            delete pData;
            watcher->data = NULL;
            delete watcher;
            watcher = NULL;
            return(false);
        }
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(pData->iFd);
        if (iter == m_mapFdAttr.end())
        {
            bRes = false;
        }
        else
        {
            if (iter->second->ulSeq != pData->ulSeq)      // 文件描述符数值相等，但已不是原来的文件描述符
            {
                bRes = false;
            }
            else
            {
                if (bCheckBeat && iter->second->dKeepAlive == 0)     // 需要发送心跳检查 或 完成心跳检查并未超时
                {
                    ev_tstamp after = iter->second->dActiveTime - ev_now(m_loop) + m_dIoTimeout;
                    if (after > 0)    // IO在定时时间内被重新刷新过，重新设置定时器
                    {
                        ev_timer_stop(m_loop, watcher);
                        ev_timer_set(watcher, after + ev_time() - ev_now(m_loop), 0);
                        ev_timer_start(m_loop, watcher);
                        return(true);
                    }
                    else    // IO已超时，发送心跳检查
                    {
                        tagMsgShell stMsgShell;
                        stMsgShell.iFd = pData->iFd;
                        stMsgShell.ulSeq = iter->second->ulSeq;
                        StepIoTimeout* pStepIoTimeout = new StepIoTimeout(stMsgShell, watcher);
                        if (pStepIoTimeout != NULL)
                        {
                            if (RegisterCallback(pStepIoTimeout))
                            {
                                oss::E_CMD_STATUS eStatus = pStepIoTimeout->Emit(ERR_OK);
                                if (STATUS_CMD_RUNNING != eStatus)
                                {
                                    // pStepIoTimeout->Start()会发送心跳包，若干返回非running状态，则表明发包时已出错，
                                    // 销毁连接过程在SentTo里已经完成，这里不需要再销毁连接
                                    DeleteCallback(pStepIoTimeout);
                                }
                                else
                                {
                                    return(true);
                                }
                            }
                            else
                            {
                                LOG4_TRACE("if (RegisterCallback(pStepIoTimeout)) else");
                                delete pStepIoTimeout;
                                pStepIoTimeout = NULL;
                                DestroyConnect(iter);
                            }
                        }
                    }
                }
                else        // 心跳检查过后的超时已是实际超时，关闭文件描述符并清理相关资源
                {
                    if (iter->second->dKeepAlive > 0)
                    {
                        ev_tstamp after = iter->second->dActiveTime - ev_now(m_loop) + iter->second->dKeepAlive;
                        if (after > 0)    // IO在定时时间内被重新刷新过，重新设置定时器
                        {
                            ev_timer_stop(m_loop, watcher);
                            ev_timer_set(watcher, after + ev_time() - ev_now(m_loop), 0);
                            ev_timer_start(m_loop, watcher);
                            return(true);
                        }
                    }
                    std::map<int, uint32>::iterator inner_iter = m_mapInnerFd.find(pData->iFd);
                    if (inner_iter == m_mapInnerFd.end())   // 非内部服务器间的连接才会在超时中关闭
                    {
                        LOG4_TRACE("io timeout and send beat, but there is no beat response received!");
                        DestroyConnect(iter);
                    }
                }
                bRes = true;
            }
        }

        ev_timer_stop(m_loop, watcher);
        pData->pWorker = NULL;
        delete pData;
        watcher->data = NULL;
        delete watcher;
        watcher = NULL;
        return(bRes);
    }

    bool OssWorker::StepTimeout(Step* pStep, struct ev_timer* watcher)
    {
        ev_tstamp after = pStep->GetActiveTime() - ev_now(m_loop) + pStep->GetTimeout();
        if (after > 0)    // 在定时时间内被重新刷新过，重新设置定时器
        {
            ev_timer_stop(m_loop, watcher);
            ev_timer_set(watcher, after + ev_time() - ev_now(m_loop), 0);
            ev_timer_start(m_loop, watcher);
            return(true);
        }
        else    // 会话已超时
        {
            LOG4_TRACE("%s(seq %lu): active_time %lf, now_time %lf, lifetime %lf",
                __FUNCTION__, pStep->GetSequence(), pStep->GetActiveTime(), ev_now(m_loop), pStep->GetTimeout());
            E_CMD_STATUS eResult = pStep->Timeout();
            if (STATUS_CMD_RUNNING == eResult)      // 超时Timeout()里重新执行Start()，重新设置定时器
            {
                ev_tstamp after = pStep->GetTimeout();
                ev_timer_stop(m_loop, watcher);
                ev_timer_set(watcher, after + ev_time() - ev_now(m_loop), 0);
                ev_timer_start(m_loop, watcher);
                return(true);
            }
            else
            {
                DeleteCallback(pStep);
                return(true);
            }
        }
    }

    bool OssWorker::SessionTimeout(Session* pSession, struct ev_timer* watcher)
    {
        ev_tstamp after = pSession->GetActiveTime() - ev_now(m_loop) + pSession->GetTimeout();
        if (after > 0)    // 定时时间内被重新刷新过，重新设置定时器
        {
            ev_timer_stop(m_loop, watcher);
            ev_timer_set(watcher, after + ev_time() - ev_now(m_loop), 0);
            ev_timer_start(m_loop, watcher);
            return(true);
        }
        else    // 会话已超时
        {
            LOG4_TRACE("%s(session_name: %s,  session_id: %s)",
                __FUNCTION__, pSession->GetSessionClass().c_str(), pSession->GetSessionId().c_str());
            if (STATUS_CMD_RUNNING == pSession->Timeout())   // 定时器时间到，执行定时操作，session时间刷新
            {
                ev_timer_stop(m_loop, watcher);
                ev_timer_set(watcher, pSession->GetTimeout() + ev_time() - ev_now(m_loop), 0);
                ev_timer_start(m_loop, watcher);
                return(true);
            }
            else
            {
                DeleteCallback(pSession);
                return(true);
            }
        }
    }

    bool OssWorker::RedisConnect(const redisAsyncContext *c, int status)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::map<redisAsyncContext*, tagRedisAttr*>::iterator attr_iter = m_mapRedisAttr.find((redisAsyncContext*)c);
        if (attr_iter != m_mapRedisAttr.end())
        {
            if (status == REDIS_OK)
            {
                attr_iter->second->bIsReady = true;
                int iCmdStatus;
                for (std::list<RedisStep*>::iterator step_iter = attr_iter->second->listWaitData.begin();
                    step_iter != attr_iter->second->listWaitData.end();)
                {
                    RedisStep* pRedisStep = (RedisStep*)(*step_iter);
                    size_t args_size = pRedisStep->GetRedisCmd()->GetCmdArguments().size() + 1;
                    const char* argv[args_size];
                    size_t arglen[args_size];
                    argv[0] = pRedisStep->GetRedisCmd()->GetCmd().c_str();
                    arglen[0] = pRedisStep->GetRedisCmd()->GetCmd().size();
                    std::vector<std::pair<std::string, bool> >::const_iterator c_iter = pRedisStep->GetRedisCmd()->GetCmdArguments().begin();
                    for (size_t i = 1; c_iter != pRedisStep->GetRedisCmd()->GetCmdArguments().end(); ++c_iter, ++i)
                    {
                        argv[i] = c_iter->first.c_str();
                        arglen[i] = c_iter->first.size();
                    }
                    iCmdStatus = redisAsyncCommandArgv((redisAsyncContext*)c, RedisCmdCallback, NULL, args_size, argv, arglen);
                    if (iCmdStatus == REDIS_OK)
                    {
                        LOG4_DEBUG("succeed in sending redis cmd: %s", pRedisStep->GetRedisCmd()->ToString().c_str());
                        attr_iter->second->listData.push_back(pRedisStep);
                        attr_iter->second->listWaitData.erase(step_iter++);
                    }
                    else    // 命令执行失败，不再继续执行，等待下一次回调
                    {
                        break;
                    }
                }
            }
            else
            {
                for (std::list<RedisStep *>::iterator step_iter = attr_iter->second->listWaitData.begin();
                    step_iter != attr_iter->second->listWaitData.end(); ++step_iter)
                {
                    RedisStep* pRedisStep = (RedisStep*)(*step_iter);
                    if (STATUS_CMD_RUNNING != pRedisStep->Callback(c, status, NULL))
                    {
                        delete pRedisStep;
                    }
                }
                attr_iter->second->listWaitData.clear();
                delete attr_iter->second;
                attr_iter->second = NULL;
                DelRedisContextAddr(c);
                m_mapRedisAttr.erase(attr_iter);
            }
        }
        return(true);
    }

    bool OssWorker::RedisDisconnect(const redisAsyncContext *c, int status)
    {
        LOG4_DEBUG("%s()", __FUNCTION__);
        std::map<redisAsyncContext*, tagRedisAttr*>::iterator attr_iter = m_mapRedisAttr.find((redisAsyncContext*)c);
        if (attr_iter != m_mapRedisAttr.end())
        {
            for (std::list<RedisStep *>::iterator step_iter = attr_iter->second->listData.begin();
                step_iter != attr_iter->second->listData.end(); ++step_iter)
            {
                LOG4_ERROR("RedisDisconnect callback error %d of redis cmd: %s",
                    c->err, (*step_iter)->GetRedisCmd()->ToString().c_str());
                (*step_iter)->Callback(c, c->err, NULL);
                delete (*step_iter);
                (*step_iter) = NULL;
            }
            attr_iter->second->listData.clear();

            for (std::list<RedisStep *>::iterator step_iter = attr_iter->second->listWaitData.begin();
                step_iter != attr_iter->second->listWaitData.end(); ++step_iter)
            {
                LOG4_ERROR("RedisDisconnect callback error %d of redis cmd: %s",
                    c->err, (*step_iter)->GetRedisCmd()->ToString().c_str());
                (*step_iter)->Callback(c, c->err, NULL);
                delete (*step_iter);
                (*step_iter) = NULL;
            }
            attr_iter->second->listWaitData.clear();

            delete attr_iter->second;
            attr_iter->second = NULL;
            DelRedisContextAddr(c);
            m_mapRedisAttr.erase(attr_iter);
        }
        return(true);
    }

    bool OssWorker::RedisCmdResult(redisAsyncContext *c, void *reply, void *privdata)
    {
        LOG4_DEBUG("%s()", __FUNCTION__);
        std::map<redisAsyncContext*, tagRedisAttr*>::iterator attr_iter = m_mapRedisAttr.find((redisAsyncContext*)c);
        if (attr_iter != m_mapRedisAttr.end())
        {
            std::list<RedisStep*>::iterator step_iter = attr_iter->second->listData.begin();
            if (NULL == reply)
            {
                std::map<const redisAsyncContext*, std::string>::iterator identify_iter = m_mapContextIdentify.find(c);
                if (identify_iter != m_mapContextIdentify.end())
                {
                    LOG4_ERROR("redis %s error %d: %s", identify_iter->second.c_str(), c->err, c->errstr);
                }
                for (; step_iter != attr_iter->second->listData.end(); ++step_iter)
                {
                    LOG4_ERROR("callback error %d of redis cmd: %s", c->err, (*step_iter)->GetRedisCmd()->ToString().c_str());
                    (*step_iter)->Callback(c, c->err, (redisReply*)reply);
                    delete (*step_iter);
                    (*step_iter) = NULL;
                }
                attr_iter->second->listData.clear();

                delete attr_iter->second;
                attr_iter->second = NULL;
                DelRedisContextAddr(c);
                m_mapRedisAttr.erase(attr_iter);
            }
            else
            {
                if (step_iter != attr_iter->second->listData.end())
                {
                    LOG4_TRACE("callback of redis cmd: %s", (*step_iter)->GetRedisCmd()->ToString().c_str());
                    /** @note 注意，若Callback返回STATUS_CMD_RUNNING，框架不回收并且不再管理该RedisStep，该RedisStep后续必须重新RegisterCallback或由开发者自己回收 */
                    if (STATUS_CMD_RUNNING != (*step_iter)->Callback(c, REDIS_OK, (redisReply*)reply))
                    {
                        delete (*step_iter);
                        (*step_iter) = NULL;
                    }
                    attr_iter->second->listData.erase(step_iter);
                    //freeReplyObject(reply);
                }
                else
                {
                    LOG4_ERROR("no redis callback data found!");
                }
            }
        }
        return(true);
    }

    time_t OssWorker::GetNowTime() const
    {
        return((time_t)ev_now(m_loop));
    }

    bool OssWorker::Pretreat(Cmd* pCmd)
    {
        LOG4_TRACE("%s(Cmd*)", __FUNCTION__);
        if (pCmd == NULL)
        {
            return(false);
        }
        pCmd->SetLabor(this);
        pCmd->SetLogger(&m_oLogger);
        return(true);
    }

    bool OssWorker::Pretreat(Step* pStep)
    {
        LOG4_TRACE("%s(Step*)", __FUNCTION__);
        if (pStep == NULL)
        {
            return(false);
        }
        pStep->SetLabor(this);
        pStep->SetLogger(&m_oLogger);
        return(true);
    }

    bool OssWorker::Pretreat(Session* pSession)
    {
        LOG4_TRACE("%s(Session*)", __FUNCTION__);
        if (pSession == NULL)
        {
            return(false);
        }
        pSession->SetLabor(this);
        pSession->SetLogger(&m_oLogger);
        return(true);
    }

    bool OssWorker::RegisterCallback(Step* pStep, ev_tstamp dTimeout)
    {
        LOG4_TRACE("%s(Step* 0x%X, lifetime %lf)", __FUNCTION__, pStep, dTimeout);
        if (pStep == NULL)
        {
            return(false);
        }
        if (pStep->IsRegistered())  // 已注册过，不必重复注册，不过认为本次注册成功
        {
            return(true);
        }
        pStep->SetLabor(this);
        pStep->SetLogger(&m_oLogger);
        pStep->SetRegistered();
        pStep->SetActiveTime(ev_now(m_loop));

        std::pair<std::map<uint32, Step*>::iterator, bool> ret
            = m_mapCallbackStep.insert(std::pair<uint32, Step*>(pStep->GetSequence(), pStep));
        if (ret.second)
        {
            ev_timer* pTimeoutWatcher = (ev_timer*)malloc(sizeof(ev_timer));
            if (pTimeoutWatcher == NULL)
            {
                return(false);
            }
            if (0.0 == dTimeout)
            {
                pStep->SetTimeout(m_dStepTimeout);
            }
            else
            {
                pStep->SetTimeout(dTimeout);
            }
            ev_timer_init(pTimeoutWatcher, StepTimeoutCallback, pStep->GetTimeout() + ev_time() - ev_now(m_loop), 0.0);
            pStep->m_pTimeoutWatcher = pTimeoutWatcher;
            pTimeoutWatcher->data = (void*)pStep;
            ev_timer_start(m_loop, pTimeoutWatcher);
            LOG4_TRACE("Step(seq %u, active_time %lf, lifetime %lf) register successful.",
                pStep->GetSequence(), pStep->GetActiveTime(), pStep->GetTimeout());
        }
        return(ret.second);
    }

    bool OssWorker::RegisterCallback(uint32 uiSelfStepSeq, Step* pStep, ev_tstamp dTimeout)
    {
        LOG4_TRACE("%s(Step* 0x%X, lifetime %lf)", __FUNCTION__, pStep, dTimeout);
        if (pStep == NULL)
        {
            return(false);
        }
        std::map<uint32, Step*>::iterator callback_iter;
        std::set<uint32>::iterator next_step_seq_iter;
        if (pStep->IsRegistered())  // 已注册过，不必重复注册，不过认为本次注册成功
        {
            // 登记前置step
            next_step_seq_iter = pStep->m_setNextStepSeq.find(uiSelfStepSeq);
            if (next_step_seq_iter != pStep->m_setNextStepSeq.end())
            {
                callback_iter = m_mapCallbackStep.find(uiSelfStepSeq);
                if (callback_iter != m_mapCallbackStep.end())
                {
                    callback_iter->second->m_setPreStepSeq.insert(pStep->GetSequence());
                }
            }
            return(true);
        }
        pStep->SetLabor(this);
        pStep->SetLogger(&m_oLogger);
        pStep->SetRegistered();
        pStep->SetActiveTime(ev_now(m_loop));

        // 登记前置step
        next_step_seq_iter = pStep->m_setNextStepSeq.find(uiSelfStepSeq);
        if (next_step_seq_iter != pStep->m_setNextStepSeq.end())
        {
            callback_iter = m_mapCallbackStep.find(uiSelfStepSeq);
            if (callback_iter != m_mapCallbackStep.end())
            {
                callback_iter->second->m_setPreStepSeq.insert(pStep->GetSequence());
            }
        }

        std::pair<std::map<uint32, Step*>::iterator, bool> ret
            = m_mapCallbackStep.insert(std::pair<uint32, Step*>(pStep->GetSequence(), pStep));
        if (ret.second)
        {
            ev_timer* pTimeoutWatcher = (ev_timer*)malloc(sizeof(ev_timer));
            if (pTimeoutWatcher == NULL)
            {
                return(false);
            }
            if (0.0 == dTimeout)
            {
                pStep->SetTimeout(m_dStepTimeout);
            }
            else
            {
                pStep->SetTimeout(dTimeout);
            }
            ev_timer_init(pTimeoutWatcher, StepTimeoutCallback, pStep->GetTimeout() + ev_time() - ev_now(m_loop), 0.0);
            pStep->m_pTimeoutWatcher = pTimeoutWatcher;
            pTimeoutWatcher->data = (void*)pStep;
            ev_timer_start(m_loop, pTimeoutWatcher);
            LOG4_TRACE("Step(seq %u, active_time %lf, lifetime %lf) register successful.",
                pStep->GetSequence(), pStep->GetActiveTime(), pStep->GetTimeout());
        }
        return(ret.second);
    }

    void OssWorker::DeleteCallback(Step* pStep)
    {
        LOG4_TRACE("%s(Step* 0x%X)", __FUNCTION__, pStep);
        if (pStep == NULL)
        {
            return;
        }
        std::map<uint32, Step*>::iterator callback_iter;
        for (std::set<uint32>::iterator step_seq_iter = pStep->m_setPreStepSeq.begin();
            step_seq_iter != pStep->m_setPreStepSeq.end();)
        {
            callback_iter = m_mapCallbackStep.find(*step_seq_iter);
            if (callback_iter == m_mapCallbackStep.end())
            {
                pStep->m_setPreStepSeq.erase(step_seq_iter++);
            }
            else
            {
                LOG4_DEBUG("step %u had pre step %u running, delay delete callback.", pStep->GetSequence(), *step_seq_iter);
                pStep->DelayTimeout();
                return;
            }
        }
        if (pStep->m_pTimeoutWatcher != NULL)
        {
            ev_timer_stop(m_loop, pStep->m_pTimeoutWatcher);
        }
        callback_iter = m_mapCallbackStep.find(pStep->GetSequence());
        if (callback_iter != m_mapCallbackStep.end())
        {
            LOG4_TRACE("delete step(seq %u)", pStep->GetSequence());
            delete pStep;
            m_mapCallbackStep.erase(callback_iter);
        }
    }

    void OssWorker::DeleteCallback(uint32 uiSelfStepSeq, Step* pStep)
    {
        LOG4_TRACE("%s(self_seq[%u], Step* 0x%X)", __FUNCTION__, uiSelfStepSeq, pStep);
        if (pStep == NULL)
        {
            return;
        }
        std::map<uint32, Step*>::iterator callback_iter;
        for (std::set<uint32>::iterator step_seq_iter = pStep->m_setPreStepSeq.begin();
            step_seq_iter != pStep->m_setPreStepSeq.end();)
        {
            callback_iter = m_mapCallbackStep.find(*step_seq_iter);
            if (callback_iter == m_mapCallbackStep.end())
            {
                LOG4_TRACE("try to erase seq[%u] from pStep->m_setPreStepSeq", *step_seq_iter);
                pStep->m_setPreStepSeq.erase(step_seq_iter++);
            }
            else
            {
                if (*step_seq_iter != uiSelfStepSeq)
                {
                    LOG4_DEBUG("step[%u] try to delete step[%u], but step[%u] had pre step[%u] running, delay delete callback.",
                        uiSelfStepSeq, pStep->GetSequence(), pStep->GetSequence(), *step_seq_iter);
                    pStep->DelayTimeout();
                    return;
                }
                else
                {
                    step_seq_iter++;
                }
            }
        }
        if (pStep->m_pTimeoutWatcher != NULL)
        {
            ev_timer_stop(m_loop, pStep->m_pTimeoutWatcher);
        }
        callback_iter = m_mapCallbackStep.find(pStep->GetSequence());
        if (callback_iter != m_mapCallbackStep.end())
        {
            LOG4_TRACE("step[%u] try to delete step[%u]", uiSelfStepSeq, pStep->GetSequence());
            delete pStep;
            m_mapCallbackStep.erase(callback_iter);
        }
    }

    /*
    bool OssWorker::UnRegisterCallback(Step* pStep)
    {
    LOG4_TRACE("%s(Step* 0x%X)", __FUNCTION__, pStep);
    if (pStep == NULL)
    {
    return(false);
    }
    if (pStep->m_pTimeoutWatcher != NULL)
    {
    ev_timer_stop (m_loop, pStep->m_pTimeoutWatcher);
    }
    std::map<uint32, Step*>::iterator iter = m_mapCallbackStep.find(pStep->GetSequence());
    if (iter != m_mapCallbackStep.end())
    {
    LOG4_TRACE("unRigester step(seq %u)", pStep->GetSequence());
    pStep->UnsetRegistered();
    m_mapCallbackStep.erase(iter);
    }
    return(true);
    }
    */

    bool OssWorker::RegisterCallback(Session* pSession)
    {
        LOG4_TRACE("%s(Session* 0x%X, lifetime %lf)", __FUNCTION__, &pSession, pSession->GetTimeout());
        if (pSession == NULL)
        {
            return(false);
        }
        if (pSession->IsRegistered())  // 已注册过，不必重复注册，不过认为本次注册成功
        {
            return(true);
        }
        pSession->SetLabor(this);
        pSession->SetLogger(&m_oLogger);
        pSession->SetRegistered();
        pSession->SetActiveTime(ev_now(m_loop));

        std::pair<std::map<std::string, Session*>::iterator, bool> ret;
        std::map<std::string, std::map<std::string, Session*> >::iterator name_iter = m_mapCallbackSession.find(pSession->GetSessionClass());
        if (name_iter == m_mapCallbackSession.end())
        {
            std::map<std::string, Session*> mapSession;
            ret = mapSession.insert(std::pair<std::string, Session*>(pSession->GetSessionId(), pSession));
            m_mapCallbackSession.insert(std::pair<std::string, std::map<std::string, Session*> >(pSession->GetSessionClass(), mapSession));
        }
        else
        {
            std::map<std::string, Session*>::iterator id_iter = name_iter->second.find(pSession->GetSessionId());
            if (id_iter != name_iter->second.end())
            {
                LOG4_ERROR("session exist!(session_id %s[ %s ])", pSession->GetSessionClass().c_str(), pSession->GetSessionId().c_str());
            }
            ret = name_iter->second.insert(std::pair<std::string, Session*>(pSession->GetSessionId(), pSession));
        }
        if (ret.second)
        {
            if (pSession->GetTimeout() != 0)
            {
                ev_timer* pTimeoutWatcher = (ev_timer*)malloc(sizeof(ev_timer));
                if (pTimeoutWatcher == NULL)
                {
                    return(false);
                }
                ev_timer_init(pTimeoutWatcher, SessionTimeoutCallback, pSession->GetTimeout() + ev_time() - ev_now(m_loop), 0.0);
                pSession->m_pTimeoutWatcher = pTimeoutWatcher;
                pTimeoutWatcher->data = (void*)pSession;
                ev_timer_start(m_loop, pTimeoutWatcher);
            }
            LOG4_TRACE("Session(session_id %s) register successful.", pSession->GetSessionId().c_str());
        }
        return(ret.second);
    }

    void OssWorker::DeleteCallback(Session* pSession)
    {
        LOG4_TRACE("%s(Session* 0x%X)", __FUNCTION__, &pSession);
        if (pSession == NULL)
        {
            return;
        }
        if (pSession->m_pTimeoutWatcher != NULL)
        {
            ev_timer_stop(m_loop, pSession->m_pTimeoutWatcher);
        }
        std::map<std::string, std::map<std::string, Session*> >::iterator name_iter = m_mapCallbackSession.find(pSession->GetSessionClass());
        if (name_iter != m_mapCallbackSession.end())
        {
            std::map<std::string, Session*>::iterator id_iter = name_iter->second.find(pSession->GetSessionId());
            if (id_iter != name_iter->second.end())
            {
                LOG4_TRACE("delete session(session_id %s)", pSession->GetSessionId().c_str());
                delete id_iter->second;
                id_iter->second = NULL;
                name_iter->second.erase(id_iter);
            }
        }
    }

    bool OssWorker::RegisterCallback(const redisAsyncContext* pRedisContext, RedisStep* pRedisStep)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        if (pRedisStep == NULL)
        {
            LOG4_TRACE("%s() pRedisStep is NULL", __FUNCTION__);
            return(false);
        }
        //如果从连接不上，再连接主的时候STEP已经注册过了，不能往下走，改为允许重复注册
        /*
          if (pRedisStep->IsRegistered())  // 已注册过，不必重复注册，不过认为本次注册成功
          {
          LOG4_TRACE("%s() pRedisStep is IsRegistered", __FUNCTION__);
          return(true);
          }*/
        pRedisStep->SetLabor(this);
        pRedisStep->SetLogger(&m_oLogger);
        pRedisStep->SetRegistered();
        /* redis回调暂不作超时处理
        ev_timer* timeout_watcher = new ev_timer();
        if (timeout_watcher == NULL)
        {
        return(false);
        }
        tagIoWatcherData* pData = new tagIoWatcherData();
        if (pData == NULL)
        {
        LOG4_ERROR("new tagIoWatcherData error!");
        delete timeout_watcher;
        return(false);
        }
        ev_timer_init (timeout_watcher, IoTimeoutCallback, 0.5 + ev_time() - ev_now(m_loop), 0.0);
        pData->ullSeq = pStep->GetSequence();
        pData->pWorker = this;
        timeout_watcher->data = (void*)pData;
        ev_timer_start (m_loop, timeout_watcher);
        */

        std::map<redisAsyncContext*, tagRedisAttr*>::iterator iter = m_mapRedisAttr.find((redisAsyncContext*)pRedisContext);
        if (iter == m_mapRedisAttr.end())
        {
            LOG4_ERROR("redis attr not exist!");
            return(false);
        }
        else
        {
            LOG4_TRACE("iter->second->bIsReady = %d", iter->second->bIsReady);
            if (iter->second->bIsReady)
            {
                int status;
                size_t args_size = pRedisStep->GetRedisCmd()->GetCmdArguments().size() + 1;
                const char* argv[args_size];
                size_t arglen[args_size];
                argv[0] = pRedisStep->GetRedisCmd()->GetCmd().c_str();
                arglen[0] = pRedisStep->GetRedisCmd()->GetCmd().size();
                std::vector<std::pair<std::string, bool> >::const_iterator c_iter = pRedisStep->GetRedisCmd()->GetCmdArguments().begin();
                for (size_t i = 1; c_iter != pRedisStep->GetRedisCmd()->GetCmdArguments().end(); ++c_iter, ++i)
                {
                    argv[i] = c_iter->first.c_str();
                    arglen[i] = c_iter->first.size();
                }
                status = redisAsyncCommandArgv((redisAsyncContext*)pRedisContext, RedisCmdCallback, NULL, args_size, argv, arglen);
                if (status == REDIS_OK)
                {
                    LOG4_DEBUG("succeed in sending redis cmd: %s", pRedisStep->GetRedisCmd()->ToString().c_str());
                    iter->second->listData.push_back(pRedisStep);
                    return(true);
                }
                else
                {
                    LOG4_ERROR("redis status %d!", status);
                    return(false);
                }
            }
            else
            {
                LOG4_TRACE("listWaitData.push_back()");
                iter->second->listWaitData.push_back(pRedisStep);
                return(true);
            }
        }
    }

    bool OssWorker::ResetTimeout(Step* pStep, struct ev_timer* watcher)
    {
        ev_tstamp after = ev_now(m_loop) + pStep->GetTimeout();
        ev_timer_stop(m_loop, watcher);
        ev_timer_set(watcher, after + ev_time() - ev_now(m_loop), 0);
        ev_timer_start(m_loop, watcher);
        return(true);
    }

    Session* OssWorker::GetSession(uint32 uiSessionId, const std::string& strSessionClass)
    {
        std::map<std::string, std::map<std::string, Session*> >::iterator name_iter = m_mapCallbackSession.find(strSessionClass);
        if (name_iter == m_mapCallbackSession.end())
        {
            return(NULL);
        }
        else
        {
            char szSession[16] = { 0 };
            snprintf(szSession, sizeof(szSession), "%u", uiSessionId);
            std::map<std::string, Session*>::iterator id_iter = name_iter->second.find(szSession);
            if (id_iter == name_iter->second.end())
            {
                return(NULL);
            }
            else
            {
                id_iter->second->SetActiveTime(ev_now(m_loop));
                return(id_iter->second);
            }
        }
    }

    Session* OssWorker::GetSession(const std::string& strSessionId, const std::string& strSessionClass)
    {
        std::map<std::string, std::map<std::string, Session*> >::iterator name_iter = m_mapCallbackSession.find(strSessionClass);
        if (name_iter == m_mapCallbackSession.end())
        {
            return(NULL);
        }
        else
        {
            std::map<std::string, Session*>::iterator id_iter = name_iter->second.find(strSessionId);
            if (id_iter == name_iter->second.end())
            {
                return(NULL);
            }
            else
            {
                id_iter->second->SetActiveTime(ev_now(m_loop));
                return(id_iter->second);
            }
        }
    }

    //bool OssWorker::RegisterCallback(Session* pSession)
    //{
    //    if (pSession == NULL)
    //    {
    //        return(false);
    //    }
    //    pSession->SetWorker(this);
    //    pSession->SetLogger(m_oLogger);
    //    pSession->SetRegistered();
    //    ev_timer* timeout_watcher = new ev_timer();
    //    if (timeout_watcher == NULL)
    //    {
    //        return(false);
    //    }
    //    uint32* pUlSeq = new uint32;
    //    if (pUlSeq == NULL)
    //    {
    //        delete timeout_watcher;
    //        return(false);
    //    }
    //    ev_timer_init (timeout_watcher, StepTimeoutCallback, 60 + ev_time() - ev_now(m_loop), 0.);
    //    *pUllSeq = pSession->GetSequence();
    //    timeout_watcher->data = (void*)pUllSeq;
    //    ev_timer_start (m_loop, timeout_watcher);
    //
    //    std::pair<std::map<uint32, Session*>::iterator, bool> ret
    //        = m_mapCallbackSession.insert(std::pair<uint32, Session*>(pSession->GetSequence(), pSession));
    //    return(ret.second);
    //}
    //
    //void OssWorker::DeleteCallback(Session* pSession)
    //{
    //    if (pSession == NULL)
    //    {
    //        return;
    //    }
    //    std::map<uint32, Session*>::iterator iter = m_mapCallbackSession.find(pSession->GetSequence());
    //    if (iter != m_mapCallbackSession.end())
    //    {
    //        delete iter->second;
    //        iter->second = NULL;
    //        m_mapCallbackSession.erase(iter);
    //    }
    //}

    bool OssWorker::Disconnect(const tagMsgShell& stMsgShell, bool bMsgShellNotice)
    {
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(stMsgShell.iFd);
        if (iter != m_mapFdAttr.end())
        {
            if (iter->second->ulSeq == stMsgShell.ulSeq)
            {
                LOG4_TRACE("if (iter->second->ulSeq == stMsgShell.ulSeq)");
                return(DestroyConnect(iter, bMsgShellNotice));
            }
        }
        return(false);
    }

    bool OssWorker::Disconnect(const std::string& strIdentify, bool bMsgShellNotice)
    {
        std::map<std::string, tagMsgShell>::iterator shell_iter = m_mapMsgShell.find(strIdentify);
        if (shell_iter == m_mapMsgShell.end())
        {
            return(true);
        }
        else
        {
            return(Disconnect(shell_iter->second, bMsgShellNotice));
        }
    }

    bool OssWorker::SetProcessName(const loss::CJsonObject& oJsonConf)
    {
        char szProcessName[64] = { 0 };
        snprintf(szProcessName, sizeof(szProcessName), "%s_W%d", oJsonConf("server_name").c_str(), m_iWorkerIndex);
        ngx_setproctitle(szProcessName);
        return(true);
    }

    bool OssWorker::Init(loss::CJsonObject& oJsonConf)
    {
        char szProcessName[64] = { 0 };
        snprintf(szProcessName, sizeof(szProcessName), "%s_W%d", oJsonConf("server_name").c_str(), m_iWorkerIndex);
        ngx_setproctitle(szProcessName);
        oJsonConf.Get("io_timeout", m_dIoTimeout);
        if (!oJsonConf.Get("step_timeout", m_dStepTimeout))
        {
            m_dStepTimeout = 0.5;
        }
        oJsonConf.Get("node_type", m_strNodeType);
        oJsonConf.Get("inner_host", m_strHostForServer);
        oJsonConf.Get("inner_port", m_iPortForServer);
        m_oCustomConf = oJsonConf["custom"];
#ifdef NODE_TYPE_ACCESS
        oJsonConf["permission"]["uin_permit"].Get("stat_interval", m_dMsgStatInterval);
        oJsonConf["permission"]["uin_permit"].Get("permit_num", m_iMsgPermitNum);
#endif
        if (!InitLogger(oJsonConf))
        {
            return(false);
        }
        StarshipCodec* pCodec = new ProtoCodec(loss::CODEC_PROTOBUF);
        pCodec->SetLogger(m_oLogger);
        m_mapCodec.insert(std::pair<loss::E_CODEC_TYPE, StarshipCodec*>(loss::CODEC_PROTOBUF, pCodec));
        pCodec = new HttpCodec(loss::CODEC_HTTP);
        pCodec->SetLogger(m_oLogger);
        m_mapCodec.insert(std::pair<loss::E_CODEC_TYPE, StarshipCodec*>(loss::CODEC_HTTP, pCodec));
        pCodec = new ClientMsgCodec(loss::CODEC_PRIVATE);
        pCodec->SetLogger(m_oLogger);
        m_mapCodec.insert(std::pair<loss::E_CODEC_TYPE, StarshipCodec*>(loss::CODEC_PRIVATE, pCodec));
        pCodec = new CodecWsExtentJson(loss::CODEC_WEBSOCKET_JSON);
        pCodec->SetLogger(m_oLogger);
        m_mapCodec.insert(std::pair<loss::E_CODEC_TYPE, StarshipCodec*>(loss::CODEC_WEBSOCKET_JSON, pCodec));
        m_pCmdConnect = new CmdConnectWorker();
        if (m_pCmdConnect == NULL)
        {
            return(false);
        }
        m_pCmdConnect->SetLogger(&m_oLogger);
        m_pCmdConnect->SetLabor(this);
        return(true);
    }

    bool OssWorker::InitLogger(const loss::CJsonObject& oJsonConf)
    {
        if (m_bInitLogger)  // 已经被初始化过，只修改日志级别
        {
            int32 iLogLevel = 0;
            oJsonConf.Get("log_level", iLogLevel);
            m_oLogger.setLogLevel(iLogLevel);
            return(true);
        }
        else
        {
            int32 iMaxLogFileSize = 0;
            int32 iMaxLogFileNum = 0;
            int32 iLogLevel = 0;
            int32 iLoggingPort = 9000;
            std::string strLoggingHost;
            std::string strLogname = m_strWorkPath + std::string("/") + oJsonConf("log_path")
                + std::string("/") + getproctitle() + std::string(".log");
            std::string strParttern = "[%D,%d{%q}][%p] [%l] %m%n";
            std::ostringstream ssServerName;
            ssServerName << getproctitle() << " " << GetWorkerIdentify();
            oJsonConf.Get("max_log_file_size", iMaxLogFileSize);
            oJsonConf.Get("max_log_file_num", iMaxLogFileNum);
            if (oJsonConf.Get("log_level", iLogLevel))
            {
                switch (iLogLevel)
                {
                case log4cplus::DEBUG_LOG_LEVEL:
                    break;
                case log4cplus::INFO_LOG_LEVEL:
                    break;
                case log4cplus::TRACE_LOG_LEVEL:
                    break;
                case log4cplus::WARN_LOG_LEVEL:
                    break;
                case log4cplus::ERROR_LOG_LEVEL:
                    break;
                case log4cplus::FATAL_LOG_LEVEL:
                    break;
                default:
                    iLogLevel = log4cplus::INFO_LOG_LEVEL;
                }
            }
            else
            {
                iLogLevel = log4cplus::INFO_LOG_LEVEL;
            }
            log4cplus::initialize();
            std::auto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout(strParttern));
            log4cplus::SharedAppenderPtr append(new log4cplus::RollingFileAppender(
                strLogname, iMaxLogFileSize, iMaxLogFileNum));
            append->setName(strLogname);
            append->setLayout(layout);
            m_oLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(strLogname));
            m_oLogger.setLogLevel(iLogLevel);
            m_oLogger.addAppender(append);
            if (oJsonConf.Get("socket_logging_host", strLoggingHost) && oJsonConf.Get("socket_logging_port", iLoggingPort))
            {
                log4cplus::SharedAppenderPtr socket_append(new log4cplus::SocketAppender(
                    strLoggingHost, iLoggingPort, ssServerName.str()));
                socket_append->setName(ssServerName.str());
                socket_append->setLayout(layout);
                socket_append->setThreshold(log4cplus::TRACE_LOG_LEVEL);
                m_oLogger.addAppender(socket_append);
            }
            LOG4_INFO("%s program begin...", getproctitle());
            m_bInitLogger = true;
            return(true);
        }
    }

    bool OssWorker::CreateEvents()
    {
        m_loop = ev_loop_new(EVFLAG_AUTO);
        if (m_loop == NULL)
        {
            return(false);
        }

        signal(SIGPIPE, SIG_IGN);
        // 注册信号事件
        ev_signal* signal_watcher = new ev_signal();
        ev_signal_init(signal_watcher, TerminatedCallback, SIGINT);
        signal_watcher->data = (void*)this;
        ev_signal_start(m_loop, signal_watcher);

        AddPeriodicTaskEvent();
        // 注册闲时处理事件         注册idle事件在Server空闲时会导致CPU占用过高，暂时弃用之，改用定时器实现
        //    ev_idle* idle_watcher = new ev_idle();
        //    ev_idle_init (idle_watcher, IdleCallback);
        //    idle_watcher->data = (void*)this;
        //    ev_idle_start (m_loop, idle_watcher);

        // 注册网络IO事件
        uint32 ulSeq = GetSequence();
        if (CreateFdAttr(m_iManagerControlFd, ulSeq))
        {
            tagMsgShell stMsgShell;
            stMsgShell.iFd = m_iManagerControlFd;
            stMsgShell.ulSeq = ulSeq;
            std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(m_iManagerControlFd);
            if (!AddIoReadEvent(iter))
            {
                LOG4_TRACE("if (!AddIoReadEvent(conn_iter))");
                DestroyConnect(iter);
                return(false);
            }
            AddInnerFd(stMsgShell);
            //        if (!AddIoErrorEvent(m_iManagerControlFd))
            //        {
            //            DestroyConnect(iter);
            //            return(false);
            //        }
        }
        else
        {
            return(false);
        }

        // 注册网络IO事件
        ulSeq = GetSequence();
        if (CreateFdAttr(m_iManagerDataFd, ulSeq))
        {
            tagMsgShell stMsgShell;
            stMsgShell.iFd = m_iManagerDataFd;
            stMsgShell.ulSeq = ulSeq;
            std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(m_iManagerDataFd);
            if (!AddIoReadEvent(iter))
            {
                LOG4_TRACE("if (!AddIoReadEvent(conn_iter))");
                DestroyConnect(iter);
                return(false);
            }
            AddInnerFd(stMsgShell);
            //        if (!AddIoErrorEvent(m_iManagerDataFd))
            //        {
            //            DestroyConnect(iter);
            //            return(false);
            //        }
        }
        else
        {
            return(false);
        }
        return(true);
    }

    void OssWorker::PreloadCmd()
    {
        Cmd* pCmdToldWorker = new CmdToldWorker();
        pCmdToldWorker->SetCmd(CMD_REQ_TELL_WORKER);
        pCmdToldWorker->SetLogger(&m_oLogger);
        pCmdToldWorker->SetLabor(this);
        m_mapCmd.insert(std::pair<int32, Cmd*>(pCmdToldWorker->GetCmd(), pCmdToldWorker));

        Cmd* pCmdUpdateNodeId = new CmdUpdateNodeId();
        pCmdUpdateNodeId->SetCmd(CMD_REQ_REFRESH_NODE_ID);
        pCmdUpdateNodeId->SetLogger(&m_oLogger);
        pCmdUpdateNodeId->SetLabor(this);
        m_mapCmd.insert(std::pair<int32, Cmd*>(pCmdUpdateNodeId->GetCmd(), pCmdUpdateNodeId));

        Cmd* pCmdNodeNotice = new CmdNodeNotice();
        pCmdNodeNotice->SetCmd(CMD_REQ_NODE_REG_NOTICE);
        pCmdNodeNotice->SetLogger(&m_oLogger);
        pCmdNodeNotice->SetLabor(this);
        m_mapCmd.insert(std::pair<int32, Cmd*>(pCmdNodeNotice->GetCmd(), pCmdNodeNotice));

        Cmd* pCmdBeat = new CmdBeat();
        pCmdBeat->SetCmd(CMD_REQ_BEAT);
        pCmdBeat->SetLogger(&m_oLogger);
        pCmdBeat->SetLabor(this);
        m_mapCmd.insert(std::pair<int32, Cmd*>(pCmdBeat->GetCmd(), pCmdBeat));

        Cmd* pCmdUpdateConfig = new CmdUpdateConfig();
        pCmdUpdateConfig->SetCmd(CMD_REQ_SERVER_CONFIG);
        pCmdUpdateConfig->SetLogger(&m_oLogger);
        pCmdUpdateConfig->SetLabor(this);
        pCmdUpdateConfig->SetConfigPath(m_strWorkPath);
        m_mapCmd.insert(std::pair<int32, Cmd*>(pCmdUpdateConfig->GetCmd(), pCmdUpdateConfig));
    }

    void OssWorker::Destroy()
    {
        LOG4_TRACE("%s()", __FUNCTION__);

        m_mapHttpAttr.clear();

        for (std::map<int32, Cmd*>::iterator cmd_iter = m_mapCmd.begin();
            cmd_iter != m_mapCmd.end(); ++cmd_iter)
        {
            delete cmd_iter->second;
            cmd_iter->second = NULL;
        }
        m_mapCmd.clear();

        for (std::map<int, tagSo*>::iterator so_iter = m_mapSo.begin();
            so_iter != m_mapSo.end(); ++so_iter)
        {
            delete so_iter->second;
            so_iter->second = NULL;
        }
        m_mapSo.clear();

        for (std::map<std::string, tagModule*>::iterator module_iter = m_mapModule.begin();
            module_iter != m_mapModule.end(); ++module_iter)
        {
            delete module_iter->second;
            module_iter->second = NULL;
        }
        m_mapModule.clear();

        for (std::map<int, tagConnectionAttr*>::iterator attr_iter = m_mapFdAttr.begin();
            attr_iter != m_mapFdAttr.end(); ++attr_iter)
        {
            LOG4_TRACE("for (std::map<int, tagConnectionAttr*>::iterator attr_iter = m_mapFdAttr.begin();");
            DestroyConnect(attr_iter);
        }

        for (std::map<loss::E_CODEC_TYPE, StarshipCodec*>::iterator codec_iter = m_mapCodec.begin();
            codec_iter != m_mapCodec.end(); ++codec_iter)
        {
            delete codec_iter->second;
            codec_iter->second = NULL;
        }
        m_mapCodec.clear();

        if (m_loop != NULL)
        {
            ev_loop_destroy(m_loop);
            m_loop = NULL;
        }

        if (m_pErrBuff != NULL)
        {
            delete[] m_pErrBuff;
            m_pErrBuff = NULL;
        }
    }

    void OssWorker::ResetLogLevel(log4cplus::LogLevel iLogLevel)
    {
        m_oLogger.setLogLevel(iLogLevel);
    }

    bool OssWorker::AddMsgShell(const std::string& strIdentify, const tagMsgShell& stMsgShell)
    {
        LOG4_TRACE("%s(%s, fd %d, seq %u)", __FUNCTION__, strIdentify.c_str(), stMsgShell.iFd, stMsgShell.ulSeq);
        std::map<std::string, tagMsgShell>::iterator shell_iter = m_mapMsgShell.find(strIdentify);
        if (shell_iter == m_mapMsgShell.end())
        {
            m_mapMsgShell.insert(std::pair<std::string, tagMsgShell>(strIdentify, stMsgShell));
        }
        else
        {
            if ((stMsgShell.iFd != shell_iter->second.iFd || stMsgShell.ulSeq != shell_iter->second.ulSeq))
            {
                LOG4_DEBUG("connect to %s was exist, replace old fd %d with new fd %d",
                    strIdentify.c_str(), shell_iter->second.iFd, stMsgShell.iFd);
                std::map<int, tagConnectionAttr*>::iterator fd_iter = m_mapFdAttr.find(shell_iter->second.iFd);
                if (GetWorkerIdentify() != strIdentify)
                {
                    LOG4_TRACE("GetWorkerIdentify() != strIdentify");
                    DestroyConnect(fd_iter);
                }
                shell_iter->second = stMsgShell;
            }
        }
        SetConnectIdentify(stMsgShell, strIdentify);
        return(true);
    }

    void OssWorker::DelMsgShell(const std::string& strIdentify)
    {
        std::map<std::string, tagMsgShell>::iterator shell_iter = m_mapMsgShell.find(strIdentify);
        if (shell_iter == m_mapMsgShell.end())
        {
            ;
        }
        else
        {
            m_mapMsgShell.erase(shell_iter);
        }

        // 连接虽然断开，但不应清除节点标识符，这样可以保证下次有数据发送时可以重新建立连接
        //    std::map<std::string, std::string>::iterator identify_iter = m_mapIdentifyNodeType.find(strIdentify);
        //    if (identify_iter != m_mapIdentifyNodeType.end())
        //    {
        //        std::map<std::string, std::pair<std::set<std::string>::iterator, std::set<std::string> > >::iterator node_type_iter;
        //        node_type_iter = m_mapNodeIdentify.find(identify_iter->second);
        //        if (node_type_iter != m_mapNodeIdentify.end())
        //        {
        //            std::set<std::string>::iterator id_iter = node_type_iter->second.second.find(strIdentify);
        //            if (id_iter != node_type_iter->second.second.end())
        //            {
        //                node_type_iter->second.second.erase(id_iter);
        //                node_type_iter->second.first = node_type_iter->second.second.begin();
        //            }
        //        }
        //        m_mapIdentifyNodeType.erase(identify_iter);
        //    }
    }

    void OssWorker::AddNodeIdentify(const std::string& strNodeType, const std::string& strIdentify)
    {
        LOG4_TRACE("%s(%s, %s)", __FUNCTION__, strNodeType.c_str(), strIdentify.c_str());
        std::map<std::string, std::string>::iterator iter = m_mapIdentifyNodeType.find(strIdentify);
        LOG4_TRACE("m_mapIdentifyNode size(): %d; m_mapNodeIdentify  size: %d!", m_mapIdentifyNodeType.size(), m_mapIdentifyNodeType.size());
        if (iter == m_mapIdentifyNodeType.end())
        {
            m_mapIdentifyNodeType.insert(iter,
                std::pair<std::string, std::string>(strIdentify, strNodeType));

            T_MAP_NODE_TYPE_IDENTIFY::iterator node_type_iter;
            node_type_iter = m_mapNodeIdentify.find(strNodeType);
            if (node_type_iter == m_mapNodeIdentify.end())
            {
                std::set<std::string, std::less<std::string> > setIdentify;
                setIdentify.insert(strIdentify);
                std::pair<T_MAP_NODE_TYPE_IDENTIFY::iterator, bool> insert_node_result;
                insert_node_result = m_mapNodeIdentify.insert(std::pair< std::string,
                    std::pair<std::set<std::string>::iterator, std::set<std::string, std::less<std::string> > > >(
                    strNodeType, std::make_pair(setIdentify.begin(), setIdentify)));    //这里的setIdentify是临时变量，setIdentify.begin()将会成非法地址
                if (insert_node_result.second == false)
                {
                    return;
                }
                insert_node_result.first->second.first = insert_node_result.first->second.second.begin();
            }
            else
            {
                std::set<std::string>::iterator id_iter = node_type_iter->second.second.find(strIdentify);
                if (id_iter == node_type_iter->second.second.end())
                {
                    node_type_iter->second.second.insert(strIdentify);
                    node_type_iter->second.first = node_type_iter->second.second.begin();
                }
            }
        }
    }

    void OssWorker::DelNodeIdentify(const std::string& strNodeType, const std::string& strIdentify)
    {
        LOG4_TRACE("%s(%s, %s)", __FUNCTION__, strNodeType.c_str(), strIdentify.c_str());
        std::map<std::string, std::string>::iterator identify_iter = m_mapIdentifyNodeType.find(strIdentify);
        if (identify_iter != m_mapIdentifyNodeType.end())
        {
            std::map<std::string, std::pair<std::set<std::string>::iterator, std::set<std::string> > >::iterator node_type_iter;
            node_type_iter = m_mapNodeIdentify.find(identify_iter->second);
            if (node_type_iter != m_mapNodeIdentify.end())
            {
                std::set<std::string>::iterator id_iter = node_type_iter->second.second.find(strIdentify);
                if (id_iter != node_type_iter->second.second.end())
                {
                    node_type_iter->second.second.erase(id_iter);
                    node_type_iter->second.first = node_type_iter->second.second.begin();
                }
            }
            m_mapIdentifyNodeType.erase(identify_iter);
        }
    }

    bool OssWorker::RegisterCallback(const std::string& strIdentify, RedisStep* pRedisStep)
    {
        LOG4_TRACE("%s(%s)", __FUNCTION__, strIdentify.c_str());
        int iPosIpPortSeparator = strIdentify.find(':');
        if (iPosIpPortSeparator == std::string::npos)
        {
            return(false);
        }
        std::string strHost = strIdentify.substr(0, iPosIpPortSeparator);
        std::string strPort = strIdentify.substr(iPosIpPortSeparator + 1, std::string::npos);
        int iPort = atoi(strPort.c_str());
        if (iPort == 0)
        {
            return(false);
        }
        std::map<std::string, const redisAsyncContext*>::iterator ctx_iter = m_mapRedisContext.find(strIdentify);
        if (ctx_iter != m_mapRedisContext.end())
        {
            LOG4_DEBUG("redis context %s", strIdentify.c_str());
            return(RegisterCallback(ctx_iter->second, pRedisStep));
        }
        else
        {
            LOG4_DEBUG("GetLabor()->AutoRedisCmd(%s, %d)", strHost.c_str(), iPort);
            return(AutoRedisCmd(strHost, iPort, pRedisStep));
        }
    }

    bool OssWorker::RegisterCallback(const std::string& strHost, int iPort, RedisStep* pRedisStep)
    {
        LOG4_TRACE("%s(%s, %d)", __FUNCTION__, strHost.c_str(), iPort);
        char szIdentify[32] = { 0 };
        snprintf(szIdentify, sizeof(szIdentify), "%s:%d", strHost.c_str(), iPort);
        std::map<std::string, const redisAsyncContext*>::iterator ctx_iter = m_mapRedisContext.find(szIdentify);
        if (ctx_iter != m_mapRedisContext.end())
        {
            LOG4_TRACE("redis context %s", szIdentify);
            return(RegisterCallback(ctx_iter->second, pRedisStep));
        }
        else
        {
            LOG4_TRACE("GetLabor()->AutoRedisCmd(%s, %d)", strHost.c_str(), iPort);
            return(AutoRedisCmd(strHost, iPort, pRedisStep));
        }
    }

    bool OssWorker::AddRedisContextAddr(const std::string& strHost, int iPort, redisAsyncContext* ctx)
    {
        LOG4_TRACE("%s(%s, %d, 0x%X)", __FUNCTION__, strHost.c_str(), iPort, ctx);
        char szIdentify[32] = { 0 };
        snprintf(szIdentify, 32, "%s:%d", strHost.c_str(), iPort);
        std::map<std::string, const redisAsyncContext*>::iterator ctx_iter = m_mapRedisContext.find(szIdentify);
        if (ctx_iter == m_mapRedisContext.end())
        {
            m_mapRedisContext.insert(std::pair<std::string, const redisAsyncContext*>(szIdentify, ctx));
            std::map<const redisAsyncContext*, std::string>::iterator identify_iter = m_mapContextIdentify.find(ctx);
            if (identify_iter == m_mapContextIdentify.end())
            {
                m_mapContextIdentify.insert(std::pair<const redisAsyncContext*, std::string>(ctx, szIdentify));
            }
            else
            {
                identify_iter->second = szIdentify;
            }
            return(true);
        }
        else
        {
            return(false);
        }
    }

    void OssWorker::DelRedisContextAddr(const redisAsyncContext* ctx)
    {
        std::map<const redisAsyncContext*, std::string>::iterator identify_iter = m_mapContextIdentify.find(ctx);
        if (identify_iter != m_mapContextIdentify.end())
        {
            std::map<std::string, const redisAsyncContext*>::iterator ctx_iter = m_mapRedisContext.find(identify_iter->second);
            if (ctx_iter != m_mapRedisContext.end())
            {
                m_mapRedisContext.erase(ctx_iter);
            }
            m_mapContextIdentify.erase(identify_iter);
        }
    }

    bool OssWorker::SendTo(const tagMsgShell& stMsgShell)
    {
        LOG4_TRACE("%s(fd %d, seq %lu) pWaitForSendBuff", __FUNCTION__, stMsgShell.iFd, stMsgShell.ulSeq);
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(stMsgShell.iFd);
        if (iter == m_mapFdAttr.end())
        {
            LOG4_ERROR("no fd %d found in m_mapFdAttr", stMsgShell.iFd);
            return(false);
        }
        else
        {
            if (iter->second->ulSeq == stMsgShell.ulSeq)
            {
                int iErrno = 0;
                int iWriteLen = 0;
                int iNeedWriteLen = (int)(iter->second->pWaitForSendBuff->ReadableBytes());
                int iWriteIdx = iter->second->pSendBuff->GetWriteIndex();
                iWriteLen = iter->second->pSendBuff->Write(iter->second->pWaitForSendBuff, iter->second->pWaitForSendBuff->ReadableBytes());
                if (iWriteLen == iNeedWriteLen)
                {
                    iNeedWriteLen = (int)iter->second->pSendBuff->ReadableBytes();
                    iWriteLen = iter->second->pSendBuff->WriteFD(stMsgShell.iFd, iErrno);
                    iter->second->pSendBuff->Compact(8192);
                    if (iWriteLen < 0)
                    {
                        if (EAGAIN != iErrno && EINTR != iErrno)    // 对非阻塞socket而言，EAGAIN不是一种错误;EINTR即errno为4，错误描述Interrupted system call，操作也应该继续。
                        {
                            LOG4_ERROR("send to fd %d error %d: %s",
                                stMsgShell.iFd, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                            DestroyConnect(iter);
                            return(false);
                        }
                        else if (EAGAIN == iErrno)  // 内容未写完，添加或保持监听fd写事件
                        {
                            iter->second->dActiveTime = ev_now(m_loop);
                            AddIoWriteEvent(iter);
                        }
                    }
                    else if (iWriteLen > 0)
                    {
                        m_iSendByte += iWriteLen;
                        iter->second->dActiveTime = ev_now(m_loop);
                        if (iWriteLen == iNeedWriteLen)  // 已无内容可写，取消监听fd写事件
                        {
                            RemoveIoWriteEvent(iter);
                        }
                        else    // 内容未写完，添加或保持监听fd写事件
                        {
                            AddIoWriteEvent(iter);
                        }
                    }
                    return(true);
                }
                else
                {
                    LOG4_ERROR("write to send buff error, iWriteLen = %d!", iWriteLen);
                    iter->second->pSendBuff->SetWriteIndex(iWriteIdx);
                    return(false);
                }
            }
        }
        return(false);
    }

    bool OssWorker::SendTo(const tagMsgShell& stMsgShell, const MsgHead& oMsgHead, const MsgBody& oMsgBody)
    {
        LOG4_TRACE("%s(fd %d, fd_seq %lu, cmd %u, msg_seq %u)",
            __FUNCTION__, stMsgShell.iFd, stMsgShell.ulSeq, oMsgHead.cmd(), oMsgHead.seq());
        std::map<int, tagConnectionAttr*>::iterator conn_iter = m_mapFdAttr.find(stMsgShell.iFd);
        if (conn_iter == m_mapFdAttr.end())
        {
            LOG4_ERROR("no fd %d found in m_mapFdAttr", stMsgShell.iFd);
            return(false);
        }
        else
        {
            if (conn_iter->second->ulSeq == stMsgShell.ulSeq)
            {
                std::map<loss::E_CODEC_TYPE, StarshipCodec*>::iterator codec_iter = m_mapCodec.find(conn_iter->second->eCodecType);
                if (codec_iter == m_mapCodec.end())
                {
                    LOG4_ERROR("no codec found for %d!", conn_iter->second->eCodecType);
                    DestroyConnect(conn_iter);
                    return(false);
                }
                E_CODEC_STATUS eCodecStatus = CODEC_STATUS_OK;
                if (loss::CODEC_PROTOBUF == conn_iter->second->eCodecType)//内部协议需要检查连接过程
                {
                    LOG4_TRACE("connect status %u", conn_iter->second->ucConnectStatus);
                    if (eConnectStatus_ok != conn_iter->second->ucConnectStatus)   // 连接尚未完成
                    {
                        if (oMsgHead.cmd() <= CMD_RSP_TELL_WORKER)   // 创建连接的过程
                        {
                            LOG4_TRACE("codec_iter->second->Encode,oMsgHead.cmd(%u),connect status %u",
                                oMsgHead.cmd(), conn_iter->second->ucConnectStatus);
                            eCodecStatus = codec_iter->second->Encode(oMsgHead, oMsgBody, conn_iter->second->pSendBuff);
                            if (oMsgHead.cmd() == CMD_RSP_TELL_WORKER)
                            {
                                conn_iter->second->ucConnectStatus = eConnectStatus_ok;
                            }
                            else
                            {
                                conn_iter->second->ucConnectStatus = eConnectStatus_connecting;
                            }
                        }
                        else    // 创建连接过程中的其他数据发送请求
                        {
                            LOG4_TRACE("codec_iter->second->Encode,oMsgHead.cmd(%u),connect status %u",
                                oMsgHead.cmd(), conn_iter->second->ucConnectStatus);
                            eCodecStatus = codec_iter->second->Encode(oMsgHead, oMsgBody, conn_iter->second->pWaitForSendBuff);
                            if (CODEC_STATUS_OK == eCodecStatus)//其他请求在连接过程中先不发送
                            {
                                return(true);
                            }
                            return(false);
                        }
                    }
                    else//连接已完成
                    {
                        LOG4_TRACE("codec_iter->second->Encode,oMsgHead.cmd(%u),connect status %u",
                            oMsgHead.cmd(), conn_iter->second->ucConnectStatus);
                        eCodecStatus = codec_iter->second->Encode(oMsgHead, oMsgBody, conn_iter->second->pSendBuff);
                    }
                }
                else
                {
                    LOG4_TRACE("codec_iter->second->Encode,oMsgHead.cmd(%u),connect status %u",
                        oMsgHead.cmd(), conn_iter->second->ucConnectStatus);
                    eCodecStatus = codec_iter->second->Encode(oMsgHead, oMsgBody, conn_iter->second->pSendBuff);
                }
                if (CODEC_STATUS_OK == eCodecStatus)
                {
                    ++m_iSendNum;
                    int iErrno = 0;
                    int iNeedWriteLen = (int)conn_iter->second->pSendBuff->ReadableBytes();
                    if (NULL == conn_iter->second->pRemoteAddr)
                    {
                        LOG4_TRACE("try send cmd[%d] seq[%lu] len %d to fd %d, identify %s",
                            oMsgHead.cmd(), oMsgHead.seq(), iNeedWriteLen, stMsgShell.iFd,
                            conn_iter->second->strIdentify.c_str());
                    }
                    else
                    {
                        LOG4_TRACE("try send cmd[%d] seq[%lu] len %d to fd %d ip %s identify %s",
                            oMsgHead.cmd(), oMsgHead.seq(), iNeedWriteLen, stMsgShell.iFd,
                            conn_iter->second->pRemoteAddr, conn_iter->second->strIdentify.c_str());
                    }
                    int iWriteLen = conn_iter->second->pSendBuff->WriteFD(stMsgShell.iFd, iErrno);
                    conn_iter->second->pSendBuff->Compact(8192);
                    if (iWriteLen < 0)
                    {
                        if (EAGAIN != iErrno && EINTR != iErrno)    // 对非阻塞socket而言，EAGAIN不是一种错误;EINTR即errno为4，错误描述Interrupted system call，操作也应该继续。
                        {
                            LOG4_ERROR("send to fd %d error %d: %s",
                                stMsgShell.iFd, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                            DestroyConnect(conn_iter);
                            return(false);
                        }
                        else if (EAGAIN == iErrno)  // 内容未写完，添加或保持监听fd写事件
                        {
                            LOG4_TRACE("write len %d, errno %d: %s",
                                iWriteLen, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                            conn_iter->second->dActiveTime = ev_now(m_loop);
                            AddIoWriteEvent(conn_iter);
                        }
                        else
                        {
                            LOG4_TRACE("write len %d, errno %d: %s",
                                iWriteLen, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                            conn_iter->second->dActiveTime = ev_now(m_loop);
                            AddIoWriteEvent(conn_iter);
                        }
                    }
                    else if (iWriteLen > 0)
                    {
                        m_iSendByte += iWriteLen;
                        conn_iter->second->dActiveTime = ev_now(m_loop);
                        if (iWriteLen == iNeedWriteLen)  // 已无内容可写，取消监听fd写事件
                        {
                            LOG4_TRACE("cmd[%d] seq[%lu] to fd %d ip %s identify %s need write %d and had written len %d",
                                oMsgHead.cmd(), oMsgHead.seq(), stMsgShell.iFd,
                                conn_iter->second->pRemoteAddr, conn_iter->second->strIdentify.c_str(),
                                iNeedWriteLen, iWriteLen);
                            RemoveIoWriteEvent(conn_iter);
                        }
                        else    // 内容未写完，添加或保持监听fd写事件
                        {
                            LOG4_TRACE("cmd[%d] seq[%lu] need write %d and had written len %d",
                                oMsgHead.cmd(), oMsgHead.seq(), iNeedWriteLen, iWriteLen);
                            AddIoWriteEvent(conn_iter);
                        }
                    }
                    return(true);
                }
                else
                {
                    LOG4_WARN("codec_iter->second->Encode failed,oMsgHead.cmd(%u),connect status %u",
                        oMsgHead.cmd(), conn_iter->second->ucConnectStatus);
                    return(false);
                }
            }
            else
            {
                LOG4_ERROR("fd %d sequence %lu not match the sequence %lu in m_mapFdAttr",
                    stMsgShell.iFd, stMsgShell.ulSeq, conn_iter->second->ulSeq);
                return(false);
            }
        }
    }

    bool OssWorker::SendTo(const std::string& strIdentify, const MsgHead& oMsgHead, const MsgBody& oMsgBody)
    {
        LOG4_TRACE("%s(identify: %s)", __FUNCTION__, strIdentify.c_str());
        std::map<std::string, tagMsgShell>::iterator shell_iter = m_mapMsgShell.find(strIdentify);
        if (shell_iter == m_mapMsgShell.end())
        {
            LOG4_TRACE("no MsgShell match %s.", strIdentify.c_str());
            return(AutoSend(strIdentify, oMsgHead, oMsgBody));
        }
        else
        {
            return(SendTo(shell_iter->second, oMsgHead, oMsgBody));
        }
    }

    bool OssWorker::SendToNext(const std::string& strNodeType, const MsgHead& oMsgHead, const MsgBody& oMsgBody)
    {
        LOG4_TRACE("%s(node_type: %s)", __FUNCTION__, strNodeType.c_str());
        std::map<std::string, std::pair<std::set<std::string>::iterator, std::set<std::string> > >::iterator node_type_iter;
        node_type_iter = m_mapNodeIdentify.find(strNodeType);
        if (node_type_iter == m_mapNodeIdentify.end())
        {
            LOG4_ERROR(" MsgShell match number: %d!", m_mapNodeIdentify.size());
            LOG4_ERROR("no MsgShell match %s!", strNodeType.c_str());
            return(false);
        }
        else
        {
            if (node_type_iter->second.first != node_type_iter->second.second.end())
            {
                std::set<std::string>::iterator id_iter = node_type_iter->second.first;
                node_type_iter->second.first++;
                return(SendTo(*id_iter, oMsgHead, oMsgBody));
            }
            else
            {
                std::set<std::string>::iterator id_iter = node_type_iter->second.second.begin();
                if (id_iter != node_type_iter->second.second.end())
                {
                    node_type_iter->second.first = id_iter;
                    node_type_iter->second.first++;
                    return(SendTo(*id_iter, oMsgHead, oMsgBody));
                }
                else
                {
                    LOG4_ERROR("no MsgShell match and no node identify found for %s!", strNodeType.c_str());
                    return(false);
                }
            }
        }
    }

    bool OssWorker::SendToWithMod(const std::string& strNodeType, unsigned int uiModFactor, const MsgHead& oMsgHead, const MsgBody& oMsgBody)
    {
        LOG4_TRACE("%s(nody_type: %s, mod_factor: %d)", __FUNCTION__, strNodeType.c_str(), uiModFactor);
        std::map<std::string, std::pair<std::set<std::string>::iterator, std::set<std::string> > >::iterator node_type_iter;
        node_type_iter = m_mapNodeIdentify.find(strNodeType);
        if (node_type_iter == m_mapNodeIdentify.end())
        {
            LOG4_ERROR("no MsgShell match %s!", strNodeType.c_str());
            return(false);
        }
        else
        {
            if (node_type_iter->second.second.size() == 0)
            {
                LOG4_ERROR("no MsgShell match %s!", strNodeType.c_str());
                return(false);
            }
            else
            {
                std::set<std::string>::iterator id_iter;
                int target_identify = uiModFactor % node_type_iter->second.second.size();
                int i = 0;
                for (i = 0, id_iter = node_type_iter->second.second.begin();
                    i < node_type_iter->second.second.size();
                    ++i, ++id_iter)
                {
                    if (i == target_identify && id_iter != node_type_iter->second.second.end())
                    {
                        return(SendTo(*id_iter, oMsgHead, oMsgBody));
                    }
                }
                return(false);
            }
        }
    }

    bool OssWorker::SendToNodeType(const std::string& strNodeType, const MsgHead& oMsgHead, const MsgBody& oMsgBody)
    {
        LOG4_TRACE("%s(node_type: %s)", __FUNCTION__, strNodeType.c_str());
        std::map<std::string, std::pair<std::set<std::string>::iterator, std::set<std::string> > >::iterator node_type_iter;
        node_type_iter = m_mapNodeIdentify.find(strNodeType);
        if (node_type_iter == m_mapNodeIdentify.end())
        {
            LOG4_ERROR("no MsgShell match %s!", strNodeType.c_str());
            return(false);
        }
        else
        {
            int iSendNum = 0;
            for (std::set<std::string>::iterator id_iter = node_type_iter->second.second.begin();
                id_iter != node_type_iter->second.second.end(); ++id_iter)
            {
                if (*id_iter != GetWorkerIdentify())
                {
                    SendTo(*id_iter, oMsgHead, oMsgBody);
                }
                ++iSendNum;
            }
            if (0 == iSendNum)
            {
                LOG4_ERROR("no MsgShell match and no node identify found for %s!", strNodeType.c_str());
                return(false);
            }
        }
        return(true);
    }

    bool OssWorker::SendTo(const tagMsgShell& stMsgShell, const HttpMsg& oHttpMsg, HttpStep* pHttpStep)
    {
        LOG4_TRACE("%s(fd %d, seq %lu)", __FUNCTION__, stMsgShell.iFd, stMsgShell.ulSeq);
        std::map<int, tagConnectionAttr*>::iterator conn_iter = m_mapFdAttr.find(stMsgShell.iFd);
        if (conn_iter == m_mapFdAttr.end())
        {
            LOG4_ERROR("no fd %d found in m_mapFdAttr", stMsgShell.iFd);
            return(false);
        }
        else
        {
            if (conn_iter->second->ulSeq == stMsgShell.ulSeq)
            {
                std::map<loss::E_CODEC_TYPE, StarshipCodec*>::iterator codec_iter = m_mapCodec.find(conn_iter->second->eCodecType);
                if (codec_iter == m_mapCodec.end())
                {
                    LOG4_ERROR("no codec found for %d!", conn_iter->second->eCodecType);
                    DestroyConnect(conn_iter);
                    return(false);
                }
                E_CODEC_STATUS eCodecStatus;
                if (loss::CODEC_WEBSOCKET_JSON == conn_iter->second->eCodecType)
                {
                    if (conn_iter->second->pWaitForSendBuff->ReadableBytes() > 0)   // 正在连接
                    {
                        eCodecStatus = ((CodecWsExtentJson*)codec_iter->second)->Encode(oHttpMsg, conn_iter->second->pWaitForSendBuff);
                    }
                    else
                    {
                        eCodecStatus = ((CodecWsExtentJson*)codec_iter->second)->Encode(oHttpMsg, conn_iter->second->pSendBuff);
                    }
                }
                else if (loss::CODEC_HTTP == conn_iter->second->eCodecType)
                {
                    if (conn_iter->second->pWaitForSendBuff->ReadableBytes() > 0)   // 正在连接
                    {
                        eCodecStatus = ((HttpCodec*)codec_iter->second)->Encode(oHttpMsg, conn_iter->second->pWaitForSendBuff);
                    }
                    else
                    {
                        eCodecStatus = ((HttpCodec*)codec_iter->second)->Encode(oHttpMsg, conn_iter->second->pSendBuff);
                    }
                }
                else
                {
                    LOG4_ERROR("the codec for fd %d is not http or websocket codec(%d)!",
                        stMsgShell.iFd, conn_iter->second->eCodecType);
                    return(false);
                }

                if (CODEC_STATUS_OK == eCodecStatus && conn_iter->second->pSendBuff->ReadableBytes() > 0)
                {
                    ++m_iSendNum;
                    if ((conn_iter->second->pIoWatcher != NULL) && (conn_iter->second->pIoWatcher->events & EV_WRITE))
                    {   // 正在监听fd的写事件，说明发送缓冲区满，此时直接返回，等待EV_WRITE事件再执行WriteFD
                        return(true);
                    }
                    LOG4_TRACE("fd[%d], seq[%u], conn_iter->second->pSendBuff 0x%x", stMsgShell.iFd, stMsgShell.ulSeq, conn_iter->second->pSendBuff);
                    int iErrno = 0;
                    int iNeedWriteLen = (int)conn_iter->second->pSendBuff->ReadableBytes();
                    int iWriteLen = conn_iter->second->pSendBuff->WriteFD(stMsgShell.iFd, iErrno);
                    conn_iter->second->pSendBuff->Compact(8192);
                    if (iWriteLen < 0)
                    {
                        if (EAGAIN != iErrno && EINTR != iErrno)    // 对非阻塞socket而言，EAGAIN不是一种错误;EINTR即errno为4，错误描述Interrupted system call，操作也应该继续。
                        {
                            LOG4_ERROR("send to fd %d error %d: %s",
                                stMsgShell.iFd, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                            DestroyConnect(conn_iter);
                            return(false);
                        }
                        else if (EAGAIN == iErrno)  // 内容未写完，添加或保持监听fd写事件
                        {
                            LOG4_TRACE("write len %d, error %d: %s",
                                iWriteLen, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                            conn_iter->second->dActiveTime = ev_now(m_loop);
                            AddIoWriteEvent(conn_iter);
                        }
                        else
                        {
                            LOG4_TRACE("write len %d, error %d: %s",
                                iWriteLen, iErrno, strerror_r(iErrno, m_pErrBuff, gc_iErrBuffLen));
                            conn_iter->second->dActiveTime = ev_now(m_loop);
                            AddIoWriteEvent(conn_iter);
                        }
                    }
                    else if (iWriteLen > 0)
                    {
                        if (pHttpStep != NULL)
                        {
                            std::map<int32, std::list<uint32> >::iterator http_step_iter = m_mapHttpAttr.find(stMsgShell.iFd);
                            if (http_step_iter == m_mapHttpAttr.end())
                            {
                                std::list<uint32> listHttpStepSeq;
                                listHttpStepSeq.push_back(pHttpStep->GetSequence());
                                m_mapHttpAttr.insert(std::pair<int32, std::list<uint32> >(stMsgShell.iFd, listHttpStepSeq));
                            }
                            else
                            {
                                http_step_iter->second.push_back(pHttpStep->GetSequence());
                            }
                        }
                        else
                        {
                            std::map<int32, std::list<uint32> >::iterator http_step_iter = m_mapHttpAttr.find(stMsgShell.iFd);
                            if (http_step_iter == m_mapHttpAttr.end())
                            {
                                std::list<uint32> listHttpStepSeq;
                                m_mapHttpAttr.insert(std::pair<int32, std::list<uint32> >(stMsgShell.iFd, listHttpStepSeq));
                            }
                        }
                        m_iSendByte += iWriteLen;
                        conn_iter->second->dActiveTime = ev_now(m_loop);
                        if (iWriteLen == iNeedWriteLen)  // 已无内容可写，取消监听fd写事件
                        {
                            LOG4_TRACE("need write len %d, and had writen len %d", iNeedWriteLen, iWriteLen);
                            RemoveIoWriteEvent(conn_iter);
                        }
                        else    // 内容未写完，添加或保持监听fd写事件
                        {
                            AddIoWriteEvent(conn_iter);
                        }
                    }
                    return(true);
                }
                else
                {
                    return(false);
                }
            }
            else
            {
                LOG4_ERROR("fd %d sequence %lu not match the sequence %lu in m_mapFdAttr",
                    stMsgShell.iFd, stMsgShell.ulSeq, conn_iter->second->ulSeq);
                return(false);
            }
        }
    }

    bool OssWorker::SentTo(const std::string& strHost, int iPort, const std::string& strUrlPath, const HttpMsg& oHttpMsg, HttpStep* pHttpStep)
    {
        char szIdentify[256] = { 0 };
        snprintf(szIdentify, sizeof(szIdentify), "%s:%d%s", strHost.c_str(), iPort, strUrlPath.c_str());
        LOG4_TRACE("%s(identify: %s)", __FUNCTION__, szIdentify);
        return(AutoSend(strHost, iPort, strUrlPath, oHttpMsg, pHttpStep));
        // 向外部发起http请求不复用连接
        //    std::map<std::string, tagMsgShell>::iterator shell_iter = m_mapMsgShell.find(szIdentify);
        //    if (shell_iter == m_mapMsgShell.end())
        //    {
        //        LOG4_TRACE("no MsgShell match %s.", szIdentify);
        //        return(AutoSend(strHost, iPort, strUrlPath, oHttpMsg, pHttpStep));
        //    }
        //    else
        //    {
        //        return(SendTo(shell_iter->second, oHttpMsg, pHttpStep));
        //    }
    }

    bool OssWorker::SetConnectIdentify(const tagMsgShell& stMsgShell, const std::string& strIdentify)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(stMsgShell.iFd);
        if (iter == m_mapFdAttr.end())
        {
            LOG4_ERROR("no fd %d found in m_mapFdAttr", stMsgShell.iFd);
            return(false);
        }
        else
        {
            if (iter->second->ulSeq == stMsgShell.ulSeq)
            {
                iter->second->strIdentify = strIdentify;
                return(true);
            }
            else
            {
                LOG4_ERROR("fd %d sequence %lu not match the sequence %lu in m_mapFdAttr",
                    stMsgShell.iFd, stMsgShell.ulSeq, iter->second->ulSeq);
                return(false);
            }
        }
    }

    bool OssWorker::AutoSend(const std::string& strIdentify, const MsgHead& oMsgHead, const MsgBody& oMsgBody)
    {
        LOG4_TRACE("%s(%s)", __FUNCTION__, strIdentify.c_str());
        int iPosIpPortSeparator = strIdentify.find(':');
        if (iPosIpPortSeparator == std::string::npos)
        {
            return(false);
        }
        int iPosPortWorkerIndexSeparator = strIdentify.rfind('.');
        std::string strHost = strIdentify.substr(0, iPosIpPortSeparator);
        std::string strPort = strIdentify.substr(iPosIpPortSeparator + 1, iPosPortWorkerIndexSeparator - (iPosIpPortSeparator + 1));
        std::string strWorkerIndex = strIdentify.substr(iPosPortWorkerIndexSeparator + 1, std::string::npos);
        int iPort = atoi(strPort.c_str());
        if (iPort == 0)
        {
            return(false);
        }
        int iWorkerIndex = atoi(strWorkerIndex.c_str());
        if (iWorkerIndex > 200)
        {
            return(false);
        }
        struct sockaddr_in stAddr;
        int iFd = -1;
        stAddr.sin_family = AF_INET;
        stAddr.sin_port = htons(iPort);
        stAddr.sin_addr.s_addr = inet_addr(strHost.c_str());
        bzero(&(stAddr.sin_zero), 8);
        iFd = socket(AF_INET, SOCK_STREAM, 0);
        if (iFd == -1)
        {
            return(false);
        }
        x_sock_set_block(iFd, 0);
        int nREUSEADDR = 1;
        setsockopt(iFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&nREUSEADDR, sizeof(int));
        uint32 ulSeq = GetSequence();
        if (CreateFdAttr(iFd, ulSeq))
        {
            std::map<int, tagConnectionAttr*>::iterator conn_iter = m_mapFdAttr.find(iFd);
            snprintf(conn_iter->second->pRemoteAddr, 32, strIdentify.c_str());
            if (AddIoTimeout(iFd, ulSeq, conn_iter->second, 1.5))
            {
                conn_iter->second->ucConnectStatus = 0;
                if (!AddIoReadEvent(conn_iter))
                {
                    LOG4_TRACE("if (!AddIoReadEvent(conn_iter))");
                    DestroyConnect(conn_iter);
                    return(false);
                }
                //            if (!AddIoErrorEvent(iFd))
                //            {
                //                DestroyConnect(iter);
                //                return(false);
                //            }
                if (!AddIoWriteEvent(conn_iter))
                {
                    LOG4_TRACE("if (!AddIoWriteEvent(conn_iter))");
                    DestroyConnect(conn_iter);
                    return(false);
                }
                std::map<loss::E_CODEC_TYPE, StarshipCodec*>::iterator codec_iter = m_mapCodec.find(conn_iter->second->eCodecType);
                if (codec_iter == m_mapCodec.end())
                {
                    LOG4_ERROR("no codec found for %d!", conn_iter->second->eCodecType);
                    DestroyConnect(conn_iter);
                    return(false);
                }
                E_CODEC_STATUS eCodecStatus = codec_iter->second->Encode(oMsgHead, oMsgBody, conn_iter->second->pWaitForSendBuff);
                if (CODEC_STATUS_OK == eCodecStatus)
                {
                    ++m_iSendNum;
                }
                else
                {
                    return(false);
                }
                //            LOG4_TRACE("fd %d, iter->second->pWaitForSendBuff->ReadableBytes()=%d",
                //                            iFd, iter->second->pWaitForSendBuff->ReadableBytes());
                m_mapSeq2WorkerIndex.insert(std::pair<uint32, int>(ulSeq, iWorkerIndex));
                tagMsgShell stMsgShell;
                stMsgShell.iFd = iFd;
                stMsgShell.ulSeq = ulSeq;
                AddMsgShell(strIdentify, stMsgShell);
                connect(iFd, (struct sockaddr*)&stAddr, sizeof(struct sockaddr));
                return(true);
            }
            else
            {
                LOG4_TRACE("if(AddIoTimeout(iFd, ulSeq, conn_iter->second, 1.5)) else");
                DestroyConnect(conn_iter);
                return(false);
            }
        }
        else    // 没有足够资源分配给新连接，直接close掉
        {
            close(iFd);
            return(false);
        }
    }

    bool OssWorker::AutoSend(const std::string& strHost, int iPort, const std::string& strUrlPath, const HttpMsg& oHttpMsg, HttpStep* pHttpStep)
    {
        LOG4_TRACE("%s(%s, %d, %s)", __FUNCTION__, strHost.c_str(), iPort, strUrlPath.c_str());
        struct sockaddr_in stAddr;
        tagMsgShell stMsgShell;
        stAddr.sin_family = AF_INET;
        stAddr.sin_port = htons(iPort);
        stAddr.sin_addr.s_addr = inet_addr(strHost.c_str());
        if (stAddr.sin_addr.s_addr == 4294967295 || stAddr.sin_addr.s_addr == 0)
        {
            struct hostent *he;
            he = gethostbyname(strHost.c_str());
            if (he != NULL)
            {
                stAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)(he->h_addr)));
            }
            else
            {
                LOG4_ERROR("gethostbyname(%s) error!", strHost.c_str());
                return(false);
            }
        }
        bzero(&(stAddr.sin_zero), 8);
        stMsgShell.iFd = socket(AF_INET, SOCK_STREAM, 0);
        if (stMsgShell.iFd == -1)
        {
            return(false);
        }
        x_sock_set_block(stMsgShell.iFd, 0);
        int nREUSEADDR = 1;
        setsockopt(stMsgShell.iFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&nREUSEADDR, sizeof(int));
        stMsgShell.ulSeq = GetSequence();
        tagConnectionAttr* pConnAttr = CreateFdAttr(stMsgShell.iFd, stMsgShell.ulSeq);
        if (pConnAttr)
        {
            pConnAttr->eCodecType = loss::CODEC_HTTP;
            snprintf(pConnAttr->pRemoteAddr, 32, strHost.c_str());
            std::map<int, tagConnectionAttr*>::iterator conn_iter = m_mapFdAttr.find(stMsgShell.iFd);
            if (AddIoTimeout(stMsgShell.iFd, stMsgShell.ulSeq, conn_iter->second, 2.5))
            {
                conn_iter->second->dKeepAlive = 10;
                if (!AddIoReadEvent(conn_iter))
                {
                    LOG4_TRACE("if (!AddIoReadEvent(conn_iter))");
                    DestroyConnect(conn_iter);
                    return(false);
                }
                if (!AddIoWriteEvent(conn_iter))
                {
                    LOG4_TRACE("if (!AddIoWriteEvent(conn_iter))");
                    DestroyConnect(conn_iter);
                    return(false);
                }
                std::map<loss::E_CODEC_TYPE, StarshipCodec*>::iterator codec_iter = m_mapCodec.find(conn_iter->second->eCodecType);
                if (codec_iter == m_mapCodec.end())
                {
                    LOG4_ERROR("no codec found for %d!", conn_iter->second->eCodecType);
                    DestroyConnect(conn_iter);
                    return(false);
                }
                E_CODEC_STATUS eCodecStatus = ((HttpCodec*)codec_iter->second)->Encode(oHttpMsg, conn_iter->second->pWaitForSendBuff);
                if (CODEC_STATUS_OK == eCodecStatus)
                {
                    ++m_iSendNum;
                }
                else
                {
                    return(false);
                }
                //            LOG4_TRACE("fd %d, iter->second->pWaitForSendBuff->ReadableBytes()=%d",
                //                            iFd, iter->second->pWaitForSendBuff->ReadableBytes());
                connect(stMsgShell.iFd, (struct sockaddr*)&stAddr, sizeof(struct sockaddr));
                if (pHttpStep)
                {
                    std::map<int32, std::list<uint32> >::iterator http_step_iter = m_mapHttpAttr.find(stMsgShell.iFd);
                    if (http_step_iter == m_mapHttpAttr.end())
                    {
                        std::list<uint32> listHttpStepSeq;
                        listHttpStepSeq.push_back(pHttpStep->GetSequence());
                        m_mapHttpAttr.insert(std::pair<int32, std::list<uint32> >(stMsgShell.iFd, listHttpStepSeq));
                    }
                    else
                    {
                        http_step_iter->second.push_back(pHttpStep->GetSequence());
                    }
                }
                else
                {
                    std::map<int32, std::list<uint32> >::iterator http_step_iter = m_mapHttpAttr.find(stMsgShell.iFd);
                    if (http_step_iter == m_mapHttpAttr.end())
                    {
                        std::list<uint32> listHttpStepSeq;
                        m_mapHttpAttr.insert(std::pair<int32, std::list<uint32> >(stMsgShell.iFd, listHttpStepSeq));
                    }
                }
                return(true);
                // 向外部发起http请求不复用连接
                //            char szIdentify[256] = {0};
                //            snprintf(szIdentify, sizeof(szIdentify), "%s:%d%s", strHost.c_str(), iPort, strUrlPath.c_str());
                //            return(AddMsgShell(szIdentify, stMsgShell));
            }
            else
            {
                LOG4_TRACE("if(AddIoTimeout(stMsgShell.iFd, stMsgShell.ulSeq, conn_iter->second, 2.5)) else");
                DestroyConnect(conn_iter);
                return(false);
            }
        }
        else    // 没有足够资源分配给新连接，直接close掉
        {
            close(stMsgShell.iFd);
            return(false);
        }
    }

    bool OssWorker::AutoRedisCmd(const std::string& strHost, int iPort, RedisStep* pRedisStep)
    {
        LOG4_TRACE("%s() redisAsyncConnect(%s, %d)", __FUNCTION__, strHost.c_str(), iPort);
        redisAsyncContext *c = redisAsyncConnect(strHost.c_str(), iPort);
        if (c->err)
        {
            /* Let *c leak for now... */
            LOG4_ERROR("error: %s", c->errstr);
            return(false);
        }
        c->data = this;
        tagRedisAttr* pRedisAttr = new tagRedisAttr();
        pRedisAttr->ulSeq = GetSequence();
        pRedisAttr->listWaitData.push_back(pRedisStep);
        pRedisStep->SetLogger(&m_oLogger);
        pRedisStep->SetLabor(this);
        pRedisStep->SetRegistered();
        m_mapRedisAttr.insert(std::pair<redisAsyncContext*, tagRedisAttr*>(c, pRedisAttr));
        //    LOG4_TRACE("redisLibevAttach(0x%X, 0x%X)", m_loop, c);
        redisLibevAttach(m_loop, c);
        //    LOG4_TRACE("redisAsyncSetConnectCallback(0x%X, 0x%X)", c, RedisConnectCallback);
        redisAsyncSetConnectCallback(c, RedisConnectCallback);
        //    LOG4_TRACE("redisAsyncSetDisconnectCallback(0x%X, 0x%X)", c, RedisDisconnectCallback);
        redisAsyncSetDisconnectCallback(c, RedisDisconnectCallback);
        //    LOG4_TRACE("RedisStep::AddRedisContextAddr(%s, %d, 0x%X)", strHost.c_str(), iPort, c);
        AddRedisContextAddr(strHost, iPort, c);
        return(true);
    }

    bool OssWorker::AutoConnect(const std::string& strIdentify)
    {
        LOG4_DEBUG("%s(%s)", __FUNCTION__, strIdentify.c_str());
        int iPosIpPortSeparator = strIdentify.find(':');
        if (iPosIpPortSeparator == std::string::npos)
        {
            return(false);
        }
        int iPosPortWorkerIndexSeparator = strIdentify.rfind('.');
        std::string strHost = strIdentify.substr(0, iPosIpPortSeparator);
        std::string strPort = strIdentify.substr(iPosIpPortSeparator + 1, iPosPortWorkerIndexSeparator - (iPosIpPortSeparator + 1));
        std::string strWorkerIndex = strIdentify.substr(iPosPortWorkerIndexSeparator + 1, std::string::npos);
        int iPort = atoi(strPort.c_str());
        if (iPort == 0)
        {
            return(false);
        }
        int iWorkerIndex = atoi(strWorkerIndex.c_str());
        if (iWorkerIndex > 200)
        {
            return(false);
        }
        struct sockaddr_in stAddr;
        int iFd = -1;
        stAddr.sin_family = AF_INET;
        stAddr.sin_port = htons(iPort);
        stAddr.sin_addr.s_addr = inet_addr(strHost.c_str());
        bzero(&(stAddr.sin_zero), 8);
        iFd = socket(AF_INET, SOCK_STREAM, 0);
        if (iFd == -1)
        {
            return(false);
        }
        x_sock_set_block(iFd, 0);
        int nREUSEADDR = 1;
        setsockopt(iFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&nREUSEADDR, sizeof(int));
        uint32 ulSeq = GetSequence();
        if (CreateFdAttr(iFd, ulSeq))
        {
            std::map<int, tagConnectionAttr*>::iterator conn_iter = m_mapFdAttr.find(iFd);
            if (AddIoTimeout(iFd, ulSeq, conn_iter->second, 1.5))
            {
                conn_iter->second->ucConnectStatus = 0;
                if (!AddIoReadEvent(conn_iter))
                {
                    LOG4_TRACE("if (!AddIoReadEvent(conn_iter))");
                    DestroyConnect(conn_iter);
                    return(false);
                }
                //            if (!AddIoErrorEvent(iFd))
                //            {
                //                DestroyConnect(iter);
                //                return(false);
                //            }
                if (!AddIoWriteEvent(conn_iter))
                {
                    LOG4_TRACE("if (!AddIoWriteEvent(conn_iter))");
                    DestroyConnect(conn_iter);
                    return(false);
                }
                m_mapSeq2WorkerIndex.insert(std::pair<uint32, int>(ulSeq, iWorkerIndex));
                tagMsgShell stMsgShell;
                stMsgShell.iFd = iFd;
                stMsgShell.ulSeq = ulSeq;
                AddMsgShell(strIdentify, stMsgShell);
                connect(iFd, (struct sockaddr*)&stAddr, sizeof(struct sockaddr));
                return(true);
            }
            else
            {
                LOG4_TRACE("if(AddIoTimeout(iFd, ulSeq, conn_iter->second, 1.5)) else");
                DestroyConnect(conn_iter);
                return(false);
            }
        }
        else    // 没有足够资源分配给新连接，直接close掉
        {
            close(iFd);
            return(false);
        }
    }

    void OssWorker::AddInnerFd(const tagMsgShell& stMsgShell)
    {
        std::map<int, uint32>::iterator iter = m_mapInnerFd.find(stMsgShell.iFd);
        if (iter == m_mapInnerFd.end())
        {
            m_mapInnerFd.insert(std::pair<int, uint32>(stMsgShell.iFd, stMsgShell.ulSeq));
        }
        else
        {
            iter->second = stMsgShell.ulSeq;
        }
        LOG4_TRACE("%s() now m_mapInnerFd.size() = %u", __FUNCTION__, m_mapInnerFd.size());
    }

    bool OssWorker::GetMsgShell(const std::string& strIdentify, tagMsgShell& stMsgShell)
    {
        LOG4_TRACE("%s(identify: %s)", __FUNCTION__, strIdentify.c_str());
        std::map<std::string, tagMsgShell>::iterator shell_iter = m_mapMsgShell.find(strIdentify);
        if (shell_iter == m_mapMsgShell.end())
        {
            LOG4_DEBUG("no MsgShell match %s.", strIdentify.c_str());
            return(false);
        }
        else
        {
            stMsgShell = shell_iter->second;
            return(true);
        }
    }

    bool OssWorker::SetClientData(const tagMsgShell& stMsgShell, loss::CBuffer* pBuff)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(stMsgShell.iFd);
        if (iter == m_mapFdAttr.end())
        {
            return(false);
        }
        else
        {
            if (iter->second->ulSeq == stMsgShell.ulSeq)
            {
                iter->second->pClientData->Write(pBuff, pBuff->ReadableBytes());
                return(true);
            }
            else
            {
                return(false);
            }
        }
    }

    bool OssWorker::HadClientData(const tagMsgShell& stMsgShell)
    {
        std::map<int, tagConnectionAttr*>::iterator conn_iter;
        conn_iter = m_mapFdAttr.find(stMsgShell.iFd);
        if (conn_iter == m_mapFdAttr.end())
        {
            return(false);
        }
        else
        {
            if (stMsgShell.ulSeq != conn_iter->second->ulSeq)
            {
                return(false);
            }
            if (conn_iter->second->pClientData != NULL)
            {
                if (conn_iter->second->pClientData->ReadableBytes() > 0)
                {
                    return(true);
                }
                else
                {
                    return(false);
                }
            }
            else
            {
                return(false);
            }
        }
    }

    bool OssWorker::GetClientData(const tagMsgShell& stMsgShell, loss::CBuffer* pBuff)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(stMsgShell.iFd);
        if (iter == m_mapFdAttr.end())
        {
            return(false);
        }
        else
        {
            if (iter->second->ulSeq == stMsgShell.ulSeq)
            {
                pBuff->Write(iter->second->pClientData->GetRawReadBuffer(), iter->second->pClientData->ReadableBytes());
                return(true);
            }
            else
            {
                return(false);
            }
        }

        return(false);
    }


    std::string OssWorker::GetClientAddr(const tagMsgShell& stMsgShell)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(stMsgShell.iFd);
        if (iter == m_mapFdAttr.end())
        {
            return("");
        }
        else
        {
            if (iter->second->ulSeq == stMsgShell.ulSeq)
            {
                if (iter->second->pRemoteAddr == NULL)
                {
                    return("");
                }
                else
                {
                    return(iter->second->pRemoteAddr);
                }
            }
            else
            {
                return("");
            }
        }
    }

    bool OssWorker::AbandonConnect(const std::string& strIdentify)
    {
        LOG4_TRACE("%s(identify: %s)", __FUNCTION__, strIdentify.c_str());
        std::map<std::string, tagMsgShell>::iterator shell_iter = m_mapMsgShell.find(strIdentify);
        if (shell_iter == m_mapMsgShell.end())
        {
            LOG4_DEBUG("no MsgShell match %s.", strIdentify.c_str());
            return(false);
        }
        else
        {
            std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(shell_iter->second.iFd);
            if (iter == m_mapFdAttr.end())
            {
                return(false);
            }
            else
            {
                if (iter->second->ulSeq == shell_iter->second.ulSeq)
                {
                    iter->second->strIdentify = "";
                    iter->second->pClientData->Clear();
                    m_mapMsgShell.erase(shell_iter);
                    return(true);
                }
                else
                {
                    return(false);
                }
            }
            return(true);
        }
    }

    void OssWorker::ExecStep(uint32 uiCallerStepSeq, uint32 uiCalledStepSeq,
        int iErrno, const std::string& strErrMsg, const std::string& strErrShow)
    {
        LOG4_TRACE("%s(caller[%u], called[%u])", __FUNCTION__, uiCallerStepSeq, uiCalledStepSeq);
        std::map<uint32, Step*>::iterator step_iter = m_mapCallbackStep.find(uiCalledStepSeq);
        if (step_iter == m_mapCallbackStep.end())
        {
            LOG4_WARN("step %u is not in the callback list.", uiCalledStepSeq);
        }
        else
        {
            if (oss::STATUS_CMD_RUNNING != step_iter->second->Emit(iErrno, strErrMsg, strErrShow))
            {
                DeleteCallback(uiCallerStepSeq, step_iter->second);
                // 处理调用者step的NextStep
                step_iter = m_mapCallbackStep.find(uiCallerStepSeq);
                if (step_iter != m_mapCallbackStep.end())
                {
                    if (step_iter->second->m_pNextStep != NULL
                        && step_iter->second->m_pNextStep->GetSequence() == uiCalledStepSeq)
                    {
                        step_iter->second->m_pNextStep = NULL;
                    }
                }
            }
        }
    }

    void OssWorker::LoadSo(loss::CJsonObject& oSoConf)
    {
        LOG4_TRACE("%s():oSoConf(%s)", __FUNCTION__, oSoConf.ToString().c_str());
        int iCmd = 0;
        int iVersion = 0;
        bool bIsload = false;
        std::string strSoPath;
        std::map<int, tagSo*>::iterator cmd_iter;
        tagSo* pSo = NULL;
        for (int i = 0; i < oSoConf.GetArraySize(); ++i)
        {
            oSoConf[i].Get("load", bIsload);
            if (bIsload)
            {
                strSoPath = m_strWorkPath + std::string("/") + oSoConf[i]("so_path");
                if (oSoConf[i].Get("cmd", iCmd) && oSoConf[i].Get("version", iVersion))
                {
                    cmd_iter = m_mapSo.find(iCmd);
                    if (cmd_iter == m_mapSo.end())
                    {
                        if (0 != access(strSoPath.c_str(), F_OK))
                        {
                            LOG4_WARN("%s not exist!", strSoPath.c_str());
                            continue;
                        }
                        pSo = LoadSoAndGetCmd(iCmd, strSoPath, oSoConf[i]("entrance_symbol"), iVersion);
                        if (pSo != NULL)
                        {
                            LOG4_INFO("succeed in loading %s", strSoPath.c_str());
                            m_mapSo.insert(std::pair<int, tagSo*>(iCmd, pSo));
                        }
                        else
                        {
                            LOG4_WARN("failed to load %s", strSoPath.c_str());
                        }
                    }
                    else
                    {
                        if (iVersion != cmd_iter->second->iVersion)
                        {
                            if (0 != access(strSoPath.c_str(), F_OK))
                            {
                                LOG4_WARN("%s not exist!", strSoPath.c_str());
                                continue;
                            }
                            pSo = LoadSoAndGetCmd(iCmd, strSoPath, oSoConf[i]("entrance_symbol"), iVersion);
                            LOG4_TRACE("%s:%d after LoadSoAndGetCmd", __FILE__, __LINE__);
                            if (pSo != NULL)
                            {
                                LOG4_INFO("succeed in loading %s", strSoPath.c_str());
                                delete cmd_iter->second;
                                cmd_iter->second = pSo;
                            }
                            else
                            {
                                LOG4_WARN("failed to load %s", strSoPath.c_str());
                            }
                        }
                        else
                        {
                            LOG4_INFO("same version(%d).no need  to load %s", cmd_iter->second->iVersion,
                                strSoPath.c_str());
                        }
                    }
                }
                else
                {
                    LOG4_WARN("cmd or version not found.failed to load %s", strSoPath.c_str());
                }
            }
            else        // 卸载动态库
            {
                strSoPath = m_strWorkPath + std::string("/") + oSoConf[i]("so_path");
                if (oSoConf[i].Get("cmd", iCmd))
                {
                    LOG4_INFO("unload %s", strSoPath.c_str());
                    UnloadSoAndDeleteCmd(iCmd);
                }
                else
                {
                    LOG4_WARN("cmd not exist.failed to unload %s", strSoPath.c_str());
                }
            }
        }
    }

    void OssWorker::ReloadSo(loss::CJsonObject& oCmds)
    {
        LOG4_WARN("OssWorker ReloadSo... ");
        LOG4_DEBUG("%s():oCmds(%s)", __FUNCTION__, oCmds.ToString().c_str());
        int iCmd = 0;
        int iVersion = 0;
        std::string strSoPath;
        std::string strSymbol;
        std::map<int, tagSo*>::iterator cmd_iter;
        tagSo* pSo = NULL;
        for (int i = 0; i < oCmds.GetArraySize(); ++i)
        {
            std::string cmd = oCmds[i].ToString();
            //        std::string::iterator it = std::remove(cmd.begin(), cmd.end(), '\"');
            //        cmd.erase(it, cmd.end());
            iCmd = atoi(cmd.c_str());
            cmd_iter = m_mapSo.find(iCmd);
            if (cmd_iter != m_mapSo.end())
            {
                strSoPath = cmd_iter->second->strSoPath;
                strSymbol = cmd_iter->second->strSymbol;
                iVersion = cmd_iter->second->iVersion;
                if (0 != access(strSoPath.c_str(), F_OK))
                {
                    LOG4_WARN("%s not exist!", strSoPath.c_str());
                    continue;
                }
                pSo = LoadSoAndGetCmd(iCmd, strSoPath, strSymbol, iVersion);
                LOG4_DEBUG("%s:%d after LoadSoAndGetCmd", __FILE__, __LINE__);
                if (pSo != NULL)
                {
                    LOG4_INFO("succeed in loading %s", strSoPath.c_str());
                    delete cmd_iter->second;
                    cmd_iter->second = pSo;
                }
                else
                {
                    LOG4_WARN("failed to load %s", strSoPath.c_str());
                }
            }
            else
            {
                LOG4_WARN("no such cmd %s", cmd.c_str());
            }
        }
    }

    tagSo* OssWorker::LoadSoAndGetCmd(int iCmd, const std::string& strSoPath, const std::string& strSymbol, int iVersion)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        tagSo* pSo = NULL;
        void* pHandle = NULL;
        pHandle = dlopen(strSoPath.c_str(), RTLD_NOW);
        char* dlsym_error = dlerror();
        if (dlsym_error)
        {
            LOG4_FATAL("cannot load dynamic lib %s!", dlsym_error);
            if (pHandle != NULL)
            {
                dlclose(pHandle);
            }
            return(pSo);
        }
        CreateCmd* pCreateCmd = (CreateCmd*)dlsym(pHandle, strSymbol.c_str());
        dlsym_error = dlerror();
        if (dlsym_error)
        {
            LOG4_FATAL("dlsym error %s!", dlsym_error);
            dlclose(pHandle);
            return(pSo);
        }
        Cmd* pCmd = pCreateCmd();
        if (pCmd != NULL)
        {
            pSo = new tagSo();
            if (pSo != NULL)
            {
                pSo->pSoHandle = pHandle;
                pSo->pCmd = pCmd;
                pSo->strSoPath = strSoPath;
                pSo->strSymbol = strSymbol;
                pSo->iVersion = iVersion;
                pSo->pCmd->SetLogger(&m_oLogger);
                pSo->pCmd->SetLabor(this);
                pSo->pCmd->SetConfigPath(m_strWorkPath);
                pSo->pCmd->SetCmd(iCmd);
                if (!pSo->pCmd->Init())
                {
                    LOG4_FATAL("Cmd %d %s init error",
                        iCmd, strSoPath.c_str());
                    delete pSo;
                    pSo = NULL;
                }
            }
            else
            {
                LOG4_FATAL("new tagSo() error!");
                delete pCmd;
                dlclose(pHandle);
            }
        }
        return(pSo);
    }

    void OssWorker::UnloadSoAndDeleteCmd(int iCmd)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::map<int, tagSo*>::iterator cmd_iter;
        cmd_iter = m_mapSo.find(iCmd);
        if (cmd_iter != m_mapSo.end())
        {
            delete cmd_iter->second;
            cmd_iter->second = NULL;
            m_mapSo.erase(cmd_iter);
        }
    }

    void OssWorker::LoadModule(loss::CJsonObject& oModuleConf)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::string strModulePath;
        int iVersion = 0;
        bool bIsload = false;
        std::string strSoPath;
        std::map<std::string, tagModule*>::iterator module_iter;
        tagModule* pModule = NULL;
        LOG4_TRACE("oModuleConf.GetArraySize() = %d,oModuleConf(%s)", oModuleConf.GetArraySize(),
            oModuleConf.ToString().c_str());
        for (int i = 0; i < oModuleConf.GetArraySize(); ++i)
        {
            oModuleConf[i].Get("load", bIsload);
            if (bIsload)
            {
                if (oModuleConf[i].Get("url_path", strModulePath) && oModuleConf[i].Get("version", iVersion))
                {
                    LOG4_TRACE("url_path = %s", strModulePath.c_str());
                    module_iter = m_mapModule.find(strModulePath);
                    if (module_iter == m_mapModule.end())
                    {
                        strSoPath = m_strWorkPath + std::string("/") + oModuleConf[i]("so_path");
                        if (0 != access(strSoPath.c_str(), F_OK))
                        {
                            LOG4_WARN("%s not exist!", strSoPath.c_str());
                            continue;
                        }
                        pModule = LoadSoAndGetModule(strModulePath, strSoPath, oModuleConf[i]("entrance_symbol"), iVersion);
                        if (pModule != NULL)
                        {
                            LOG4_INFO("succeed in loading %s with module path \"%s\".",
                                strSoPath.c_str(), strModulePath.c_str());
                            m_mapModule.insert(std::pair<std::string, tagModule*>(strModulePath, pModule));
                        }
                    }
                    else
                    {
                        if (iVersion != module_iter->second->iVersion)
                        {
                            strSoPath = m_strWorkPath + std::string("/") + oModuleConf[i]("so_path");
                            if (0 != access(strSoPath.c_str(), F_OK))
                            {
                                LOG4_WARN("%s not exist!", strSoPath.c_str());
                                continue;
                            }
                            pModule = LoadSoAndGetModule(strModulePath, strSoPath, oModuleConf[i]("entrance_symbol"), iVersion);
                            LOG4_TRACE("%s:%d after LoadSoAndGetCmd", __FILE__, __LINE__);
                            if (pModule != NULL)
                            {
                                LOG4_INFO("succeed in loading %s", strSoPath.c_str());
                                delete module_iter->second;
                                module_iter->second = pModule;
                            }
                        }
                        else
                        {
                            LOG4_INFO("already exist same version:%s", strSoPath.c_str());
                        }
                    }
                }
            }
            else        // 卸载动态库
            {
                if (oModuleConf[i].Get("url_path", strModulePath))
                {
                    strSoPath = m_strWorkPath + std::string("/") + oModuleConf[i]("so_path");
                    UnloadSoAndDeleteModule(strModulePath);
                    LOG4_INFO("unload %s", strSoPath.c_str());
                }
            }
        }
    }

    void OssWorker::ReloadModule(loss::CJsonObject& oUrlPaths)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::map<std::string, tagModule*>::iterator module_iter;
        tagModule* pModule = NULL;
        LOG4_TRACE("oUrlPaths.GetArraySize() = %d,oUrlPaths(%s)", oUrlPaths.GetArraySize(),
            oUrlPaths.ToString().c_str());
        std::string strSoPath;
        std::string strSymbol;
        int iVersion(0);
        for (int i = 0; i < oUrlPaths.GetArraySize(); ++i)
        {
            std::string url_path = oUrlPaths[i].ToString();
            std::string::iterator it = std::remove(url_path.begin(), url_path.end(), '\"');
            url_path.erase(it, url_path.end());
            LOG4_TRACE("url_path = %s", url_path.c_str());
            module_iter = m_mapModule.find(url_path);
            if (module_iter != m_mapModule.end())
            {
                strSoPath = module_iter->second->strSoPath;
                strSymbol = module_iter->second->strSymbol;
                iVersion = module_iter->second->iVersion;
                if (0 != access(strSoPath.c_str(), F_OK))
                {
                    LOG4_WARN("%s not exist!", strSoPath.c_str());
                    continue;
                }
                pModule = LoadSoAndGetModule(url_path, strSoPath, strSymbol, iVersion);
                LOG4_TRACE("%s:%d after ReloadModule", __FILE__, __LINE__);
                if (pModule != NULL)
                {
                    LOG4_INFO("succeed in loading %s", strSoPath.c_str());
                    delete module_iter->second;
                    module_iter->second = pModule;
                }
            }
            else
            {
                LOG4_WARN("no such url_path %s", url_path.c_str());
            }
        }
    }

    tagModule* OssWorker::LoadSoAndGetModule(const std::string& strModulePath, const std::string& strSoPath, const std::string& strSymbol, int iVersion)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        tagModule* pSo = NULL;
        void* pHandle = NULL;
        pHandle = dlopen(strSoPath.c_str(), RTLD_NOW);
        char* dlsym_error = dlerror();
        if (dlsym_error)
        {
            LOG4_FATAL("cannot load dynamic lib %s!", dlsym_error);
            return(pSo);
        }
        CreateCmd* pCreateModule = (CreateCmd*)dlsym(pHandle, strSymbol.c_str());
        dlsym_error = dlerror();
        if (dlsym_error)
        {
            LOG4_FATAL("dlsym error %s!", dlsym_error);
            dlclose(pHandle);
            return(pSo);
        }
        Module* pModule = (Module*)pCreateModule();
        if (pModule != NULL)
        {
            pSo = new tagModule();
            if (pSo != NULL)
            {
                pSo->pSoHandle = pHandle;
                pSo->pModule = pModule;
                pSo->strSoPath = strSoPath;
                pSo->strSymbol = strSymbol;
                pSo->iVersion = iVersion;
                pSo->pModule->SetLogger(&m_oLogger);
                pSo->pModule->SetLabor(this);
                pSo->pModule->SetConfigPath(m_strWorkPath);
                pSo->pModule->SetModulePath(strModulePath);
                if (!pSo->pModule->Init())
                {
                    LOG4_FATAL("Module %s %s init error", strModulePath.c_str(), strSoPath.c_str());
                    delete pSo;
                    pSo = NULL;
                }
            }
            else
            {
                LOG4_FATAL("new tagSo() error!");
                delete pModule;
                dlclose(pHandle);
            }
        }
        return(pSo);
    }

    void OssWorker::UnloadSoAndDeleteModule(const std::string& strModulePath)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::map<std::string, tagModule*>::iterator module_iter;
        module_iter = m_mapModule.find(strModulePath);
        if (module_iter != m_mapModule.end())
        {
            delete module_iter->second;
            module_iter->second = NULL;
            m_mapModule.erase(module_iter);
        }
    }

    bool OssWorker::AddPeriodicTaskEvent()
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        ev_timer* timeout_watcher = new ev_timer();
        if (timeout_watcher == NULL)
        {
            LOG4_ERROR("new timeout_watcher error!");
            return(false);
        }
        ev_timer_init(timeout_watcher, PeriodicTaskCallback, NODE_BEAT + ev_time() - ev_now(m_loop), 0.);
        timeout_watcher->data = (void*)this;
        ev_timer_start(m_loop, timeout_watcher);
        return(true);
    }

    bool OssWorker::AddIoReadEvent(int iFd)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        ev_io* io_watcher = NULL;
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(iFd);
        if (iter != m_mapFdAttr.end())
        {
            if (NULL == iter->second->pIoWatcher)
            {
                io_watcher = new ev_io();
                if (io_watcher == NULL)
                {
                    LOG4_ERROR("new io_watcher error!");
                    return(false);
                }
                tagIoWatcherData* pData = new tagIoWatcherData();
                if (pData == NULL)
                {
                    LOG4_ERROR("new tagIoWatcherData error!");
                    delete io_watcher;
                    return(false);
                }
                pData->iFd = iFd;
                pData->ulSeq = iter->second->ulSeq;
                pData->pWorker = this;
                ev_io_init(io_watcher, IoCallback, iFd, EV_READ);
                io_watcher->data = (void*)pData;
                iter->second->pIoWatcher = io_watcher;
                ev_io_start(m_loop, io_watcher);
            }
            else
            {
                io_watcher = iter->second->pIoWatcher;
                ev_io_stop(m_loop, io_watcher);
                ev_io_set(io_watcher, io_watcher->fd, io_watcher->events | EV_READ);
                ev_io_start(m_loop, io_watcher);
            }
        }
        return(true);
    }

    bool OssWorker::AddIoWriteEvent(int iFd)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        ev_io* io_watcher = NULL;
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(iFd);
        if (iter != m_mapFdAttr.end())
        {
            if (NULL == iter->second->pIoWatcher)
            {
                io_watcher = new ev_io();
                if (io_watcher == NULL)
                {
                    LOG4_ERROR("new io_watcher error!");
                    return(false);
                }
                tagIoWatcherData* pData = new tagIoWatcherData();
                if (pData == NULL)
                {
                    LOG4_ERROR("new tagIoWatcherData error!");
                    delete io_watcher;
                    return(false);
                }
                pData->iFd = iFd;
                pData->ulSeq = iter->second->ulSeq;
                pData->pWorker = this;
                ev_io_init(io_watcher, IoCallback, iFd, EV_WRITE);
                io_watcher->data = (void*)pData;
                iter->second->pIoWatcher = io_watcher;
                ev_io_start(m_loop, io_watcher);
            }
            else
            {
                io_watcher = iter->second->pIoWatcher;
                ev_io_stop(m_loop, io_watcher);
                ev_io_set(io_watcher, io_watcher->fd, io_watcher->events | EV_WRITE);
                ev_io_start(m_loop, io_watcher);
            }
        }
        return(true);
    }

    //bool OssWorker::AddIoErrorEvent(int iFd)
    //{
    //    LOG4_TRACE("%s()", __FUNCTION__);
    //    ev_io* io_watcher = NULL;
    //    std::map<int, tagConnectionAttr*>::iterator iter =  m_mapFdAttr.find(iFd);
    //    if (iter != m_mapFdAttr.end())
    //    {
    //        if (NULL == iter->second->pIoWatcher)
    //        {
    //            io_watcher = new ev_io();
    //            if (io_watcher == NULL)
    //            {
    //                LOG4_ERROR("new io_watcher error!");
    //                return(false);
    //            }
    //            tagIoWatcherData* pData = new tagIoWatcherData();
    //            if (pData == NULL)
    //            {
    //                LOG4_ERROR("new tagIoWatcherData error!");
    //                delete io_watcher;
    //                return(false);
    //            }
    //            pData->iFd = iFd;
    //            pData->ullSeq = iter->second->ullSeq;
    //            pData->pWorker = this;
    //            ev_io_init (io_watcher, IoCallback, iFd, EV_ERROR);
    //            io_watcher->data = (void*)pData;
    //            iter->second->pIoWatcher = io_watcher;
    //            ev_io_start (m_loop, io_watcher);
    //        }
    //        else
    //        {
    //            io_watcher = iter->second->pIoWatcher;
    //            ev_io_set(io_watcher, io_watcher->fd, io_watcher->events | EV_ERROR);
    //        }
    //    }
    //    return(true);
    //}

    bool OssWorker::RemoveIoWriteEvent(int iFd)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        ev_io* io_watcher = NULL;
        std::map<int, tagConnectionAttr*>::iterator iter = m_mapFdAttr.find(iFd);
        if (iter != m_mapFdAttr.end())
        {
            if (NULL != iter->second->pIoWatcher)
            {
                if (iter->second->pIoWatcher->events & EV_WRITE)
                {
                    io_watcher = iter->second->pIoWatcher;
                    ev_io_stop(m_loop, io_watcher);
                    ev_io_set(io_watcher, io_watcher->fd, io_watcher->events & ~EV_WRITE);
                    ev_io_start(m_loop, iter->second->pIoWatcher);
                }
            }
        }
        return(true);
    }

    bool OssWorker::AddIoReadEvent(std::map<int, tagConnectionAttr*>::iterator iter)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        ev_io* io_watcher = NULL;
        if (NULL == iter->second->pIoWatcher)
        {
            io_watcher = new ev_io();
            if (io_watcher == NULL)
            {
                LOG4_ERROR("new io_watcher error!");
                return(false);
            }
            tagIoWatcherData* pData = new tagIoWatcherData();
            if (pData == NULL)
            {
                LOG4_ERROR("new tagIoWatcherData error!");
                delete io_watcher;
                return(false);
            }
            pData->iFd = iter->first;
            pData->ulSeq = iter->second->ulSeq;
            pData->pWorker = this;
            ev_io_init(io_watcher, IoCallback, iter->first, EV_READ);
            io_watcher->data = (void*)pData;
            iter->second->pIoWatcher = io_watcher;
            ev_io_start(m_loop, io_watcher);
        }
        else
        {
            io_watcher = iter->second->pIoWatcher;
            ev_io_stop(m_loop, io_watcher);
            ev_io_set(io_watcher, io_watcher->fd, io_watcher->events | EV_READ);
            ev_io_start(m_loop, io_watcher);
        }
        return(true);
    }

    bool OssWorker::AddIoWriteEvent(std::map<int, tagConnectionAttr*>::iterator iter)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        ev_io* io_watcher = NULL;
        if (NULL == iter->second->pIoWatcher)
        {
            io_watcher = new ev_io();
            if (io_watcher == NULL)
            {
                LOG4_ERROR("new io_watcher error!");
                return(false);
            }
            tagIoWatcherData* pData = new tagIoWatcherData();
            if (pData == NULL)
            {
                LOG4_ERROR("new tagIoWatcherData error!");
                delete io_watcher;
                return(false);
            }
            pData->iFd = iter->first;
            pData->ulSeq = iter->second->ulSeq;
            pData->pWorker = this;
            ev_io_init(io_watcher, IoCallback, iter->first, EV_WRITE);
            io_watcher->data = (void*)pData;
            iter->second->pIoWatcher = io_watcher;
            ev_io_start(m_loop, io_watcher);
        }
        else
        {
            io_watcher = iter->second->pIoWatcher;
            ev_io_stop(m_loop, io_watcher);
            ev_io_set(io_watcher, io_watcher->fd, io_watcher->events | EV_WRITE);
            ev_io_start(m_loop, io_watcher);
        }
        return(true);
    }

    bool OssWorker::RemoveIoWriteEvent(std::map<int, tagConnectionAttr*>::iterator iter)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        ev_io* io_watcher = NULL;
        if (NULL != iter->second->pIoWatcher)
        {
            if (iter->second->pIoWatcher->events & EV_WRITE)
            {
                io_watcher = iter->second->pIoWatcher;
                ev_io_stop(m_loop, io_watcher);
                ev_io_set(io_watcher, io_watcher->fd, io_watcher->events & ~EV_WRITE);
                ev_io_start(m_loop, iter->second->pIoWatcher);
            }
        }
        return(true);
    }

    bool OssWorker::DelEvents(ev_io** io_watcher_addr)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        if (io_watcher_addr == NULL)
        {
            return(false);
        }
        if (*io_watcher_addr == NULL)
        {
            return(false);
        }
        ev_io_stop(m_loop, *io_watcher_addr);
        tagIoWatcherData* pData = (tagIoWatcherData*)(*io_watcher_addr)->data;
        delete pData;
        (*io_watcher_addr)->data = NULL;
        delete (*io_watcher_addr);
        (*io_watcher_addr) = NULL;
        io_watcher_addr = NULL;
        return(true);
    }

    bool OssWorker::AddIoTimeout(int iFd, uint32 ulSeq, tagConnectionAttr* pConnAttr, ev_tstamp dTimeout)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        if (pConnAttr->pTimeWatcher != NULL)
        {
            ev_timer_stop(m_loop, pConnAttr->pTimeWatcher);
            ev_timer_set(pConnAttr->pTimeWatcher, m_dIoTimeout + ev_time() - ev_now(m_loop), 0);
            ev_timer_start(m_loop, pConnAttr->pTimeWatcher);
            return(true);
        }
        else
        {
            ev_timer* timeout_watcher = new ev_timer();
            if (timeout_watcher == NULL)
            {
                LOG4_ERROR("new timeout_watcher error!");
                return(false);
            }
            tagIoWatcherData* pData = new tagIoWatcherData();
            if (pData == NULL)
            {
                LOG4_ERROR("new tagIoWatcherData error!");
                delete timeout_watcher;
                return(false);
            }
            ev_timer_init(timeout_watcher, IoTimeoutCallback, dTimeout + ev_time() - ev_now(m_loop), 0.);
            pData->iFd = iFd;
            pData->ulSeq = ulSeq;
            pData->pWorker = this;
            timeout_watcher->data = (void*)pData;
            if (pConnAttr != NULL)
            {
                pConnAttr->pTimeWatcher = timeout_watcher;
            }
            ev_timer_start(m_loop, timeout_watcher);
            return(true);
        }
    }

    tagConnectionAttr* OssWorker::CreateFdAttr(int iFd, uint32 ulSeq, loss::E_CODEC_TYPE eCodecType)
    {
        LOG4_DEBUG("%s(fd[%d], seq[%u], codec[%d])", __FUNCTION__, iFd, ulSeq, eCodecType);
        std::map<int, tagConnectionAttr*>::iterator fd_attr_iter;
        fd_attr_iter = m_mapFdAttr.find(iFd);
        if (fd_attr_iter == m_mapFdAttr.end())
        {
            tagConnectionAttr* pConnAttr = new tagConnectionAttr();
            if (pConnAttr == NULL)
            {
                LOG4_ERROR("new pConnAttr for fd %d error!", iFd);
                return(NULL);
            }
            pConnAttr->pRecvBuff = new loss::CBuffer();
            if (pConnAttr->pRecvBuff == NULL)
            {
                delete pConnAttr;
                LOG4_ERROR("new pConnAttr->pRecvBuff for fd %d error!", iFd);
                return(NULL);
            }
            pConnAttr->pSendBuff = new loss::CBuffer();
            if (pConnAttr->pSendBuff == NULL)
            {
                delete pConnAttr;
                LOG4_ERROR("new pConnAttr->pSendBuff for fd %d error!", iFd);
                return(NULL);
            }
            pConnAttr->pWaitForSendBuff = new loss::CBuffer();
            if (pConnAttr->pWaitForSendBuff == NULL)
            {
                delete pConnAttr;
                LOG4_ERROR("new pConnAttr->pWaitForSendBuff for fd %d error!", iFd);
                return(NULL);
            }
            pConnAttr->pRemoteAddr = (char*)malloc(32);
            if (pConnAttr->pRemoteAddr == NULL)
            {
                delete pConnAttr;
                LOG4_ERROR("new pConnAttr->pRemoteAddr for fd %d error!", iFd);
                return(NULL);
            }
            pConnAttr->pClientData = new loss::CBuffer();
            if (pConnAttr->pClientData == NULL)
            {
                delete pConnAttr;
                LOG4_ERROR("new pConnAttr->pClientData for fd %d error!", iFd);
                return(NULL);
            }
            pConnAttr->dActiveTime = ev_now(m_loop);
            pConnAttr->ulSeq = ulSeq;
            pConnAttr->eCodecType = eCodecType;
            m_mapFdAttr.insert(std::pair<int, tagConnectionAttr*>(iFd, pConnAttr));
            return(pConnAttr);
        }
        else
        {
            LOG4_ERROR("fd %d is exist!", iFd);
            return(NULL);
        }
    }

    bool OssWorker::DestroyConnect(std::map<int, tagConnectionAttr*>::iterator iter, bool bMsgShellNotice)
    {
        if (iter == m_mapFdAttr.end())
        {
            return(false);
        }
        LOG4_DEBUG("%s disconnect, identify %s", iter->second->pRemoteAddr, iter->second->strIdentify.c_str());
        int iResult = close(iter->first);
        if (0 != iResult)
        {
            if (EINTR != errno)
            {
                LOG4_ERROR("close(%d) failed, result %d and errno %d", iter->first, iResult, errno);
                return(false);
            }
            else
            {
                LOG4_DEBUG("close(%d) failed, result %d and errno %d", iter->first, iResult, errno);
            }
        }
        tagMsgShell stMsgShell;
        stMsgShell.iFd = iter->first;
        stMsgShell.ulSeq = iter->second->ulSeq;
        std::map<int, uint32>::iterator inner_iter = m_mapInnerFd.find(iter->first);
        if (inner_iter != m_mapInnerFd.end())
        {
            LOG4_TRACE("%s() m_mapInnerFd.size() = %u", __FUNCTION__, m_mapInnerFd.size());
            m_mapInnerFd.erase(inner_iter);
        }
        std::map<int32, std::list<uint32> >::iterator http_step_iter = m_mapHttpAttr.find(stMsgShell.iFd);
        if (http_step_iter != m_mapHttpAttr.end())
        {
            m_mapHttpAttr.erase(http_step_iter);
        }
        DelMsgShell(iter->second->strIdentify);
        if (bMsgShellNotice)
        {
            MsgShellNotice(stMsgShell, iter->second->strIdentify, iter->second->pClientData);
        }
        DelEvents(&(iter->second->pIoWatcher));
        delete iter->second;
        iter->second = NULL;
        m_mapFdAttr.erase(iter);
        return(true);
    }

    void OssWorker::MsgShellNotice(const tagMsgShell& stMsgShell, const std::string& strIdentify, loss::CBuffer* pClientData)
    {
        LOG4_TRACE("%s()", __FUNCTION__);
        std::map<int32, tagSo*>::iterator cmd_iter;
        cmd_iter = m_mapSo.find(CMD_REQ_DISCONNECT);
        if (cmd_iter != m_mapSo.end() && cmd_iter->second != NULL)
        {
            MsgHead oMsgHead;
            MsgBody oMsgBody;
            oMsgBody.set_body(strIdentify);
            if (pClientData != NULL)
            {
                if (pClientData->ReadableBytes() > 0)
                {
                    oMsgBody.set_additional(pClientData->GetRawReadBuffer(), pClientData->ReadableBytes());
                }
            }
            oMsgHead.set_cmd(CMD_REQ_DISCONNECT);
            oMsgHead.set_seq(GetSequence());
            oMsgHead.set_msgbody_len(oMsgBody.ByteSize());
            cmd_iter->second->pCmd->AnyMessage(stMsgShell, oMsgHead, oMsgBody);
        }
    }

    bool OssWorker::Dispose(const tagMsgShell& stMsgShell,
        const MsgHead& oInMsgHead, const MsgBody& oInMsgBody,
        MsgHead& oOutMsgHead, MsgBody& oOutMsgBody)
    {
        LOG4_DEBUG("%s(cmd %u, seq %lu)", __FUNCTION__, oInMsgHead.cmd(), oInMsgHead.seq());
        OrdinaryResponse oRes;
        oOutMsgHead.Clear();
        oOutMsgBody.Clear();
        LOG4_DEBUG("gc_uiCmdReq %d : oInMsgHead %d", gc_uiCmdReq, oInMsgHead.cmd());
        if (gc_uiCmdReq & oInMsgHead.cmd())    // 新请求
        {
            std::map<int32, Cmd*>::iterator cmd_iter;
            cmd_iter = m_mapCmd.find(gc_uiCmdBit & oInMsgHead.cmd());
            if (cmd_iter != m_mapCmd.end() && cmd_iter->second != NULL)
            {
                LOG4_DEBUG("cmd_iter is end...");
                cmd_iter->second->AnyMessage(stMsgShell, oInMsgHead, oInMsgBody);
            }
            else
            {
                LOG4_DEBUG("cmd is not end");
                std::map<int, tagSo*>::iterator cmd_so_iter;
                cmd_so_iter = m_mapSo.find(gc_uiCmdBit & oInMsgHead.cmd());
                if (cmd_so_iter != m_mapSo.end() && cmd_so_iter->second != NULL)
                {
                    LOG4_DEBUG("cmd2 is not end");
                    cmd_so_iter->second->pCmd->AnyMessage(stMsgShell, oInMsgHead, oInMsgBody);
                }
                else        // 没有对应的cmd，是需由接入层转发的请求
                {
                    if (CMD_REQ_SET_LOG_LEVEL == oInMsgHead.cmd())
                    {
                        LogLevel oLogLevel;
                        if (!oLogLevel.ParseFromString(oInMsgBody.body()))
                        {
                            LOG4_WARN("failed to parse oLogLevel,body(%s)", oInMsgBody.body().c_str());
                        }
                        else
                        {
                            LOG4_INFO("CMD_REQ_SET_LOG_LEVEL:log level set to %d", oLogLevel.log_level());
                            m_oLogger.setLogLevel(oLogLevel.log_level());
                        }
                    }
                    else if (CMD_REQ_RELOAD_SO == oInMsgHead.cmd())
                    {
                        loss::CJsonObject oSoConfJson;
                        if (!oSoConfJson.Parse(oInMsgBody.body()))
                        {
                            LOG4_WARN("failed to parse oSoConfJson:(%s)", oInMsgBody.body().c_str());
                        }
                        else
                        {
                            LOG4_INFO("CMD_REQ_RELOAD_SO:update so conf to oSoConfJson(%s)", oSoConfJson.ToString().c_str());
                            LoadSo(oSoConfJson);
                        }
                    }
                    else if (CMD_REQ_RELOAD_MODULE == oInMsgHead.cmd())
                    {
                        loss::CJsonObject oModuleConfJson;
                        if (!oModuleConfJson.Parse(oInMsgBody.body()))
                        {
                            LOG4_WARN("failed to parse oModuleConfJson:(%s)", oInMsgBody.body().c_str());
                        }
                        else
                        {
                            LOG4_INFO("CMD_REQ_RELOAD_MODULE:update module conf to oModuleConfJson(%s)", oModuleConfJson.ToString().c_str());
                            LoadModule(oModuleConfJson);
                        }
                    }
                    else if (CMD_REQ_RELOAD_LOGIC_CONFIG == oInMsgHead.cmd())
                    {
                        loss::CJsonObject oConfJson;
                        if (!oConfJson.Parse(oInMsgBody.body()))
                        {
                            LOG4_WARN("failed to parse oConfJson:(%s)", oInMsgBody.body().c_str());
                        }
                        else
                        {
                            loss::CJsonObject oCmds;
                            if (oConfJson.Get("cmd", oCmds))
                            {
                                LOG4_INFO("reload so conf to oCmds(%s)", oCmds.ToString().c_str());
                                ReloadSo(oCmds);
                            }
                            loss::CJsonObject oUrlPaths;
                            if (oConfJson.Get("url_path", oUrlPaths))
                            {
                                LOG4_INFO("reload module conf to oUrlPaths(%s)", oUrlPaths.ToString().c_str());
                                ReloadModule(oUrlPaths);
                            }
                        }
                    }
                    else
                    {
#ifdef NODE_TYPE_ACCESS
                        std::map<int, uint32>::iterator inner_iter = m_mapInnerFd.find(stMsgShell.iFd);
                        if (inner_iter != m_mapInnerFd.end())   // 内部服务往客户端发送  if (std::string("0.0.0.0") == strFromIp)
                        {
                            cmd_so_iter = m_mapSo.find(CMD_REQ_TO_CLIENT);
                            if (cmd_so_iter != m_mapSo.end())
                            {
                                LOG4_WARN("Node_Type_ACCESS  gc_uiCmdReq  cmd_iter->second->AnyMseege: %d", m_mapCmd.size());
                                cmd_so_iter->second->pCmd->AnyMessage(stMsgShell, oInMsgHead, oInMsgBody);
                            }
                            else
                            {
                                LOG4_DEBUG("%s, %d CMD_RSP_SYS_ERROR\n", __FUNCTION__, __LINE__);
                                snprintf(m_pErrBuff, gc_iErrBuffLen, " no handler to dispose cmd %u!", oInMsgHead.cmd());
                                LOG4_ERROR(m_pErrBuff);
                                oRes.set_err_no(ERR_UNKNOWN_CMD);
                                oRes.set_err_msg(m_pErrBuff);
                                oOutMsgBody.set_body(oRes.SerializeAsString());
                                oOutMsgHead.set_cmd(CMD_RSP_SYS_ERROR);
                                oOutMsgHead.set_seq(oInMsgHead.seq());
                                oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
                            }
                        }
                        else
                        {
                            cmd_so_iter = m_mapSo.find(CMD_REQ_FROM_CLIENT);
                            if (cmd_so_iter != m_mapSo.end() && cmd_so_iter->second != NULL)
                            {
                                cmd_so_iter->second->pCmd->AnyMessage(stMsgShell, oInMsgHead, oInMsgBody);
                            }
                            else
                            {
                                LOG4_DEBUG("%s, %d CMD_RSP_SYS_ERROR \n", __FUNCTION__, __LINE__);
                                snprintf(m_pErrBuff, gc_iErrBuffLen, " no handler to dispose cmd %u!", oInMsgHead.cmd());
                                LOG4_ERROR(m_pErrBuff);
                                oRes.set_err_no(ERR_UNKNOWN_CMD);
                                oRes.set_err_msg(m_pErrBuff);
                                oOutMsgBody.set_body(oRes.SerializeAsString());
                                oOutMsgHead.set_cmd(CMD_RSP_SYS_ERROR);
                                oOutMsgHead.set_seq(oInMsgHead.seq());
                                oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
                            }
                        }
#else
                        LOG4_DEBUG("%s, %d , %d CMD_RSP_SYS_ERROR\n", __FUNCTION__, __LINE__, oInMsgHead.cmd());
                        snprintf(m_pErrBuff, gc_iErrBuffLen, " no handler to dispose cmd %u!", oInMsgHead.cmd());
                        LOG4_ERROR(m_pErrBuff);
                        oRes.set_err_no(ERR_UNKNOWN_CMD);
                        oRes.set_err_msg(m_pErrBuff);
                        oOutMsgBody.set_body(oRes.SerializeAsString());
                        oOutMsgHead.set_cmd(CMD_RSP_SYS_ERROR);
                        oOutMsgHead.set_seq(oInMsgHead.seq());
                        oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
#endif
                    }
                }
            }
        }
        else    // 回调
        {
            std::map<uint32, Step*>::iterator step_iter;
            step_iter = m_mapCallbackStep.find(oInMsgHead.seq());
            if (step_iter != m_mapCallbackStep.end())   // 步骤回调
            {
                LOG4_TRACE("receive message, cmd = %d",
                    oInMsgHead.cmd());
                if (step_iter->second != NULL)
                {
                    //                if (oss::CMD_RSP_SYS_ERROR == oInMsgHead.cmd())   框架层不应截止系统错误，业务层会有逻辑，对于无逻辑的也要求加上对系统错误处理
                    //                {
                    //                    OrdinaryResponse oError;
                    //                    if (oError.ParseFromString(oInMsgBody.body()))
                    //                    {
                    //                        LOG4_ERROR("cmd[%u] seq[%u] callback error %d: %s!",
                    //                                        oInMsgHead.cmd(), oInMsgHead.seq(),
                    //                                        oError.err_no(), oError.err_msg().c_str());
                    //                        DeleteCallback(step_iter->second);
                    //                        return(false);
                    //                    }
                    //                }
                    E_CMD_STATUS eResult;
                    step_iter->second->SetActiveTime(ev_now(m_loop));
                    LOG4_TRACE("cmd %u, seq %u, step_seq %u, step addr 0x%x, active_time %lf",
                        oInMsgHead.cmd(), oInMsgHead.seq(), step_iter->second->GetSequence(),
                        step_iter->second, step_iter->second->GetActiveTime());
                    eResult = step_iter->second->Callback(stMsgShell, oInMsgHead, oInMsgBody);
                    //                LOG4_TRACE("cmd %u, seq %u, step_seq %u, step addr 0x%x",
                    //                                oInMsgHead.cmd(), oInMsgHead.seq(), step_iter->second->GetSequence(),
                    //                                step_iter->second);
                    if (eResult != STATUS_CMD_RUNNING)
                    {
                        DeleteCallback(step_iter->second);
                    }
                }
            }
            else
            {
                snprintf(m_pErrBuff, gc_iErrBuffLen, "no callback or the callback for seq %lu had been timeout!", oInMsgHead.seq());
                LOG4_WARN(m_pErrBuff);
                //            oRes.set_err_no(ERR_NO_CALLBACK);
                //            oRes.set_err_msg(m_pErrBuff);
                //            oOutMsgBody.set_body(oRes.SerializeAsString());
                //            oOutMsgHead.set_cmd(oInMsgHead.cmd() + 1);
                //            oOutMsgHead.set_seq(oInMsgHead.seq());
                //            oOutMsgHead.set_msgbody_len(oOutMsgBody.ByteSize());
            }
        }
        return(true);
    }

    bool OssWorker::Dispose(const tagMsgShell& stMsgShell,
        const HttpMsg& oInHttpMsg, HttpMsg& oOutHttpMsg)
    {
        LOG4_DEBUG("%s() oInHttpMsg.type() = %d, oInHttpMsg.path() = %s",
            __FUNCTION__, oInHttpMsg.type(), oInHttpMsg.path().c_str());
        oOutHttpMsg.Clear();
        if (HTTP_REQUEST == oInHttpMsg.type())    // 新请求
        {
            std::map<std::string, tagModule*>::iterator module_iter;
            module_iter = m_mapModule.find(oInHttpMsg.path());
            if (module_iter == m_mapModule.end())
            {
                module_iter = m_mapModule.find("/im/switch");
                if (module_iter == m_mapModule.end())
                {
                    snprintf(m_pErrBuff, gc_iErrBuffLen, "no module to dispose %s!", oInHttpMsg.path().c_str());
                    LOG4_ERROR(m_pErrBuff);
                    oOutHttpMsg.set_type(HTTP_RESPONSE);
                    oOutHttpMsg.set_status_code(404);
                    oOutHttpMsg.set_http_major(oInHttpMsg.http_major());
                    oOutHttpMsg.set_http_minor(oInHttpMsg.http_minor());
                }
                else
                {
                    module_iter->second->pModule->AnyMessage(stMsgShell, oInHttpMsg);
                }
            }
            else
            {
                module_iter->second->pModule->AnyMessage(stMsgShell, oInHttpMsg);
            }
        }
        else
        {
            std::map<int32, std::list<uint32> >::iterator http_step_iter = m_mapHttpAttr.find(stMsgShell.iFd);
            if (http_step_iter == m_mapHttpAttr.end())
            {
                LOG4_ERROR("no callback for http response from %s!", oInHttpMsg.url().c_str());
            }
            else
            {
                if (http_step_iter->second.begin() != http_step_iter->second.end())
                {
                    std::map<uint32, Step*>::iterator step_iter;
                    step_iter = m_mapCallbackStep.find(*http_step_iter->second.begin());
                    if (step_iter != m_mapCallbackStep.end() && step_iter->second != NULL)   // 步骤回调
                    {
                        E_CMD_STATUS eResult;
                        step_iter->second->SetActiveTime(ev_now(m_loop));
                        eResult = ((HttpStep*)step_iter->second)->Callback(stMsgShell, oInHttpMsg);
                        if (eResult != STATUS_CMD_RUNNING)
                        {
                            DeleteCallback(step_iter->second);
                        }
                    }
                    else
                    {
                        snprintf(m_pErrBuff, gc_iErrBuffLen, "no callback or the callback for seq %lu had been timeout!",
                            *http_step_iter->second.begin());
                        LOG4_WARN(m_pErrBuff);
                    }
                    http_step_iter->second.erase(http_step_iter->second.begin());
                }
                else
                {
                    LOG4_ERROR("no callback for http response from %s!", oInHttpMsg.url().c_str());
                }
            }
        }
        return(true);
    }

} /* namespace oss */

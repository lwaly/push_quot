/*******************************************************************************
* Project:  行情数据推送服务
* @file     pushdata.cpp
* @brief
* @author
* @date:    2018年6月27日
* @note
* Modify history:
******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <memory>
#include <stdio.h>  
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <iostream>

#include "pushdata.hpp"
#include "ev.h"
#include "CBuffer.hpp"
#include "protocol.pb.h"

#define BIT_ITME 1   ///心跳间隔30秒
#define READ_BUFFER_SIZE 8192  //接收的最大字节数
const char* heartbeat = "00000"; //心跳
struct ev_loop *loop = NULL;
ev_timer* timeout_watcher;
ev_io* io_watcher;
char *m_buffer;   //接收数据缓冲区
size_t m_buffer_len;  //接收数据缓冲区长度  
struct conndata
{
    int lastFd;
    string connIp;
    int connPort;
    ev_tstamp lastSendTime;
    struct ev_loop *ploop;
    CallbackMethod func;
    string errinfo;
} *pdata;



string& gerError()
{
    return pdata->errinfo;
}

static int connectSvr(string ip, int port)
{
    struct sockaddr_in stAddr;
    bzero(&stAddr, sizeof(stAddr));
    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons(port);
    stAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    int Fd = socket(AF_INET, SOCK_STREAM, 0);
    struct timeval timeout = { 5, 0 };
    setsockopt(Fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));
    if (connect(Fd, (struct sockaddr*)&stAddr, sizeof(stAddr)) < 0)
    {
        close(Fd);
        return -1;
    }
    return Fd;
}

static void timeout_cb(EV_P_ ev_timer *timewatcher, int revents)
{
    conndata* tempdata = (conndata*)timewatcher->data;
    ev_tstamp after = tempdata->lastSendTime - ev_now(tempdata->ploop) + BIT_ITME;
    //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
    if (after > 0)    // IO在定时时间内被重新刷新过，重新设置定时器
    {
        //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
        ev_timer_stop(tempdata->ploop, timewatcher);
        ev_timer_set(timewatcher, after + ev_time() - ev_now(tempdata->ploop), 0);
        ev_timer_start(tempdata->ploop, timewatcher);
    }
    else    // IO已超时，发送心跳检查   
    {
        //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
        int iErrno = 0;
        protocol::MsgBody oMsgBody;
        protocol::MsgHead oMsgHead;
        CBuffer oWriteBuff;

        oMsgHead.set_cmd(2001);
        oMsgHead.set_seq(1);
        oMsgHead.set_msgbody_len(0);

        oWriteBuff.Write(oMsgHead.SerializeAsString().c_str(), oMsgHead.ByteSize());

        int iWriteSize = oWriteBuff.WriteFD(tempdata->lastFd, iErrno);

        ////printf("iWritesize=%d head=%s body=%s\n", iWriteSize, oMsgHead.SerializeAsString().c_str(), oMsgBody.SerializeAsString().c_str());
        if (iWriteSize < 0)
        {
            if (EAGAIN != iErrno && EINTR != iErrno)    // 对非阻塞socket而言，EAGAIN不是一种错误;EINTR即errno为4，错误描述Interrupted system call，操作也应该继续。
            {
                close(tempdata->lastFd);
                tempdata->lastFd = connectSvr(tempdata->connIp, tempdata->connPort);
                if (tempdata->lastFd < 0)
                {
                    tempdata->errinfo = "fail to reconnect";
                }
                else
                {
                    tempdata->errinfo = "reconnect successful";
                }
                ev_timer_stop(tempdata->ploop, timewatcher);
                ev_timer_set(timewatcher, BIT_ITME, 0);
                ev_timer_start(tempdata->ploop, timewatcher);
                return;
            }
            //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
        }
        //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
        ev_timer_stop(tempdata->ploop, timewatcher);
        ev_timer_set(timewatcher, BIT_ITME, 0.0);
        ev_timer_start(tempdata->ploop, timewatcher);

        //ev_timer_init(timeout_watcher, timeout_cb, BIT_ITME, 0);
        //timeout_watcher->data = (void*)tdata;
        //ev_timer_start(loop, timeout_watcher);
    }
}

int my_atoi(char* pstr)
{
    int Ret_Integer = 0;
    int Integer_sign = 1;

    /*
    * 判断指针是否为空
    */
    if (pstr == NULL)
    {
        return 0;
    }

    /*
    * 把数字字符串逐个转换成整数，并把最后转换好的整数赋给Ret_Integer
    */
    while (*pstr >= '0' && *pstr <= '9')
    {
        Ret_Integer = Ret_Integer * 10 + int(*pstr - '0');
        pstr++;
    }

    return Ret_Integer;
}


/*
Converts an int or long into a character string
将一个整数转化为字符串
*/
void my_itoa(int n, char s[])
{
    int i, j, sign;
    char cTemp[5] = { '\0' };
    i = 0;
    do{
        cTemp[i++] = n % 10 + '0';    //取下一个数字
    } while ((n /= 10) > 0);      //循环相除

    cTemp[i] = '\0';
    for (j = i - 1; j >= 0; j--)        //生成的数字是逆序的，所以要逆序输出
    {
        s[i - 1 - j] = cTemp[j];
    }
}

static void IoRedata(EV_P_ ev_io *watcher, int revents)
{
    if (revents & EV_READ)
    {
        conndata* tempdata = (conndata*)watcher->data;
        bool isreadsuccess = false;
        int bytes_read = 0;

        int m_read_idx = 0;
        CBuffer oReadBuff;
        int iErrno = 0;
        protocol::MsgBody oMsgBody;
        protocol::MsgHead oMsgHead;
        oMsgHead.set_cmd(0);
        oMsgHead.set_seq(0);
        oMsgHead.set_msgbody_len(0);
        int iHeadSize = oMsgHead.ByteSize();
        while (true)
        {
            //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
            int iReadSize = oReadBuff.ReadFD(tempdata->lastFd, iErrno);
            //printf("func=%s line=%d  iReadSize=%d error=%d\n", __FUNCTION__, __LINE__, iReadSize, iErrno);
            if (iReadSize < 0)
            {
                if (EAGAIN != iErrno && EINTR != iErrno)    // 对非阻塞socket而言，EAGAIN不是一种错误;EINTR即errno为4，错误描述Interrupted system call，操作也应该继续。
                {
                    //printf("read from fd %d error %d: %s\n", tempdata->lastFd, errno, strerror(errno));
                }
                break;;
            }

            //printf("func=%s line=%d iHeadSize=%d\n", __FUNCTION__, __LINE__, iHeadSize);
            while (oReadBuff.ReadableBytes() >= iHeadSize)
            {
                //printf("func=%s line=%d read bytes=%d write=%d %s\n", __FUNCTION__, __LINE__, oReadBuff.GetReadIndex(), oReadBuff.WriteableBytes(), oReadBuff.GetRawReadBuffer());
                if (oMsgHead.ParseFromArray(oReadBuff.GetRawReadBuffer(), iHeadSize))
                {
                    //printf("func=%s line%d cmd=%d\n", __FUNCTION__, __LINE__, oMsgHead.cmd());
                    if (oReadBuff.ReadableBytes() >= iHeadSize + oMsgHead.msgbody_len())
                    {
                        //char c[64] = { '\0' };
                        //memcpy(c, oReadBuff.GetRawReadBuffer() + iHeadSize, oMsgHead.msgbody_len());
                        //printf("func=%s line=%d %d\n", __FUNCTION__, __LINE__, oMsgHead.msgbody_len());
                        if (0 == oMsgHead.msgbody_len())
                        {
                            //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
                            oReadBuff.SkipBytes(iHeadSize);
                        }
                        else if (oMsgBody.ParseFromArray(oReadBuff.GetRawReadBuffer() + iHeadSize, oMsgHead.msgbody_len()))
                        {
                            //string str= oMsgBody.body();
                            //                             char c[64] = { '\0' };
                            //                             memcpy(c, oMsgBody.body().c_str(), oMsgBody.body().size());
                            //                             printf("func=%s line=%d %s %d\n", __FUNCTION__, __LINE__, c, oMsgBody.body().size());
                            //                             protocol::QuotConfigGetRes rsp;
                            //                             if (rsp.ParseFromArray(c, oMsgBody.body().size()))
                            //                             {
                            //                                 tempdata->func(const_cast<char *>(rsp.supported_resolutions().c_str()));
                            //                                 printf("supported_resolutions=%s\n", rsp.supported_resolutions().c_str());
                            //                             }
                            char c[64] = { '\0' };
                            memcpy(c, oMsgBody.body().c_str(), oMsgBody.body().size());
                            //                             protocol::QuotConfigGetRes rsp;
                            //                             if (rsp.ParseFromArray(c, oMsgBody.body().size()))
                            //                             {
                            //                                 //tempdata->func(const_cast<char *>(rsp.supported_resolutions().c_str()));
                            //                                 printf("func=%s line=%d supported_resolutions=%s\n", __FUNCTION__, __LINE__, rsp.supported_resolutions().c_str());
                            //                             }
                            tempdata->func(c, oMsgBody.body().size());
                            oReadBuff.SkipBytes(iHeadSize + oMsgBody.ByteSize());
                            //oReadBuff.Compact(65535);
                        }
                        else
                        {
                            //printf("oMsgBody.ParseFromArray() failed!\n");
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    //printf("oMsgHead.ParseFromArray() failed!\n");
                }
            }
        }
    }
}

void StopSvr()
{
    ev_timer_stop(pdata->ploop, timeout_watcher);
    ev_io_stop(pdata->ploop, io_watcher);

    if (m_buffer != NULL)
    {
        free(m_buffer);
        m_buffer = NULL;
    }

    if (pdata->lastFd > 0)
    {
        close(pdata->lastFd);
    }
    if (pdata != NULL)
    {
        delete pdata;
        pdata = NULL;
    }
    if (timeout_watcher != NULL)
    {
        delete timeout_watcher;
        timeout_watcher = NULL;
    }
    if (io_watcher != NULL)
    {
        delete io_watcher;
        io_watcher = NULL;
    }
    if (loop != NULL)
    {
        ev_loop_destroy(loop);
        loop = NULL;
    }
}

static void* ThreadFunc(void *arg)
{
    conndata *tdata = (conndata*)arg;
    loop = EV_DEFAULT;
    tdata->ploop = loop;
    tdata->lastSendTime = ev_now(loop);

    //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
    timeout_watcher = new ev_timer();
    ev_timer_init(timeout_watcher, timeout_cb, BIT_ITME, 0.0);
    timeout_watcher->data = (void*)tdata;
    ev_timer_start(loop, timeout_watcher);

    io_watcher = new ev_io();
    ev_io_init(io_watcher, IoRedata, tdata->lastFd, EV_READ);
    io_watcher->data = (void*)tdata;
    ev_io_start(loop, io_watcher);

    ev_run(loop, 0);
}

int StartSvr(string hostip, int port, CallbackMethod CallAppFunc)
{
    signal(SIGPIPE, SIG_IGN);
    string strHost = hostip;
    int iPort = port;

    pdata = new conndata();
    pdata->connIp = strHost;
    pdata->connPort = iPort;
    pdata->func = CallAppFunc;
    pdata->errinfo = "success";

    m_buffer_len = READ_BUFFER_SIZE;
    m_buffer = (char*)malloc(READ_BUFFER_SIZE);
    //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
    if (NULL == m_buffer)
    {
        pdata->errinfo = "failed to malloc";
        return -1;
    }
    memset(m_buffer, 0, sizeof(m_buffer));
    //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
    int connfd = connectSvr(strHost, iPort);
    if (connfd < 0)
    {
        pdata->errinfo = "failed to connect";
        return -1;
    }
    pdata->lastFd = connfd;
    //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
    pthread_t worker;
    if (pthread_create(&worker, NULL, ThreadFunc, (void *)pdata) != 0)
    {
        pdata->errinfo = "failed to create thread";
        return -1;
    }
    //printf("func=%s line=%d\n", __FUNCTION__, __LINE__);
    pthread_detach(worker);

    return 0;
}

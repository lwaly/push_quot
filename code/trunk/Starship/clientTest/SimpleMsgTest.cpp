#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <iostream>
#include "CBuffer.hpp"
#include "msg.pb.h"
#include "protocol.pb.h"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("%s ip port \"cmd\" \"msg\", now argc %d\n", argv[0], argc);
        exit(1);
    }
    int iConnectResult = 0;
    int iErrno = 0;
    struct sockaddr_in stAddr;
    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons(atoi(argv[2]));
    stAddr.sin_addr.s_addr = inet_addr(argv[1]);
    bzero(&(stAddr.sin_zero), 8);
    int iWriteIdx = 0;
    int iWriteSize = 0;
    int iReadSize = 0;
    loss::CBuffer oReadBuff;
    loss::CBuffer oWriteBuff;
    MsgBody oMsgBody;
    OrdinaryResponse oRsp;
    MsgHead oMsgHead;
    oMsgHead.set_cmd(0);
    oMsgHead.set_msgbody_len(0);
    oMsgHead.set_seq(0);
    bool bResult = false;
    int iReadWriteSize = 0;
    int iHeadSize = oMsgHead.ByteSize();
    printf("iHeadSize=%d\n", iHeadSize);
    int iSockFd = socket(AF_INET, SOCK_STREAM, 0);
    iConnectResult = connect(iSockFd, (struct sockaddr*)&stAddr, sizeof(struct sockaddr));

    if (iConnectResult == 0)
    {
        while (true)
        {
            protocol::QuotConfigGetReq req;

            oMsgBody.set_body(req.SerializeAsString());
            oMsgBody.set_session_id(1);

            oMsgHead.set_cmd(2001);
            oMsgHead.set_seq(1);
            oMsgHead.set_msgbody_len(0);

            oWriteBuff.Write(oMsgHead.SerializeAsString().c_str(), oMsgHead.ByteSize());
            //oWriteBuff.Write(oMsgBody.SerializeAsString().c_str(), oMsgBody.ByteSize());

            iWriteSize = oWriteBuff.WriteFD(iSockFd, iErrno);

            //printf("iWritesize=%d head=%s body=%s\n", iWriteSize, oMsgHead.SerializeAsString().c_str(), oMsgBody.SerializeAsString().c_str());
            if (iWriteSize < 0)
            {
                if (EAGAIN != iErrno && EINTR != iErrno)    // �Է�����socket���ԣ�EAGAIN����һ�ִ���;EINTR��errnoΪ4����������Interrupted system call������ҲӦ�ü�����
                {
                    printf("write to fd %d error %d: %s\n", iSockFd, errno, strerror(errno));
                }
            }
            else
            {
            }
            oWriteBuff.Compact(65535);

            iReadSize = oReadBuff.ReadFD(iSockFd, iErrno);
            printf("line%d iReadSize=%d\n", __LINE__, iReadSize);
            if (iReadSize < 0)
            {
                if (EAGAIN != iErrno && EINTR != iErrno)
                {
                    printf("read from fd %d error %d: %s\n", iSockFd, errno, strerror(errno));
                }
            }
            else
            {
            }

            while (oReadBuff.ReadableBytes() >= iHeadSize)
            {
                printf("line=%d read bytes=%d write=%d %s\n", __LINE__, oReadBuff.GetReadIndex(), oReadBuff.WriteableBytes(), oReadBuff.GetRawReadBuffer());
                if (oMsgHead.ParseFromArray(oReadBuff.GetRawReadBuffer(), iHeadSize))
                {
                    printf("line%d cmd=%d\n", __LINE__, oMsgHead.cmd());
                    if (oReadBuff.ReadableBytes() >= iHeadSize + oMsgHead.msgbody_len())
                    {
                        if (0==oMsgHead.msgbody_len())
                        {
                            printf("line%d\n", __LINE__);
                            oReadBuff.SkipBytes(iHeadSize);
                        }
                        else if (oMsgBody.ParseFromArray(oReadBuff.GetRawReadBuffer() + iHeadSize, oMsgHead.msgbody_len()))
                        {
                            printf("%s\n", oMsgBody.body().c_str());
                            protocol::QuotConfigGetRes rsp;
                            if (rsp.ParseFromString(oMsgBody.body()))
                            {
                                printf("supported_resolutions=%s\n", rsp.supported_resolutions().c_str());
                            }
                            oReadBuff.SkipBytes(iHeadSize + oMsgBody.ByteSize());
                            oReadBuff.Compact(65535);
                        }
                        else
                        {
                            printf("oMsgBody.ParseFromArray() failed!\n");
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    printf("oMsgHead.ParseFromArray() failed!\n");
                }
                sleep(1);
            }
            sleep(1);
        }
    }
    else
    {
        printf("connect error %d!\n", errno);
    }
    close(iSockFd);

    return(0);
}



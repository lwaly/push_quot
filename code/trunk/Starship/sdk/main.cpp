#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pushdata.hpp"
#include "protocol.pb.h"

void CallBack(char str[], int lengtn);
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("%s ip port \"cmd\" \"msg\", now argc %d\n", argv[0], argc);
        return -1;
    }
    StartSvr(argv[1], atoi(argv[2]), CallBack);
    while (true)
    {
        sleep(1000);
    }
    StopSvr();
    return 0;
}

void CallBack(char str[],int lengtn)
{
    static int index = 0;
    index++;
    protocol::QuotConfigGetRes rsp;
    if (rsp.ParseFromArray(const_cast<const char *>(str), lengtn))
    {
        //tempdata->func(const_cast<char *>(rsp.supported_resolutions().c_str()));
        printf("func=%s line=%d supported_resolutions=%s\n", __FUNCTION__, __LINE__, rsp.supported_resolutions().c_str());
    }
    printf("func=%s line=%d %s %d length=%d\n", __FUNCTION__, __LINE__, str, index, lengtn);
}

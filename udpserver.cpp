#include "mysock.h"

#include "malloc.h"

int f_udpserver()
{
    CSockUdpServer udpserver;
    CSockUdpClient udpclient;
    udpserver.Listen();

    while(true)
    {
        int BufLen = 8;
        char *buf = (char *)malloc(BufLen);
        bzero(buf, BufLen);
        int len;

        while((len = udpserver.RecvFrom(udpserver.GetSock(), buf, BufLen, MSG_PEEK, &udpclient)) == BufLen)
        {
            buf = (char *)realloc(buf, BufLen*2);
            bzero(buf, BufLen);
            BufLen *= 2;
        }
        bzero(buf, BufLen);
        len = udpserver.RecvFrom(udpserver.GetSock(), buf, BufLen, 0, &udpclient);
        if (len > 0)
        {
            cout << "recvfrom addr : " << udpclient.GetAddr() << endl;
            cout << "recvfrom port : " << udpclient.GetPort() << endl;
            cout << "recvfrom len : " << len << endl;
            cout << "recvfrom data : " << buf << endl;
            cout << endl;
        }
        free(buf);
        usleep(200);

    }


    return 0;
}

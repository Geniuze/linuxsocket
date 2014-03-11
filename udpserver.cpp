#include "mysock.h"

int f_udpserver()
{
    CSockUdpServer udpserver;
    CSockUdpClient udpclient;
    udpserver.Listen();

    while(true)
    {
        char buf[1024] = {0};
        int len;
        len = udpserver.RecvFrom(udpserver.GetSock(), buf, sizeof(buf), 0, &udpclient);
        if (len > 0)
        {
            cout << "recvfrom addr : " << udpclient.GetAddr() << endl;
            cout << "recvfrom port : " << udpclient.GetPort() << endl;
            cout << "recvfrom len : " << len << endl;
            cout << "recvfrom data : " << buf << endl;
            cout << endl;
        }
        usleep(200);

    }


    return 0;
}

#include "mysock.h"

int f_udpclient()
{
    CSockUdpServer server("127.0.0.1", 8000);
    CSockUdpClient client;
    client.SockCreate();
    char buf[1024] = "hello server";
    client.SendTo(client.GetSock(),buf , strlen(buf), 0, &server);
    return 0;
}

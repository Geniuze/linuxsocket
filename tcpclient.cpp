#include "mysock.h"

int f_tcpclient()
{
    CSockTcpClient client;
    client.Connect("127.0.0.1", 8000);
    client.Send("hello server", 12);
    client.Close();
    return 0;
}

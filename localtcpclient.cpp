#include "mysock.h"

int f_localtcpclient()
{
    CSockLocalTcpClient client;
    client.Connect("/tmp/server.sock");
    client.Send("hello server", 12);
    client.Close();
    return 0;
}

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>

using namespace std;

#define DEFAULT_PORT 8000

class ISock
{
public:
    ISock():_family(AF_INET)
    {

    }
    ISock(int family):_family(family)
    {

    }
    void SetFamily(int family)
    {
        _family = family;
    }
    int GetFamily()
    {
        return _family;
    }

    virtual ~ISock(){}
private:
    int _family;
};

class CSockIn : public ISock
{
public:
    CSockIn() : ISock(),_type(SOCK_STREAM),_port(DEFAULT_PORT),_addr("0.0.0.0")
    {

    }
    CSockIn(string addr, unsigned short port) : ISock(),_type(SOCK_STREAM),_port(port),_addr(addr)
    {

    }
    CSockIn(int family, int type, string addr, unsigned short port) : ISock(family),_type(type),_port(port),_addr(addr)
    {

    }
    void SetType(int type)
    {
        _type = type;
    }
    void SetPort(unsigned short port)
    {
        _port = port;
    }
    void SetAddr(string addr)
    {
        _addr = addr;
    }
    void SetAddr(char *addr)
    {
        _addr = addr;
    }
    void SetSock(int sock)
    {
        _sock = sock;
    }

    int GetType()
    {
        return _type;
    }
    unsigned short GetPort()
    {
        return _port;
    }
    string GetAddr()
    {
        return _addr;
    }
    int GetSock()
    {
        return _sock;
    }

    virtual ~CSockIn()
    {

    }
    int SocketCreate()
    {
        _sock = socket(ISock::GetFamily(), _type, 0);
        if (-1 == _sock )
        {
            perror("socket");
            return -1;
        }
        return _sock;
    }

    int BindAddr()
    {
        cout << "bind " << _addr << ":" << _port << endl;
        struct sockaddr_in local;
        local.sin_family = ISock::GetFamily();
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = inet_addr(_addr.c_str());

        if (-1 == bind(_sock, (const sockaddr *)&local, sizeof(local)))
        {
            perror("bind");
            return -1;
        }
        return 0;
    }

    int Listen()
    {
        if (-1 == listen(_sock, 10))
        {
            perror("listen");
            return -1;
        }
        return 0;
    }
    int Accept(CSockIn &client)
    {
        struct sockaddr_in sin_client;
        socklen_t addrlen = sizeof(sin_client);
        char dstaddr[100] = {0};
        int client_sock;

        if (-1 == (client_sock = accept(_sock, (struct sockaddr*)&sin_client, &addrlen)))
        {
            perror("accept");
            return -1;
        }
        client.SetType(_type);
        client.SetFamily(sin_client.sin_family);
        client.SetPort(ntohs(sin_client.sin_port));
        client.SetAddr(inet_ntop(sin_client.sin_family, &sin_client.sin_addr.s_addr, dstaddr, sizeof(dstaddr)));
        client.SetSock(client_sock);
        return client_sock;
    }
    int SetNonBlock()
    {
        int opts = fcntl(_sock, F_GETFL);
        if (-1 == opts)
        {
            perror("fcntl");
            return -1;
        }
        opts |= O_NONBLOCK;
        if(-1 == fcntl(_sock, F_SETFL, opts))
        {
            perror("fcntl");
            return -1;
        }
        return 0;
    }

private:
    int _type;
    unsigned short _port;
    string _addr;
    int _sock;
};

class CSockTcpServer : public CSockIn
{
public:
    CSockTcpServer() : CSockIn()
    {

    }
    CSockTcpServer(string addr, unsigned short port) : CSockIn(addr, port)
    {

    }
    CSockTcpServer(int family, int type, string addr, unsigned short port) : CSockIn(family, type, addr, port)
    {

    }
    virtual ~CSockTcpServer()
    {

    }
    int Listen()
    {
        if (-1 == SocketCreate())
        {
            return -1;
        }
        if (-1 == SetNonBlock())
        {
            return -1;
        }
        if (-1 == BindAddr())
        {
            return -1;
        }
        if (-1 == CSockIn::Listen())
        {
            return -1;
        }
        return 0;
    }



};




















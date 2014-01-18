#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <string.h>

using namespace std;

#define DEFAULT_PORT 8000

#define bzero(p, len) \
    memset(p, 0, len)

class ISock
{
public:
    ISock():_family(AF_INET), _type(SOCK_STREAM)
    {

    }
    ISock(int family, int type) : _family(family), _type(type)
    {

    }

    void SetFamily(int family)
    {
        _family = family;
    }
    void SetType(int type)
    {
        _type = type;
    }
    int GetFamily()
    {
        return _family;
    }
    int GetType()
    {
        return _type;
    }

    virtual ~ISock(){}
private:
    int _family;
    int _type;
};

class CSockIn : public ISock
{
public:
    CSockIn() : ISock(),_port(DEFAULT_PORT),_addr("0.0.0.0")
    {

    }
    CSockIn(string addr, unsigned short port) : ISock(),_port(port),_addr(addr)
    {

    }
    CSockIn(int family, int type, string addr, unsigned short port) : ISock(family, type),_port(port),_addr(addr)
    {

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
        _sock = socket(ISock::GetFamily(), ISock::GetType(), 0);
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
        client.SetType(ISock::GetType());
        client.SetFamily(sin_client.sin_family);
        client.SetPort(ntohs(sin_client.sin_port));
        client.SetAddr(inet_ntop(sin_client.sin_family, &sin_client.sin_addr.s_addr, dstaddr, sizeof(dstaddr)));
        client.SetSock(client_sock);
        return client_sock;
    }
    int Connect()
    {
        //client
        struct sockaddr_in server;
        bzero(&server, sizeof(server));
        server.sin_family = ISock::GetFamily();
        server.sin_port = htons(_port);
        server.sin_addr.s_addr = inet_addr(_addr.c_str());
        if (-1 == connect(_sock, (struct sockaddr *)&server, sizeof(server)))
        {
            perror("connect");
            return -1;
        }
        return 0;
    }
    int Recv(char *buf, size_t len)
    {
        return recv(_sock, buf, len, 0);
    }
    int Send(const char *buf, size_t len)
    {
        return send(_sock, buf, len, 0);
    }
    int Close()
    {
        shutdown(_sock, SHUT_RDWR);
        close(_sock);
        return 0;
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

class CSockTcpClient : public CSockIn
{
public:
    CSockTcpClient() : CSockIn()
    {

    }
    CSockTcpClient(string addr, unsigned short port) : CSockIn(addr, port)
    {

    }
    CSockTcpClient(int family, int type, string addr, unsigned short port) : CSockIn(family, type, addr, port)
    {

    }
    virtual ~CSockTcpClient()
    {

    }
    int Connect()
    {
        if (-1 == SocketCreate())
            return -1;
        if (-1 == CSockIn::Connect())
            return -1;
        return 0;
    }
    int Connect(string addr, unsigned short port)
    {
        SetAddr(addr);
        SetPort(port);
        return this->Connect();
    }

};

class CSockUn : public ISock
{
public:
    CSockUn() : ISock(), _path("/tmp/server.sock")
    {

    }
    CSockUn(string path) : ISock(), _path(path)
    {

    }
    CSockUn(int family, int type, string path) : CSock(family, type), _path(path)
    {

    }
    virtual ~CSockUn()
    {

    }

    void SetPath(string path)
    {
        _path = path;
    }
private:
    string _path;
};




















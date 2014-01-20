#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <string.h>

using namespace std;

#define DEFAULT_PORT 8000
#define DEFAULT_LOCALSERVER_PATH "/tmp/server.sock"

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
    void SetSock(int sock)
    {
        _sock = sock;
    }
    int GetSock()
    {
        return _sock;
    }

    virtual void SetPort(unsigned short port)
    {
        cout << "ISock::" << __func__ << "->port=" << port << endl;
    }

    virtual void SetAddr(string addr)
    {
        cout << "ISock::" << __func__ << "->addr=" << addr << endl;
    }

    virtual void SetAddr(char * addr)
    {
        cout << "ISock::" << __func__ << "->addr=" << addr << endl;
    }

    virtual unsigned short GetPort()
    {
        return 0;
    }

    virtual string GetAddr()
    {
        return "";
    }

    virtual void SetPath(string path)
    {
        path = "";
    }

    virtual void SetPath(char *path)
    {
        cout << "ISock::" << __func__ << "->path=" << path << endl;
    }

    virtual string GetPath()
    {
        return "";
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

    int SockCreate()
    {
        _sock = socket(_family, _type, 0);
        if (-1 == _sock)
        {
            perror("socket");
            return -1;
        }

        return _sock;
    }

    virtual int BindAddr()
    {
        return 0;
    }

    int Listen(int num = 10)
    {
        if (-1 == listen(_sock, num))
        {
            perror("listen");
            return -1;
        }
        return 0;
    }

    virtual int Accept(ISock *client)
    {
        cout << "ISock::" << __func__ << "->client=" << client << endl;
        return 0;
    }

    virtual int Connect()
    {
        return 0;
    }

    virtual int Connect(string addr, unsigned short port)
    {
        cout << "ISock::" << __func__ << "->addr=" << addr << endl;
        cout << "ISock::" << __func__ << "->port=" << port << endl;
        return 0;
    }

    virtual int Connect(string path)
    {
        path = "";
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
    virtual ~ISock(){}
private:
    int _family;
    int _type;
    int _sock;
};

class CSockIn : public ISock
{
public:
    CSockIn() : ISock(AF_INET, SOCK_STREAM),_port(DEFAULT_PORT),_addr("0.0.0.0")
    {

    }
    CSockIn(string addr, unsigned short port) : ISock(AF_INET, SOCK_STREAM),_port(port),_addr(addr)
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

    unsigned short GetPort()
    {
        return _port;
    }
    string GetAddr()
    {
        return _addr;
    }

    virtual ~CSockIn()
    {

    }

    int BindAddr()
    {
        cout << "bind " << _addr << ":" << _port << endl;
        struct sockaddr_in local;
        local.sin_family = ISock::GetFamily();
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = inet_addr(_addr.c_str());

        if (-1 == bind(ISock::GetSock(), (const sockaddr *)&local, sizeof(local)))
        {
            perror("bind");
            return -1;
        }
        return 0;
    }

    int Accept(ISock *client)
    {
        struct sockaddr_in sin_client;
        socklen_t addrlen = sizeof(sin_client);
        char dstaddr[100] = {0};
        int client_sock;

        if (-1 == (client_sock = accept(ISock::GetSock(), (struct sockaddr*)&sin_client, &addrlen)))
        {
            perror("accept");
            return -1;
        }
        client->SetType(ISock::GetType());
        client->SetFamily(sin_client.sin_family);
        client->SetPort(ntohs(sin_client.sin_port));
        client->SetAddr(inet_ntop(sin_client.sin_family, &sin_client.sin_addr.s_addr, dstaddr, sizeof(dstaddr)));
        client->SetSock(client_sock);
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
        if (-1 == connect(ISock::GetSock(), (struct sockaddr *)&server, sizeof(server)))
        {
            perror("connect");
            return -1;
        }
        return 0;
    }

private:
    unsigned short _port;
    string _addr;
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
        if (-1 == SockCreate())
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
        if (-1 == ISock::Listen())
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
        if (-1 == SockCreate())
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
    CSockUn() : ISock(AF_UNIX, SOCK_STREAM), _path(DEFAULT_LOCALSERVER_PATH)
    {

    }
    CSockUn(string path) : ISock(AF_UNIX, SOCK_STREAM), _path(path)
    {

    }
    CSockUn(int family, int type, string path) : ISock(family, type), _path(path)
    {

    }
    virtual ~CSockUn()
    {

    }

    void SetPath(string path)
    {
        _path = path;
    }
    void SetPath(char *path)
    {
        _path = path;
    }

    string GetPath()
    {
        return _path;
    }



    int BindAddr()
    {
        cout << "bind:" << _path << endl;
        struct sockaddr_un sun;
        sun.sun_family = ISock::GetFamily();
        strcpy(sun.sun_path, _path.c_str());

        if (-1 == bind(ISock::GetSock(), (struct sockaddr *)&sun, sizeof(sun)))
        {
            perror("bind");
            return -1;
        }
        return 0;
    }

    int Accept(ISock *client)
    {
        struct sockaddr_un sun;
        socklen_t len = sizeof(sun);

        int client_fd = accept(ISock::GetSock(), (struct sockaddr*)&sun, &len);
        if (-1 == client_fd)
        {
            perror("accept");
            return -1;
        }
        client->SetFamily(sun.sun_family);
        client->SetPath(sun.sun_path);
        client->SetSock(client_fd);
        client->SetType(ISock::GetType());
        return 0;
    }
    int Connect()
    {
        struct sockaddr_un sun;
        sun.sun_family = ISock::GetFamily();
        strcpy(sun.sun_path, GetPath().c_str());

        if (-1 == connect(ISock::GetSock(), (struct sockaddr*)&sun, sizeof(sun)))
        {
            perror("connect");
            return -1;
        }
        return 0;
    }

private:
    string _path;
};

class CSockLocalTcpServer : public CSockUn
{
public:
    CSockLocalTcpServer() : CSockUn()
    {

    }
    CSockLocalTcpServer(string path) : CSockUn(path)
    {

    }
    CSockLocalTcpServer(int family, int type, string path) : CSockUn(family, type, path)
    {

    }
    virtual ~CSockLocalTcpServer()
    {

    }
    int Listen()
    {
        if (-1 == SockCreate())
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
        if (-1 == ISock::Listen())
        {
            return -1;
        }
        return 0;
    }
};

class CSockLocalTcpClient : public CSockUn
{
public:
    CSockLocalTcpClient() : CSockUn()
    {

    }
    CSockLocalTcpClient(string path) : CSockUn(path)
    {

    }
    CSockLocalTcpClient(int family, int type, string path) : CSockUn(family, type, path)
    {

    }
    virtual ~CSockLocalTcpClient()
    {

    }
    int Connect()
    {
        if (-1 == SockCreate())
            return -1;
        if (-1 == CSockUn::Connect())
            return -1;
        return 0;
    }
    int Connect(string path)
    {
        SetPath(path);
        return this->Connect();
    }
};


















#include <iostream>
#include "mysock.h"
#include <sys/epoll.h>
#include <list>
#include <sys/signal.h>

using namespace std;

#define MAX_EVENTS 10

int f_tcpserver()
{
    CSockTcpServer tcpserver;
    int epollfd, ndfs;
    struct epoll_event ev, events[MAX_EVENTS];
	list<CSockIn> clients;

    signal(SIGPIPE, SIG_IGN);
    if (-1 == tcpserver.Listen())
        return -1;

    epollfd = epoll_create(MAX_EVENTS);
    if (-1 == epollfd)
    {
        perror("epoll_create");
        return -1;
    }
    ev.events = EPOLLIN;
    ev.data.fd = tcpserver.GetSock();

    if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev))
    {
        perror("epoll_ctl");
        return -1;
    }
    while(1)
    {
        ndfs = epoll_wait(epollfd, events, MAX_EVENTS, -1); //ndfs is how many events occured
        if (-1 == ndfs)
        {
            perror("epoll_wait");
            return -1;
        }

        for (int n = 0; n < ndfs; ++n)
        {
            if (events[n].data.fd == tcpserver.GetSock())
            {
                CSockIn client;
                if (-1 == tcpserver.Accept(client))
                {
                    return -1;
                }
                cout << "accept##" << client.GetAddr() << ":" << client.GetPort() << endl;

                ev.data.fd = client.GetSock();
                ev.events = EPOLLIN;

                if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, ev.data.fd, &ev))
                {
                    return -1;
                }
                clients.push_back(client);
            }
            else
            {
                //cout << "new event from cilents..." << endl;
                list<CSockIn>::iterator it = clients.end();
                for(it = clients.begin(); it != clients.end(); ++it)
                {
                    if(it->GetSock() == events[n].data.fd)
                    {
                        break;
                    }
                }
                if (it != clients.end())
                {
                    char buf[1024] = {0};
                    int len = recv(it->GetSock(), buf, sizeof(buf), 0);
					//cout << "test code ...[len=" << len << "]" << endl;
                    if (0 >= len) // always have event and "recv" return 0 when remote close tcp connect
                    {
                        ev.data.fd = it->GetSock();
                        ev.events = EPOLLIN;
                        if (-1 == epoll_ctl(epollfd, EPOLL_CTL_DEL, ev.data.fd, &ev))
                        {
                            perror("epoll_ctl");
                            return -1;
                        }
                        shutdown(it->GetSock(), SHUT_RDWR);
                        close(it->GetSock());
                        clients.erase(it);
                        continue;
                    }
                    cout << "\nrecv from : " << it->GetAddr() << ":" << it->GetPort() << endl;
                    cout << "data length : " << len << endl;
                    cout << "data : " << buf << endl;
                }
            }
        }
    }
    return 0;
}

#include <iostream>
#include "mysock.h"
#include <sys/epoll.h>
#include <list>
#include <sys/signal.h>
#include <malloc.h>

using namespace std;

#define MAX_EVENTS 10

int f_localtcpserver()
{
    CSockLocalTcpServer localtcpserver;
    int epollfd, ndfs;
    struct epoll_event ev, events[MAX_EVENTS];
    list<ISock*> clients;

    signal(SIGPIPE, SIG_IGN);
    if (-1 == localtcpserver.Listen())
        return -1;

    epollfd = epoll_create(MAX_EVENTS);
    if (-1 == epollfd)
    {
        perror("epoll_create");
        return -1;
    }
    ev.events = EPOLLIN;
    ev.data.fd = localtcpserver.GetSock();

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
            if (events[n].data.fd == localtcpserver.GetSock())
            {
                ISock *client = new CSockUn;
                if (-1 == localtcpserver.Accept(client))
                {
                    return -1;
                }
                cout << "accept##" << client->GetAddr() << ":" << client->GetPort() << endl;

                ev.data.fd = client->GetSock();
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
                list<ISock*>::iterator it = clients.end();
                for(it = clients.begin(); it != clients.end(); ++it)
                {
                    if((*it)->GetSock() == events[n].data.fd)
                    {
                        break;
                    }
                }
                if (it != clients.end())
                {
                    int BufLen = 8;
                    char *buf = (char *)malloc(BufLen);
                    bzero(buf, BufLen);
                    int len;

                    while((len = recv((*it)->GetSock(), buf, BufLen, MSG_PEEK)) == BufLen)
                    {
                        buf = (char *)realloc(buf, BufLen*2);
                        bzero(buf, BufLen);
                        BufLen *= 2;
                    }
                    bzero(buf, BufLen);
                    len = recv((*it)->GetSock(), buf, BufLen, 0);
                    //cout << "test code ...[len=" << len << "]" << endl;
                    if (0 >= len) // always have event and "recv" return 0 when remote close tcp connect
                    {
                        ev.data.fd = (*it)->GetSock();
                        ev.events = EPOLLIN;
                        if (-1 == epoll_ctl(epollfd, EPOLL_CTL_DEL, ev.data.fd, &ev))
                        {
                            perror("epoll_ctl");
                            return -1;
                        }
                        shutdown((*it)->GetSock(), SHUT_RDWR);
                        close((*it)->GetSock());
                        delete *it;
                        clients.erase(it);
                        free(buf);
                        continue;
                    }
                    cout << "\nrecv from : " << (*it)->GetAddr() << ":" << (*it)->GetPort() << endl;
                    cout << "data length : " << len << endl;
                    cout << "data : " << buf << endl;
                    free(buf);
                }
            }
        }
    }
    return 0;
}


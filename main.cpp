#include <iostream>
#include <string>
#include <netinet/in.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <sstream>
#include <arpa/inet.h>
#include <signal.h>
using namespace std;
#define MAX_EVENTS 10
#define MAX_CLIENTS 10

#define EXIT_FAILURE -1
struct FdAddr_Map
{
	int fd;
	sockaddr addr;
};

bool setnonblocking(int fd)
{
	int opts = fcntl(fd, F_GETFL);
	if (-1 == opts)
	{
		perror("fcntl");
		return false;
	}
	opts |= O_NONBLOCK;
	if (-1 == fcntl(fd, F_SETFL, opts))
	{
		perror("fcntl");
		return false;
	}
	return true;
}
int main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
	struct epoll_event ev, events[MAX_EVENTS]; 
	int listen_sock, conn_sock, nfds, epollfd;
	

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listen_sock)
	{
		perror("socket");
		_exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in local, client;
	struct FdAddr_Map clients[MAX_CLIENTS];
	memset(clients, 0, sizeof(clients));

	socklen_t  addrlen = sizeof(local);
	local.sin_family = AF_INET;
	local.sin_port = htons(8000);
	//local.sin_addr.s_addr = inet_addr("10.0.1.35");
	local.sin_addr.s_addr = INADDR_ANY;


	if (-1 == bind(listen_sock, (struct sockaddr *)&local, addrlen))
	{
		perror("bind");
		_exit(EXIT_FAILURE);
	}

	if (-1 == listen(listen_sock, 100))
	{
		perror("listen");
		_exit(EXIT_FAILURE);
	}

	epollfd = epoll_create(10);
	if (-1 == epollfd)
	{
		perror("epoll_create");
		_exit(EXIT_FAILURE);
	}

	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
	{
		perror("epoll_ctl:listen_sock");
		_exit(EXIT_FAILURE);
	}

	for(;;)
	{
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (nfds == -1)
		{
			perror("epoll_wait");
			_exit(EXIT_FAILURE);
		}
		for (int n = 0; n < nfds; ++n)
		{
			if (events[n].data.fd == listen_sock)
			{
				conn_sock = accept(listen_sock, (struct sockaddr *)&client, &addrlen);
				if (-1 == conn_sock)
				{
					perror("accept");
					_exit(EXIT_FAILURE);
				}
				if (MAX_CLIENTS <= conn_sock)
				{
					cout << "clients map is full." << endl;
					shutdown(conn_sock, SHUT_RDWR);
					close(conn_sock);
				}

				memcpy(&clients[conn_sock].addr, &client, sizeof(struct sockaddr));
				clients[conn_sock].fd = conn_sock;

				setnonblocking(conn_sock);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;

				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
				{
					perror("epoll_ctl:conn_sock");
					_exit(EXIT_FAILURE);
				}
			}
			else
			{
				stringstream ssClient;
				string strSend = "";
				char dst[100] = {0};
				
				
				
				ssClient << "hello_client:" << inet_ntop(AF_INET, &(((struct sockaddr_in*)&clients[events[n].data.fd].addr)->sin_addr.s_addr), dst, sizeof(dst));
				ssClient << ":" << ntohs(((struct sockaddr_in*)&clients[events[n].data.fd].addr)->sin_addr.s_addr);
				strSend = ssClient.str();
				cout << "send-to-client:" << send(events[n].data.fd, strSend.c_str(), strSend.length(), 0) << endl;
			}
		}
	}
	
	return 0;
}



















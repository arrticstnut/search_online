///
/// @file    SocketUtil.h
/// @author
///

#ifndef __CC_SOCKETUTIL_H__
#define __CC_SOCKETUTIL_H__

#include "cppLog.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>

namespace cc
{

	inline int createSocketFd()
	{
		int fd = ::socket(AF_INET, SOCK_STREAM, 0);
		if(fd == -1)
		{
			logErrorLoc(string("socket create error!:") + strerror(errno));
			//perror("socket create error!");
		}
		return fd;
	}

	inline void setNonblock(int fd)
	{
		int flags = ::fcntl(fd, F_GETFL, 0);
		flags |= O_NONBLOCK;
		::fcntl(fd, F_SETFL, flags);
	}

	inline int createEpollFd()
	{
		int efd = ::epoll_create1(0);
		if(-1 == efd)
		{
			logErrorLoc(string("epoll_create1 error:") + strerror(errno));
			//perror("epoll_create1 error");
			exit(EXIT_FAILURE);
		}
		return efd;
	}

	inline int createEventFd()
	{
		int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
		if(-1 == evtfd)
		{
			logErrorLoc(string("eventfd create error:") + strerror(errno));
			//perror("eventfd create error");
		}
		return evtfd;
	}

	inline void addEpollFdRead(int efd, int fd)
	{
		struct epoll_event ev;
		ev.data.fd = fd;
		ev.events = EPOLLIN;
		int ret = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev);
		if(-1 == ret)
		{
			//perror("epoll_ctl add error");
			logErrorLoc(string("epoll_ctl add error:") + strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	inline void delEpollReadFd(int efd, int fd)
	{
		struct epoll_event ev;
		ev.data.fd = fd;
		int ret = epoll_ctl(efd, EPOLL_CTL_DEL, fd, &ev);
		if(-1 == ret)
		{
			//perror("epoll_ctl del error");
			logErrorLoc(string("epoll_ctl del error:") + strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	//预览数据
	inline size_t recvPeek(int sockfd, void * buf, size_t len)
	{
		int nread;
		do
		{
			nread = ::recv(sockfd, buf, len, MSG_PEEK);
		}while(nread == -1 && errno == EINTR);
		return nread;
	}

	//通过预览数据，判断conn是否关闭
	inline bool isConnectionClosed(int sockfd)
	{
		char buf[1024];
		int nread = recvPeek(sockfd, buf, sizeof(buf));
		if(-1 == nread)
		{
			//perror("recvPeek--- ");
			logErrorLoc(string("recvPeek:") + strerror(errno));
			return true;
		}
		return (0 == nread);
	}

}//end of namespace

#endif

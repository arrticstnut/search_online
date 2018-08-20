///
/// @file    EpollPoller.cc
/// @author  
///

#include "EpollPoller.h"
#include "SocketUtil.h"
#include "Acceptor.h"
#include "cppLog.h"
#include <assert.h>
//
#include <iostream>
using std::cout;
using std::endl;
using namespace cc;

namespace cc
{

	EpollPoller::EpollPoller(Acceptor & acceptor)
		: acceptor_(acceptor)
		  , epollfd_(createEpollFd())
		  , listenfd_(acceptor_.fd())
		  , wakeupfd_(createEventFd())
		  , isLooping_(false)
		  , mutex_()
		  , eventsList_(1024)
	{
		addEpollFdRead(epollfd_, listenfd_);
		addEpollFdRead(epollfd_, wakeupfd_);
	}

	EpollPoller::~EpollPoller()
	{
		::close(epollfd_);
	}

	void EpollPoller::loop()
	{
		isLooping_ = true;
		while(isLooping_)
		{
			waitEpollfd();
		}
	}

	void EpollPoller::unloop()
	{
		if(isLooping_)
			isLooping_ = false;
	}


	void EpollPoller::runInLoop(const Functor & cb)
	{
		{
			MutexLockGuard mlg(mutex_);
			pendingFunctors_.push_back(cb);
		}
		wakeup();
	}

	void EpollPoller::doPendingFunctors()
	{
		std::vector<Functor> functors;
		{
			MutexLockGuard mlg(mutex_);
			functors.swap(pendingFunctors_);
		}
		for(size_t i = 0; i < functors.size(); ++i)
		{
			functors[i]();
		}
	}


	void EpollPoller::wakeup()
	{
		uint64_t one = 1;
		ssize_t n = ::write(wakeupfd_, &one, sizeof(one));
		if(n != sizeof(one))
		{
			//perror("EpollPoller::wakeup() n != 8");
			logErrorLoc(string("EpollPoller::wakeup() n != 8") + strerror(errno));
		}
	}

	void EpollPoller::handleRead()
	{
		uint64_t one = 1;
		ssize_t n = ::read(wakeupfd_, &one, sizeof(one));
		if(n != sizeof(one))
		{
			logErrorLoc(string("EpollPoller::handleRead() n != 8") + strerror(errno));
			//perror("EpollPoller::handleRead() n != 8");
		}
	}


	void EpollPoller::setConnectionCallback(EpollCallback cb)
	{
		onConnectionCb_ = cb;
	}

	void EpollPoller::setMessageCallback(EpollCallback cb)
	{
		onMessageCb_ = cb;
	}

	void EpollPoller::setCloseCallback(EpollCallback cb)
	{
		onCloseCb_ = cb;
	}

	void EpollPoller::waitEpollfd()
	{
		int nready;
		do
		{
			nready = ::epoll_wait(epollfd_, 
					&(*eventsList_.begin()),
					eventsList_.size(),
					600000);
		}while(nready == -1 && errno == EINTR);

		if(nready == -1)
		{
			//perror("epoll_wait error");
			logErrorLoc(string("epoll_wait error") + strerror(errno));
			exit(EXIT_FAILURE);
		}
		else if(nready == 0)
		{
			//printf("epoll_wait timeout\n");	
			logDebug("epoll_wait timeout");
		}
		else
		{
			//做一个扩容的操作
			if(nready == static_cast<int>(eventsList_.size()))
			{
				eventsList_.resize(eventsList_.size() * 2);
			}

			//遍历每一个激活的文件描述符
			for(int idx = 0; idx != nready; ++idx)
			{
				if(eventsList_[idx].data.fd == listenfd_)
				{
					if(eventsList_[idx].events & EPOLLIN)
					{
						handleConnection();
					}
				}
				else if(eventsList_[idx].data.fd == wakeupfd_)
				{
					if(eventsList_[idx].events & EPOLLIN)
					{
						handleRead();
						doPendingFunctors();
					}
				}
				else
				{
					if(eventsList_[idx].events & EPOLLIN)
					{
						handleMessage(eventsList_[idx].data.fd);
					}
				}
			}//end for
		}//end else
	}

	void EpollPoller::handleConnection()
	{
		int peerfd = acceptor_.accept(); 
		addEpollFdRead(epollfd_, peerfd);

		TcpConnectionPtr conn(new TcpConnection(peerfd, this));
		conn->setConnectionCallback(onConnectionCb_);
		conn->setMessageCallback(onMessageCb_);
		conn->setCloseCallback(onCloseCb_);

		std::pair<ConnectionMap::iterator, bool> ret;
		ret = connMap_.insert(std::make_pair(peerfd, conn));
		assert(ret.second == true);
		(void)ret;
		//connMap_[peerfd] = conn;

		conn->handleConnectionCallback();
	}

	void EpollPoller::handleMessage(int peerfd)
	{
		bool isClosed = isConnectionClosed(peerfd);
		ConnectionMap::iterator it = connMap_.find(peerfd);
		assert(it != connMap_.end());

		if(isClosed)
		{
			it->second->handleCloseCallback();
			delEpollReadFd(epollfd_, peerfd);
			connMap_.erase(it);
		}
		else
		{
			it->second->handleMessageCallback();
		}
	}

}// end of namespace

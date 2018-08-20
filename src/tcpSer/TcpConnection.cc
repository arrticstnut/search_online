///
/// @file    TcpConnection.cc
/// @author
///

#include "TcpConnection.h"
#include "EpollPoller.h"
#include "cppLog.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
using std::cout;
using std::endl;
using namespace cc;


namespace cc
{

	TcpConnection::TcpConnection(int sockfd, EpollPoller * loop)
		: sockfd_(sockfd)
		  , sockIO_(sockfd)
		  , localAddr_(cc::Socket::getLocalAddr(sockfd))
		  , peerAddr_(cc::Socket::getPeerAddr(sockfd))
		  , isShutdownWrite_(false)
		  , loop_(loop)
	{
		sockfd_.nonblock();
		//logDebugLoc("TcpConnection");
	}


	TcpConnection::~TcpConnection()
	{
		if(!isShutdownWrite_)
		{
			isShutdownWrite_ = true;
			shutdown();
		}
		//logDebugLoc("~TcpConnection");
		//printf("~TcpConnection()\n");
	}

	std::string TcpConnection::receive()
	{
		char buf[65536];
		memset(buf, 0, sizeof(buf));
		size_t ret = sockIO_.readline(buf, sizeof(buf));
		if(ret == 0)
		{
			return std::string();
		}
		else
			return std::string(buf);
	}

	void TcpConnection::send(const std::string & msg)
	{
		sockIO_.writen(msg.c_str(), msg.size());
	}

	//针对php服务器
	void TcpConnection::sendAndClose(const std::string & msg)
	{
		send(msg);
		shutdown();
	}

	//它是在计算线程里面调用的
	void TcpConnection::sendInLoop(const std::string & msg)
	{
		//loop_->runInLoop(std::bind(&TcpConnection::sendAndClose, this, msg));
		loop_->runInLoop(std::bind(&TcpConnection::sendAndClose, this, msg));
	}

	void TcpConnection::shutdown()
	{
		if(!isShutdownWrite_)
			sockfd_.shutdownWrite();
		isShutdownWrite_ = true;
	}

	std::string TcpConnection::toString()
	{
		char str[100];
		snprintf(str, sizeof(str), "%s:%d -> %s:%d",
				localAddr_.ip().c_str(),
				localAddr_.port(),
				peerAddr_.ip().c_str(),
				peerAddr_.port());
		return std::string(str);
	}


	void TcpConnection::setConnectionCallback(TcpConnectionCallback cb)
	{
		onConnectionCb_ = cb;
	}

	void TcpConnection::setMessageCallback(TcpConnectionCallback cb)
	{
		onMessageCb_ = cb;
	}

	void TcpConnection::setCloseCallback(TcpConnectionCallback cb)
	{
		onCloseCb_ = cb;
	}

	void TcpConnection::handleConnectionCallback()
	{
		if(onConnectionCb_)
			onConnectionCb_(shared_from_this());
	}

	void TcpConnection::handleMessageCallback()
	{
		if(onMessageCb_)
			onMessageCb_(shared_from_this());
	}

	void TcpConnection::handleCloseCallback()
	{
		if(onCloseCb_)
			onCloseCb_(shared_from_this());
	}

}// end of namespace cc

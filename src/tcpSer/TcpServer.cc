///
/// @file    TcpServer.cc
/// @author
///

#include "TcpServer.h"
#include "SocketUtil.h"

namespace cc
{

	TcpServer::TcpServer(unsigned short port)
		: acceptor_(createSocketFd(), InetAddress(port)),
		epollfd_(acceptor_)
	{
		acceptor_.ready();
	}


	TcpServer::TcpServer(const char * pIp, unsigned short port)
		: acceptor_(createSocketFd(), InetAddress(pIp, port)),
		epollfd_(acceptor_)
	{
		acceptor_.ready();
	}

	void TcpServer::start()
	{
		epollfd_.loop();
	}

	void TcpServer::stop()
	{
		epollfd_.unloop();
	}

	void TcpServer::setConnectionCallback(TcpServerCallback cb)
	{
		epollfd_.setConnectionCallback(cb);
	}

	void TcpServer::setMessageCallback(TcpServerCallback cb)
	{
		epollfd_.setMessageCallback(cb);
	}

	void TcpServer::setCloseCallback(TcpServerCallback cb)
	{
		epollfd_.setCloseCallback(cb);
	}

}//end of namespace cc

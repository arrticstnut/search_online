///
/// @file    Socket.h
/// @author
///

#ifndef __CC_SOCKET_H
#define __CC_SOCKET_H

#include "Noncopyable.h"

namespace cc
{
	class InetAddress;
	class Socket : Noncopyable
	{
		public:
			Socket(int sockfd);
			Socket();
			~Socket();

			void shutdownWrite();
			int fd()const{	return sockfd_;	}

			void nonblock();

			static InetAddress getLocalAddr(int sockfd);
			static InetAddress getPeerAddr(int sockfd);
		private:
			int sockfd_;
	};
}// end of namespace

#endif

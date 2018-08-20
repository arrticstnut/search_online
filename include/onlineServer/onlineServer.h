///
/// @file    onlineServer.h
/// @author
///
#ifndef __CC_ONLINESERVER_H__
#define __CC_ONLINESERVER_H__
#include "configuration.h" 
#include "wordQuery.h"
#include "Threadpool.h"
#include "redisPool.h"
#include "TcpServer.h"
#include "queryTask.h"
#include <functional>
namespace cc
{
	class OnlineServer
	{
		public:
			OnlineServer(Configuration & conf);
			static void onConnection(const cc::TcpConnectionPtr & conn);
			static void onMessage(const cc::TcpConnectionPtr & conn,WordQuery *wordQueryPtr,RedisPool *redisPtr,cc::Threadpool *pThreadPool);
			static void onClose(const cc::TcpConnectionPtr & conn);
		private:
			Configuration & _conf;
			cc::RedisPool _redis;
			WordQuery _wordQuery;
			cc::Threadpool _threadPool;
			cc::TcpServer _tcpServer;
	};
}//end of namespace
#endif

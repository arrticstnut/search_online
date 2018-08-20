///
/// @file    queryTask.h
/// @author
///
#ifndef __CC_TASK_H__
#define __CC_TASK_H__
#include "TcpConnection.h"
#include "wordQuery.h"
#include "redisPool.h"
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
using std::cout;
using std::endl;
using std::string;

namespace cc
{

	class QueryTask
	{
		public:
			QueryTask(cc::TcpConnectionPtr spConn,WordQuery *wordQueryPtr,RedisPool *redisPtr,string & queryStr);
			void process();
		private:
			cc::TcpConnectionPtr _spConn;
			WordQuery * _wordQueryPtr;
			RedisPool * _redisPtr;
			string _queryStr;
	};
}//end of namespace
#endif

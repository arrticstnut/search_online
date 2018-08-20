///
/// @file    Thread.h
/// @author
///

#ifndef __CC_THREAD_H__
#define __CC_THREAD_H__
#include "Noncopyable.h"
#include <pthread.h>
#include <functional>

namespace cc
{

	class Thread
		: Noncopyable
	{
		public:
			typedef std::function<void()> ThreadCallback;

			Thread(ThreadCallback && cb);
			~Thread();

			void start();

			void join();

			pthread_t getThreadId() const
			{	return _pthId;	}

		private:
			static void * threadFunc(void *);

		private:
			pthread_t _pthId;
			bool _isRunning;
			ThreadCallback _cb;
	};

}//end of namespace

#endif

///
/// @file    Thread.cc
/// @author
///

#include "Thread.h"

namespace cc
{

	Thread::Thread(ThreadCallback && cb)
		: _pthId(0)
		  , _isRunning(false)
		  , _cb(std::move(cb))
	{
	}

	Thread::~Thread()
	{
		if(_isRunning)
		{
			_isRunning = false;
			pthread_detach(_pthId);
		}
	}

	void Thread::start()
	{
		pthread_create(&_pthId, NULL, threadFunc, this);
		_isRunning = true;
	}

	void Thread::join()
	{
		pthread_join(_pthId, NULL);
		_isRunning=false;
	}

	void * Thread::threadFunc(void * arg)
	{
		Thread * pthread = static_cast<Thread*>(arg);
		if(pthread)
			pthread->_cb();	
		return NULL;
	}

}//end of namespace cc


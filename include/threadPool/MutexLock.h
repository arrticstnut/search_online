///
/// @file    MutexLock.h
/// @author
///

#ifndef __CC_MUTEXLOCK_H__
#define __CC_MUTEXLOCK_H__

#include "Noncopyable.h"
#include <pthread.h>


namespace cc
{

	class MutexLock
		: Noncopyable
	{
		public:
			MutexLock();
			~MutexLock();

			void lock();
			void unlock();
			pthread_mutex_t * getMutexLockPtr();

		private:
			pthread_mutex_t _mutex;
			bool _isLocked;
	};

	class MutexLockGuard
	{
		public:
			MutexLockGuard(MutexLock & mutex)
				: _mutex(mutex)
			{
				_mutex.lock();
			}

			~MutexLockGuard()
			{
				_mutex.unlock();
			}
		private:
			MutexLock & _mutex;
	};

}//end of namespace


#endif

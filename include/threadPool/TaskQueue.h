///
/// @file    TaskQueue.h
/// @author
///

#ifndef __CC_TASKQUEUE_H__
#define __CC_TASKQUEUE_H__

#include "MutexLock.h"
#include "Condition.h"
#include <iostream>
#include <queue>
#include <functional>
using std::cout;
using std::endl;
using std::queue;


namespace cc
{

	class TaskQueue
	{
		typedef std::function<void()> ElemType;
		public:
		TaskQueue(size_t size);

		bool full() const;
		bool empty() const;
		void push(const ElemType & value);
		void push(ElemType && value);
		ElemType pop();

		void wakeup();

		private:
		size_t _size;
		queue<ElemType> _que;
		MutexLock _mutex;
		Condition _notFull;
		Condition _notEmpty;
		bool _flag;
	};

}//end of namespace
#endif

///
/// @file    Threadpool.h
/// @author
///

#ifndef __CC_THREADPOOL_H__
#define __CC_THREADPOOL_H__

#include "TaskQueue.h"

#include <iostream>
#include <vector>
#include <memory>
#include <functional>
using std::cout;
using std::endl;
using std::vector;
using std::shared_ptr;


namespace cc
{

	typedef std::function<void()> Task;

	class Thread;

	class Threadpool
	{
		public:
			Threadpool(size_t threadNum, size_t queSize);
			~Threadpool();

			void start();
			void stop();
			void addTask(const Task & task);
		private:
			Task getTask();

			void threadFunc();

		private:
			size_t _threadNum;
			size_t _queSize;
			vector<shared_ptr<Thread>> _threads;
			TaskQueue _taskQue;
			bool _isExit;
	};

}//end of namespace

#endif

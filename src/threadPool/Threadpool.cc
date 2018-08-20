///
/// @file    Threadpool.cc
/// @author
///

#include "Threadpool.h"
#include "Thread.h"

#include <unistd.h>
#include <iostream>
using std::cout;
using std::endl;

namespace cc
{

	Threadpool::Threadpool(size_t threadNum, size_t queSize)
		: _threadNum(threadNum)
		  , _queSize(queSize)
		  , _taskQue(_queSize)
		  , _isExit(false)
	{
		_threads.reserve(_threadNum);
	}

	Threadpool::~Threadpool()
	{
		if(!_isExit)
			stop();
	}


	void Threadpool::start()
	{
		for(size_t idx = 0; idx != _threadNum; ++idx)
		{
			shared_ptr<Thread> sp(new Thread(
						std::bind(&Threadpool::threadFunc, this)));
			_threads.push_back(sp);
		}

		for(auto & thread : _threads)
		{
			thread->start();
		}
	}

	//线程池退出就是要让每一个子线程退出
	void Threadpool::stop()
	{
		if(!_isExit)
		{
			//保证任务队列之中的任务全部执行完毕
			while(!_taskQue.empty())
			{
				::sleep(1);
			}

			//之后，才去退出线程池
			_isExit = true;
			//唤醒taskQueue的条件变量
			_taskQue.wakeup();

			for(auto & thread : _threads)
			{
				thread->join();//如果线程没有退出，就一直阻塞在这里
			}
		}
	}



	void Threadpool::addTask(const Task & task)
	{
		_taskQue.push(task);
	}

	Task Threadpool::getTask()
	{
		return _taskQue.pop();
	}

	//每一个子线程都是在执行threadFunc成员函数
	void Threadpool::threadFunc()
	{
		while(!_isExit)
		{
			Task task = getTask();
			if(task)
				task();
		}
	}



}//end of namespace cc

///
/// @file    TaskQueue.cc
/// @author
///

#include "TaskQueue.h"

#include <iostream>
using std::cout;
using std::endl;

namespace cc
{

	TaskQueue::TaskQueue(size_t size)
		: _size(size)
		  , _mutex()
		  , _notFull(_mutex)
		  , _notEmpty(_mutex)
		  , _flag(true)
	{}


	bool TaskQueue::full() const
	{
		return _que.size() == _size;
	}

	bool TaskQueue::empty() const
	{	
		return _que.empty();
	}

	//该操作在生产者线程中进行
	void TaskQueue::push(const ElemType & value)
	{
		//要操作临界资源_que,上来就要先加锁
		MutexLockGuard mlg(_mutex);//给互斥锁加上一个保护类，防止死锁
		while(full())	//如果条件不满足，就等待，等待的过程中会解锁
			_notFull.wait();//使用while循环，是为了防止条件变量被异常唤醒
		//一旦条件变量被唤醒，就会执行加锁

		_que.push(value);
		_notEmpty.notify();
	}

	//该方法在消费者线程中进行
	TaskQueue::ElemType TaskQueue::pop()
	{
		MutexLockGuard mlg(_mutex);
		while(_flag && empty())
			_notEmpty.wait();

		if(_flag)
		{
			ElemType value = _que.front();
			_que.pop();
			_notFull.notify();
			return value;
		}else
			return NULL;
	}

	void TaskQueue::wakeup()
	{
		_flag = false;
		_notEmpty.notifyall();
	}


}//end of namespace cc

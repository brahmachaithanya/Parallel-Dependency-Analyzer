#ifndef THREAD_PL_H
#define THREAD_PL_H

/////////////////////////////////////////////////////////////////////
//  ThreadPl.h  -  Creates threadpool							   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 15 7000, Windows 10			   //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
/////////////////////////////////////////////////////////////////////

/*
Module Operations:
==================
This module creates a thread pool with user specified number of threads in the pool

Public Interface:
=================
ThreadPl<WorkResult, FileName> threadPool
threadPool.performTask(FileName file)
threadPool.setThreadPoolSize(size_t size);
threadPool.executeWorkItems(WorkItem<Output, FileName>* pWi);
threadPool.BeginThread(bool restart = false);
threadPool.fetchOutputQueue();

Build Process:
==============
Required files
- Cpp11-BlockingQueue.h

Build commands
- devenv Project_3.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 15 Apr 16
- Initial release
*/

#include <thread>
#include<vector>
#include<functional>
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"

template<typename Output, typename FileName>
using WorkItem = std::function<Output(FileName)>;

template<typename Output, typename FileName>
class ThreadPl {
public:
	ThreadPl();
	void setThreadPoolSize(size_t size);
	void executeWorkItems(WorkItem<Output, FileName>* pWi);
	void BeginThread(bool restart = false);
	void sleep();
	Output fetchOutputQueue();
	void performTask(FileName pFile);
	~ThreadPl();

private:
	size_t _threadCount;
	std::vector<std::thread*> _threads;
	BlockingQueue<FileName> _fileQueue;
	BlockingQueue<Output> _outputQueue;
	WorkItem<Output, FileName>* pWi;
	std::mutex _mtx1;
};

template<typename Output, typename FileName>
ThreadPl<Output, FileName>::ThreadPl() {
}

template<typename Output, typename FileName>
void ThreadPl<Output, FileName>::setThreadPoolSize(size_t size) {
	_threadCount = size;
}

template<typename Output, typename FileName>
void ThreadPl<Output, FileName>::executeWorkItems(WorkItem<Output, FileName>* Wi) {
	pWi = Wi;
}

template<typename Output, typename FileName>
Output ThreadPl<Output, FileName>::fetchOutputQueue()
{
	return _outputQueue.deQ();
}

template<typename Output, typename FileName>
void ThreadPl<Output, FileName>::performTask(FileName pFile)
{
	_fileQueue.enQ(pFile);
}

template<typename Output, typename FileName>
void ThreadPl<Output, FileName>::sleep()
{
	for (size_t i = 0; i < _threads.size(); i++) {
		_threads[i]->join();
	}
}

template<typename Output, typename FileName>
void ThreadPl<Output, FileName>::BeginThread(bool restart = false)
{
	std::function<void()> threadProc =
		[&]() {
		while (true)
		{
			FileName pFile = _fileQueue.deQ();
			if (pFile == "")
			{
				_fileQueue.enQ("");
				return;
			}
			std::unique_lock<std::mutex> l(_mtx1);
			Output result = (*pWi)(pFile);
			_outputQueue.enQ(result);
		}
	};
	for (size_t i = 0; i < _threadCount; i++) {
		_threads.push_back(new std::thread(threadProc));
	}
}

template<typename Output, typename FileName>
ThreadPl<Output, FileName>::~ThreadPl()
{
	for (size_t i = 0; i < _threads.size(); i++) {
		delete _threads[i];
	}
}

#endif
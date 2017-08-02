#ifndef THREAD_EXECUTION_TASK_H
#define THREAD_EXECUTION_TASK_H
/////////////////////////////////////////////////////////////////////
//  ThreadTask.h - Wrapper class for Thread Pool				   //
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
Wrapper Class is created for Thread Pool

Public Interface:
=================
ThreadTask<std::string, std::string> task
task.performTask(FileName file)

Build Process:
==============
Required files
- ThreadPl.h

Build commands
- devenv Project_3.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 15 Apr 16
- Initial release
*/
#include <string>
#include "ThreadPl.h"

template<typename Output, typename FileName>
class ThreadTask {
public:
	void startThreadPool(size_t noOfThreads);
	void performTask(FileName pFile);
	void executeWorkItems(WorkItem<Output, FileName>* pWi);
	Output getOutput();
	void sleep();
	void stopWork();
private:
	ThreadPl<Output, FileName> threadPl;
};

template<typename Output, typename FileName>
void ThreadTask<Output, FileName>::startThreadPool(size_t noOfThreads) {
	threadPl.setThreadPoolSize(noOfThreads);
	threadPl.BeginThread();
}

template<typename Output, typename FileName>
void ThreadTask<Output, FileName>::performTask(FileName pFile) {
	threadPl.performTask(pFile);
}

template<typename Output, typename FileName>
void ThreadTask<Output, FileName>::executeWorkItems(WorkItem<Output, FileName>* pWi) {
	threadPl.executeWorkItems(pWi);
}

template<typename Output, typename FileName>
Output ThreadTask<Output, FileName>::getOutput() {
	return threadPl.fetchOutputQueue();
}

template<typename Output, typename FileName>
void ThreadTask<Output, FileName>::sleep() {
	threadPl.sleep();
}

template<typename Output, typename FileName>
void ThreadTask<Output, FileName>::stopWork() {
	threadPl.performTask("");
}

#endif

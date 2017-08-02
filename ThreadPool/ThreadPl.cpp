#include "ThreadPl.h"
/////////////////////////////////////////////////////////////////////
//  ThreadPool.cpp												   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 15 7000, Windows 10			   //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
/////////////////////////////////////////////////////////////////////

using ResultQueue = std::string;
using FileName = std::string;

#ifdef THREAD_PL
int main(int argc, char *argv[]) {
	ThreadPl<ResultQueue, FileName> processor;
	processor.setThreadPoolSize(5);
	processor.BeginThread();

	WorkItem<ResultQueue, FileName> wi = [](FileName) {
		return " Message from thread";
	};
	processor.executeWorkItems(&wi);

	for (int i = 0; i < 5; i++) {
		processor.performTask("perform task");
	}

	for (int i = 0; i < 5; i++) {
		std::cout << "\n  " << processor.fetchOutputQueue();
	}
	processor.performTask("");
	processor.sleep();
}
#endif
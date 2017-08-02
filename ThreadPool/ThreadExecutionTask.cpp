#include "ThreadExecutionTask.h"
/////////////////////////////////////////////////////////////////////
//  ThreadTask.cpp												   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 15 7000, Windows 10			   //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
/////////////////////////////////////////////////////////////////////

#ifdef THREAD_EXECUTION_TASK

int main() {
	WorkItem<std::string, std::string> wi = [](std::string) {
		return "Message from thread ";
	};

	ThreadTask<std::string, std::string> task;
	task.startThreadPool(5);
	task.executeWorkItems(&wi);
	for (int i = 0; i < 5; i++) {
		task.performTask("Performing task : Message during thread execution");
	}

	for (int i = 0; i < 5; i++) {
		std::cout << "\n  " << task.getOutput();
	}

	task.stopWork();
	task.sleep();
	return 0;
}

#endif 


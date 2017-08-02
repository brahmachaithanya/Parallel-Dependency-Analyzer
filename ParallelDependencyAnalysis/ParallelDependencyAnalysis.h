#ifndef PARALLEL_DEPENDENCY_H
#define PARALLEL_DEPENDENCY_H
/////////////////////////////////////////////////////////////////////
//  ParallelDependencyAnalysis.h - For Pass1 for the dependency    //
//									analyser					   //
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
This module uses Thread task and does the dependency analysis parallely

Public Interface:
=================

parallelDependencyAnalysis.Pass1(std::vector<std::string> Files)
parallelDependencyAnalysis.Pass2(std::vector<std::string> Files)

Build Process:
==============
Required files
- TypeTable.h, ThreadExecutionTask.h, ConfigureParser.h, Parser.h, FileMgr.h, DependencyAnalyzer.h

Build commands
- devenv Project_3.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 15 Apr 16
- Initial release
*/

#include "../TypeTable/TypeTable.h"
#include "../ThreadPool/ThreadExecutionTask.h"
#include "../Parser/ConfigureParser.h"
#include "../Parser/Parser.h"
#include "../FileMgr/FileMgr.h"
#include "../DependencyAnalyzer/DependencyAnalyzer.h"

class ParallelDependencyAnalysis {
public:
	void Pass1(std::vector<std::string> inputFiles);
	void Pass2(std::vector<std::string> inputFiles);
	std::unordered_map<std::string, std::unordered_map<std::string, int>> dependencyTable;
private:
	TypeTable<TypeTableValue> completeTypeTable;
	ThreadTask< TypeTable<TypeTableValue>, std::string> task;
	ThreadTask< TypeTable<TypeTableValue>, std::string> dependencyTask;
};

#endif 


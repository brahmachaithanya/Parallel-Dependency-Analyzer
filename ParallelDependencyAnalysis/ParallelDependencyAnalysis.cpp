/////////////////////////////////////////////////////////////////////
//  ParallelDependencyAnalysis.cpp - For Pass1 for the dependency  //
//									analyser					   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 15 7000, Windows 10			   //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
/////////////////////////////////////////////////////////////////////

#include "ParallelDependencyAnalysis.h"

using namespace Utilities;
using namespace FileSystem;
using namespace Scanner;

#define Util StringHelper

void ParallelDependencyAnalysis::Pass1(std::vector<std::string> inputFiles) {
	WorkItem<TypeTable<TypeTableValue>, std::string> typeAnalysisWorkItem = [&](std::string file) {
		std::string fileSpec = FileSystem::Path::getFullFileSpec(file);
		std::string msg = "\nProcessing file " + fileSpec;
		Util::title(msg);
		putline();

		ConfigParseToConsole configure;
		Parser* pParser = configure.Build();
		if (pParser)
		{
			if (!configure.Attach(file))
			{
				std::cout << "\n  could not open file " << fileSpec << std::endl;
			}
		}
		else
		{
			std::cout << "\n\n  Parser not built\n\n";
		}
		
		Repository::getInstance()->getFileName() = file; 
		while (pParser->next())
			pParser->parse();
		TypeTable<TypeTableValue> typeTable = Repository::getInstance()->TypeTable();
		typeTable.showTypeTable();
		return typeTable;
	};
	
	task.startThreadPool(4);
	task.executeWorkItems(&typeAnalysisWorkItem);

	for (size_t i = 0; i < inputFiles.size(); i++)
		task.performTask(inputFiles[i]);

	task.stopWork();
	task.sleep();

	for (size_t i = 0; i < inputFiles.size(); i++) {
		TypeTable<TypeTableValue> table = task.getOutput();
		completeTypeTable.getRecords().insert(table.getRecords().begin(), table.getRecords().end());
	}

	std::cout << "\n\n Complete Type Table:\n\n";
	std::cout << "----------------------------------\n";
	completeTypeTable.showTypeTable();
};
void ParallelDependencyAnalysis::Pass2(std::vector<std::string> inputFiles) {
	DependencyAnalyzer dependencyAnalyzer(completeTypeTable);
	WorkItem< TypeTable<TypeTableValue>, std::string> dependencyWorkItem = [&](std::string file) {
		dependencyAnalyzer.analyze(file);
		TypeTable<TypeTableValue> table;
		return table;
	};

	dependencyTask.executeWorkItems(&dependencyWorkItem);
	dependencyTask.startThreadPool(4);
	for (size_t i = 0; i < inputFiles.size(); i++)
		dependencyTask.performTask(inputFiles[i]);
	dependencyTask.stopWork();
	dependencyTask.sleep();

	std::cout << "\n\n Dependency Table:\n\n";
	std::cout << "----------------------------------\n";
	dependencyAnalyzer.display();
	dependencyTable = dependencyAnalyzer.dependencyTable;
};

#ifdef DEPENDENCY_ANALYSIS
int main()
{
	ParallelDependencyAnalysis obj;
	std::vector<std::string> inputFiles;
	inputFiles.push_back("ParallelDependencyAnalysis.h");
	inputFiles.push_back("ParallelDependencyAnalysis.cpp");
	obj.Pass1(inputFiles);
	obj.Pass2(inputFiles);
}

#endif // DEBUG


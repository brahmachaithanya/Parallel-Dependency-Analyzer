#ifndef DEPENDENCYANALYZER_H
#define DEPENDENCYANALYZER_H

/////////////////////////////////////////////////////////////////////
//  DependencyAnalyzer.h - Used to analyse the dependency in files //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
/////////////////////////////////////////////////////////////////////

/*
Module Operations:
==================
This module analyses the dependency in files which is stored to a type table 

Public Interface:
=================
DependencyAnalyzer dependencyAnalyzer(completeTypeTable)
dependencyAnalyzer.analyze(String fileName)
dependencyAnalyzer.display()

Build Process:
==============
Required files
- TypeTable.h, FileMgr.h

Build Command: devenv Project_3.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 15 Apr 16
- Initial release
*/

#include "string"
#include "unordered_map"
#include "../TypeTable/TypeTable.h"
#include "../FileMgr/FileMgr.h"

class DependencyAnalyzer {

public:
	DependencyAnalyzer(TypeTable<TypeTableValue> table);
	void analyze(std::string file);
	void display();
	std::string extractFileName(const std::string& s, char c);

	std::unordered_map<std::string, std::unordered_map<std::string, int>> dependencyTable;

private:
	TypeTable<TypeTableValue> typeTable;

};

#endif

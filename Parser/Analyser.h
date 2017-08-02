#ifndef ANALYZER_H
#define ANALYZER_H
/////////////////////////////////////////////////////////////////////
//  Analyser.h - Display metrics								   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 5000 Windows 10				   //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
//  Source:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
Module Operations :
== == == == == == == == ==
This module displays metrics and warnings.

Build commands
- devenv Project_3.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 14 Mar 16
- first release
*/
#include "AbstractSyntaxTree.h"

class Analyser {
public:
	Analyser();
	void displayMetrics(element* root);
	void displayTable(std::string fileName);
	void displayWarnings();
private:
	AbstractSyntaxTree tree;
	std::string functions;
};

#endif


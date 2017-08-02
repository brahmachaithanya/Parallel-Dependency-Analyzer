#ifndef AbstractSyntaxTree_H
#define AbstractSyntaxTree_H
/////////////////////////////////////////////////////////////////////
//  AbstractSyntaxTree.h - Tree									   //
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
This module define an abstract syntax tree

Build commands
- devenv Project_3.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 14 Mar 16
- first release
*/

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include "../ScopeStack/ScopeStack.h"

struct element {

	std::string type;
	std::string name;
	size_t lineCountStart = 0;
	size_t lineCountEnd = 0;
	size_t lineCount = 0;
	size_t complexity = 1;
	std::string show()
	{
		std::ostringstream temp;
		temp << "(";
		temp << type;
		temp << ", ";
		temp << name;
		temp << ", ";
		temp << lineCountStart;
		temp << ", ";
		temp << lineCountEnd;
		temp << ", ";
		temp << lineCount;
		temp << ", ";
		temp << complexity;
		temp << ")";
		return temp.str();
	}
	std::vector<element*> _children;
};

class AbstractSyntaxTree {
public:
	AbstractSyntaxTree();
	size_t size();
	element* getRoot();
	element* getTopElement();
	void TreeWalk(element* pItem);
	void addChildNode(element* item);
	void removeNode(size_t lineCountEnd);
	void findComplexity();
private:
	ScopeStack<element*> treeNode;
};

#endif


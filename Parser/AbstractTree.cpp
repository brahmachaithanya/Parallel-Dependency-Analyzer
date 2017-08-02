/////////////////////////////////////////////////////////////////////
//  AbstractTree.cpp											   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 5000 Windows 10				   //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
/////////////////////////////////////////////////////////////////////

#include "AbstractSyntaxTree.h"

	AbstractSyntaxTree::AbstractSyntaxTree() {
		element* pItem = new element;
		pItem->type = "unknown";
		pItem->name = "Global Namespace";
		pItem->lineCountStart = 0;
		treeNode.push(pItem);
	}

	element* AbstractSyntaxTree::getRoot()
	{
		return *treeNode.begin();
	}
	element* AbstractSyntaxTree::getTopElement()
	{
		return treeNode.top();
	}

	void AbstractSyntaxTree::TreeWalk(element* root)
	{
		static size_t indentLevel = 0;
		std::cout << "\n  " << std::string(2 * indentLevel, ' ') << root->show();
		auto iter = root->_children.begin();
		++indentLevel;
		while (iter != root->_children.end())
		{
			TreeWalk(*iter);
			++iter;
		}
		--indentLevel;
	}

	void AbstractSyntaxTree::addChildNode(element* item) {
		(treeNode.top()->_children).push_back(item); 
		treeNode.push(item);
	}

	void AbstractSyntaxTree::removeNode(size_t endLineNo) {
		treeNode.top()->lineCountEnd = endLineNo;
		treeNode.top()->lineCount = (endLineNo - treeNode.top()->lineCountStart) + 1;
		findComplexity();
		treeNode.pop();
	}

	void AbstractSyntaxTree::findComplexity() {
		size_t sumOfComplexity = 0;
		std::vector<element*>::iterator itr = (treeNode.top()->_children).begin();
		while (itr != (treeNode.top()->_children).end())
		{
			sumOfComplexity = sumOfComplexity + (*itr)->complexity;
			itr++;
		}
		treeNode.top()->complexity = treeNode.top()->complexity + sumOfComplexity;
	}

	size_t AbstractSyntaxTree::size() {
		return treeNode.size();
	}
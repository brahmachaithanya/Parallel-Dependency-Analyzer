/////////////////////////////////////////////////////////////////////
//  Analyser.cpp												   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 5000 Windows 10				   //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
/////////////////////////////////////////////////////////////////////
#include "Analyser.h"
#include "ActionsAndRules.h"

Analyser::Analyser() {
	tree = Repository::getInstance()->Tree();
	functions = "";
}

void Analyser::displayMetrics(element* root) {
	auto itr = root->_children.begin();
	while (itr != root->_children.end())
	{
		if ((*itr)->type == "function") {
			std::cout <<"*\t"<< (*itr)->complexity << "\t" << (*itr)->lineCount  << "\t" << (*itr)->name;
			if ((*itr)->complexity > 10 || (*itr)->lineCount > 50) {
				functions += (*itr)->name + "\n";
			}
			std::cout << std::endl;
		}
		displayMetrics(*itr);
		itr++;
	}
}

void Analyser::displayWarnings() {
	std::cout << "\nWarnings :\n";
	if (functions == "") {
		std::cout << "All functions are good.";
	}
	else {
		std::cout << functions;
	}
}

void Analyser::displayTable(std::string fileName) {
	std::cout << "\n\n\n";
	std::cout << "Metric Analysis for "+fileName;
	std::cout << "\n----------------\n";
	std::cout << "\t" << "CC" << "\t" <<  "Count"   << "\t" << "Function" << std::endl;
	std::cout << "****************************************************" << std::endl;
	displayMetrics(tree.getRoot());
	std::cout << "****************************************************" << std::endl;
}
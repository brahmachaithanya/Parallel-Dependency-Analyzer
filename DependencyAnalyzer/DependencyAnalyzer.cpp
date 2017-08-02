/////////////////////////////////////////////////////////////////////
//  DependencyAnalyzer.cpp -Used to analyse the dependency in files//
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
/////////////////////////////////////////////////////////////////////

#include "DependencyAnalyzer.h"
#include "../Tokenizer/Tokenizer.h"

using namespace Scanner;
using namespace FileSystem;

DependencyAnalyzer::DependencyAnalyzer(TypeTable<TypeTableValue> table) {
	typeTable = table;
}
void DependencyAnalyzer::analyze(std::string file) {
	std::string fileSpec = FileSystem::Path::getFullFileSpec(file);
	Toker toker;
	std::fstream in(fileSpec);
	toker.attach(&in);

	while (true)
	{
		std::string token = toker.getTok();
		if (token == "")
			break;
		
		if (typeTable.find(token)) {
			TypeTableValue record = typeTable[token];
			std::string dependantFile = extractFileName(record._fileName, (unsigned char)'//');
			std::string currentFile = extractFileName(file, (unsigned char)'//');
			if (currentFile != dependantFile)
				dependencyTable[currentFile].insert(std::make_pair(dependantFile, 0));
		}
	}
}

void DependencyAnalyzer::display() {
	std::cout << "\n\n";
		for (auto& record : dependencyTable) {
		std::string fileNames = "";
		std::cout << "\n \n";
		std::cout << "File =";
		std::cout << std::setw(30) << record.first;
		for (auto& file : record.second) {
			fileNames = fileNames + file.first + ", ";
		}
		std::cout << "\nDependents = ";
		std::cout << std::setw(400) << fileNames;
		std::cout << "\n  ";
	}
}

std::string DependencyAnalyzer::extractFileName(const std::string& s, char c) {
	std::vector<std::string> v;
	std::string::size_type i = 0;
	std::string::size_type j = s.find(c);

	while (j != std::string::npos) {
		v.push_back(s.substr(i, j - i));
		i = ++j;
		j = s.find(c, j);

		if (j == std::string::npos)
			v.push_back(s.substr(i, s.length()));
	}
	return v[v.size() - 1];
}
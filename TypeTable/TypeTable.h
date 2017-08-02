#ifndef TYPETABLE_H
#define TYPETABLE_H
/////////////////////////////////////////////////////////////////////
//  TypeTable.h - Stores all the types of a given file			   //
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
This module provides the table structure to hold the TYpes identified during typeAnalysis

Public Interface:
=================
TypeTable<TableRecord> table
table.showTypeTable()

Build Process:
==============
Required files
- Utilities.h

Maintenance History:
====================
ver 1.0 : 15 Apr 16
- Initial release
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <string>
#include "../Utilities/Utilities.h"

class TypeTableValue {
public:
	using TypeName = std::string;
	using FileName = std::string;
	using Namespace = std::string;
	using Type = std::string;

	std::string& getName() { return _name; }
	Type& getType() { return _type; }
	FileName& getFilename() { return _fileName; }
	Namespace& getNamespace() { return _namespace; }
public:
	std::string _name;
	std::string _type;
	std::string _fileName;
	std::string _namespace;
};

//data holder for the Types using unordered map
template<typename T>
class TypeTable
{
public:

	using TypeName = std::string;
	using Value = T;
	using iterator = typename std::unordered_map<TypeName, T>::iterator;

	void addRecord(const T& record, std::string key)
	{
		_records[key] = record;
	}
	T& operator[](std::string key)
	{
		return _records.at(key);
	}
	bool find(std::string key) {
		std::unordered_map<TypeName, T>::const_iterator found = _records.find(key);
		if (found == _records.end())
			return false;
		return true;
	}

	void showTypeTable()
	{
		std::cout << std::left << "\n  ";
		std::cout << std::setw(30) << "Name";
		std::cout << std::setw(20) << "Type";
		std::cout << std::setw(20) << "Filename";

		for (auto& record : _records) {
			std::cout << "\n  ";
			std::cout << std::setw(30) << record.second.getName();
			std::cout << std::setw(20) << record.second.getType();
			std::cout << std::setw(20) << record.second.getFilename();
		}
	}

	std::unordered_map<TypeName, T>& getRecords() {
		return _records;
	}
private:
	//unordered map to hold the records
	std::unordered_map<TypeName, Value> _records; 
};

#endif

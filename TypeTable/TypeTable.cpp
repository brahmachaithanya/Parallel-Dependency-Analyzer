//#ifdef TYPE_TABLE
/////////////////////////////////////////////////////////////////////
//  TypeTable.cpp - Stores type table of files					   //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 15 7000, Windows 10			   //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
/////////////////////////////////////////////////////////////////////

#include "TypeTable.h"

int main() {
	TypeTable<TypeTableValue> table;

	TypeTableValue value;
	value.getName() = "Tokenizer";
	value.getType() = "class";
	value.getFilename() = "Tokenizer.h";

	table.addRecord(value, value.getName());
	value.getName() = "display";
	value.getType() = "function";
	value.getFilename() = "Tokenizer.h";

	table.addRecord(value, value.getName());

	table.showTypeTable();
}
//#endif
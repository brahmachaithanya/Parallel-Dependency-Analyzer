#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 2.3                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 5000 Windows 10				   //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
//  Source:        Jim Fawcett, CST 2-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations:
  ==================
  This module defines several action classes.  Its classes provide
  specialized services needed for specific applications.  The modules
  Parser, SemiExpression, and Tokenizer, are intended to be reusable
  without change.  This module provides a place to put extensions of
  these facilities and is not expected to be reusable.

  Public Interface:
  =================
  Toker t(someFile);              // create tokenizer instance
  SemiExp se(&t);                 // create a SemiExp attached to tokenizer
  Parser parser(se);              // now we have a parser
  Rule1 r1;                       // create instance of a derived Rule class
  Action1 a1;                     // create a derived action
  r1.addAction(&a1);              // register action with the rule
  parser.addRule(&r1);            // register rule with parser
  while(se.getSemiExp())          // get semi-expression
	parser.parse();               //   and parse it

  Build Process:
  ==============
  Required files
	- Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,
	  ActionsAndRules.h, ActionsAndRules.cpp, ConfigureParser.cpp,
	  ItokCollection.h, SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp

	  Build commands
	  - devenv Project_3.sln /rebuild debug

  Maintenance History:
  ====================
  ver 2.3 : 15 Apr 16
  - added new Rules for identifying classes, struct and enum.
  - type analysis is added to Repository Class
  ver 2.2 : 14 Mar 16
  - added new rules for handling control statements
  ver 2.1 : 15 Feb 16
  - small functional change to a few of the actions changes display strategy
  - preface the (new) Toker and SemiExp with Scanner namespace
  ver 2.0 : 01 Jun 11
  - added processing on way to building strong code analyzer
  ver 1.1 : 17 Jan 09
  - changed to accept a pointer to interfaced ITokCollection instead
	of a SemiExpression
  ver 1.0 : 12 Jan 06
  - first release

*/

#include <queue>
#include <string>
#include <sstream>
#include "Parser.h"
#include "../SemiExp/itokcollection.h"
#include "../ScopeStack/ScopeStack.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "AbstractSyntaxTree.h"
#include "../TypeTable/TypeTable.h"

///////////////////////////////////////////////////////////////
// ScopeStack element is application specific
/* ToDo:
 * - chanage lineCount to two fields: lineCountStart and lineCountEnd
 */

 ///////////////////////////////////////////////////////////////
 // Repository instance is used to share resources
 // among all actions.
 /*
  * ToDo:
  * - add AST Node class
  * - provide field to hold root of AST
  * - provide interface to access AST
  * - provide helper functions to build AST, perhaps in Tree class
  */
class Repository  // application specific
{
	//ScopeStack<element*> stack;
	Scanner::Toker* p_Toker;
	static Repository* repository;
	AbstractSyntaxTree tree;
	
	TypeTable<TypeTableValue> typeTable;
	std::string fileName;

public:
	Repository(Scanner::Toker* pToker)
	{
		p_Toker = pToker;
		repository = this;
	}
	static Repository* getInstance() {
		return repository;
	}
	Scanner::Toker* Toker()
	{
		return p_Toker;
	}
	size_t lineCount()
	{
		return (size_t)(p_Toker->currentLineCount());
	}
	AbstractSyntaxTree& Tree() {
		return tree;
	}
	TypeTable<TypeTableValue>& TypeTable() {
		return typeTable;
	}
	std::string& getFileName() {
		return fileName;
	}
};

///////////////////////////////////////////////////////////////
// rule to detect beginning of anonymous scope

class BeginningOfScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		if (pTc->find("{") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePush : public IAction
{
	Repository* p_Repos;
public:
	HandlePush(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		element *elem = new element;
		elem->type = "unknown";
		elem->name = "anonymous";
		elem->lineCountStart = p_Repos->lineCount();
		p_Repos->Tree().addChildNode(elem);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect end of scope

class EndOfScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		if (pTc->find("}") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePop : public IAction
{
	Repository* p_Repos;
public:
	HandlePop(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		if (p_Repos->Tree().size() == 0)
			return;
		p_Repos->Tree().removeNode(p_Repos->lineCount());

	}
};

///////////////////////////////////////////////////////////////
// rule to detect preprocessor statements

class PreprocStatement : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		if (pTc->find("#") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to print preprocessor statement to console

class PrintPreproc : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		//std::cout << "\n\n  Preproc Stmt: " << pTc->show().c_str();
	}
};

///////////////////////////////////////////////////////////////
// rule to detect function definitions

class FunctionDefinition : public IRule
{
public:
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i < 5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("(");
			if (len < tc.length() && !isSpecialKeyWord(tc[len - 1]))
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push function name onto ScopeStack

class PushFunction : public IAction
{
	Repository* p_Repos;
public:
	PushFunction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// push function scope
		std::string name = (*pTc)[pTc->find("(") - 1];
		if (name == "]") { //handle function 
			name = (*pTc)[pTc->find("(") - 3];
			name += (*pTc)[pTc->find("(") - 2];
			name += (*pTc)[pTc->find("(") - 1];
		}

		// push function scope
		name = (*pTc)[pTc->find("(") - 1];
		element* elem = new element();
		elem->type = "function";
		elem->name = name;
		elem->lineCountStart = p_Repos->lineCount();
		p_Repos->Tree().addChildNode(elem);

		
		TypeTableValue value;
		value.getName() = name;
		value.getType() = "function";
		value.getFilename() = p_Repos->getFileName();
		if (!p_Repos->TypeTable().find(name))
			p_Repos->TypeTable().addRecord(value, name);
	}
};

///////////////////////////////////////////////////////////////
// action to send semi-expression that starts a function def
// to console

class PrintFunction : public IAction
{
	Repository* p_Repos;
public:
	PrintFunction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		//std::cout << "\n  FuncDef: " << pTc->show().c_str();
	}
};

///////////////////////////////////////////////////////////////
// action to send signature of a function def to console

class PrettyPrintFunction : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		pTc->remove("public");
		pTc->remove(":");
		pTc->trimFront();
		size_t len = pTc->find(")");
		//std::cout << "\n\n  Pretty Stmt:    ";
		/*for (size_t i = 0; i < len + 1; ++i)
			std::cout << (*pTc)[i] << " ";
		std::cout << "\n";*/
	}
};

///////////////////////////////////////////////////////////////
// rule to detect declaration 

class Declaration : public IRule          // declar ends in semicolon
{                                         // has type, name, modifiers &
public:                                   // initializers.  So eliminate
	bool isModifier(const std::string& tok) // modifiers and initializers.
	{                                       // If you have two things left
		const size_t numKeys = 22;            // its declar else executable.
		const static std::string keys[numKeys] = {
		  "const", "extern", "friend", "mutable", "signed", "static",
		  "typedef", "typename", "unsigned", "volatile", "&", "*", "std", "::",
		  "public", "protected", "private", ":", "typename", "typedef", "++", "--"
		};
		for (int i = 0; i < numKeys; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	void condenseTemplateTypes(ITokCollection& tc)
	{
		size_t start = tc.find("<");
		size_t end = tc.find(">");
		if (start >= end || start == 0)
			return;
		else
		{
			if (end == tc.length())
				end = tc.find(">::");
			if (end == tc.length())
				return;
		}
		std::string save = tc[end];
		std::string tok = tc[start - 1];
		for (size_t i = start; i < end + 1; ++i)
			tok += tc[i];
		for (size_t i = start; i < end + 1; ++i)
			tc.remove(start);
		if (save == ">::")
		{
			tok += tc[start + 1];
			tc.remove(start);
		}
		tc[start - 1] = tok;
		//std::cout << "\n  -- " << tc.show();
	}
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i < 5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	void removeInvocationParens(ITokCollection& tc)
	{
		size_t start = tc.find("(");
		size_t end = tc.find(")");
		if (start >= end || end == tc.length() || start == 0)
			return;
		if (isSpecialKeyWord(tc[start - 1]))
			return;
		for (size_t i = start; i < end + 1; ++i)
			tc.remove(start);
		//std::cout << "\n  -- " << tc.show();
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& in = *pTc;
		Scanner::SemiExp tc;
		for (size_t i = 0; i < in.length(); ++i)
			tc.push_back(in[i]);

		if (tc[tc.length() - 1] == ";" && tc.length() > 2)
		{
			//std::cout << "\n  ++ " << tc.show();
			removeInvocationParens(tc);
			condenseTemplateTypes(tc);

			// remove modifiers, comments, newlines, returns, and initializers

			Scanner::SemiExp se;
			for (size_t i = 0; i < tc.length(); ++i)
			{
				if (isModifier(tc[i]))
					continue;
				if (se.isComment(tc[i]) || tc[i] == "\n" || tc[i] == "return")
					continue;
				if (tc[i] == "=" || tc[i] == ";")
					break;
				else
					se.push_back(tc[i]);
			}
			//std::cout << "\n  ** " << se.show();
			if (se.length() == 2)  // type & name, so declaration
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to show declaration 

class ShowDeclaration : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		Scanner::SemiExp se;
		for (size_t i = 0; i < tc.length(); ++i)
			if (!se.isComment(tc[i]))
				se.push_back(tc[i]);
//		std::cout << "\n  Declaration: " << se.show();
	}
};

///////////////////////////////////////////////////////////////
// rule to detect executable 

class Executable : public IRule           // declar ends in semicolon
{                                         // has type, name, modifiers &
public:                                   // initializers.  So eliminate
	bool isModifier(const std::string& tok) // modifiers and initializers.
	{                                       // If you have two things left
		const size_t numKeys = 22;            // its declar else executable.
		const static std::string keys[numKeys] = {
		  "const", "extern", "friend", "mutable", "signed", "static",
		  "typedef", "typename", "unsigned", "volatile", "&", "*", "std", "::",
		  "public", "protected", "private", ":", "typename", "typedef", "++", "--"
		};
		for (int i = 0; i < numKeys; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	void condenseTemplateTypes(ITokCollection& tc)
	{
		size_t start = tc.find("<");
		size_t end = tc.find(">");
		if (start >= end || start == 0)
			return;
		else
		{
			if (end == tc.length())
				end = tc.find(">::");
			if (end == tc.length())
				return;
		}
		std::string save = tc[end];
		std::string tok = tc[start - 1];
		for (size_t i = start; i < end + 1; ++i)
			tok += tc[i];
		for (size_t i = start; i < end + 1; ++i)
			tc.remove(start);
		if (save == ">::")
		{
			tok += tc[start + 1];
			tc.remove(start);
		}
		tc[start - 1] = tok;
		//std::cout << "\n  -- " << tc.show();
	}

	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i < 5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	void removeInvocationParens(ITokCollection& tc)
	{
		size_t start = tc.find("(");
		size_t end = tc.find(")");
		if (start >= end || end == tc.length() || start == 0)
			return;
		if (isSpecialKeyWord(tc[start - 1]))
			return;
		for (size_t i = start; i < end + 1; ++i)
			tc.remove(start);
		//std::cout << "\n  -- " << tc.show();
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& in = *pTc;
		Scanner::SemiExp tc;
		for (size_t i = 0; i < in.length(); ++i)
			tc.push_back(in[i]);

		if (tc[tc.length() - 1] == ";" && tc.length() > 2)
		{
			//std::cout << "\n  ++ " << tc.show();
			removeInvocationParens(tc);
			condenseTemplateTypes(tc);

			// remove modifiers, comments, newlines, returns, and initializers

			Scanner::SemiExp se;
			for (size_t i = 0; i < tc.length(); ++i)
			{
				if (isModifier(tc[i]))
					continue;
				if (se.isComment(tc[i]) || tc[i] == "\n" || tc[i] == "return")
					continue;
				if (tc[i] == "=" || tc[i] == ";")
					break;
				else
					se.push_back(tc[i]);
			}
			//std::cout << "\n  ** " << se.show();
			if (se.length() != 2)  // not a declaration
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to show executable

class ShowExecutable : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		Scanner::SemiExp se;
		for (size_t i = 0; i < tc.length(); ++i)
		{
			if (!se.isComment(tc[i]))
				se.push_back(tc[i]);
		}
		// show cleaned semiExp
		//std::cout << "\n  Executable: " << se.show();
	}
};

///////////////////////////////////////////////////////////////
// rule to detect Control Statements
class ControlStatement : public IRule
{
public:
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i < 5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("(");
			if ((len < tc.length() && isSpecialKeyWord(tc[len - 1])))
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to show Control Statements
class ShowControlStatement : public IAction
{
	Repository* p_Repos;
public:
	ShowControlStatement(Repository* pRepos)
	{
		p_Repos = pRepos;
	}

	void doAction(ITokCollection*& pTc)
	{
		std::string name = (*pTc)[pTc->find("(") - 1];
		element* elem = new element();
		elem->type = "Control Statement";
		elem->name = name;
		elem->lineCountStart = p_Repos->lineCount();
		p_Repos->Tree().addChildNode(elem);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect Try and Else Clauses 
class HandleTryAndElse : public IRule
{
public:
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "try", "else","do" };
		for (int i = 0; i < 3; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	void removeNewLine(ITokCollection*& pTc) {
		while (pTc->remove("\n"));
	}
	bool doTest(ITokCollection*& pTc)
	{
		removeNewLine(pTc);
		ITokCollection& tc = *pTc;
		size_t len = tc.find("{");
		if (len < tc.length() && tc.length() > 1 && isSpecialKeyWord(tc[len - 1]))
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to show Try and Else Clauses
class ShowTryAndElseStatement : public IAction
{
	Repository* p_Repos;
public:
	ShowTryAndElseStatement(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		std::string name = (*pTc)[pTc->find("{") - 1];
		element* elem = new element();
		elem->type = "TryAndElse statement";
		elem->name = name;
		elem->lineCountStart = p_Repos->lineCount();
		p_Repos->Tree().addChildNode(elem);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect classes
class FindClassDefinition : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("class");
			if (len < tc.length())
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// rule to show classes
class PushClassDefinition : public IAction
{
	Repository* p_Repos;
public:
	PushClassDefinition(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{

		std::string name = (*pTc)[pTc->find("class") + 1];
		element* elem = new element();
		elem->type = "class";
		elem->name = name;
		elem->lineCountStart = p_Repos->lineCount();
		p_Repos->Tree().addChildNode(elem);

		TypeTableValue value;
		value.getName() = name;
		value.getType() = "class";
		value.getFilename() = p_Repos->getFileName();
		p_Repos->TypeTable().addRecord(value, name);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect struct 
class FindStructDefinition : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("struct");
			if (len < tc.length())
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to show Struct
class PushStructDefinition : public IAction
{
	Repository* p_Repos;
public:
	PushStructDefinition(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{

		std::string name = (*pTc)[pTc->find("struct") + 1];
		element* elem = new element();
		elem->type = "struct";
		elem->name = name;
		elem->lineCountStart = p_Repos->lineCount();
		p_Repos->Tree().addChildNode(elem);

		TypeTableValue value;
		value.getName() = name;
		value.getType() = "struct";
		value.getFilename() = p_Repos->getFileName();
		p_Repos->TypeTable().addRecord(value, name);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect enums 
class FindEnumDefinition : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("enum");
			if (len < tc.length())
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};
///////////////////////////////////////////////////////////////
// action to show enums 
class ShowEnum : public IAction
{
	Repository* p_Repos;
public:
	ShowEnum(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		std::string name = (*pTc)[pTc->find("enum") + 1];
		element* elem = new element();
		elem->type = "enum";
		elem->name = name;
		elem->lineCountStart = p_Repos->lineCount();
		p_Repos->Tree().addChildNode(elem);

		TypeTableValue value;
		value.getName() = name;
		value.getType() = "enum";
		value.getFilename() = p_Repos->getFileName();
		p_Repos->TypeTable().addRecord(value, name);
	}
};

#endif
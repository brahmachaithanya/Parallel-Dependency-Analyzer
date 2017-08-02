/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - builds and configures parsers            //
//  ver 2.2                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2005                                 //
//  Platform:      Dell Inspiron 5000 Windows 10				   //
//  Application:   CSE687 PROJECT 3, Sp16		                   //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
//  Source:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include <fstream>
#include "Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

using namespace Scanner;

//----< destructor releases all parts >------------------------------

ConfigParseToConsole::~ConfigParseToConsole()
{
	// when Builder goes out of scope, everything must be deallocated

	delete pHandlePush;
	delete pBeginningOfScope;
	delete pHandlePop;
	delete pEndOfScope;
	delete pPrintFunction;
	delete pPushFunction;
	delete pFunctionDefinition;
	delete pDeclaration;
	delete pShowDeclaration;
	delete pExecutable;
	delete pShowExecutable;
	delete pControlStatement;
	delete pShowControlStatement;
	delete pHandleTryAndElse;
	delete pShowTryAndElseStatement;
	delete pFindClassDefinition;
	delete pPushClassDefinition;
	delete pFindStructDefinition;
	delete pPushStructDefinition;
	delete pFindEnumDefinition;
	delete pShowEnum;
	delete pRepo;
	delete pParser;
	delete pSemi;
	delete pToker;
	pIn->close();
	delete pIn;
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseToConsole::Attach(const std::string& name, bool isFile)
{
	if (pToker == 0)
		return false;
	pIn = new std::ifstream(name);
	if (!pIn->good())
		return false;
	return pToker->attach(pIn);
}
//----< Here's where alll the parts get assembled >----------------

Parser* ConfigParseToConsole::Build()
{
	try
	{
		return Assembler();
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
		return 0;
	}
}

Parser* ConfigParseToConsole::Assembler()
{	pToker = new Toker;
	pToker->returnComments(false);
	pSemi = new SemiExp(pToker);
	pParser = new Parser(pSemi);
	pRepo = new Repository(pToker);
	pFunctionDefinition = new FunctionDefinition;
	pPushFunction = new PushFunction(pRepo);  // no action
	pPrintFunction = new PrintFunction(pRepo);
	pFunctionDefinition->addAction(pPushFunction);
	pFunctionDefinition->addAction(pPrintFunction);
	pParser->addRule(pFunctionDefinition);
	pControlStatement = new ControlStatement;
	pShowControlStatement = new ShowControlStatement(pRepo);
	pControlStatement->addAction(pShowControlStatement);
	pParser->addRule(pControlStatement);
	pHandleTryAndElse = new HandleTryAndElse;
	pShowTryAndElseStatement = new ShowTryAndElseStatement(pRepo);
	pHandleTryAndElse->addAction(pShowTryAndElseStatement);
	pParser->addRule(pHandleTryAndElse);
	pDeclaration = new Declaration;
	pShowDeclaration = new ShowDeclaration;
	pDeclaration->addAction(pShowDeclaration);
	pParser->addRule(pDeclaration);
	pExecutable = new Executable;
	pShowExecutable = new ShowExecutable;
	pExecutable->addAction(pShowExecutable);
	pParser->addRule(pExecutable);
	pFindClassDefinition = new FindClassDefinition();
	pPushClassDefinition = new PushClassDefinition(pRepo);
	pFindClassDefinition->addAction(pPushClassDefinition);
	pParser->addRule(pFindClassDefinition);
	pFindStructDefinition = new FindStructDefinition();
	pPushStructDefinition = new PushStructDefinition(pRepo);
	pFindStructDefinition->addAction(pPushStructDefinition);
	pParser->addRule(pFindStructDefinition);
	pFindEnumDefinition = new FindEnumDefinition();
	pShowEnum = new ShowEnum(pRepo);
	pFindEnumDefinition->addAction(pShowEnum);
	pParser->addRule(pFindEnumDefinition);
	pBeginningOfScope = new BeginningOfScope();
	pHandlePush = new HandlePush(pRepo);
	pBeginningOfScope->addAction(pHandlePush);
	pParser->addRule(pBeginningOfScope);
	pEndOfScope = new EndOfScope();
	pHandlePop = new HandlePop(pRepo);
	pEndOfScope->addAction(pHandlePop);
	pParser->addRule(pEndOfScope);
	return pParser;}

#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ConfigureParser module\n "
		<< std::string(32, '=') << std::endl;

	// collecting tokens from files, named on the command line

	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	for (int i = 1; i < argc; ++i)
	{
		std::cout << "\n  Processing file " << argv[i];
		std::cout << "\n  " << std::string(16 + strlen(argv[i]), '-');

		ConfigParseToConsole configure;
		Parser* pParser = configure.Build();
		try
		{
			if (pParser)
			{
				if (!configure.Attach(argv[i]))
				{
					std::cout << "\n  could not open file " << argv[i] << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;
			}
			// now that parser is built, use it

			while (pParser->next())
				pParser->parse();
			std::cout << "\n\n";
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
		std::cout << "\n\n";
	}
}

#endif

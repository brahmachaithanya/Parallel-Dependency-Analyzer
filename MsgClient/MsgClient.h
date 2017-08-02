#ifndef MSGCLIENT_H
#define MSGCLIENT_H
/////////////////////////////////////////////////////////////////////
//  MsgClient.h - Client for dependency based remote code repository//
//  ver 1.1                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 5000 Windows 10				   //
//  Application:   Prototype for CSE687 Pr4, Sp16                  //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
//	Source:        Jim Fawcett, Syracuse University, CST 4-187     //
//                 jfawcett@twcny.rr.com                           //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages and stores files.
*
* It's purpose is to provide a very simple illustration of how to use
* the Socket Package provided for Project #4.
*

Public Interface:
=================

MsgClient c1(msgQ);
c1.RetrieveDirs("../TestFiles", "*.*");
c1.generateDepdntXml(dependencies);
*
Build Process:
==============

* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*

Build Command: devenv Project4.sln /rebuild debug
Maintenance History:
====================
ver 1.1 : 04 May 16
- Added new methods to help check-in files and compute dependencies
ver 1.0 : 02 May 16
- Initial release
*/

#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../ParallelDependencyAnalysis/ParallelDependencyAnalysis.h"
#include <iostream>
#include <string>
#include <direct.h>
#include <thread>


using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;
using File = std::string;
using Pattern = std::string;


/////////////////////////////////////////////////////////////////////
// The ClientCounter class creates a sequential number for every client
//
class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;

/////////////////////////////////////////////////////////////////////
// MsgClient class
// - It is created as a class so that more than one instance could run on a child thread

class MsgClient
{
public:
	using EndPoint = std::string;
	std::vector<std::string> inputFiles;
	MsgClient(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	void Clientexec(const size_t NumMessages, const size_t TimeBetweenMessages);
	void generateDepdntXml(std::unordered_map<std::string, std::unordered_map<std::string, int>> dependencies);
	void requestServer(std::string filename);
	void RetrieveDirs(File path, Pattern pattern);
	void PushFilesToAnalyse(std::vector<File> files, File file);	
private:
	bool bconnClosed;
	BlockingQueue<HttpMessage>& msgQ_;
	SocketSystem ss;
	SocketConnecter si;

	void HandlePostMessage(HttpMessage& msg, Socket& sckt);
	HttpMessage FormMessage(const std::string& msgBody, const EndPoint& ep, size_t n);
	void SendServerMsg(HttpMessage& msg, Socket& sckt);
	bool sendFiletoServer(Socket& sckt, const std::string& fqname);
	void reqFileFromServer(const std::string& filename, Socket& sckt);
	bool readBinaryFile(const std::string& filename, Socket& sckt, size_t fileSize);
	HttpMessage MessageReader(Socket& sckt);	
};

#endif

///////////////////////////////////////////////////////////////////////
// MsgServer.h - Server to manager remote checking                   //
// ver 1.1                                                           //
// Language:    C++, Visual Studio 2015                              //
// Platform:      Dell Inspiron 5000 Windows 10				         //
// Application:   CSE687 Pr4, Sp16: Dependency based Remote Code     //
//	                                Repository                       //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // 
// Source:		Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                  				 //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package implements a server that receives HTTP style messages and
* files from multiple concurrent clients and simply displays the messages
* and stores files.
*
* It's purpose is to provide a very simple illustration of how to use
* the Socket Package provided for Project #4.
*
* Public Interfaces:
*   SocketSystem ss;
*   SocketListener sl(8080, Socket::IP6);
*   ClientHandler cp(msgQ);
*    sl.start(cp);
*	while (true)
*   {
*		HttpMessage msg = msgQ.deQ();
*	}
*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*
* Build Process:
* --------------
*
* Build Command: devenv Project4.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.1 : 04 May 2016
* - added functionality for generating dependencies
* - handle read from Server
* ver 1.0 : 02 May 2016
* - first release
*/

#pragma once

#include "../Sockets/Sockets.h"
#include "../Logger/Cpp11-BlockingQueue.h"
#include "../HttpMessage/HttpMessage.h"
#include <vector>

using version = std::string;
using DependencyFile = std::string;
using DependencyList = std::vector<std::string>;
/////////////////////////////////////////////////////////////////////
// ClientHandler class
/////////////////////////////////////////////////////////////////////
// - instances of this class are passed by reference to a SocketListener
// - when the listener returns from Accept with a socket it creates an
//   instance of this class to manage communication with the client.
// - You need to be careful using data members of this class
//   because each client handler thread gets a reference to this 
//   instance so you may get unwanted sharing.
// - I may change the SocketListener semantics (this summer) to pass
//   instances of this class by value.
// - that would mean that all ClientHandlers would need either copy or
//   move semantics.
//
class ClientHandler
{
public:
	ClientHandler(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	void HandleGetMessage(HttpMessage& getMsg, Socket& socket);
	void HandlePostMessage(HttpMessage& postMsg, Socket& socket);	
private:
	bool bConnClosed;
	std::string prevSavedVersion = "0.0";
	std::string prevSavedFile = "";
	HttpMessage MessageReader(Socket& socket);
	version versionControl(std::string filename, std::string xmlFile1Name);
	BlockingQueue<HttpMessage>& msgQ_;
	bool readBinaryFile(const std::string& filename, size_t fileSize, Socket& socket, version ver);
	version getXMLItem(std::string versionXml);
	void sendFile(HttpMessage& msg, std::string filename, std::string folderName);
	std::string ClientHandler::GetRecentVerFile(std::string serverFolder);
	DependencyList RetrieveFileDepList(DependencyFile fileName);
};
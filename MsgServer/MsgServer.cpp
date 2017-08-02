///////////////////////////////////////////////////////////////////////
// MsgServer.cpp - Server to manager remote checking                 //
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
* This package implements a server that receives HTTP  messages and
* files from multiple concurrent clients and displays the messages
* and also stores the files.
*
* The purpose of this package is to provide a simple illustration of how to use
* the Socket Package provided for Project #4.
*/
/** Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp

*/
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../MsgServer/MsgServer.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <fstream>
#include <direct.h>

using Show = StaticLogger<1>;
using namespace Utilities;

//----< frame messages to client >------------------
using EndPoint = std::string;
//----< Attributes are added to the messages in this function>------------------
HttpMessage makeMessage(size_t sz, const std::string& body, const EndPoint& ep){

	HttpMessage HttpMsg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8080";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (sz)
	{
	case 1:
		HttpMsg.clear();
		HttpMsg.addAttribute(HttpMessage::attribute("POST", "Message"));
		HttpMsg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		HttpMsg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		HttpMsg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		HttpMsg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			HttpMsg.addAttribute(attrib);
		}
		break;
	default:
		HttpMsg.clear();
		HttpMsg.addAttribute(HttpMessage::attribute("Error", "unknown emessage type"));
	}
	return HttpMsg;
}


//----< Frame messages processing is defined here>------------------

HttpMessage ClientHandler::MessageReader(Socket& socket){

	bConnClosed = false;
	HttpMessage HttpMsg;

	// parsing the message attributes

	while (true)
	{
		std::string attributeStr = socket.recvString('\n');
		if (attributeStr.size() > 1)
		{

			HttpMessage::Attribute attribute = HttpMessage::parseAttribute(attributeStr);
			HttpMsg.addAttribute(attribute);
		}
		else
		{
			break;
		}
	}
	// connection is broken and recvString returns an empty string if the client has finished execution

	if (HttpMsg.attributes().size() == 0)
	{
		bConnClosed = true;
		return HttpMsg;
	}
	// read the message body if POST - all the messages here are POSTs

	if (HttpMsg.attributes()[0].first == "POST")
	{
		// file message	  
		HandlePostMessage(HttpMsg, socket);
	}
	else
	{
		// request for a file or a list of files here
		HandleGetMessage(HttpMsg, socket);
	}
	return HttpMsg;
}
//----<method to send messages from server side >------------------
void SendServerMsg(HttpMessage& msg, Socket& socket){

	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
	std::cout << "\n  sending the server message back to client....";
}


//----< extraction request sent from client are handled in this method>------------------

void ClientHandler::HandleGetMessage(HttpMessage& Httpmsg, Socket& sckt) {

	std::string fName = Httpmsg.findValue("file");
	std::string recentFileVer = GetRecentVerFile(fName);
	std::vector<std::string> files = FileSystem::Directory::getFiles("./ServerDownload/" + fName + "/", recentFileVer + ".*");

	// do not get the xml versions
	for (size_t i = 0; i < files.size(); ++i)
	{
		Show::write("\n\n  sending the file " + files[i]);
		sendFile(Httpmsg, files[i], Httpmsg.findValue("file"));
	}

	// get the dependencies of the package also
	DependencyFile DepFName = "./ServerDownload/" + fName + "/" + recentFileVer + "_Dep.xml";
	DependencyList dependencyList = RetrieveFileDepList(DepFName);
	for (size_t i = 0; i < dependencyList.size(); i++)
	{
		std::string fNameWithoutExt = StringHelper::extractFileName(dependencyList[i], '.');
		std::string folderName = Utilities::StringHelper::getFolderNameFromFile(fNameWithoutExt);
		DependencyFile dependentFile = dependencyList[i];
		std::cout << "\n  Sending the Dependent Files :: " << dependentFile;
		sendFile(Httpmsg, dependentFile, folderName);
	}
}


//----< post requests sent by clients are handled>------------------

void ClientHandler::HandlePostMessage(HttpMessage& HttpMsg, Socket& sckt){

	std::string fName = HttpMsg.findValue("file");
	if (fName == "")
	{
		// message body is read here
		size_t numBytes = 0;
		size_t pos = HttpMsg.findAttribute("content-length");
		if (pos < HttpMsg.attributes().size())
		{
			numBytes = Converter<size_t>::toValue(HttpMsg.attributes()[pos].second);
			Socket::byte* scktBuff = new Socket::byte[numBytes + 1];
			sckt.recv(numBytes, scktBuff);
			scktBuff[numBytes] = '\0';
			std::string msgBody(scktBuff);
			HttpMsg.addBody(msgBody);
			delete[] scktBuff;
		}
	}
	
	else 
	{
		size_t szContent;
		std::string szString = HttpMsg.findValue("content-length");
		if (szString != "")
			szContent = Converter<size_t>::toValue(szString);
		else
			return; //message;

		readBinaryFile(fName, szContent, sckt, "");
	
		// construct message body
		HttpMsg.removeAttribute("content-length");
		std::string strBody = "<file>" + fName + "</file>";
		szString = Converter<size_t>::toString(strBody.size());
		HttpMsg.addAttribute(HttpMessage::Attribute("content-length", szString));
		HttpMsg.addBody(strBody);
	}
	
}

//----< get the list of dependant files for a given filename>--------------------

DependencyList ClientHandler::RetrieveFileDepList(DependencyFile fileName){

	DependencyList dependencyList;
	std::fstream depInPtr;
	depInPtr.open(fileName, std::fstream::in);
	std::string input;
	DependencyFile dependencyFile;
	while (!depInPtr.eof())
	{
		depInPtr >> input;
		std::string dependentFile = getXMLItem(input);
		dependencyFile = Utilities::StringHelper::extractFileName(dependentFile, '.');
		DependencyFile dependencyFileVersion = GetRecentVerFile(dependencyFile);
		std::string extension = Utilities::StringHelper::getExtension(dependentFile);
		dependencyList.push_back(dependencyFileVersion + "." + extension);
	}
	return dependencyList;
}

//----<This method sends a file from a given folder>---------
void ClientHandler::sendFile(HttpMessage& HttpMsg, std::string filename, std::string folderName) {

	HttpMsg.removeAttribute("content-length");
	std::string bodyString = "<file> FILE </file>";
	std::string sizeString1 = Converter<size_t>::toString(bodyString.size());
	HttpMsg.addAttribute(HttpMessage::Attribute("content-length", sizeString1));
	HttpMsg.addBody(bodyString);
	std::cout << "\n  GET message is received";
	std::string toAddr = HttpMsg.findValue("fromAddr");
	std::string fromAddr = HttpMsg.findValue("toAddr");
	//std::string filename = HttpMsg.findValue("file");
	std::string fqname = "./ServerDownload/" + folderName + "/" + filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return;
	HttpMessage HttpMsg1 = makeMessage(1, "", "localhost:8081");
	HttpMsg1.addAttribute(HttpMessage::Attribute("file", filename));
	HttpMsg1.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	SocketConnecter si;
	while (!si.connect("localhost", 8081))
	{
		Show::write("\n server waiting to establish a connection");
		::Sleep(100);
	}
	SendServerMsg(HttpMsg1, si);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		si.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
}

//----< read a binary file from socket and save >--------------------
/*
* This method creates a folder and saves the package.
* The method expects the sender to send a message prior to ts execution.
* During execution the method sends bytes continuosly until
* the entire fileSize bytes are used up. This function also creates a folder
* and stores package inside them.
*/
bool ClientHandler::readBinaryFile(const std::string& filename, size_t fileSize, Socket& socket, version vers)
{
	std::string fqname = "";
	std::string recvFolderName = StringHelper::extractFileName(filename, '.');
	std::string xmlfName = recvFolderName;
	std::string PathFolder = "./ServerDownload/" + recvFolderName + "/";
	_mkdir(PathFolder.c_str());
	version ver = versionControl(recvFolderName, PathFolder + xmlfName);
	if (filename.find("xml") > filename.size())
		fqname = PathFolder + "ver" + ver + "_" + filename;
	else
		fqname = PathFolder + "ver" + ver + "_" + recvFolderName + "_Dep.xml";
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		/*
		* This error handling is incomplete.  The client will continue
		* to send bytes, but if the file can't be opened, then the server
		* doesn't gracefully collect and dump them as it should.  That's
		* an exercise left for students.
		*/
		Show::write("\n\n  cannot open the file " + fqname);
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;
		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}

//----< method to get the recent version of the file from the server folder>--------------------
std::string ClientHandler::GetRecentVerFile(std::string serverFolder) 
{

	int verNum;
	std::string PathFolder = "./ServerDownload/" + serverFolder + "/";
	std::string xmlVersionFile = PathFolder + serverFolder + "_ver" + ".xml";
	std::string FileLine;
	std::fstream outFilePtr;
	outFilePtr.open(xmlVersionFile, std::fstream::in);
	outFilePtr >> FileLine;
	std::string newVersion = getXMLItem(FileLine);
	std::stringstream ss;
	ss << newVersion;
	ss >> verNum;
	std::stringstream ss1;
	ss1 << verNum;
	std::string ver;
	ss1 >> ver;
	outFilePtr.close();
	std::string fqname = "ver" + ver + "_" + serverFolder;
	std::cout << "\n  Version of the file name ::  " << fqname;
	return fqname;
}
//----<This method returns the latest version of the file to be saved>---------
version ClientHandler::versionControl(std::string filename, std::string xmlFileSpec)
{
	int verNum = 1;
	std::string xmlVersionFile = xmlFileSpec + "_ver" + ".xml";
	std::string FileLine;
	std::fstream outFilePtr;
	std::fstream outFilePtr1;
	if (filename == prevSavedFile)
	{
		return prevSavedVersion;
	}
	outFilePtr.open(xmlVersionFile, std::fstream::in);
	outFilePtr >> FileLine;
	std::cout << "\n  Contents of the file ::" << FileLine;
	if (FileLine.size() == 0)
	{
		std::fstream xmlOutPointer(xmlVersionFile, std::fstream::out);
		std::cout << "File Length is zero";
		xmlOutPointer << "<version>1.0</version>" << std::endl;
		xmlOutPointer.close();
		prevSavedVersion = "1";
		prevSavedFile = filename;
	}
	else
	{
		std::fstream xmlOutPointer(xmlVersionFile, std::fstream::out);
		std::string newVersion = getXMLItem(FileLine);
		std::stringstream ss;
		ss << newVersion;
		ss >> verNum;
		verNum++;
		std::string EndVer;
		std::cout << "\n  Version Number of the file :: " << verNum;
		std::stringstream ss1;
		ss1 << verNum;
		ss1 >> EndVer;
		std::cout << "\n  The new version of the file converted :: " << EndVer;
		// write the latest version to the file
		xmlOutPointer << "<version>" + EndVer + ".0" + "</version>" << std::endl;
		xmlOutPointer.close();
		prevSavedVersion = EndVer;
		prevSavedFile = filename;
	}
	outFilePtr.close();

	// return the latest version number for the file to be saved with.
	std::string version;
	std::stringstream ss;
	ss << verNum;
	ss >> version;
	std::cout << "\n.. The latest version ::" << version;
	return version;
}
//----<This method  retrieves an item from XML>---------
version ClientHandler::getXMLItem(std::string versionXml) {
	unsigned first = versionXml.find(">");
	unsigned last = versionXml.find("</");
	std::string strNew = versionXml.substr(first + 1, (last - first) - 1);
	return strNew;
}


//----<This method  defines the receiver functionality >---------

void ClientHandler::operator()(Socket socket)
{
	/*
	* There could be a possiblerace condition because of close connection.
	* If two clients send files at the same time they may make changes
	* which are incompatible. This causes the MessageReader to
	* be called one more time.
	*/
	while (true)
	{
		HttpMessage HttpMsg = MessageReader(socket);
		if (bConnClosed || HttpMsg.bodyString() == "quit")
		{
			Show::write("\n\n  Terminating the clienthandler thread");
			break;
		}
		msgQ_.enQ(HttpMsg);
	}
}

//----< test stub for message server >--------------------------------

int main()
{
	::SetConsoleTitle(L"HttpMessage Server - Runs Forever");

	Show::attach(&std::cout);
	Show::start();
	Show::title("\n  HttpMessage Server started");

	BlockingQueue<HttpMessage> msgQ;

	try
	{
		SocketSystem ss;
		SocketListener sl(8080, Socket::IP6);
		ClientHandler cp(msgQ);
		sl.start(cp);
		/*
		* Since this is a server the loop below never terminates.
		* We could easily change that by sending a distinguished
		* message for shutdown.
		*/

		// connect start

		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			Show::write("\n\n contents of received server message :\n" + msg.bodyString());
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
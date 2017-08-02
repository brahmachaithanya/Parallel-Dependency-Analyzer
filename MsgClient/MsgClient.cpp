/////////////////////////////////////////////////////////////////////
//  MsgClient.cpp - declares new parsing rules and actions		   //
//  ver 1.1                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Inspiron 5000 Windows 10				   //
//  Application:   Prototype for CSE687 Pr4, Sp16                  //
// Author:      Brahmachaitahnya Sadashiva, Syracuse University(CE)//
//              bsadashi@syr.edu                                   // s																  
//	Source:        Jim Fawcett, Syracuse University, CST 4-187     //
//                 jfawcett@twcny.rr.com                           //
/////////////////////////////////////////////////////////////////////
#include "MsgClient.h"

//----< Method to retrieve the file directories>------------------
void MsgClient::RetrieveDirs(File FilePath, Pattern ptrn) {
	std::vector<std::string> dirs = FileSystem::Directory::getDirectories(FilePath);

	for (size_t iter = 2; iter < dirs.size(); iter++)
	{
		RetrieveDirs(FilePath + dirs[iter] + '/', ptrn);
	}
	std::vector<std::string> files = FileSystem::Directory::getFiles(FilePath + '/', ptrn);
	PushFilesToAnalyse(files, FilePath + '/');
}


//----< notify the receiver that a file is being received>-------------------------------------

bool MsgClient::sendFiletoServer(Socket& sckt, const std::string& fName){

	// socket is connected

	std::string fqname = "./TestFiles/" + fName;
	FileSystem::FileInfo fi(fqname);
	size_t szFile = fi.size();
	std::string szStr = Converter<size_t>::toString(szFile);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	// If file is not good then return
	if (!file.isGood())
		return false;

	HttpMessage Httpmsg = FormMessage("", "localhost::8080", 1);
	Httpmsg.addAttribute(HttpMessage::Attribute("file", fName));
	Httpmsg.addAttribute(HttpMessage::Attribute("content-length", szStr));
	SendServerMsg(Httpmsg, sckt);
	const size_t szBlk = 2048;
	Socket::byte buffer[szBlk];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(szBlk);
		if (blk.size() == 0)
		{
			break;
		}
		for (size_t iter = 0; iter < blk.size(); ++iter)
		{
			buffer[iter] = blk[iter];
		}
		sckt.send(blk.size(), buffer);
		if (!file.isGood())
		{
			break;
		}
	}
	file.close();
	return true;
}


//----< Function to send files for analysis>------------------
void MsgClient::PushFilesToAnalyse(std::vector<File> files, File file){

	for (size_t iter = 0; iter < files.size(); iter++) 
	{
		inputFiles.push_back(file + files[iter]);
	}
}

//----< Method to create dependent xml files>------------------
void MsgClient::generateDepdntXml(std::unordered_map<std::string, std::unordered_map<std::string, int>> dependencies) {
	std::unordered_map<std::string, std::unordered_map<std::string, int>> DependentPackages;

	for (auto& DepRec : dependencies) {
		std::string filePackage = StringHelper::extractFileName(DepRec.first, '.');
		for (auto& file : DepRec.second) {
			DependentPackages[filePackage].insert(std::make_pair(file.first, 1));
		}
	}

	for (auto& DepRec : DependentPackages) {
		std::fstream xmlPtrOut("./TestFiles/" + DepRec.first + ".xml", std::fstream::out);
		for (auto& file : DepRec.second) {
			xmlPtrOut << "<file>" << file.first << "</file>" << std::endl;
		}
		xmlPtrOut.close();
	}
}

//----< factory method to create messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage MsgClient::FormMessage(const std::string& body, const EndPoint& ep, size_t n){

	HttpMessage Httpmsg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8081";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		Httpmsg.clear();
		Httpmsg.addAttribute(HttpMessage::attribute("POST", "Message"));
		Httpmsg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		Httpmsg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		Httpmsg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		Httpmsg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			Httpmsg.addAttribute(attrib);
		}
		break;
	case 2:
		Httpmsg.clear();
		Httpmsg.addAttribute(HttpMessage::attribute("GET", "Message"));
		Httpmsg.addAttribute(HttpMessage::Attribute("mode", "twoway"));
		Httpmsg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		Httpmsg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));
		Httpmsg.addBody(body);
		break;
	default:
		Httpmsg.clear();
		Httpmsg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return Httpmsg;
}

//----< send server message using the socket >----------------------------------

void MsgClient::SendServerMsg(HttpMessage& Httpmsg, Socket& sckt){

	std::string msgString = Httpmsg.toString();
	sckt.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< runs two separate clients on separate threads of their own >------

bool MsgClient::readBinaryFile(const std::string& fName, Socket& sckt, size_t szFile){

	std::cout << "\n The file name ::  " << fName;
	std::string receivingFolderName = StringHelper::extractFileName(fName, '.');
	std::cout << "\n\nThe files that are requested by clients are stored in folder :: 'ClientDownload' \n\n";
	std::string folderLocation = "./ClientDownload/" + receivingFolderName + "/";
	_mkdir(folderLocation.c_str());
	std::string fqname = folderLocation + fName;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		Show::write("\n\n  cannot open the file " + fqname);
		return false;
	}

	const size_t szBlk = 2048;
	Socket::byte buffer[szBlk];

	size_t bytesToRead;
	while (true)
	{
		if (szFile > szBlk)
			bytesToRead = szBlk;
		else
			bytesToRead = szFile;
		sckt.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t iter = 0; iter < bytesToRead; ++iter)
			blk.push_back(buffer[iter]);

		file.putBlock(blk);
		if (szFile < szBlk)
			break;
		szFile -= szBlk;
	}
	file.close();
	return true;
}


//----< post requests sent by clients are handled>------------------

void MsgClient::HandlePostMessage(HttpMessage& Httpmsg, Socket& sckt){

	std::string fName = Httpmsg.findValue("file");
	if (fName != "")
	{
		size_t szContent;
		std::string szStr = Httpmsg.findValue("content-length");
		if (szStr != "")
			szContent = Converter<size_t>::toValue(szStr);
		else
			return; //Httpmsg;

		readBinaryFile(fName, sckt, szContent);

		// construct message body
		//std::cout << "\n\n  File name is not equal to null ";
		Httpmsg.removeAttribute("content-length");
		std::string bodyString = "<file>" + fName + "</file>";
		szStr = Converter<size_t>::toString(bodyString.size());
		Httpmsg.addAttribute(HttpMessage::Attribute("content-length", szStr));
		Httpmsg.addBody(bodyString);
	}
	else
	{
		// read message body
		size_t numBytes = 0;
		size_t pos = Httpmsg.findAttribute("content-length");
		if (pos < Httpmsg.attributes().size())
		{
			numBytes = Converter<size_t>::toValue(Httpmsg.attributes()[pos].second);
			Socket::byte* buffer = new Socket::byte[numBytes + 1];
			sckt.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			Httpmsg.addBody(msgBody);
			delete[] buffer;
		}
	}
}

//----< Frame messages processing is defined here>------------------

HttpMessage MsgClient::MessageReader(Socket& sckt){

	bconnClosed = false;
	HttpMessage Httpmsg;

	// read message attributes

	while (true)
	{
		std::string attribString = sckt.recvString('\n');
		if (attribString.size() > 1)
		{
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			Httpmsg.addAttribute(attrib);
		}
		else
		{
			break;
		}
	}
	// If client is done, connection breaks and recvString returns empty string

	if (Httpmsg.attributes().size() == 0)
	{
		bconnClosed = true;
		return Httpmsg;
	}
	// read body if POST - all messages in this demo are POSTs

	if (Httpmsg.attributes()[0].first == "POST")
	{
		// is this a file message?	  
		std::cout << "\n\n  The message received is POST";
		HandlePostMessage(Httpmsg, sckt);
	}
	
	return Httpmsg;
}

//----<This method  defines the clients functionality >---------

void MsgClient::operator()(Socket sckt){

	/*
	* There could be a possiblerace condition because of close connection.
	* If two clients send files at the same time they may make changes
	* which are incompatible. This causes the MessageReader to
	* be called one more time.
	*/
	while (true)
	{
		HttpMessage Httpmsg = MessageReader(sckt);
		if (bconnClosed || Httpmsg.bodyString() == "quit")
		{
			Show::write("\n\n  Terminating the server thread");
			break;
		}
		msgQ_.enQ(Httpmsg);
	}
}
//----<This method  makes a request to the server>---------

void MsgClient::requestServer(std::string fName){

	try
	{
		// connect
		SocketSystem ss;
		SocketConnecter si;

		Show::attach(&std::cout);
		Show::start();

		while (!si.connect("localhost", 8080))
		{
			Show::write("\n\n client waiting for establishing a connection");
			::Sleep(100);
		}

		HttpMessage Httpmsg = FormMessage("", "localhost:8080", 2);

		Httpmsg.addAttribute(HttpMessage::Attribute("file", fName));
		SendServerMsg(Httpmsg, si);
		Show::write("\n\n  The client newclient sent\n" + Httpmsg.toIndentedString());
		::Sleep(200);
	}
	catch (std::exception e)
	{
		Show::write("\n  Exception caught: ");
		std::string exMsg = "\n  " + std::string(e.what()) + "\n\n";
		Show::write(exMsg);
	}
}


//----< defines the functinality of the client >--------------------

void MsgClient::Clientexec(const size_t NumMessages, const size_t TimeBetweenMessages){

	ClientCounter counter;
	size_t szCount = counter.count();
	std::string StringCount = Utilities::Converter<size_t>::toString(szCount);
	Show::attach(&std::cout);
	Show::start();

	Show::title(
		"Starting HttpMessage client" + StringCount +
		" on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id())
		);
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n Client is waiting to establish a connection");
			::Sleep(100);
		}
		HttpMessage Httpmsg;

		std::cout << "\n\n\nChecking in all files from 'TestFiles' folder to server..\n\n";
		std::vector<std::string> files = FileSystem::Directory::getFiles("./TestFiles", "*.*");
		for (size_t iter = 0; iter < files.size(); ++iter)
		{
			Show::write("\n\n  Sending file :: " + files[iter]);
			sendFiletoServer(si, files[iter]);
		}
		Httpmsg = FormMessage("quit", "toAddr:localhost:8080", 1);
		SendServerMsg(Httpmsg, si);
		Show::write("\n\n  client " + StringCount + " sent\n" + Httpmsg.toIndentedString());
	}
	catch (std::exception& except)
	{
		Show::write("\n  Exception caught: ");
		std::string exMsg = "\n  " + std::string(except.what()) + "\n\n";
		Show::write(exMsg);
	}
}


//----< Method to request for a file>------------------
void MsgClient::reqFileFromServer(const std::string& fName, Socket& sckt) {
	SocketSystem ss;
	SocketConnecter si;

	Show::attach(&std::cout);
	Show::start();

	while (!si.connect("localhost", 8080))
	{
		Show::write("\n \nClient is waiting to connect...\n");
		::Sleep(100);
	}

	HttpMessage Httpmsg = FormMessage("", "localhost::8080", 2);
	Httpmsg.addAttribute(HttpMessage::Attribute("file", fName));
	SendServerMsg(Httpmsg, si);
}

/// <summary>
/// Mains this instance.
/// </summary>
/// <returns></returns>
int main()
{
	::SetConsoleTitle(L"Clients Running on Threads");

	Show::title("Demonstrating two Http Message Clients each running on a child thread");
	BlockingQueue<HttpMessage> msgQ;
	MsgClient ObjClient(msgQ);

	ObjClient.RetrieveDirs("./TestFiles", "*.*");
	ParallelDependencyAnalysis PDepAnlys;
	PDepAnlys.Pass1(ObjClient.inputFiles);
	PDepAnlys.Pass2(ObjClient.inputFiles);
	std::cout << "\n \n Getting dependencies for all the files in the folder 'TestFiles' . \n\n";

	std::unordered_map<std::string, std::unordered_map<std::string, int>> dependencyTable = PDepAnlys.dependencyTable;
	std::cout << "\n \n Saving all dependencies as XML in the folder 'TestFiles' . ";
	ObjClient.generateDepdntXml(dependencyTable);
	std::thread Objthread(
		[&]() { ObjClient.Clientexec(20, 100); } // 20 messages separated by 100 millisec
	);

	Objthread.join();

	try
	{
		SocketSystem scktSystem;
		SocketListener scktListener(8081, Socket::IP6);
		MsgClient msgCLient(msgQ);
		scktListener.start(msgCLient);
		msgCLient.requestServer("Logger");
		std::cout << "\n \n Client is listening at port:8081 \n\n\n";
		while (true)
		{
			// Dequeing messages
			HttpMessage msg = msgQ.deQ();
			Show::write("\n\n  Client received message contents::\n" + msg.bodyString());
		}
	}
	catch (std::exception& except)
	{
		// Write exception messages
		Show::write("\n  Exception caught: ");
		std::string exceptionMsg = "\n  " + std::string(except.what()) + "\n\n";
		Show::write(exceptionMsg);
	}
}
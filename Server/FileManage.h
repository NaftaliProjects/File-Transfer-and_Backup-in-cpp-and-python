#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include <cstdio>
#include <cstdlib>

/*

Author : Naftali Davidov


FileManage class
manage payLoad sending to client 
and payLoad saving on server side

also monitoring directory and files status for clients 


*/


using boost::asio::ip::tcp;

class FileManage {
private:
	const static int MAX_LENGTH = 1024;
public:
	void static listFiles(std::string& directoryPath, tcp::socket& socket);

	bool static backUpAFile(std::string& directoryPath, tcp::socket& socket);

	void static restoreFile(std::string& directoryPath, tcp::socket& socket);

	void static deleteFile(std::string& directoryPath);

	bool static isEmpty(std::string& directoryPath);

	bool static isExist(std::string& directoryPath);
	
	
};



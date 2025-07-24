#ifndef _REQUEST_
#define _REQUEST_

/*

Author : Naftali Davidov

Request class 
recives request and handle it 

*/

#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


class Request {
private:
    const static int MAX_LENGTH = 1024;
    

    const static uint8_t FILE_TO_BACKUP = 100;
    const static uint8_t RESTORE_FILE   = 200;
    const static uint8_t DELETE_FILE    = 201;
    const static uint8_t LIST_FILES     = 202;

    struct RequestHeader {
        uint32_t userID;
        uint8_t version;
        uint8_t op;
        uint16_t nameLen;
        char* fileName;
    } requestHeader;

    struct RequestPayLoad {
        uint32_t size;
        char payload[1024];

    } requestPayLoad;

    


public:

    //constructor
    Request(tcp::socket &sock);

	//methods
	void  listenForHeader(tcp::socket& socket);

    void handleRequest(tcp::socket& sock);

    void listFiles(tcp::socket& sock);

    void backUpAFile(tcp::socket& sock);

    void restoreFile(tcp::socket& sock);

    void deleteFile(tcp::socket& sock);
    


};

#endif
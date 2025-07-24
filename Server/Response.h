#ifndef _RESPONSE_
#define _RESPONSE_

/*

Author : Naftali Davidov


Response class
sends response to clients

*/

#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


class Response{
private:
    

    struct ResponseHeader {
        uint8_t version;
        uint16_t status;
        uint16_t nameLen;
        char* fileName;
    } rsponseHeader;

    


public:

    const static int MAX_LENGTH = 1024;
    const static uint16_t RESTORED_FILE_SUCCEED = 210;
    const static uint16_t LISTING_FILES = 211;
    const static uint16_t BACKUP_DELETE_SUCCEED = 212;

    const static uint16_t FILE_NOT_EXIST_ERROR = 1001;
    const static uint16_t NO_FILES_FOR_CLIENT_ERROR = 1002;
    const static uint16_t SERVER_ERROR = 1003;

    //constructor
    Response(uint8_t version, uint16_t status, uint16_t nameLen = NULL, const char* fileName = nullptr);

    //methods
    void  sendHeader(tcp::socket& socket);

};

#endif
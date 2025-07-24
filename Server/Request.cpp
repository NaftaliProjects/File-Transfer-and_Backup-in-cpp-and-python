#include "Request.h"
#include "FileManage.h"
#include "Response.h"
Request::Request(tcp::socket& sock)
{
    listenForHeader(sock);
}


//listen for clienets request header to understand the client and the request details
void Request::listenForHeader(tcp::socket& sock)
{
    //get request header
    char buffer[MAX_LENGTH];
    size_t request = boost::asio::read(sock, boost::asio::buffer(buffer, sizeof(buffer)));

   
    size_t index = 0;
    std::memcpy(&requestHeader.userID, buffer + index, sizeof(requestHeader.userID));

    
    index += sizeof(requestHeader.userID);
    std::memcpy(&requestHeader.version, buffer + index, sizeof(requestHeader.version));

    
    index += sizeof(requestHeader.version);
    std::memcpy(&requestHeader.op, buffer + index, sizeof(requestHeader.op));

    
    uint8_t op = requestHeader.op;
    if (op == FILE_TO_BACKUP || op == RESTORE_FILE || op == DELETE_FILE)
    {

        index += sizeof(requestHeader.op);
        std::memcpy(&requestHeader.nameLen, buffer + index, sizeof(requestHeader.nameLen));
       

        index += sizeof(requestHeader.nameLen);
        requestHeader.fileName = new char[requestHeader.nameLen + 1];
        std::memcpy(requestHeader.fileName, buffer + index, requestHeader.nameLen);
        requestHeader.fileName[requestHeader.nameLen] = '\0';
       
    }
    else
    {
        //make them null
        requestHeader.nameLen = NULL;
        requestHeader.fileName = nullptr;
    }
}

//handle request according to protocol
void Request::handleRequest(tcp::socket& sock)
{
    std::cout << "handling request:" << std::endl;
    switch (requestHeader.op) {
    case Request::LIST_FILES:
        std::cout << "request is to list the files:" << std::endl;
        listFiles(sock);
        break;

    case Request::FILE_TO_BACKUP:
        std::cout << "request is to backUp a file:" << std::endl;
        backUpAFile(sock);
        break;

    case Request::RESTORE_FILE:
        std::cout << "request is to restor a file:" << std::endl;
        restoreFile(sock);
        break;

    case Request::DELETE_FILE:
        std::cout << "request is to delete a file:" << std::endl;
        deleteFile(sock);
        break;

    default:
     
        break;
    }
    std::cout << "handling request ended:" << std::endl;
}


//uses FileManage to list filest to client, sends status accordingly
void Request::listFiles(tcp::socket& sock)
{
    std::string directoryPath = "c:\\backupsvr";
    directoryPath += '\\';
    directoryPath += std::to_string(requestHeader.userID);

    // Check and create the user's directory if not exists
    std::filesystem::create_directories(directoryPath);
   
    
    uint8_t version = 1;
    std::cout << "generating a response head..." << std::endl;
    if (!FileManage::isEmpty(directoryPath))
    {
        //status is alright, send payLoad
        
        uint16_t status = Response::LISTING_FILES;
        Response response(version, status);
        response.sendHeader(sock);
        FileManage::listFiles(directoryPath, sock);
    }
    else
    {
        uint16_t status = Response::NO_FILES_FOR_CLIENT_ERROR;
        Response response(version, status);
        response.sendHeader(sock);
    }
    
    
    

}

//uses FileManage to delete file for client, sends status accordingly
void Request::deleteFile(tcp::socket& sock)
{
    std::string directoryPath = "c:\\backupsvr";
    directoryPath += '\\';
    directoryPath += std::to_string(requestHeader.userID);

    // Check and create the user's directory if not exists
    std::filesystem::create_directories(directoryPath);

    directoryPath += '\\';
    directoryPath += requestHeader.fileName;

    uint8_t version = 1;
    std::cout << "generating a response head..." << std::endl;
    if (!FileManage::isEmpty(directoryPath))
    {
        //status is alright, send payLoad
        uint16_t status = Response::BACKUP_DELETE_SUCCEED;
        Response response(version, status, requestHeader.nameLen, requestHeader.fileName);
        response.sendHeader(sock);
        FileManage::deleteFile(directoryPath);
    }
    else
    {
        //status isnt alright, send error
        uint16_t status = Response::FILE_NOT_EXIST_ERROR;
        Response response(version, status, requestHeader.nameLen, requestHeader.fileName);
        response.sendHeader(sock);
    }

    
}


//uses FileManage to restore file for client, sends status accordingly
void Request::restoreFile(tcp::socket& sock)
{
    std::string directoryPath = "c:\\backupsvr";
    directoryPath += '\\';
    directoryPath += std::to_string(requestHeader.userID);

    // Check and create the user's directory if not exists
    std::filesystem::create_directories(directoryPath);

    directoryPath += '\\';
    directoryPath += requestHeader.fileName;

    uint8_t version = 1;
    std::cout << "generating a response head..." << std::endl;
    if (FileManage::isExist(directoryPath))
    {
        uint16_t status = Response::RESTORED_FILE_SUCCEED;
        Response response(version, status, requestHeader.nameLen, requestHeader.fileName);
        response.sendHeader(sock);

        
        FileManage::restoreFile(directoryPath, sock);
    }
    else
    {
        uint16_t status = Response::FILE_NOT_EXIST_ERROR;
        Response response(version, status, requestHeader.nameLen, requestHeader.fileName);
        response.sendHeader(sock);
    }
}


//uses FileManage to backUp a file for client, sends status accordingly
void Request::backUpAFile(tcp::socket& sock)
{
    std::string directoryPath = "c:\\backupsvr";
    directoryPath += '\\';
    directoryPath += std::to_string(requestHeader.userID);

    // Check and create the user's directory if not exists
    std::filesystem::create_directories(directoryPath);

    directoryPath += '\\';
    directoryPath += requestHeader.fileName;

    uint8_t version = 1;
    std::cout << "generating a response head..." << std::endl;
    if (FileManage::backUpAFile(directoryPath, sock))
    {
        uint16_t status = Response::BACKUP_DELETE_SUCCEED;
       
        Response response(version, status, requestHeader.nameLen, requestHeader.fileName);
        response.sendHeader(sock);
    }
    else
    {
        uint16_t status = Response::SERVER_ERROR;
        Response response(version, status);
        response.sendHeader(sock);
    }

}



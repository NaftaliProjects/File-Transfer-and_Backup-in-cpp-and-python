#include "Response.h"

//generate a response header
Response::Response(uint8_t version, uint16_t status, uint16_t nameLen, const char* fileName)
{
  rsponseHeader.version = version;
  rsponseHeader.status = status;
  rsponseHeader.nameLen = nameLen;

  
  if (nameLen) {
      
      
      // Calculate the length of the string once
      rsponseHeader.nameLen = strlen(fileName);

      // Allocate memory for the string (+1 for null terminator)
      rsponseHeader.fileName = new char[rsponseHeader.nameLen + 1];

      // Copy the string into the allocated memory
      std::memcpy(rsponseHeader.fileName, fileName, rsponseHeader.nameLen);

      // Add the null terminator
      rsponseHeader.fileName[rsponseHeader.nameLen] = '\0';

      
  }
  else
      rsponseHeader.fileName = nullptr;

  std::cout << " response header was created successfuly " << std::endl;

}

//sends header to clients
void Response::sendHeader(tcp::socket& sock)
{
    //get request header
    char buffer[MAX_LENGTH];
    memset(buffer, '\0', MAX_LENGTH);

    size_t indx = 0;
    std::memcpy(buffer, &rsponseHeader.version , sizeof(rsponseHeader.version));

    indx += sizeof(rsponseHeader.version);
    std::memcpy(buffer + indx , &rsponseHeader.status, sizeof(rsponseHeader.status));



    if (rsponseHeader.nameLen  && rsponseHeader.fileName)
    {
        indx += sizeof(rsponseHeader.status);
        std::memcpy(buffer + indx, &rsponseHeader.nameLen, sizeof(rsponseHeader.nameLen));
        indx += sizeof(rsponseHeader.nameLen);
        std::memcpy(buffer + indx, rsponseHeader.fileName, std::strlen(rsponseHeader.fileName));
    }
    

    boost::asio::write(sock, boost::asio::buffer(buffer, MAX_LENGTH));

    std::cerr << "response was sended " << std::endl;

    
}

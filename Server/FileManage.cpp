#include "FileManage.h"


//check if clients has any files in dir
bool FileManage::isEmpty(std::string& directoryPath)
{
    return std::filesystem::is_empty(directoryPath);
}

// check if a given file exist in client dir
bool FileManage::isExist(std::string& directoryPath)
{
    return std::filesystem::exists(directoryPath);
}

//list files inside the clients directory
void FileManage::listFiles(std::string& directoryPath, tcp::socket& sock)
{
    std::cout << "sending list of files : " << directoryPath << std::endl;
    
    try {

        uint32_t payLoadSIze = 0; 
        //count the size of all file names
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            payLoadSIze += entry.path().string().size() + 1; // +1 for newline character
        }

        char buffer[MAX_LENGTH];
        memset(buffer, '\0', MAX_LENGTH);
        std::memcpy(buffer, &payLoadSIze, sizeof(payLoadSIze));
        uint32_t offset = sizeof(payLoadSIze);

        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            
            std::string filePath = entry.path().string();
            filePath += '\n'; 
            

            for (char ch : filePath) {
                if (offset == sizeof(buffer)) {
                    // Buffer is full, send it
                    boost::asio::write(sock, boost::asio::buffer(buffer, sizeof(buffer)));
                    memset(buffer, '\0', sizeof(buffer));
                    offset = 0;
                }
                buffer[offset++] = ch;
            }
        }

        // Send remaining data in the buffer
        if (offset > 0) {
            boost::asio::write(sock, boost::asio::buffer(buffer, offset));
        }

    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
   
}

//restore a given file for clients
void FileManage::restoreFile(std::string& directoryPath, tcp::socket& sock)
{
    std::ifstream FileToRestore(directoryPath, std::ios::binary);
    if (!FileToRestore.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << directoryPath << std::endl;
        return;
    }

    char buffer[MAX_LENGTH];
    memset(buffer, '\0', 1024);

    std::filesystem::path p{directoryPath};
    uint32_t fileSize = std::filesystem::file_size(p);
    
    std::cout << "file size to restore = " << fileSize << std::endl;
    std::memcpy(buffer, &fileSize, sizeof(fileSize));


    // Write initial payload to file
    size_t offset = sizeof(fileSize);
    size_t remainingBytesInBuffer = fileSize;

    if (remainingBytesInBuffer > 0){
        FileToRestore.read(buffer + offset, sizeof(buffer) - offset);
        boost::asio::write(sock, boost::asio::buffer(buffer, MAX_LENGTH));
        remainingBytesInBuffer -= fileSize - offset;
        memset(buffer, '\0', MAX_LENGTH);
    }

    size_t totalRead = remainingBytesInBuffer;

    // Continue reading until the full file is received
    while (totalRead < fileSize) {
        size_t toRead = std::min(sizeof(buffer), fileSize - totalRead);
        
        FileToRestore.read(buffer, toRead);
        totalRead += toRead;
        boost::asio::write(sock, boost::asio::buffer(buffer, MAX_LENGTH));
        memset(buffer, '\0', MAX_LENGTH);
    }

    std::cout << "File successfully restored. " << std::endl;
    FileToRestore.close();
}


//saves a file that was sended from client
bool FileManage::backUpAFile(std::string& directoryPath, tcp::socket& sock)
{
    std::ofstream FileToBackUp(directoryPath, std::ios::binary);
    if (!FileToBackUp.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << directoryPath << std::endl;
        return false;
    }

    char buffer[MAX_LENGTH];
    char payLoad[MAX_LENGTH];
    uint32_t fileSize = 0;

    // Read the first chunk (which includes the file size)
    size_t bytesRead = boost::asio::read(sock, boost::asio::buffer(buffer, MAX_LENGTH));
    if (bytesRead < sizeof(fileSize)) {
        return false;
    }

    // Extract the file size
    std::memcpy(&fileSize, buffer, sizeof(fileSize));
    std::cout << "File size to read: " << fileSize << " bytes" << std::endl;

    

    // Write initial payload to file
    size_t offset = sizeof(fileSize);
    size_t remainingBytesInBuffer = bytesRead - offset;

    if (remainingBytesInBuffer > 0) {
        FileToBackUp.write(buffer + offset, remainingBytesInBuffer);
    }

    size_t totalRead = remainingBytesInBuffer;

    // Continue reading until the full file is received
    while (totalRead < fileSize) {
        size_t toRead = std::min(sizeof(buffer), fileSize - totalRead);
        bytesRead = boost::asio::read(sock, boost::asio::buffer(buffer, toRead));
        FileToBackUp.write(buffer, bytesRead);
        totalRead += bytesRead;
    }

    std::cout << "File successfully written. Total bytes read: " << totalRead << std::endl;
    FileToBackUp.close();
    return true;
}


//delete a given file for client 
void FileManage::deleteFile(std::string& directoryPath)
{
    if (std::remove(directoryPath.c_str()) == 0) {
        std::cout << "File removed successfully.\n";
    }
    else {
        std::perror("Error removing file");
    }
}

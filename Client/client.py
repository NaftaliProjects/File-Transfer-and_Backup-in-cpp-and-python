"""
Student : Naftali Davidov

ver     : 19/01/2025 

this is a client class and the main program 
the program create a class and each method create a seperate request to send to a c++ server 
the server recive the request (backUp , restore, listfiles , delete) and send a results answer back to the client
"""

import socket
import struct
from pathlib import Path

import clientRequest
import clientResponse
import Protocol
import random


class Client:
    def __init__(self, clientId, version, ip, port, filesNameList ,filePathsList ):
        self.clientId = clientId
        self.version = version
        self.ip = ip
        self.port = port
        self.filesNameList = filesNameList
        self.filePathsList = filePathsList
        
    
    def askForBackUp(self, fileNum):
        print(f"client : {self.clientId} asking to back up a file:")
        request = clientRequest.Request(self.clientId, self.version, Protocol.Op.UPLOAD, self.filesNameList[fileNum], self.filePathsList[fileNum])
        with socket.create_connection((self.ip, self.port)) as sock:
            header = request.getHeaderByteArray()
            sock.sendall(header)
            request.uploadFile(sock)
            response = clientResponse.Response(sock)
        
        
        
    def askForFilesList(self):
        print(f"client : {self.clientId} asking to view a list of files that are backedUp:")
        request = clientRequest.Request(self.clientId, self.version, Protocol.Op.LIST_FILES)
        with socket.create_connection((self.ip, self.port)) as sock:
            header = request.getHeaderByteArray()
            sock.sendall(header)   
            response = clientResponse.Response(sock)
            
            
    def askForFileRestore(self, fileNum) :
        print(f"client : {self.clientId} asking to restore a file:")
        request = clientRequest.Request(self.clientId, self.version, Protocol.Op.RESTORE, self.filesNameList[fileNum], self.filePathsList[fileNum])
        with socket.create_connection((self.ip, self.port)) as sock:
            header = request.getHeaderByteArray()
            sock.sendall(header)
            response = clientResponse.Response(sock)
           
           
    def askForFilDelete(self, fileNum) :
        print(f"client : {self.clientId} asking to delete a file:")
        request = clientRequest.Request(self.clientId, self.version, Protocol.Op.DELETE, self.filesNameList[fileNum], self.filePathsList[fileNum])
        with socket.create_connection((self.ip, self.port)) as sock:
            header = request.getHeaderByteArray()
            sock.sendall(header)
            response = clientResponse.Response(sock)



def createAndStartClient():
    ip = 0
    port = 1
    with open("server.info") as f:
        info = f.readline()
    
    ip = (info[:9])
    port = (info[10:14])
    
    backupInfo = []
    with open("backup.info") as f:
        for line in f:
            backupInfo.append(line.strip())
            
    filePaths = []
    for fileName in backupInfo:
        filePaths.append(Path(fileName))
        
    clientId = random.randrange(1000, 9999, 1)
    version = 1
    
    client = Client(clientId, version, ip, port, backupInfo, filePaths)
    
    print(f" ---- client : {client.clientId} has started running ---- ")
    
    client.askForFilesList()    # 4
    
    client.askForBackUp(0)      # 5
    
    client.askForBackUp(1)      # 6
    
    client.askForFilesList()    # 7
    
    client.askForFileRestore(0) # 8
    
    client.askForFilDelete(0)   # 9
    
    client.askForFileRestore(0) # 10
    
    print(f"----  client : {client.clientId} has finished running ---- ")
    
    
def main():

    createAndStartClient() # maman had no instruction to make the client also multi threaded 
                           # the server on the other hand can handle multiple requests at the same time


if __name__ == "__main__":
    main()

"""
Student : Naftali Davidov

ver     : 19/01/2025 

clientRsponse module consist of the Rsponse class that hold the Rsponse protocol header
and responsible to recive the Rsponse payLoad as well
"""

import socket
import struct
from pathlib import Path

import Protocol #a python file that i made

class Response:
    def __init__(self, sock):
            data = sock.recv(Protocol.ProtocolConfig.BUFFER_SIZE)
           
            #header
            self.version = data[0]
            
            self.status = int.from_bytes(data[1:3], byteorder='little')
            
            
            if self.status in [Protocol.Status.FILE_DOESNT_EXIST_EROR, Protocol.Status.SEVER_EMPTY_FROM_FILES_ERROR, Protocol.Status.SERVER_ERROR]:
                print(f'Error status = {self.status}')
                
            if self.status in [Protocol.Status.RESTORE_SUCCEED,Protocol.Status.BACKUP_OR_DELETE_SUCCEED , Protocol.Status.FILE_DOESNT_EXIST_EROR]:
                self.nameLen = int.from_bytes(data[3:5], byteorder='little')
                self.fileName = data[5:5 + self.nameLen].decode('ascii')  
                print(f"file name is : {self.fileName}, len of file name = {self.nameLen}")
            
        
                
            if self.status == Protocol.Status.LISTING_FILES:
                self.listenListFIles(sock)
                
            if self.status == Protocol.Status.RESTORE_SUCCEED:
                self.listenRestoredFile(sock)
            
            if self.status == Protocol.Status.BACKUP_OR_DELETE_SUCCEED:
                print(f"file : {self.fileName} backuped or deleted successfully")
                
            if self.status == Protocol.Status.FILE_DOESNT_EXIST_EROR:
                print(f"file : {self.fileName} does not exist on the server")
                
            if self.status == Protocol.Status.SEVER_EMPTY_FROM_FILES_ERROR:
                print("User does not have any file on this server")
                
            if self.status == Protocol.Status.SERVER_ERROR:
                print("Unkown Server error accured, request wasnt complete")
            
    
    def listenListFIles(self, sock):
        data = sock.recv(Protocol.ProtocolConfig.BUFFER_SIZE)
        self.size = int.from_bytes(data[0:4], byteorder='little')
        remaining_size = self.size
        
        file_names = data[4:]
        remaining_size -= len(file_names)
        
        while remaining_size > 0:
            data = sock.recv(Protocol.ProtocolConfig.BUFFER_SIZE)
            file_names += data  # Append new data to the buffer
            remaining_size -= len(data)
            
        try:
            decoded_names = file_names.decode('ascii')
            file_list = decoded_names.strip().split('\n')
            print("Received file names:")
            for file_name in file_list:
                print(file_name)
        except UnicodeDecodeError:
            print("Error: ")
                
    
    def listenRestoredFile(self, sock):
        #open a file
        fileName = self.fileName.strip('\0')
        with open(fileName, 'wb') as f:
            data = sock.recv(Protocol.ProtocolConfig.BUFFER_SIZE)
            self.size = int.from_bytes(data[0:4], byteorder='little')
            remaining_size = self.size
            
            fileData = data[4:]
            f.write(fileData)
            remaining_size -= len(fileData)
            
            while remaining_size > 0:
                data = sock.recv(Protocol.ProtocolConfig.BUFFER_SIZE)
                f.write(data)  
                remaining_size -= len(data)
                
            print(f"file : {self.fileName} restored successfully")
            
        
    def listenToPayLoad(self, socket):
       
        if not self.file_path.exists():
            raise FileNotFoundError(f"File not found: {self.file_path}")

        file_size = self.file_path.stat().st_size
        if file_size > Protocol.ProtocolConfig.BUFFER_SIZE:
            raise ValueError(f"File size exceeds maximum allowed size of {Protocol.ProtocolConfig.MAX_FILE_SIZE} bytes")
        
       
        with open(self.file_path, 'rb') as f:
            # First packet: Send file size (4 bytes) + up to 1020 bytes of file data
            first_chunk_size = Protocol.ProtocolConfig.BUFFER_SIZE - 4  # 1024 - 4 bytes for size
            first_chunk = f.read(first_chunk_size)

            # Create the first packet
            first_packet = bytearray(Protocol.ProtocolConfig.BUFFER_SIZE)

            # Pack file size at the start (uint32 - 4 bytes)
            
            struct.pack_into('<I', first_packet, 0, file_size)

            # Copy file data after the size
            first_packet[4:4 + len(first_chunk)] = first_chunk
            
            # Send first packet
            socket.sendall(first_packet)
            
            # Continue sending rest of the file in 1024-byte chunks
            bytes_sent = len(first_chunk)
            while bytes_sent < file_size:
                # Create a new 1024-byte packet
                packet = bytearray(Protocol.ProtocolConfig.BUFFER_SIZE)
                
                # Read up to 1024 bytes
                chunk = f.read(Protocol.ProtocolConfig.BUFFER_SIZE)
                chunk_size = len(chunk)
                
                # Copy chunk into packet
                packet[:chunk_size] = chunk
                
                # If chunk is smaller than 1024, packet will be padded with zeros
                socket.sendall(packet)
                bytes_sent += chunk_size

        print(f"File {self.file_path.name} sent successfully")

"""
Student : Naftali Davidov
ID      : 315188342
ver     : 19/01/2025 

clientRequest module consist of the Request class that hold the request protocol header
and responsible to sends the request payLoad as well
"""

import socket
import struct
from pathlib import Path

import Protocol #a python file that i made

class Request:
    def __init__(self, user_id: int, version: int, operation: Protocol.Op, fileName: str = None, filePath: str = None):
            #header
            self.user_id = user_id
            self.version = version
            self.operation = operation
            self.fileName = fileName
            
            #payLoad
            self.filePath = filePath
            
    #generate the header before sending
    def getHeaderByteArray(self) -> bytearray:
        """Create a binary request according to the protocol"""
        header = bytearray(Protocol.ProtocolConfig.BUFFER_SIZE)
        index = 0

        
        struct.pack_into('<I', header, index, self.user_id)
        index += 4

       
        struct.pack_into('<B', header, index, self.version)
        index += 1

       
        struct.pack_into('<B', header, index, self.operation)
        index += 1

        # insert filename if required
        if self.operation in [Protocol.Op.UPLOAD, Protocol.Op.RESTORE,Protocol.Op.DELETE]:
                                                                                      
            name_len = len(self.fileName)
            # Pack nameLen (uint16)
            struct.pack_into('!H', header, index, name_len)
            index += 2
            # Pack fileName (char[])
            fileName = self.fileName.encode('utf-8')
            header[index:index + name_len] = fileName

        return header
        
    def uploadFile(self, socket):
        """Upload a file to the server"""
        if not self.filePath.exists():
            raise FileNotFoundError(f"File not found: {self.filePath}")

        fileSize = self.filePath.stat().st_size
        
        
        
        with open(self.filePath, 'rb') as f:
            # First packet: Send file size (4 bytes) + up to 1020 bytes of file data
            firstChunkSize = Protocol.ProtocolConfig.BUFFER_SIZE - 4  # 1024 - 4 bytes for size
            firstChunk = f.read(firstChunkSize)

            # Create the first packet
            firstPacket = bytearray(Protocol.ProtocolConfig.BUFFER_SIZE)

            # Pack file size at the start (uint32 - 4 bytes)
            
            struct.pack_into('<I', firstPacket, 0, fileSize)

            # Copy file data after the size
            firstPacket[4:4 + len(firstChunk)] = firstChunk
            
            # Send first packet
            socket.sendall(firstPacket)
            
            # Continue sending rest of the file in 1024-byte chunks
            bytes_sent = len(firstChunk)
            while bytes_sent < fileSize:
                # Create a new 1024-byte packet
                packet = bytearray(Protocol.ProtocolConfig.BUFFER_SIZE)
                
                # Read up to 1024 bytes
                chunk = f.read(Protocol.ProtocolConfig.BUFFER_SIZE)
                chunkSize = len(chunk)
                
                # Copy chunk into packet
                packet[:chunkSize] = chunk
                
                # If chunk is smaller than 1024, packet will be padded with zeros
                socket.sendall(packet)
                bytes_sent += chunkSize

        print(f"File {self.filePath.name} sent successfully")
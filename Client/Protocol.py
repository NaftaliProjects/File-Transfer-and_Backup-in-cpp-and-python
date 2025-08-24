"""
Student : Naftali Davidov

ver     : 19/01/2025 

Here we hold some protocol configuration 
"""

class Op: 
    UPLOAD =    100
    RESTORE =  200
    DELETE =    201
    LIST_FILES = 202
    
class Status:
    RESTORE_SUCCEED =              210
    LISTING_FILES =                211
    BACKUP_OR_DELETE_SUCCEED =     212
    FILE_DOESNT_EXIST_EROR =       1001
    SEVER_EMPTY_FROM_FILES_ERROR = 1002
    SERVER_ERROR =                 1003
    
    
    
class ProtocolConfig:
    """Configuration constants for the protocol"""
    BUFFER_SIZE = 1024
    

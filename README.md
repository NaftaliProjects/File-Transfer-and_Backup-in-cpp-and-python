# File Backup and Restoration System

This project is a client-server application designed for backing up, restoring, deleting, and listing files. The system consists of a Python client and a C++ server that communicate over a TCP network connection.

---

##  Features

* **File Backup:** Upload local files from the client to the C++ server for secure storage.
* **File Restoration:** Download backed-up files from the server back to the client.
* **File Deletion:** Remotely delete specific files stored on the server.
* **File Listing:** Request and view a list of all files currently backed up on the server.
* **Robust Protocol:** A custom protocol ensures reliable communication and data integrity between the client and server.

---

##  Technologies

* **Client:** Python 3.x
    * `socket`: For establishing and managing network connections.
    * `struct`: For packing and unpacking binary data according to the protocol.
    * `pathlib`: For handling file system paths in a modern, object-oriented way.
* **Server:** C++
    * **Boost.Asio:** A cross-platform C++ library for network and low-level I/O programming. This is used to handle asynchronous I/O and manage client connections.
    * `std::thread`: For creating a new thread for each client session, allowing the server to handle multiple requests concurrently.

---

## Project Structure

The project is organized into two main parts: the Python client and the C++ server.

### Client Files (Python)
* **`Client.py`**: The main application file for the client. It contains the `Client` class which initiates requests to the server (backup, restore, delete, list). It also has the `createAndStartClient()` function, which sets up the client using configuration files and executes a series of operations.
* **`clientRequest.py`**: Defines the `Request` class, which is responsible for building and sending request packets to the server based on the defined protocol.
* **`clientResponse.py`**: Defines the `Response` class, which handles receiving and parsing response packets from the server. It interprets status codes and handles the payload for different operations.
* **`Protocol.py`**: A configuration file defining constants for the communication protocol, including operation codes (`Op`) and status codes (`Status`).
* **`backup.info`**: A text file listing the names of files the client should attempt to back up.
* **`server.info`**: A text file containing the IP address and port of the C++ server.

### Server Files (C++)
* **`main.cpp`**: The server's entry point. It sets up the Boost.Asio I/O context and an acceptor to listen for incoming connections on a specified port. It creates a new thread for each client to handle requests, allowing for concurrency.
* **`Request.h`**: Declares the `Request` class, which is used by the server to listen for and parse incoming client requests. It contains methods to handle different operations (backup, restore, delete, list).
* **`Response.h`**: Declares the `Response` class, used by the server to construct and send response packets back to the client.
* **`FileManage.h`**: Declares the `FileManage` class, a utility class that abstracts file system operations on the server side, such as checking for file existence, handling file I/O, and managing directories.

---

## 🔧 Setup and Installation

### Server Setup (C++)

1.  **Install Boost.Asio:** Ensure you have the Boost libraries, specifically Boost.Asio, installed on your system.
    * For Ubuntu/Debian: `sudo apt-get install libboost-all-dev`
    * For macOS (using Homebrew): `brew install boost`
    * For Windows: Download and build from the [official Boost website](https://www.boost.org/).
2.  **Compile the Server:** Compile the C++ source files using a C++ compiler like `g++`. Link against the Boost libraries.
    ```bash
    g++ -std=c++17 main.cpp -o server -lboost_system -pthread
    ```
3.  **Run the Server:** Execute the compiled binary, providing the port number as a command-line argument.
    ```bash
    ./server 1337
    ```
    (Replace `1337` with your desired port number).

### Client Setup (Python)

1.  **Install Dependencies:** The Python client uses only standard libraries, so no additional installation is required.
2.  **Configure:**
    * **`server.info`**: Create this file and add the server's IP address and port on a single line, separated by a space.
        Example: `127.0.0.1 1337`
    * **`backup.info`**: Create this file and list the full paths of the files you wish to back up, with each file on a new line.
3.  **Run the Client:** Execute the `Client.py` script.
    ```bash
    python Client.py
    ```

---

##  Protocol Description

The communication between the client and server follows a custom binary protocol.

### Request Format (Client to Server)
| Field | Type | Size (bytes) | Description |
| :--- | :--- | :--- | :--- |
| `userID` | `uint32` | 4 | A unique client identifier. |
| `version` | `uint8` | 1 | Protocol version. |
| `op` | `uint8` | 1 | Operation code (e.g., `UPLOAD`, `RESTORE`). |
| `nameLen` | `uint16` | 2 | Length of the file name. |
| `fileName` | `char[]` | `nameLen` | Name of the file being processed. |
| `payload` | `char[]` | variable | File data for `UPLOAD` and `RESTORE` operations. |

### Response Format (Server to Client)
| Field | Type | Size (bytes) | Description |
| :--- | :--- | :--- | :--- |
| `version` | `uint8` | 1 | Protocol version. |
| `status` | `uint16` | 2 | Status code indicating success or an error. |
| `nameLen` | `uint16` | 2 | Length of the file name (if applicable). |
| `fileName` | `char[]` | `nameLen` | Name of the file (if applicable). |
| `payload` | `char[]` | variable | Data, such as a restored file or a list of files. |

For detailed packet structures and operations, please refer to the `Protocol.py` file.

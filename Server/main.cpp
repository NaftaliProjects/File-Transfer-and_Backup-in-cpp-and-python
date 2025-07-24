/*
Author : Naftali Davidov


this is the main program for the server side for maman14
the server starts each session using a thread 
the server can get multiple request and handle them using the Request class 
mangaging fiels using FileManage class 
and sendding back response using Response class 
*/

#include <cstdlib>
#include <thread>
#include "Request.h"
#include "Response.h"
#include "FileManage.h"


void session(tcp::socket sock) {
    try {
        Request clientRequest(sock);  //getting client request
        
        clientRequest.handleRequest(sock);
       
        sock.close();
    }
    catch (std::exception& e) {
        std::cerr << "Exception in session: " << e.what() << "\n";
    }
}

//split each session to a different thread
void server(boost::asio::io_context& io_context, unsigned short port)
{
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
        std::thread(session, a.accept()).detach();
    }
}

int main(int argc, char* argv[]) {
    
    try {

        if (argc != 2) {
            std::cerr << "Usage: blocking_tcp_echo_server <port>\n";
            return 1;
        }
        boost::asio::io_context io_context;
        server(io_context, std::atoi(argv[1]));
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

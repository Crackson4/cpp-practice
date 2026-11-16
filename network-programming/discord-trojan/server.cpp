#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234
#define SERVER_ADDR "10.1.1.133"

int main()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // create server socket

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);                   // server port
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDR); // IP Address of the server

    bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)); // bind the socket
    std::cout << "Listening for connections..." << std::endl;
    listen(serverSocket, 5); // listen for connections

    int clientSocket = accept(serverSocket, nullptr, nullptr); // accept connection to client

    // recieve data from the client
    char buffer[256] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);

    std::cout << "Message from client:\n"
              << buffer << std::endl;

    close(serverSocket);

    /*
    NOT TO TOUCH! (Unless you are NorthDestiny11) DO NOT EVEN HIGHLIGHT MIYAGI!
            clientSocket.on("connect", () = > {

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                });
    */

    return 0;
}
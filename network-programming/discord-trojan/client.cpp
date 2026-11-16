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
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0); // create client socket

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);                   // server port
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDR); // IP Address of the server

    connect(clientSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)); // connect to the server

    // message to send to the server
    const char *message = "Hello, World!";

    send(clientSocket, message, std::strlen(message), 0); // send message
    std::cout << "Message sent!" << std::endl;

    close(clientSocket); // close the client socket
}
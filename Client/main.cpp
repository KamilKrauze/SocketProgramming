#include <iostream>

#include <Socked/socked.h>


int main() {
    // Step 1: Initialize Winsock
    skdInitSocket();

    // Step 2: Create a Socket
    SKDSocket client_socket{0};
    skdCreateSocket(client_socket, AF_INET, SOCK_STREAM, 0);

    // Step 3: Define Server Address
    skdBindSocketBin(client_socket, AF_INET, inet_addr("127.0.0.1"), 8080);

    // Step 4: Connect to the Server
    if (connect(client_socket.socket, (struct sockaddr*)&client_socket.address, sizeof(client_socket.address)) == SOCKET_ERROR) {
        std::cerr << "Connection failed. Error Code: " << WSAGetLastError() << std::endl;
        skdDestroySocket(client_socket);
        std::cin.get();

        return 1;
    }
    std::cout << "Connected to the server!" << std::endl;

    // Step 5: Send and Receive Data
    const char* message = "Hello, Server!";
    send(client_socket.socket, message, strlen(message), 0);
    std::cout << "Message sent: " << message << std::endl;

    char buffer[2048];
    int bytes_received = recv(client_socket.socket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::cout << "Server replied: " << buffer << std::endl;
    }

    // Step 6: Close Socket and Cleanup
    skdDestroySocket(client_socket);
    std::cout << "Client disconnected." << std::endl;
    std::cin.get();

    return 0;
}
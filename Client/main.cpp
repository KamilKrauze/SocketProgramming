#include <iostream>

#include <Socked/socked.h>

int main() {
    skdInitSocket();

    SKDSocket client_socket{0};
    skdCreateSocket(client_socket, AF_INET, SOCK_STREAM, 0);

    skdSetSocketOpt(client_socket, SOL_SOCKET, SO_REUSEADDR, 1);
    skdBindSocket(client_socket, AF_INET, "127.0.0.1", 1234);


    // Step 4: Connect to the Server
    skdConnectSocket(client_socket);
    std::cout << "Connected to the server: " << inet_ntoa(client_socket.address.sin_addr) << ":" << ntohs(client_socket.address.sin_port) << "\n";

    // Step 5: Send and Receive Data
    const char* message = "Hello, Server!";
    send(client_socket.socket, message, strlen(message), 0);
    std::cout << "Message sent: " << message << std::endl;

    char buffer[1024];
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
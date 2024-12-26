#include <iostream>
#include <future>
#include <vector>

#include <Socked/socked.h>

std::mutex mtx;
std::atomic_bool shouldQuit = false;

void handle_client(SKDSocket& client_socket) {
    char buffer[1024];
    int bytes_received = 0;

    std::cout << "Client connected" << std::endl;

    // Communicate with the client
    while ((bytes_received = recv(client_socket.socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::cout << "Received from client: " << buffer << std::endl;

        // Echo the message back to the client
        //send(client_socket.socket, buffer, bytes_received, 0);
    }

    if (!strcmp(buffer, "/quit"))
    {
        shouldQuit = true;
        std::cout << "Client sent KILL cmd." << std::endl;
    }

    std::cout << "Client disconnected" << std::endl;
    skdCloseSocket(client_socket);
}

int main() {
    // Step 1: Initialize Winsock
    skdInitSocket();

    // Step 2: Create a Socket
    SKDSocket server_socket{0};
    skdCreateSocket(server_socket, AF_INET, SOCK_STREAM, 0);
    // Step 3: Bind the Socket
    skdBindSocket(server_socket, AF_INET, "0.0.0.0", 1234);

    // Step 4: Listen for Incoming Connections
    skdCreateListener(server_socket, 5);
    std::cout << "Server is listening on port " << ntohs(server_socket.specs.sin_port) << "\n";

    while (!shouldQuit) {
        SKDSocket client{ 0 };
        int client_addr_size = sizeof(client.specs);

        client.socket = accept(server_socket.socket, (struct sockaddr*)&client.specs, &client_addr_size);
        if (client.socket == INVALID_SOCKET) {
            std::cerr << "Accept failed. Error Code: " << WSAGetLastError() << std::endl;
            continue; // Handle the next client
        }

        auto fut = std::async(std::launch::async, handle_client, std::ref(client));
        mtx.lock();
        fut.get();
        mtx.unlock();
    }

    skdDestroySocket(server_socket);
    std::cout << "Server shutdown." << std::endl;
    return 0;
}

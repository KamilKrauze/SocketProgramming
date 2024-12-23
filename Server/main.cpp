#include <iostream>
#include <thread>
#include <vector>

#include <Socked/socked.h>

// Function to handle a single client
void handle_client(SKDSocket& client_socket) {
    char buffer[2048];
    int bytes_received;

    std::cout << "Client connected. Thread ID: " << std::this_thread::get_id() << "\n";

    // Communicate with the client
    while ((bytes_received = recv(client_socket.socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::cout << "Received from client: " << buffer << "\n";

        // Echo the message back to the client
        send(client_socket.socket, buffer, bytes_received, 0);
    }

    std::cout << "Client disconnected. Thread ID: " << std::this_thread::get_id() << "\n";

    skdCloseSocket(client_socket);
}

int main() {
    // Step 1: Initialize Winsock
    skdInitSocket();

    // Step 2: Create a Socket
    SKDSocket server_socket{0};
    skdCreateSocket(server_socket, AF_INET, SOCK_STREAM, 0);

    // Step 3: Bind the Socket
    skdBindSocketBin(server_socket, AF_INET, INADDR_ANY, 8080);

    // Step 4: Listen for Incoming Connections
    skdCreateListener(server_socket, 5);
    std::cout << "Server is listening on port " << server_socket.port << std::endl;

    // Step 5: Accept and Handle Multiple Clients
    std::vector<std::thread> client_threads;
    while (true) {
        SKDSocket client{0};
        int client_addr_size = sizeof(client.address);

        client.socket = accept(server_socket.socket, (struct sockaddr*)&client.address, &client_addr_size);
        if (client.socket == INVALID_SOCKET) {
            std::cerr << "Accept failed. Error Code: " << WSAGetLastError() << std::endl;
            continue; // Handle the next client
        }

        // Launch a new thread to handle the client
        client_threads.emplace_back(handle_client, client);
    }

    // Step 6: Cleanup
    for (auto& t : client_threads) {
        if (t.joinable()) {
            t.join(); // Wait for all threads to finish
        }
    }

    skdDestroySocket(server_socket);
    std::cout << "Server shutdown." << std::endl;

    return 0;
}

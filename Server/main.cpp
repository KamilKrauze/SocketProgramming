#include <iostream>
#include <future>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <Socked/socked.h>
#include <string>

SkdSocket server_socket{ 0 };

std::mutex mtx;
std::atomic_bool shouldQuit = false;
std::atomic_int clientCounter = 0;

// Thread-safe map to store client information
std::map<int, SkdSocket> clients;
std::mutex clientsMutex;

void cleanupClients()
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& [id, sock] : clients) {
        skdCloseSocket(sock);
    }
    clients.clear();
}

void handle_client(SkdSocket client_socket, int clientID) {
    char buffer[1024];
    int bytes_received = 0;

    std::cout << "Client " << clientID << " connected" << std::endl;

    // Communicate with the client
    while (!shouldQuit.load()) {
        bytes_received = skdReceive(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0)
        {
            continue;
        }

        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::cout << "Received from client " << clientID << ": " << buffer << std::endl;

        if (strcmp(buffer, "/quit") == 0) {
            std::cout << "Client " << clientID << " sent quit command." << std::endl;
            shouldQuit.store(true);
            break;
        }

        if (strcmp(buffer, "/leave") == 0) {
            break;
        }

        // Echo the message to other clients
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            for (auto& [otherID, otherClient] : clients) {
                if (otherID != clientID) { // Skip the sender
                    skdSend(otherClient, buffer, bytes_received, 0);
                }
            }
        }
    }

    // Clean up when the client disconnects
    std::cout << "Client " << clientID << " disconnected" << std::endl;
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(clientID);
    }
    skdCloseSocket(client_socket);

    if (shouldQuit.load())
    {
        cleanupClients();
        skdCloseSocket(server_socket);
        skdCleanupSocket();
        std::cout << "Server shutdown." << std::endl;
    }
}


int main() {
    // Step 1: Initialize Winsock
    skdInitSocket();


    // Step 2: Create a Socket
    skdCreateSocket(server_socket, AF_INET, SOCK_STREAM, 0);
    skdSetSocketOpt(server_socket, SOL_SOCKET, SO_RCVTIMEO, 3000);

    // Step 3: Bind the Socket
    skdBindSocket(server_socket, AF_INET, "0.0.0.0", 1234);

    // Step 4: Listen for Incoming Connections
    skdCreateListener(server_socket, 5);
    std::cout << "Server is listening on port " << ntohs(server_socket.specs.port) << "\n";

    int error = 0;
    while (!shouldQuit.load()) {
        SkdSocket client{};
        int client_addr_size = sizeof(client.specs);

        skdAccept(server_socket, client);
        
        if ((error = WSAGetLastError()) != 10004 && client.socket == INVALID_SOCKET) {
            std::cerr << "Accept failed. Error Code: " << error << std::endl;
            continue; // Handle the next client
        }

        // Assign a unique ID to the new client
        int clientID = clientCounter.fetch_add(1);

        // Add client to the registry
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients[clientID] = client;
        }

        // Handle the client in a separate thread
        std::thread(handle_client, client, clientID).detach();
    }

    // Clean up remaining clients
    cleanupClients();

    skdCloseSocket(server_socket);
    skdCleanupSocket();
    std::cout << "Server shutdown." << std::endl;
    std::cin.get();
    return 0;
}

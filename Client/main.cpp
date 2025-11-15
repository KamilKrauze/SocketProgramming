#include <iostream>
#include <thread>
#include <atomic>
#include <cstring>

#include <Socked/socked.h>

SkdSocket client_socket{ 0 };
std::atomic_bool shouldQuit = false;

void fetch_data_from_srv()
{
    while (!shouldQuit.load())
    {
        char buffer[1024];
        int bytes = 0;
        bytes = skdReceive(client_socket, buffer, sizeof(buffer), 0);
        if (bytes > 0) {
            buffer[bytes] = '\0'; // Null-terminate the received data
            std::cout << buffer << std::endl;
        }
    }
    std::cout << "Shutting down recv thread\n";
}

uint32_t my_strlen(char* string)
{
    if (string == nullptr)
    {
        return 0;
    }
    
    char* temp = string;
    while (*temp != '\0')
    {
        ++temp;
    }
    int result = temp - string;
    return result;
}

int main() {
    skdInitSocket();

    skdCreateSocket(client_socket, AF_INET, SOCK_STREAM, 0);

    int timeout = 500;
    skdSetSocketOpt(client_socket, SOL_SOCKET, SO_REUSEADDR, 1);
    skdSetSocketOpt(client_socket, SOL_SOCKET, SO_RCVTIMEO, timeout);

    skdSetSocketSpecs(client_socket, AF_INET, "127.0.0.1", 1234);

    skdPrintSocketAsNetwork(client_socket);
    skdPrintSocketAsHost(client_socket);
    skdPrintSocket(client_socket);

    // Step 4: Connect to the Server
    skdConnectSocket(client_socket);
    std::cout << "Connected to the server: " 
        << skdGetIPv4AddressAsHost(client_socket) << ":" 
        << skdGetPortAsHost(client_socket.specs.port) << "\n";


    std::thread recv_th = std::thread(fetch_data_from_srv);

    // Step 5: Send and Receive Data
    char* message = new char();
    while (!shouldQuit.load())
    {
        std::cout << "\r> ";
        //std::cin >> message;
        std::cin.getline(message, 1024);
        skdSend(client_socket, message, my_strlen(message), 0);
        shouldQuit.store(strcmp(message, "/quit") == 0);
    }

    recv_th.join();
    
    // Step 6: Close Socket and Cleanup
    skdCleanupSocket();
    std::cout << "Client disconnected." << std::endl;
    std::cin.get();

    return 0;
}
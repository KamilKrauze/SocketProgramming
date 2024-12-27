#include <iostream>
#include <thread>

#include <Socked/socked.h>

SkdSocket client_socket{ 0 };
std::atomic_bool shouldQuit = false;

void fetch_data_from_srv()
{
    while (!shouldQuit.load())
    {
        char buffer[1024];
        int bytes_received = recv(client_socket.socket, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Null-terminate the received data
            std::cout << buffer << std::endl;
        }
    }
    std::cout << "\rShutting down recv thread\n";
}

int main() {
    skdInitSocket();

    skdCreateSocket(client_socket, AF_INET, SOCK_STREAM, 0);

    int timeout = 500;
    skdSetSocketOpt(client_socket, SOL_SOCKET, SO_REUSEADDR, 1);
    skdSetSocketOpt(client_socket, SOL_SOCKET, SO_RCVTIMEO, timeout);

    skdSetSocketSpecs(client_socket, AF_INET, "192.168.0.25", 1234);


    // Step 4: Connect to the Server
    skdConnectSocket(client_socket);
    in_addr addr{};
    addr.S_un.S_addr = client_socket.specs.address.data;
    std::cout << "Connected to the server: " << inet_ntoa(addr) << ":" << ntohs(client_socket.specs.port) << "\n";

    std::thread recv_th = std::thread(fetch_data_from_srv);

    // Step 5: Send and Receive Data
    char* message = new char();
    while (!shouldQuit.load())
    {
        std::cout << "\r> ";
        //std::cin >> message;
        std::cin.getline(message, 1024);

        send(client_socket.socket, message, strlen(message), 0);
        shouldQuit.store(strcmp(message, "/quit") == 0);
    }

    // Step 6: Close Socket and Cleanup
    skdDestroySocket(client_socket);
    std::cout << "Client disconnected." << std::endl;
    std::cin.get();

    return 0;
}
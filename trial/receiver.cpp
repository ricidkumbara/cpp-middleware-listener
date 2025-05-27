#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define LISTEN_PORT 8080  // Must match the sender's SERVER_PORT

// Function to simulate keyboard typing
void TypeText(const std::string& text) {
    for (char c : text) {
        SHORT vk = VkKeyScanA(c);  // Get virtual key code
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vk;
        SendInput(1, &input, sizeof(INPUT));

        // Release key
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create UDP socket!" << std::endl;
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(LISTEN_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udpSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed!" << std::endl;
        return -1;
    }

    char buffer[1024];
    sockaddr_in clientAddr;
    int clientLen = sizeof(clientAddr);

    std::cout << "Waiting for scanner input...\n";

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int recvLen = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &clientLen);
        if (recvLen > 0) {
            buffer[recvLen] = '\0';
            std::cout << "Received Scanned Data: " << buffer << std::endl;

            // Simulate typing the received text
            TypeText(buffer);
        }
    }

    closesocket(udpSocket);
    WSACleanup();
    return 0;
}

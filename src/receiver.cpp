#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define LISTEN_PORT 8080  // Must match the sender's SERVER_PORT

// Function to simulate keyboard typing
void TypeText(const std::string& text) {
    for (char c : text) {
        // Convert to uppercase
        char upperChar = std::toupper(static_cast<unsigned char>(c));

        SHORT vk = VkKeyScanA(upperChar);
        if (vk == -1)
            continue; // Unsupported character

        BYTE virtualKey = LOBYTE(vk);
        BYTE shiftState = HIBYTE(vk);

        // Press Shift if needed
        if (shiftState & 1) {
            INPUT shiftDown = {0};
            shiftDown.type = INPUT_KEYBOARD;
            shiftDown.ki.wVk = VK_SHIFT;
            SendInput(1, &shiftDown, sizeof(INPUT));
        }

        // Press key
        INPUT keyDown = {0};
        keyDown.type = INPUT_KEYBOARD;
        keyDown.ki.wVk = virtualKey;
        SendInput(1, &keyDown, sizeof(INPUT));

        // Release key
        INPUT keyUp = keyDown;
        keyUp.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &keyUp, sizeof(INPUT));

        // Release Shift if needed
        if (shiftState & 1) {
            INPUT shiftUp = {0};
            shiftUp.type = INPUT_KEYBOARD;
            shiftUp.ki.wVk = VK_SHIFT;
            shiftUp.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &shiftUp, sizeof(INPUT));
        }
    }

    // Press Enter
    INPUT enterDown = {0};
    enterDown.type = INPUT_KEYBOARD;
    enterDown.ki.wVk = VK_RETURN;
    SendInput(1, &enterDown, sizeof(INPUT));

    // Release Enter
    INPUT enterUp = enterDown;
    enterUp.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &enterUp, sizeof(INPUT));
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

    std::cout << "Receiver is running...\n";

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

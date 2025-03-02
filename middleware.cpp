#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <cctype>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "192.168.156.66"
#define SERVER_PORT 8080
#define TIMEOUT_MS 1000  // 1 second timeout

HHOOK hKeyboardHook;
SOCKET udpSocket;
sockaddr_in serverAddr;
std::string scannedData;
std::chrono::steady_clock::time_point lastInputTime;

// Initialize UDP socket
void InitUDP() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
}

// Send UDP message
void SendUDPMessage(const std::string& message) {
    sendto(udpSocket, message.c_str(), message.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
}

// Check timeout and clear data
void CheckTimeout() {
    if (!scannedData.empty() &&
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastInputTime).count() > TIMEOUT_MS) {
        std::cout << "Timeout: Clearing scanned data.\n";
        scannedData.clear();
    }
}

// Convert virtual key to ASCII character (preserving case)
char GetCharFromKey(DWORD vkCode) {
    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);

    WCHAR unicodeChar;
    if (ToAscii(vkCode, MapVirtualKey(vkCode, MAPVK_VK_TO_VSC), keyboardState, (LPWORD)&unicodeChar, 0) == 1) {
        return (char)unicodeChar;
    }
    return 0;
}

// Keyboard hook callback
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        char key = GetCharFromKey(pKey->vkCode);

        if (pKey->vkCode == VK_RETURN) {
            if (!scannedData.empty()) {
                std::cout << "Scanned Data: " << scannedData << std::endl;
                SendUDPMessage(scannedData += VK_RETURN);
                scannedData.clear();
            }
        } else if (key >= 32 && key <= 126) {
            scannedData += key;
            lastInputTime = std::chrono::steady_clock::now();
            std::cout << scannedData << std::endl;
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// Set keyboard hook
void SetHook() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
}

// Unhook and cleanup
void Unhook() {
    UnhookWindowsHookEx(hKeyboardHook);
    closesocket(udpSocket);
    WSACleanup();
}

int main() {
    InitUDP();
    SetHook();
    lastInputTime = std::chrono::steady_clock::now();
    std::cout << "Listening for scanner input (Alphanumeric only)... Press ESC to exit.\n";

    MSG msg;
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        CheckTimeout();
        if (GetAsyncKeyState(VK_ESCAPE)) break;
        // Sleep(100);
    }

    Unhook();
    return 0;
}

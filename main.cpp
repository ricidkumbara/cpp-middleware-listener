#include <windows.h>
#include <iostream>
#include <fstream>

HHOOK hKeyboardHook;
std::ofstream logFile("scanned_data.txt", std::ios::app);  // File to store scanned data
std::string scannedData;  // Buffer for storing input

// Callback function for keyboard events
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        char key = MapVirtualKeyA(pKey->vkCode, MAPVK_VK_TO_CHAR);

        if (pKey->vkCode == VK_RETURN) {  // Scanner usually sends "Enter" at the end
            std::cout << "Scanned Data: " << scannedData << std::endl;
            logFile << scannedData << std::endl;  // Save to file
            scannedData.clear();  // Reset buffer
        } else {
            scannedData += key;  // Append to buffer
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// Function to set the keyboard hook
void SetHook() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (!hKeyboardHook) {
        std::cerr << "Failed to install hook!" << std::endl;
    }
}

// Function to remove the hook
void Unhook() {
    UnhookWindowsHookEx(hKeyboardHook);
}

int main() {
    SetHook();
    std::cout << "Listening for scanner input... Press ESC to exit." << std::endl;

    // Keep the program running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (GetAsyncKeyState(VK_ESCAPE)) break;  // Exit on ESC key
    }

    Unhook();
    logFile.close();
    return 0;
}

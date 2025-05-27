#include "udp_sender.hpp"

#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

static SOCKET udpSocket;
static sockaddr_in receiverAddr;

int initialize_socket(std::string serverIP, int serverPort)
{
    WSADATA wsaData;
    int wsaInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaInit != 0)
    {
        std::cerr << "WSAStartup failed: " << wsaInit << std::endl;
        return 0;
    }

    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    }

    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(serverPort);                  // Target port
    receiverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str()); // Target IP
    return 1;
}

int send_msg(const char *message)
{
    int sendResult = sendto(udpSocket, message, strlen(message), 0,
                            (sockaddr *)&receiverAddr, sizeof(receiverAddr));
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "sendto failed: " << WSAGetLastError() << std::endl;
        closesocket(udpSocket);
        WSACleanup();
        return 0;
    }

    std::cout << "Message sent: " << message << std::endl;
    return 1;
}

void end_socket()
{
    closesocket(udpSocket);
    WSACleanup();
}
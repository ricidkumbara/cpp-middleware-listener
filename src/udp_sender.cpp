#include "udp_sender.hpp"

#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

static std::string serverIP;
static int serverPort;
static SOCKET udpSocket;
static sockaddr_in receiverAddr;

void load_ip_env_config()
{
    std::ifstream envFile(".env");
    if (!envFile)
    {
        std::cerr << "Error: .env file not found!" << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(envFile, line))
    {
        std::istringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value))
        {
            if (key == "SERVER_IP")
                serverIP = value;
            else if (key == "SERVER_PORT")
                serverPort = std::stoi(value);
        }
    }

    if (serverIP.empty() || serverPort == 0)
    {
        std::cerr << "Error: Invalid .env configuration!" << std::endl;
        exit(1);
    }
}

int initialize_socket()
{
    load_ip_env_config();

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
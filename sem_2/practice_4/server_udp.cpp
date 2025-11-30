#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "12345"

int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;

    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // ===== ШАГ 1: Инициализация Winsock =====
    // WSAStartup() инициализирует сетевую подсистему Windows
    // MAKEWORD(2, 2) означает версию Winsock 2.2
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket() failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(atoi(DEFAULT_PORT));

    iResult = bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        printf("bind() failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    while (true) {
        int bytesReceived = recvfrom(ListenSocket, recvbuf, recvbuflen - 1, 0,
                                      (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (bytesReceived == SOCKET_ERROR) {
            printf("[SERVER] recvfrom() failed with error: %d\n", WSAGetLastError());
            continue; // Продолжаем ждать следующего пакета
        }

        recvbuf[bytesReceived] = '\0';

        char clientIP[INET_ADDRSTRLEN];
        InetNtop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        printf("[RECEIVED] From %s:%d (%d bytes)\n", clientIP, clientPort, bytesReceived);
        printf("[MESSAGE] %s\n", recvbuf);
        printf("----------------------------------------\n");

        int bytesSent = sendto(ListenSocket, recvbuf, bytesReceived, 0,
                               (struct sockaddr*)&clientAddr, clientAddrLen);

        if (bytesSent == SOCKET_ERROR) {
            printf("[SERVER] sendto() failed with error: %d\n", WSAGetLastError());
        } else {
            printf("[SENT] Echo to %s:%d (%d bytes)\n\n", clientIP, clientPort, bytesSent);
        }
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}
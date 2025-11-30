#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "12345"
#define DEFAULT_SERVER "127.0.0.1"

int __cdecl main(int argc, char **argv) 
{
    WSADATA wsaData;
    int iResult;

    SOCKET ClientSocket = INVALID_SOCKET;

    struct sockaddr_in serverAddr;
    char sendbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ClientSocket == INVALID_SOCKET) {
        printf("socket() failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(DEFAULT_PORT));

    iResult = InetPton(AF_INET, DEFAULT_SERVER, &serverAddr.sin_addr.s_addr);
    if (iResult != 1) {
        printf("InetPton() failed with error: %d\n", iResult);
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    while (true) {
        // Получаем сообщение от пользователя
        printf("Enter message: ");
        fgets(sendbuf, DEFAULT_BUFLEN, stdin);

        // Удаляем символ новой строки (\n)
        size_t len = strlen(sendbuf);
        if (len > 0 && sendbuf[len - 1] == '\n') {
            sendbuf[len - 1] = '\0';
            len--;
        }

        // Проверяем команду выхода
        if (strcmp(sendbuf, "exit") == 0) {
            printf("\n[CLIENT] Exiting...\n");
            break;
        }

        int bytesSent = sendto(ClientSocket, sendbuf, (int)len, 0,
                               (struct sockaddr*)&serverAddr, sizeof(serverAddr));

        if (bytesSent == SOCKET_ERROR) {
            printf("sendto() failed with error: %d\n", WSAGetLastError());
            continue; // Попробуем отправить следующее сообщение
        }

        printf("[SENT] %d bytes sent to %s:%s\n", bytesSent, DEFAULT_SERVER, DEFAULT_PORT);

        char buffer[1024];
        struct sockaddr_in sourceAddr;
        int sourceAddrLen = sizeof(sourceAddr);

        int bytesReceived = recvfrom(ClientSocket, buffer, sizeof(buffer) - 1, 0,
                                      (struct sockaddr*)&sourceAddr, &sourceAddrLen);

        if (bytesReceived == SOCKET_ERROR) {
            printf("recvfrom() failed with error: %d\n", WSAGetLastError());
            continue; // Продолжаем отправку следующих сообщений
        }

        buffer[bytesReceived] = '\0';

        // Выводим полученное эхо
        printf("[RECEIVED] %d bytes from %s:%d\n", bytesReceived, 
               DEFAULT_SERVER, ntohs(sourceAddr.sin_port));
        printf("[ECHO] %s\n", buffer);
        printf("----------------------------------------\n\n");
    }

    closesocket(ClientSocket);
    WSACleanup();
    return 0;

}
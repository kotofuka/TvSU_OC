#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>
#include <chrono>
#include <thread>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

// Структура для хранения состояния клиента
struct Client {
    SOCKET socket;
    int id;
    bool processing = false;
    std::chrono::steady_clock::time_point processingStartTime;
};

int __cdecl main(void) 
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    
    printf("Echo Server started on port %s\n", DEFAULT_PORT);
    printf("Waiting for client connections...\n\n");
    
    // Хранилище информации о клиентах
    std::map<SOCKET, Client> clients;
    int clientCounter = 0;

    // Вечный event loop
    while (true) {
        // Проверяем listening socket на новые подключения (blocking accept)
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket != INVALID_SOCKET) {
            // Устанавливаем non-blocking режим для клиентского сокета
            u_long mode = 1;
            ioctlsocket(ClientSocket, FIONBIO, &mode);

            // Добавляем нового клиента
            Client newClient;
            newClient.socket = ClientSocket;
            newClient.id = ++clientCounter;
            newClient.processing = false;
            newClient.processingStartTime = std::chrono::steady_clock::now();

            clients[ClientSocket] = newClient;
            printf("[Client %d] Connected\n", newClient.id);
        }

        // Итерируем по всем подключенным клиентам
        std::vector<SOCKET> socketsToRemove;

        for (auto& pair : clients) {
            SOCKET clientSocket = pair.first;
            Client& client = pair.second;

            // Пытаемся прочитать данные из non-blocking сокета
            int bytesRead = recv(clientSocket, recvbuf, recvbuflen, 0);

            if (bytesRead > 0) {
                // Получены данные
                printf("[Client %d] Bytes received: %d\n", client.id, bytesRead);
                printf("[Client %d] Data: ", client.id);
                fwrite(recvbuf, 1, bytesRead, stdout);
                printf("\n");

                // Начинаем обработку (имитация длительной операции)
                client.processing = true;
                client.processingStartTime = std::chrono::steady_clock::now();
                printf("[Client %d] Processing started...\n", client.id);

                // Имитация обработки - ждем 2 секунды
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));

                // Отправляем echo ответ
                int iSendResult = send(clientSocket, recvbuf, bytesRead, 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("[Client %d] send failed with error: %d\n", client.id, WSAGetLastError());
                    socketsToRemove.push_back(clientSocket);
                } else {
                    printf("[Client %d] Bytes sent: %d\n", client.id, iSendResult);
                    printf("[Client %d] Processing finished\n", client.id);
                }

                client.processing = false;

            } else if (bytesRead == 0) {
                // Клиент закрыл соединение
                printf("[Client %d] Connection closed by client\n", client.id);
                socketsToRemove.push_back(clientSocket);

            } else if (bytesRead == SOCKET_ERROR) {
                // Ошибка - проверяем тип ошибки
                int error = WSAGetLastError();

                if (error == WSAEWOULDBLOCK) {
                    // Нет данных для чтения - это нормально для non-blocking сокета
                    // do something (ждем новые данные)
                } else {
                    // Это настоящая ошибка
                    printf("[Client %d] recv failed with error: %d\n", client.id, error);
                    socketsToRemove.push_back(clientSocket);
                }
            }
        }

        // Удаляем отключенные клиенты
        for (SOCKET socket : socketsToRemove) {
            printf("[Client %d] Closing socket\n", clients[socket].id);
            closesocket(socket);
            clients.erase(socket);
        }

        // Малая задержка чтобы не жечь CPU в пустом цикле
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Cleanup
    for (auto& pair : clients) {
        closesocket(pair.first);
    }
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}

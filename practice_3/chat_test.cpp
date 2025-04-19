#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <windows.h>

HANDLE hSemaphore;
std::string filename = "chat.txt";
std::string username;
std::streampos lastReadPos = 0;

void inputThread() {
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (!message.empty()) {
            WaitForSingleObject(hSemaphore, INFINITE);
            std::ofstream outfile(filename, std::ios::app);
            if (outfile.is_open()) {
                outfile << username << ": " << message << std::endl;
                outfile.close();
            }
            ReleaseSemaphore(hSemaphore, 1, NULL);
        }
    }
}

void readerThread() {
    while (true) {
        WaitForSingleObject(hSemaphore, INFINITE);
        std::ifstream infile(filename);
        if (infile.is_open()) {
            infile.seekg(lastReadPos);
            std::string line;
            while (std::getline(infile, line)) {
                std::cout << line << std::endl;
                lastReadPos = infile.tellg();
            }
            infile.close();
        }
        ReleaseSemaphore(hSemaphore, 1, NULL);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Использование: " << argv[0] << " <имя_пользователя>" << std::endl;
        return 1;
    }
    username = argv[1];

    hSemaphore = CreateSemaphore(NULL, 1, 1, "ChatSemaphore");
    if (hSemaphore == NULL) {
        std::cerr << "Ошибка создания/открытия семафора: " << GetLastError() << std::endl;
        return 1;
    }

    std::thread input(inputThread);
    std::thread reader(readerThread);

    input.join();
    reader.join();

    CloseHandle(hSemaphore);
    return 0;
}
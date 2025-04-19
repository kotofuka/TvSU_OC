#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <windows.h>

HANDLE hSemaphore;
std::string filename = "chat.txt";
std::string username;
std::streampos last_read_pos = 0;

void writer_to_console(){
    std::string message;
    while(true){
        std::getline(std::cin, message);
        if (!message.empty()){
            WaitForSingleObject(hSemaphore, INFINITE);
            std::ofstream txt;
            txt.open(filename);
            txt << username << ": " << message << std::endl;
            txt.close();
            ReleaseSemaphore(hSemaphore, 1, NULL);
        }
    }
}

void reader_from_console(){
    while(true){
        WaitForSingleObject(hSemaphore, INFINITE);
        std::ifstream txt(filename);
        if (txt.is_open()){
            txt.seekg(last_read_pos);
            std::string line;
            while(std::getline(txt, line)){
                std::cout << line << std::endl;
                last_read_pos = txt.tellg();
            }
            txt.close();
        }
        ReleaseSemaphore(hSemaphore, 1, NULL);
    }
}

int main(){
    std::cout << "Input username: ";
    std::cin >> username;

    hSemaphore = CreateSemaphore(NULL, 1, 1, "ChatSemaphore");
    if (hSemaphore == NULL){
        std::cerr << "Error from create/open semaphore" << std::endl;
        return 1;
    }

    std::thread sender(writer_to_console);
    std::thread recipient(reader_from_console);

    sender.join();
    recipient.join();

    CloseHandle(hSemaphore);
    return 0;
}
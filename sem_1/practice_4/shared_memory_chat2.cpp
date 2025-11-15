#include <iostream>
#include <windows.h>
#include <thread>
#include <string>
#include <chrono>

const int shared_memory_size = 1024; 
const LPCSTR shared_memory_name = "SharedMemory";

HANDLE shared_memory_handle;
HANDLE hSemaphore;
std::string username;
char* shared_memory;

void reader_from_console(){
    std::string message;
    while(true){
        std::getline(std::cin, message);
        if (message.empty()) continue;
        WaitForSingleObject(hSemaphore, INFINITE);
        // if (!message.empty()) continue;
        message = username + ": " + message; 
        strncpy(shared_memory, message.c_str(), shared_memory_size - 1);
        shared_memory[shared_memory_size - 1] = '\0';
        ReleaseSemaphore(hSemaphore, 1, NULL);
    }
}

void writer_to_console(){
    while(true){
        while(shared_memory->flag.load(std::memory_order_acquire) == 0){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << shared_memory->message << std::endl << std::endl;
        shared_memory->flag.store(0, std::memory_order_release);
    }
}

int main(){
    std::cout << "Input username: ";
    std::cin >> username;

    shared_memory_handle = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        shared_memory_size,
        shared_memory_name
    );
    
    shared_memory = (char*)MapViewOfFile(
        shared_memory_handle,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        0
    );

    hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);

    std::thread sender(writer_to_console);
    std::thread recipient(reader_from_console);

    sender.join();
    recipient.detach();

    UnmapViewOfFile(shared_memory);
    CloseHandle(shared_memory_handle);
    return 0;

}
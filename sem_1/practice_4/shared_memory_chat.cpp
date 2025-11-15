#include <iostream>
#include <windows.h>
#include <thread>
#include <string>
#include <atomic>
#include <chrono>


const int shared_memory_size = 1024; 
const LPCSTR shared_memory_name = "SharedMemory";

struct SharedData{
    std::atomic_int flag;
    char message[shared_memory_size - sizeof(INT)];
};

HANDLE shared_memory_handle;
std::string username;
SharedData* shared_memory;

void reader_from_console(){
    while(true){
        while(shared_memory->flag.load(std::memory_order_acquire) == 1){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::string message;
        std::getline(std::cin, message);
        if (message.empty()) continue;
        message = username + ": " + message; 
        strncpy_s(shared_memory->message, message.c_str(), sizeof(shared_memory->message));
        shared_memory->flag.store(1, std::memory_order_release);
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
    
    shared_memory = (SharedData*)MapViewOfFile(
        shared_memory_handle,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        0
    );

    std::thread sender(writer_to_console);
    std::thread recipient(reader_from_console);

    sender.join();
    recipient.detach();

    UnmapViewOfFile(shared_memory);
    CloseHandle(shared_memory_handle);
    return 0;

}
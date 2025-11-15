#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>
#include <map>
#include <vector>

struct File{
    std::string name;
    std::string content;
    std::mutex mutex;

    File(std::string name): name(name){}
    File(const File&) = delete;
    File& operator=(const File&) = delete;
};
const int file_size = 10;
std::vector<File> files;

int find_file(std::string name){
    for(size_t i = 0; i < files.size(); i++){
        if (files[i].name == name) {
            return i;
        }
    }
    return -1;
}

void create_file(std::string name){
    if (find_file(name) != -1) return;
    files.emplace_back(File(name));
}

void delete_file(std::string name){
    int i = find_file(name);
    if (i == -1) return;
    std::cout<< "delete " << name << std::endl;
    {
        std::lock_guard<std::mutex> lock(files[i].mutex);
    }
    files.erase(files.begin() + i);
}

void write_file(std::string name, std::string content){
    int i = find_file(name);
    if (i == -1) return;
    std::lock_guard<std::mutex> lock(files[i].mutex);
    files[i].content = content;

}

void read_file(std::string name){
    int i = find_file(name);
    if (i == -1) {
        std::cout << "file \"" << name << "\" is not found" << std::endl << std::endl;
        return;
    }
    std::cout << "file \"" << name << "\": " << files[i].content << std::endl << std::endl;
}

int main(){
    std::cout << "start" << std::endl;
    for (int i = 0; i < file_size; i++){
        create_file("file_" + std::to_string(i));
        write_file("file_" + std::to_string(i), "content_" + std::to_string(i));
    }

    for(int i = 5; i < 8; i++){
        delete_file("file_" + std::to_string(i));
    }

    for(auto& f : files){
        read_file(f.name);
    }
    return 0;
}
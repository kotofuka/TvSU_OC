#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>
#include <map>
#include <vector>
#include <list> // Используем std::list вместо std::vector

struct File {
    std::string name;
    std::string content;
    std::mutex mutex;
    File(std::string name) : name(name) {}
    File(const File&) = delete;
    File& operator=(const File&) = delete;
};

const int file_size = 8;
const int NUM_Of_TESTS = 20;
std::list<File> files; // Изменено на std::list

int find_file(const std::string& name) {
    for (auto it = files.begin(); it != files.end(); ++it) {
        if (it->name == name) {
            return std::distance(files.begin(), it);
        }
    }
    return -1;
}

void create_file(const std::string& name) {
    if (find_file(name) != -1) return;
    files.emplace_back(name); // Создаем объект напрямую в списке
}

void delete_file(const std::string& name) {
    int i = find_file(name);
    if (i == -1) return;
    auto it = std::next(files.begin(), i);
    // while(!it->mutex.try_lock()){
    //     std::cout<< "not lock for delete" << std::endl;
    // }
    std::lock_guard<std::mutex> lock(it->mutex);
    std::cout << "delete " << name << std::endl;
    files.erase(it);
}

void write_file(const std::string& name, const std::string& content) {
    int i = find_file(name);
    if (i == -1) return;
    auto it = std::next(files.begin(), i);
    std::lock_guard<std::mutex> lock(it->mutex);
    std::cout<< "write" << std::endl;
    it->content = content;
}

void read_file(const std::string& name) {
    int i = find_file(name);
    if (i == -1) {
        std::cout << "file \"" << name << "\" is not found" << std::endl << std::endl;
        return;
    }
    auto it = std::next(files.begin(), i);
    std::lock_guard<std::mutex> lock(it->mutex);
    i = find_file(name);
    if (i == -1) return;
    it = std::next(files.begin(), i);
    std::cout << "file \"" << name << "\": " << it->content << std::endl << std::endl;
}

void randomize_action(int x){
    for(int l = 0; l < file_size; l++){
        int i = rand()%file_size;
        std::string filename = "file" + std::to_string(i);
        create_file(filename);// create
        for(size_t j = 0; j<NUM_Of_TESTS; ++j) {
            switch(rand() % 8) {
                case 0: 
                    create_file(filename); 
                    break; // create

                case 1: 
                    delete_file(filename); 
                    break; // delete

                case 2:
                case 3:
                case 4: 
                    read_file(filename); 
                    break; // read

                case 5:
                case 6:
                case 7: 
                    write_file(filename, "content" + std::to_string(i)); break; // write
            }
        }
    }
    std::cout << std::endl << "thread_" << x << " finish" << std::endl;
    return;
}

int main() {
    // std::cout << "start" << std::endl;
    // for (int i = 0; i < file_size; i++) {
    //     create_file("file_" + std::to_string(i));
    //     write_file("file_" + std::to_string(i), "content_" + std::to_string(i));
    // }
    // for (int i = 5; i < 8; i++) {
    //     delete_file("file_" + std::to_string(i));
    // }
    // for (const auto& f : files) {
    //     read_file(f.name);
    // }

    std::vector<std::thread> threads;
    int n = 8;
    for(int i = 0; i < n; i++){
        threads.emplace_back(randomize_action, i);
    }

    for(auto &th : threads){
        th.join();
    }
    // std::cout << "finish" << std::endl;
    return 0;
}
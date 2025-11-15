#include <iostream>
#include <semaphore>
#include <thread>
#include <fstream>
#include <unistd.h>

using namespace std;

// declare funcs
void console_usr(int id);
string read_from_console();
void write_to_file(string message);
string read_from_file();

// global vars
binary_semaphore sem(1);

// funcs
void console_usr(int id){
    string usr_name = "User" + to_string(id);
    string message;
    int n = 4;
    while (n > 0){
        sem.acquire();
        message = read_from_file();
        write_to_file(message + "\n" + usr_name + ":" + "\n" + read_from_console());
        n--;
        sem.release();
        sleep(1);
    }
}

string read_from_console(){
    string message;
    getline(cin, message);
    return message;
}

string read_from_file(){
    ifstream txt;
    string buf, message = "";
    txt.open("output_file.txt");
    while(getline(txt, buf, '\n')){
        message += buf + "\n";
    }
    txt.close();
    return message;
}

void write_to_file(string message){
    ofstream txt;
    txt.open("output_file.txt");
    txt << message;
    txt.close();
}

// entry point
int main(){
    ofstream txt;
    txt.open("output_file.txt", std::ofstream::out | std::ofstream::trunc);
    txt.close();
    thread th1 (console_usr, 1);
    thread th2 (console_usr, 2);

    th1.join();
    th2.join();
    return 0;
}

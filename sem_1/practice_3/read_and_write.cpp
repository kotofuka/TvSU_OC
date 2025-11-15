#include <iostream>
#include<semaphore>
#include <thread>
#include <mutex>
#include <fstream>


using namespace std;

mutex mut_w, mut_r;
handle semaphore = NULL;

void read_to_file(){
    mut_r.lock();
    ifstream txt;
    string message;
    txt.open("output_file.txt")
    string buf;
    while(getline(txt, buf, '\n')){
        message += buf;
    }
    txt.close();
    mut_r.unlock();
}

void write_to_file(){
    mut_w.lock();
    ofstream txt;
    txt.open("output_file.txt");
    string message;
    string buf;
    while(getline(txt, buf, '\n')){
        cin << string;
        out << string << endl;
    }
    out.close();
    mut_w.unlock();
}

int main(){
    thread th1(write_to_file);
    thread th2(read_to_file);

    th1.join();
    th2.join();
}
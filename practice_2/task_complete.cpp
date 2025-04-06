#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>

using namespace std;

// global scope vars
int visit_default = 0;
int visit_mutex = 0;
atomic<int> visit_atomic = 0;
int n = 1000000;

// repairing vars
mutex mutex_var;
auto timeStart = chrono::steady_clock::now();
chrono::duration<double, milli> diffTime;

// visit funcs
void func_visit_default(){
    for (int j; j < n; j++) {visit_default++;}
}

void func_visit_mutex(){
    for (int j; j < n; j++) {
        mutex_var.lock();

    
        visit_mutex++;
        mutex_var.unlock();

    }
}

void func_visit_atomic(){
    for (int j; j < n; j++) {visit_atomic++;}
}

// main
int main(){
    vector <thread> threads;

    // default init
    for (int i = 0; i < 8; i++){
        threads.emplace_back(func_visit_default);
    }

    for(auto &th : threads){
        th.join();
    }
    diffTime = chrono::steady_clock::now() - timeStart;
    cout <<"result with default: " << visit_default << " | runtime = " << diffTime.count() << "ms" << endl;

    threads.clear();

    timeStart = chrono::steady_clock::now();
    // mutex init
    for (int i = 0; i < 8; i++){
        threads.emplace_back(func_visit_mutex);
    }

    for(auto &th : threads){
        th.join();
    }
    diffTime = chrono::steady_clock::now() - timeStart;
    cout <<"result with mutux: " << visit_mutex << " | runtime = " << diffTime.count() << "ms" << endl;

    threads.clear();

    timeStart = chrono::steady_clock::now();
    // atomic init
    for (int i = 0; i < 8; i++){
        threads.emplace_back(func_visit_atomic);
    }

    for(auto &th : threads){
        th.join();
    }
    diffTime = chrono::steady_clock::now() - timeStart;
    cout <<"result with atomic: " << visit_atomic << " | runtime = " << diffTime.count() << "ms" << endl;

}
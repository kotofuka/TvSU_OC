#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>


using namespace std;

int visitD = 0;
int visitM = 0;
atomic<int> atomic_visit = 0;
mutex mt1;
auto timeStart = chrono::steady_clock::now();
chrono::duration<double, milli> diffTime;

void visitFuncMut(){
    mt1.lock();
    visitM++;
    mt1.unlock();
}

void visitFuncAtom(){
    atomic_visit++;
}

void visitDef(){
    
    visitD++;
}

int main(){

    vector<thread> threadsDef;
    timeStart = chrono::steady_clock::now();
    for(int i = 0; i < 4; i++){
        threadsDef.emplace_back(visitDef);
    }

    for(auto &th : threadsDef){
        th.join();
    }
    diffTime = chrono::steady_clock::now() - timeStart;
    cout <<"result with default: " << visitD << " | runtime = " << diffTime.count() << "ms" << endl;

    vector<thread> threadsMut;
    timeStart = chrono::steady_clock::now();
    for(int i = 0; i < 10000; i++){
        threadsMut.emplace_back(visitFuncMut);
    }

    for(auto &th : threadsMut){
        th.join();
    }
    diffTime = chrono::steady_clock::now() - timeStart;
    cout <<"result with mutex: " << visitM << " | runtime = " << diffTime.count() << "ms" << endl;

    vector<thread> threadsAtom;
    timeStart = chrono::steady_clock::now();
    for(int i = 0; i < 10000; i++){
        threadsAtom.emplace_back(visitFuncAtom);
    }

    for(auto &th : threadsAtom){
        th.join();
    }
    diffTime = chrono::steady_clock::now() - timeStart;
    cout <<"result with atomic: " << atomic_visit << " | runtime = " << diffTime.count() << "ms" << endl;
}
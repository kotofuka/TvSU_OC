#include <iostream>
#include <thread>
#include <vector>

using namespace std;

int visit = 0;

void visitFunc(){
    visit++;
}

int main(){
    vector<thread> threads;
    for(int i = 0; i < 10000; i++){
        threads.emplace_back(visitFunc);
    }

    for(auto &th : threads){
        th.join();
    }
    cout << visit << endl;
}
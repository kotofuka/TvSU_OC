#include <iostream>
#include<thread>

void a(int n){
    std::cout << "a: n = " << n << std::endl;
}

int main(){
    std::thread th(a, 5);

    th.join();
    std::cout << "thread finished" << std::endl;
}
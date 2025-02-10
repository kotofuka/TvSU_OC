#include <iostream>
#include <thread>
#include <set>

void a(int a, int b){
    std::cout << "Hello function a(" << a << b << ")"<< std::endl;
}

int main(){
    std::thread th1(a, 1, 2);
    th1.join();
    std::thread th2(a, 2, 3);
    th2.join();
    std::cout << "Hello world" << std::endl;

    std::set<int> st = {1, 2};
    st.contains(1);

}
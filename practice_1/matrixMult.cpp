#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <random>

using namespace std;

random_device rd;
mt19937 rng(rd());
uniform_int_distribution<std::mt19937::result_type> random100(0,1000);
auto timeStart = chrono::steady_clock::now();
chrono::duration<double, milli> diffTime;

void calculateElem(vector<int> r1, vector<int> r2, vector<vector<int>> &res, int x, int y){
    for(int i = 0; i < r1.size(); i++){
        res[x][y] += r2[i] * r1[i];
        // cout << r1[i] << " * " << r2[i] << endl;
    }
    // cout << endl;
    
}

void calculateString(int n,int i, vector<vector<int>> &matrix1, vector<vector<int>> &matrix2, vector<vector<int>> &matrixRes){
    for(int j = 0; j < n; j++){
        vector<int> r1(n, 0);
        vector<int> r2(n, 0);
        for(int k = 0; k < n; k++){
            r1[k] = matrix1[i][k];
            r2[k] = matrix2[k][j];
        }
        calculateElem(r1, r2, matrixRes, i, j);
    }
}

void multMatrix(int n, vector<vector<int>> &matrix1, vector<vector<int>> &matrix2, vector<vector<int>> &matrixRes){
    for(int i = 0; i < n; i++){
        calculateString(n, i, matrix1, matrix2, matrixRes);
    }
}

void calculateForThread(int i, int n, vector<vector<int>> &matrix1, vector<vector<int>> &matrix2, vector<vector<int>> &matrixRes){
    // cout<< "(" << i << ", " << n << ")\n";

    for(; i < n; i++){
        calculateString(matrix1.size(), i, matrix1, matrix2, matrixRes);
    }
}

void multMatrixWithThreads(int n, int countThreads, vector<vector<int>> &matrix1, vector<vector<int>> &matrix2, vector<vector<int>> &matrixRes){

    vector<shared_ptr<thread>> threadList;

    int val = n / countThreads + (n % countThreads == 0 ? 0 : 1);
    cout << val << endl;

    for(int i = 0; i < countThreads; i++){
        if (n <= i * val) break;
        threadList.push_back(shared_ptr<thread>(new thread(calculateForThread, (val * i), (n - val * (i + 1)  >= 0? val * (i + 1) : val * i + (n % (val * i))), matrix1, matrix2, matrixRes)));
        //  cout << i << " thread(calculateForThread, " << val * i << ", " << (n - val * (i + 1)  >= 0? val * (i + 1) : val * i + (n % (val * i)))  << ")" << endl;
    }

    for(int i = 0; i < threadList.size(); i++){
        cout << i << " " << (*threadList[i]).joinable() << endl;
        if((*threadList[i]).joinable()) (*threadList[i]).join(); 
    }
}

vector<vector<int>> generateMatrix(int n){
    vector<vector<int>> matrix(n, vector<int>(n, 0));

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            matrix[i][j] = random100(rng);
        }
    }

    return matrix;
}

void printMatrix(vector<vector<int>> &mat){
    for(auto list: mat){
        for(auto elem: list){
            cout<< elem << "\t";
        }
        cout << endl;
    }
}

void solveForMatrixOnN(int n, int proces_count){
    vector<vector<int>> matrix1 = generateMatrix(n);

    vector<vector<int>> matrix2 = generateMatrix(n);
    
    vector<vector<int>> matrixRes(n, vector<int>(n, 0));
    
    timeStart = chrono::steady_clock::now();
    multMatrix(n, matrix1, matrix2, matrixRes);
    diffTime = chrono::steady_clock::now() - timeStart;
    cout << "runtime without threads on matrix["<< n << "*" << n<<"] = " << diffTime.count() << " ms"<< endl;

    timeStart = chrono::steady_clock::now();
    multMatrixWithThreads(n, proces_count, matrix1, matrix2, matrixRes);
    diffTime = chrono::steady_clock::now() - timeStart;
    cout << "runtime with threads on matrix["<< n << "*" << n<<"] = " << diffTime.count() << " ms"<< endl;
}

int main(){
    const unsigned int proces_count = thread::hardware_concurrency();

    solveForMatrixOnN(11, proces_count);

    // printMatrix(matrixRes);

    // cout << endl << "max process_count = " << proces_count << endl;

    return 0;
}
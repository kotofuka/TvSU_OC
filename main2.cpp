#include "stdint.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <ranges>
#include <ratio>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>


using namespace std;
const int n = 3;

// const int processor_count = std::thread::hardware_concurrency();
const int processor_count = 1;

void print_matrix(vector<vector<int>> matrix) {
  return;
  cout << "res: \n";
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      cout << matrix[i][j] << ' ';
    }
    cout << std::endl;
  }
}

void mult_matrix_chunk(vector<vector<int>> &res, int start, int end,
                       const vector<vector<int>> &mt1) {
  for (int i = start; i < end; ++i) {
    for (int j = 0; j < n; ++j) {
      for (int k = 0; k < n; ++k) {
        res[i][j] += mt1[i][k] * mt1[k][j];
      }
    }
  }
}

int main() {
  std::random_device dev;
  std::mt19937 rng(dev());
  rng.seed(0);
  std::uniform_int_distribution<std::mt19937::result_type> dist(1, 3);

  vector<vector<int>> mt1(n, vector<int>(n, 0));
  vector<vector<int>> res(n, vector<int>(n, 0));
  int processor_count = thread::hardware_concurrency();

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      mt1[i][j] = dist(rng); // random
    }
  }
  auto start = std::chrono::steady_clock::now();

  const int chunk_size = n / processor_count + 1;

  vector<std::thread> threads;
  for (int th_i = 0; th_i < processor_count; ++th_i) {
    threads.push_back(std::thread([&res, chunk_size, th_i, &mt1]() {
      mult_matrix_chunk(res, chunk_size * th_i, min(chunk_size * (th_i + 1), n), mt1);
    }));
  }

  for (auto &th : threads) {
    th.join();
  }

  auto end = std::chrono::steady_clock::now();

  print_matrix(res);
  auto diff = end - start;

  std::cout << std::chrono::duration<double, std::milli>(diff).count() << " ms"
            << std::endl;
}

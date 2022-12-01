#include "btree_with_new.hpp"
#include <iostream>
#include <random>
#include <vector>

int main() {
  auto t = btree_alloc(1);
  int n = 10000;

  std::vector<KeyType> arr;  // = {845,707,195,672,665,963,415,152,611,353};
  std::random_device dev;
  std::uniform_int_distribution<KeyType> range(1, 100000);
  for (int i = 0; i < n; ++i) {
    arr.push_back(range(dev));
    btree_insert(t, arr[i]);
  }

    // for (const auto& e : arr) {
    //   std::cout << e << ',';
    // }
//   std::cout << '\n';
//   for (int i = 0; i < n; ++i) {
//     btree_delete(t, range(dev));
//   }

  for (int i = 0; i < n; ++i) {
    btree_delete(t, arr[i]);
  }

  //   auto iter = btree_iter_start(t);

  //   for (int i = 0; i < n; ++i) {
  //     KeyType x;
  //     std::cout << x << ' ';
  //   }

  //   btree_iter_end(iter);
  btree_free(t);
}
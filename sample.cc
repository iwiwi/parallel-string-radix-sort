#include "parallel_string_radix_sort.h"

#include <iostream>
#include <string>

int main() {
  // Sorting std::string
  {
    std::string data[5] = {
      "hoge",
      "piyo",
      "fuga",
      "foo",
      "bar",
    };

    parallel_string_radix_sort::Sort(data, 5);

    for (int i = 0; i < 5; ++i) {
      std::cout << data[i] << std::endl;
    }
    std::cout << std::endl;
  }

  // Sorting const char*
  {
    const char *data[5] = {
      "hoge",
      "piyo",
      "fuga",
      "foo",
      "bar",
    };

    parallel_string_radix_sort::Sort(data, 5);

    for (int i = 0; i < 5; ++i) {
      std::cout << data[i] << std::endl;
    }
    std::cout << std::endl;
  }

  // When you perform sorting more than once, you can avoid
  // the cost of initialization using |ParallelStringRadixSort| class
  {
    parallel_string_radix_sort::ParallelStringRadixSort<std::string> psrs;
    psrs.Init(10);  // Maximum number of the elements

    std::string data[5] = {
      "hoge",
      "piyo",
      "fuga",
      "foo",
      "bar",
    };

    psrs.Sort(data, 5);

    for (int i = 0; i < 5; ++i) {
      std::cout << data[i] << std::endl;
    }
    std::cout << std::endl;
  }

  return 0;
}

#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <algorithm>
#include <cassert>
#include<stdio.h>
#include "parallel_string_radix_sort.h"

const int MAX_LEN = 30;
const int TRIAL = 3;
const size_t NUM_ELEMS = 30000000;

struct __bench__ {
  double start;
  char msg[100];
  __bench__(const char* format, ...)
  __attribute__((format(printf, 2, 3)))
  {
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    start = sec();
  }
  ~__bench__() {
    if (sec() - start > 0.2)
    fprintf(stderr, "%s: %.6f sec\n", msg, sec() - start);
  }
  double sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
  }
  operator bool() { return false; }
};

#define benchmark(...) if(__bench__ __b__ = __bench__(__VA_ARGS__)); else

void InitRandom(const char **a, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    int len = 1 + rand() % MAX_LEN;

    char *s = new char[len + 1];
    for (int j = 0; j < len; ++j) {
      s[j] = 'a' + rand() % 26;
    }
    s[len] = '\0';

    a[i] = s;
  }
}

void DeleteAll(const char **a, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    delete [] a[i];
  }
}

class Compare {
 public:
  bool operator()(const char* const &a, const char* const &b) {
    return strcmp(a, b) < 0;
  }
};

int main() {
  const char **strings = new const char*[NUM_ELEMS];
  parallel_string_radix_sort::ParallelStringRadixSort<const char*> psrs;
  psrs.Init(NUM_ELEMS);

  for (int t = 0; t < TRIAL; ++t) {
    InitRandom(strings, NUM_ELEMS);
    benchmark("ParallelStringRadixSort::Sort(%d)", t) {
      psrs.Sort(strings, NUM_ELEMS);
    }
    DeleteAll(strings, NUM_ELEMS);
  }

  for (int t = 0; t < TRIAL; ++t) {
    InitRandom(strings, NUM_ELEMS);
    benchmark("std::sort(%d)", t) {
      std::sort(strings, strings + NUM_ELEMS, Compare());
    }
    DeleteAll(strings, NUM_ELEMS);
  }

  delete [] strings;
}

// Copyright 2011, Takuya Akiba
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Takuya Akiba nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Usage:
//   % g++ -O3 parallel_string_radix_sort_test.cc -lgtest -lgtest_main -fopenmp
//   % ./a.out

#include "parallel_string_radix_sort.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <gtest/gtest.h>

using testing::Types;

namespace {
template<typename T> T GenerateString(int max_len, int num_alpha);

template<> std::string GenerateString(int max_len, int num_alpha) {
  int len = 1 + rand() % max_len;
  std::string res(len, 'a');
  for (int i = 0; i < len; ++i) {
    res[i] += rand() % num_alpha;
  }
  return res;
}

template<> const char *GenerateString(int max_len, int num_alpha) {
  int len = 1 + rand() % max_len;
  char *res = new char[len + 1];
  for (int i = 0; i < len; ++i) {
    res[i] = 'a' + rand() % num_alpha;
  }
  res[len] = '\0';
  return res;
}

template<typename T> void DeleteString(T str);

template<> void DeleteString(std::string str __attribute__((unused))) {}

template<> void DeleteString(const char *str) {
  delete [] str;
}

template<typename T>
T *GenerateStrings(size_t num_elems, int max_len, int num_alpha) {
  T *res = new T[num_elems];
  for (size_t i = 0; i < num_elems; ++i) {
    res[i] = GenerateString<T>(max_len, num_alpha);
  }
  return res;
}

template<typename T>
void DeleteStrings(T *strs, size_t num_elems) {
  for (size_t i = 0; i < num_elems; ++i) {
    DeleteString(strs[i]);
  }
  delete [] strs;
}

template<typename StringType> class Compare;

template<> class Compare<const char*> {
 public:
  inline bool operator()(const char * const &a, const char * const &b) {
    return strcmp(a, b) < 0;
  }
};

template<> class Compare<std::string> {
 public:
  inline bool operator()(const std::string &a, const std::string &b) {
    return a.compare(b) < 0;
  }
};

template<typename StringType, size_t kMaxElems,
         int kMaxLen, int kNumAlpha, int kNumTrial>
class Tester {
 public:
  void Run() {
    parallel_string_radix_sort::ParallelStringRadixSort<StringType> psrs;
    psrs.Init(kMaxElems);

    for (int t = 0; t < kNumTrial; ++t) {
      size_t num_elems = rand() % (1 + kMaxElems);
      StringType *dat
          = GenerateStrings<StringType>(num_elems, kMaxLen, kNumAlpha);

      StringType *ans = new StringType[num_elems];
      for (size_t i = 0; i < num_elems; ++i) ans[i] = dat[i];
      std::stable_sort(ans, ans + num_elems, Compare<StringType>());

      psrs.Sort(dat, num_elems);

      for (size_t i = 0; i < num_elems; ++i) {
        ASSERT_EQ(std::string(ans[i]), std::string(dat[i]));
      }

      DeleteStrings(dat, num_elems);
      delete [] ans;
    }
  }
};
}  // namespace

typedef Types<
  // Small random cases
  Tester<const char*,    100,      10, 26, 1000>,
  Tester<std::string,    100,      10, 26, 1000>,

  // Large random cases
  Tester<const char*, 100000,      10, 26,   10>,
  Tester<std::string, 100000,      10, 26,   10>,

  // "AAA..."
  Tester<const char*, 100000,      10,  1,   10>,
  Tester<std::string, 100000,      10,  1,   10>,
  Tester<const char*,    100,   10000,  1,   10>,
  Tester<std::string,    100,   10000,  1,   10>,
  Tester<const char*,      2, 1000000,  1,   10>,  // Evil cases which may cause
  Tester<std::string,      2, 1000000,  1,   10>,  // stack overflow on
                                                   // fragile implementations :)
  // "ABAB..."
  Tester<const char*, 100000,      10,  2,   10>,
  Tester<std::string, 100000,      10,  2,   10>,
  Tester<const char*,    100,   10000,  2,   10>,
  Tester<std::string,    100,   10000,  2,   10>,
  Tester<const char*,      2, 1000000,  2,   10>,
  Tester<std::string,      2, 1000000,  2,   10>,

  // Size 0
  Tester<const char*,      0,      10, 26,    1>,
  Tester<std::string,      0,      10, 26,    1>
> Testers;

template<typename T>
class TestRunner : public testing::Test {};
TYPED_TEST_CASE(TestRunner, Testers);

TYPED_TEST(TestRunner, run) {
  TypeParam tester;
  tester.Run();
}

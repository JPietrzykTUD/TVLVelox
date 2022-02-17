#include <iostream>
#include "Filter.h"

#include "folly/Benchmark.h"
#include "folly/Portability.h"
#include "folly/Random.h"
#include "folly/Varint.h"
#include "folly/init/Init.h"
#include "folly/lang/Bits.h"


using namespace facebook::velox;
using namespace facebook::velox::common;

std::vector<int64_t> sparseValues;
std::vector<int64_t> denseValues;
std::unique_ptr<BigintValuesUsingHashTable> filter;

int32_t run4x64(const std::vector<int64_t>& data) {
   using TV = facebook::velox::common::vecext;
   int64_t count = 0;
   assert(data.size() % TV::vector_element_count() == 0);
   for (auto i = 0; i < data.size(); i += TV::vector_element_count()) {
         auto result = filter->test(tvl::loadu< TV >(data.data() + i));

         count += tvl::get_msb< TV >( result );
      }
   return count;
}

BENCHMARK(simdDense) {
   folly::doNotOptimizeAway(run4x64(denseValues));
}
BENCHMARK_RELATIVE(simdSparse) {
   folly::doNotOptimizeAway(run4x64(sparseValues));
}

int32_t main(int32_t argc, char* argv[]) {
   constexpr int32_t kNumValues = 1000000;
   constexpr int32_t kFilterValues = 1000;
   folly::init(&argc, &argv);

   std::vector<int64_t> filterValues;
   filterValues.reserve(kFilterValues);
   for (auto i = 0; i < kFilterValues; ++i) {
         filterValues.push_back(i * 1000);
      }
   filter = std::make_unique<BigintValuesUsingHashTable>(
      filterValues.front(), filterValues.back(), filterValues, false);
   denseValues.resize(kNumValues);
   sparseValues.resize(kNumValues);
   for (auto i = 0; i < kNumValues; ++i) {
         denseValues[i] = (folly::Random::rand32() % 3000) * 1000;
         sparseValues[i] = (folly::Random::rand32() % 100000) * 1000;
      }

   folly::runBenchmarks();
   return 0;
}

//============================================================================
///home/pietrzyk/tmp/tmp.L2VpIMzURo/main.cpp      relative  time/iter  iters/s
//============================================================================
//simdDense  SSE                                               8.98ms   111.32
//simdDense  AVX2                                              4.59ms   217.87
//simdDense  AVX512                                            3.19ms   313.62
//simdSparse SSE                                   430.50%     2.09ms   479.24
//simdSparse AVX2                                  354.71%     1.29ms   772.82
//simdSparse AVX512                                378.49%   842.45us    1.19K
//============================================================================
#include <iostream>
#include <cstdint>
#include "tvl/tvlintrin.hpp"
#include "FilterTVL.h"
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
#ifdef NEON
   std::unique_ptr<BigintValuesUsingHashTableTVL< tvl::simd< int64_t, tvl::neon > > > filter_neon;
#else
   std::unique_ptr<BigintValuesUsingHashTableTVL< tvl::simd< int64_t, tvl::sse > > > filter_sse;
   std::unique_ptr<BigintValuesUsingHashTableTVL< tvl::simd< int64_t, tvl::avx2 > > > filter_avx2;
   std::unique_ptr<BigintValuesUsingHashTableTVL< tvl::simd< int64_t, tvl::avx512 > > > filter_avx512;
#endif

template< tvl::VectorProcessingStyle Vec >
int32_t run(const std::vector<int64_t>& data) {
   int64_t count = 0;
   assert(data.size() % Vec::vector_element_count() == 0);
   for (auto i = 0; i < data.size(); i += Vec::vector_element_count()) {
#ifdef NEON
         auto result = filter_neon->test(tvl::loadu< Vec >(data.data() + i));
         count += tvl::get_msb< Vec >( result );
#else
      if constexpr( std::is_same_v< Vec, tvl::simd< int64_t, tvl::sse > >) {
         auto result = filter_sse->test(tvl::loadu< Vec >(data.data() + i));
         count += tvl::get_msb< Vec >( result );
      } else if constexpr( std::is_same_v< Vec, tvl::simd< int64_t, tvl::avx2 > >) {
         auto result = filter_avx2->test(tvl::loadu< Vec >(data.data() + i));
         count += tvl::get_msb< Vec >( result );
      } else if constexpr( std::is_same_v< Vec, tvl::simd< int64_t, tvl::avx512 > >) {
         auto result = filter_avx512->test(tvl::loadu< Vec >(data.data() + i));
         count += tvl::get_msb< Vec >( result );
      } else {
         throw "Wrong Vec";
      }
#endif
   }
   return count;
}

int32_t run1x64(const std::vector<int64_t>& data) {
   int32_t count = 0;
   for (auto i = 0; i < data.size(); ++i) {
         count += filter->testInt64(data[i]);
      }
   return count;
}

#ifndef NEON
int32_t run4x64(const std::vector<int64_t>& data) {
   using TV = simd::Vectors<int64_t>;
   int32_t count = 0;
   assert(data.size() % 4 == 0);
   for (auto i = 0; i < data.size(); i += 4) {
         auto result = filter->test4x64(simd::Vectors<int64_t>::load(data.data() + i));
         count += TV::compareBitMask(TV::compareResult(result));
      }
   return count;
}
#endif

BENCHMARK(denseScalar) {
   folly::doNotOptimizeAway(run1x64(denseValues));
}
#ifndef NEON
   BENCHMARK_RELATIVE(denseTVLSSE) {
      folly::doNotOptimizeAway(run< tvl::simd< int64_t, tvl::sse > >(denseValues));
   }
   BENCHMARK_RELATIVE(denseAVX2) {
      folly::doNotOptimizeAway(run4x64(denseValues));
   }
   BENCHMARK_RELATIVE(denseTVLAVX2) {
      folly::doNotOptimizeAway(run< tvl::simd< int64_t, tvl::avx2 > >(denseValues));
   }
   BENCHMARK_RELATIVE(denseTVLAVX512) {
      folly::doNotOptimizeAway(run< tvl::simd< int64_t, tvl::avx512 > >(denseValues));
   }
#else
   BENCHMARK_RELATIVE(denseTVLNeon) {
      folly::doNotOptimizeAway(run< tvl::simd< int64_t, tvl::neon > >(denseValues));
   }
#endif

BENCHMARK(sparseScalar) {
   folly::doNotOptimizeAway(run1x64(sparseValues));
}
#ifndef NEON
   BENCHMARK_RELATIVE(sparseTVLSSE) {
      folly::doNotOptimizeAway(run< tvl::simd< int64_t, tvl::sse > >(sparseValues));
   }
   BENCHMARK_RELATIVE(sparseAVX2) {
      folly::doNotOptimizeAway(run4x64(sparseValues));
   }
   BENCHMARK_RELATIVE(sparseTVLAVX2) {
      folly::doNotOptimizeAway(run< tvl::simd< int64_t, tvl::avx2 > >(sparseValues));
   }
   BENCHMARK_RELATIVE(sparseTVLAVX512) {
      folly::doNotOptimizeAway(run< tvl::simd< int64_t, tvl::avx512 > >(sparseValues));
   }
#else
   BENCHMARK_RELATIVE(sparseTVLNeon) {
      folly::doNotOptimizeAway(run< tvl::simd< int64_t, tvl::neon > >(sparseValues));
   }
#endif

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
#ifdef NEON
   filter_neon = std::make_unique<BigintValuesUsingHashTableTVL< tvl::simd< int64_t, tvl::neon > > >(
      filterValues.front(), filterValues.back(), filterValues, false);
#else

   filter_sse = std::make_unique<BigintValuesUsingHashTableTVL< tvl::simd< int64_t, tvl::sse > > >(
      filterValues.front(), filterValues.back(), filterValues, false);
   filter_avx2 = std::make_unique<BigintValuesUsingHashTableTVL< tvl::simd< int64_t, tvl::avx2 > > >(
      filterValues.front(), filterValues.back(), filterValues, false);
   filter_avx512 = std::make_unique<BigintValuesUsingHashTableTVL< tvl::simd< int64_t, tvl::avx512 > > >(
      filterValues.front(), filterValues.back(), filterValues, false);
#endif
   denseValues.resize(kNumValues);
   sparseValues.resize(kNumValues);
   for (auto i = 0; i < kNumValues; ++i) {
         denseValues[i] = (folly::Random::rand32() % 3000) * 1000;
         sparseValues[i] = (folly::Random::rand32() % 100000) * 1000;
      }

   folly::runBenchmarks();
   return 0;
}
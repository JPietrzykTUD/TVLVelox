/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

#include <folly/Range.h>
#include <folly/container/F14Set.h>

#include "StringView.h"
//#include "Exceptions.h"

#include "tvl/tvlintrin.hpp"




namespace facebook::velox::common{

#ifdef AVX512
   using vecext = tvl::simd< int64_t, tvl::avx512 >;
#elif defined(AVX2)
   using vecext = tvl::simd< int64_t, tvl::avx2 >;
#elif defined(SSE)
   using vecext = tvl::simd< int64_t, tvl::sse >;
#elif defined(NEON)
   using vecext = tvl::simd< int64_t, tvl::neon >;
#else
   using vecext = tvl::simd< int64_t, tvl::scalar>;
#endif
   using register_type = typename vecext::register_type;

   enum class FilterKind{
      kAlwaysFalse, kAlwaysTrue, kIsNull, kIsNotNull, kBoolValue, kBigintRange, kBigintValuesUsingHashTable, kBigintValuesUsingBitmask, kDoubleRange, kFloatRange, kBytesRange, kBytesValues, kBigintMultiRange, kMultiRange, };

/**
 * A simple filter (e.g. comparison with literal) that can be applied
 * efficiently while extracting values from an ORC stream.
 */
   class Filter{
      protected:
         Filter( bool deterministic, bool nullAllowed, FilterKind kind )
            : nullAllowed_( nullAllowed ), deterministic_( deterministic ), kind_( kind ){ }

      public:
         virtual ~Filter( ) = default;

         // Templates parametrized on filter need to know determinism at compile
         // time. If this is false, deterministic() will be consulted at
         // runtime.
         static constexpr bool deterministic = true;

         FilterKind kind( ) const{
            return kind_;
         }

         /// Return a copy of this filter. If nullAllowed is set, modified the
         /// nullAllowed flag in the copy to match.
         virtual std::unique_ptr< Filter > clone(
            std::optional< bool > nullAllowed = std::nullopt
         ) const = 0;

         /**
          * A filter becomes non-deterministic when applies to nested column,
          * e.g. a[1] > 10 is non-deterministic because > 10 filter applies only to
          * some positions, e.g. first entry in a set of entries that correspond to a
          * single top-level position.
          */
         virtual bool isDeterministic( ) const{
            return deterministic_;
         }

         /**
          * When a filter applied to a nested column fails, the whole top-level
          * position should fail. To enable this functionality, the filter keeps track
          * of the boundaries of top-level positions and allows the caller to find out
          * where the current top-level position started and how far it continues.
          * @return number of positions from the start of the current top-level
          * position up to the current position (excluding current position)
          */
         virtual int getPrecedingPositionsToFail( ) const{
            return 0;
         }

         /**
          * @return number of positions remaining until the end of the current
          * top-level position
          */
         virtual int getSucceedingPositionsToFail( ) const{
            return 0;
         }

         virtual bool testNull( ) const{
            return nullAllowed_;
         }

         /**
          * Used to apply is [not] null filters to complex types, e.g.
          * a[1] is null AND a[3] is not null, where a is an array(array(T)).
          *
          * In these case, the exact values are not known, but it is known whether they
          * are null or not. Furthermore, for some positions only nulls are allowed
          * (a[1] is null), for others only non-nulls (a[3] is not null), and for the
          * rest both are allowed (a[2] and a[N], where N > 3).
          */
         virtual bool testNonNull( ) const{
            throw "not supported";
         }

         virtual bool testInt64( int64_t /* unused */ ) const{
            throw "not supported";
         }

         virtual register_type test( register_type x ){
            alignas( vecext::vector_alignment() )
               std::array< int64_t, vecext::vector_element_count() > tmp_buf{ };
            for( auto i = 0; i < vecext::vector_element_count(); ++i ) {
               tmp_buf[ i ] = testInt64( x[ i ] ) ? -1LL : 0LL;
            }

            return tvl::load< vecext >( tmp_buf.data( ) );
         }

         virtual bool testDouble( double /* unused */ ) const{
            throw "not supported";
         }

         virtual bool testFloat( float /* unused */ ) const{
            throw "not supported";
         }

         virtual bool testBool( bool /* unused */ ) const{
            throw "not supported";
         }

         virtual bool testBytes( const char * /* unused */, int32_t /* unused */ ) const{
            throw "not supported";
         }

         // Returns true if it is useful to call testLength before other
         // tests. This should be true for string IN and equals because it is
         // possible to fail these based on the length alone. This would
         // typically be false of string ranges because these cannot be
         // generally decided without looking at the string itself.
         virtual bool hasTestLength( ) const{
            return false;
         }

         /**
          * Filters like string equality and IN, as well as conditions on cardinality
          * of lists and maps can be at least partly decided by looking at lengths
          * alone. If this is false, then no further checks are needed. If true,
          * eventual filters on the data itself need to be evaluated.
          */
         virtual bool testLength( int32_t /* unused */ ) const{
            throw "not supported";
         }

         // Tests 8 lengths at a time.
//         virtual __m256si test8xLength( __m256si lengths ) const{
//            return ( __m256si ) _mm256_setr_epi32(
//               testLength( simd::Vectors< int32_t >::extract< 0 >( lengths ) ) ? -1 : 0, testLength( simd::Vectors< int32_t >::extract< 1 >( lengths ) ) ? -1 : 0, testLength( simd::Vectors< int32_t >::extract< 2 >( lengths ) ) ? -1 : 0, testLength(
//                  simd::Vectors< int32_t >::extract< 3 >( lengths )
//               ) ? -1 : 0, testLength( simd::Vectors< int32_t >::extract< 4 >( lengths ) ) ? -1 : 0, testLength( simd::Vectors< int32_t >::extract< 5 >( lengths ) ) ? -1 : 0, testLength( simd::Vectors< int32_t >::extract< 6 >( lengths ) ) ? -1 : 0
//               , testLength( simd::Vectors< int32_t >::extract< 7 >( lengths ) ) ? -1 : 0
//            );
//         }

         // Returns true if at least one value in the specified range can pass the
         // filter. The range is defined as all values between min and max inclusive
         // plus null if hasNull is true.
         virtual bool
         testInt64Range( int64_t /*min*/, int64_t /*max*/, bool /*hasNull*/ ) const{
            throw "not supported";
         }

         // Returns true if at least one value in the specified range can pass the
         // filter. The range is defined as all values between min and max inclusive
         // plus null if hasNull is true.
         virtual bool testDoubleRange( double /*min*/, double /*max*/, bool /*hasNull*/ )
         const{
            throw "not supported";
         }

         virtual bool testBytesRange(
            std::optional< std::string_view > /*min*/, std::optional< std::string_view > /*max*/, bool /*hasNull*/
         ) const{
            throw "not supported";
         }

         // Combines this filter with another filter using 'AND' logic.
         virtual std::unique_ptr< Filter > mergeWith( const Filter * /*other*/) const{
            throw "not supported";
         }

         virtual std::string toString( ) const;

      protected:
         const bool nullAllowed_;

      private:
         const bool deterministic_;
         const FilterKind kind_;
   };

/// IN-list filter for integral data types. Implemented as a hash table. Good
/// for large number of values that do not fit within a small range.
   class BigintValuesUsingHashTable final
      : public Filter{
      public:
         /// @param min Minimum value.
         /// @param max Maximum value.
         /// @param values A list of unique values that pass the filter. Must contain
         /// at least two entries.
         /// @param nullAllowed Null values are passing the filter if true.
         BigintValuesUsingHashTable(
            int64_t min, int64_t max, const std::vector< int64_t > & values, bool nullAllowed
         );

         BigintValuesUsingHashTable(
            const BigintValuesUsingHashTable & other, bool nullAllowed
         )
            : Filter( true, nullAllowed, other.kind( ) ), min_( other.min_ ), max_( other.max_ ), hashTable_( other.hashTable_ ), containsEmptyMarker_( other.containsEmptyMarker_ ), values_( other.values_ ), sizeMask_( other.sizeMask_ ){ }

         std::unique_ptr< Filter > clone(
            std::optional< bool > nullAllowed = std::nullopt
         ) const final{
            if( nullAllowed )
               {
                  return std::make_unique< BigintValuesUsingHashTable >(
                     * this, nullAllowed.value( )
                  );
               }else
               {
                  return std::make_unique< BigintValuesUsingHashTable >( * this );
               }
         }

         bool testInt64( int64_t value ) const final;

         register_type test( register_type x ) final;

         bool testInt64Range( int64_t min, int64_t max, bool hashNull ) const final;

         std::unique_ptr< Filter > mergeWith( const Filter * other ) const final;

         int64_t min( ) const{
            return min_;
         }

         int64_t max( ) const{
            return max_;
         }

         std::string toString( ) const final{
            return fmt::format(
               "BigintValuesUsingHashTable: [{}, {}] {}", min_, max_, nullAllowed_ ? "with nulls" : "no nulls"
            );
         }

      private:
         std::unique_ptr< Filter >
         mergeWith( int64_t min, int64_t max, const Filter * other ) const;

         static constexpr int64_t kEmptyMarker = 0xdeadbeefbadefeedL;
         // from Murmur hash
         static constexpr uint64_t M = 0xc6a4a7935bd1e995L;

         const int64_t min_;
         const int64_t max_;
         std::vector< int64_t > hashTable_;
         bool containsEmptyMarker_ = false;
         std::vector< int64_t > values_;
         int32_t sizeMask_;
   };

// Helper for applying filters to different types
   template< typename TFilter, typename T >
      static inline bool applyFilter( TFilter & filter, T value ){
         if( std::is_same< T, int8_t >::value || std::is_same< T, int16_t >::value ||
             std::is_same< T, int32_t >::value || std::is_same< T, int64_t >::value )
            {
               return filter.testInt64( value );
            }else if( std::is_same< T, float >::value )
            {
               return filter.testFloat( value );
            }else if( std::is_same< T, double >::value )
            {
               return filter.testDouble( value );
            }else if( std::is_same< T, bool >::value )
            {
               return filter.testBool( value );
            }else
            {
               throw "Bad argument type to filter";
            }
      }

   template< typename TFilter >
      static inline bool applyFilter( TFilter & filter, folly::StringPiece value ){
         return filter.testBytes( value.data( ), value.size( ) );
      }

   template< typename TFilter >
      static inline bool applyFilter( TFilter & filter, StringView value ){
         return filter.testBytes( value.data( ), value.size( ) );
      }

// Creates a hash or bitmap based IN filter depending on value distribution.
   std::unique_ptr< Filter > createBigintValues(
      const std::vector< int64_t > & values, bool nullAllowed
   );

} // namespace facebook::velox::common
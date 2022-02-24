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
#include "FilterMisc.h"



namespace facebook::velox::common{



/**
 * A simple filter (e.g. comparison with literal) that can be applied
 * efficiently while extracting values from an ORC stream.
 */
   template< tvl::VectorProcessingStyle Vec >
   class FilterTVL{
      public:
         using register_type = typename Vec::register_type;
      protected:
         FilterTVL( bool deterministic, bool nullAllowed, FilterKind kind )
            : nullAllowed_( nullAllowed ), deterministic_( deterministic ), kind_( kind ){ }

      public:
         virtual ~FilterTVL( ) = default;

         // Templates parametrized on filter need to know determinism at compile
         // time. If this is false, deterministic() will be consulted at
         // runtime.
         static constexpr bool deterministic = true;

         FilterKind kind( ) const{
            return kind_;
         }

         /// Return a copy of this filter. If nullAllowed is set, modified the
         /// nullAllowed flag in the copy to match.
         virtual std::unique_ptr< FilterTVL > clone(
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
            alignas( Vec::vector_alignment() )
               std::array< int64_t, Vec::vector_element_count() > tmp_buf{ };
            for( auto i = 0; i < Vec::vector_element_count(); ++i ) {
               tmp_buf[ i ] = testInt64( x[ i ] ) ? -1LL : 0LL;
            }

            return tvl::load< Vec >( tmp_buf.data( ) );
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
         virtual std::unique_ptr< FilterTVL > mergeWith( const FilterTVL * /*other*/) const{
            throw "not supported";
         }

         virtual std::string toString( ) const {
            const char * strKind = "<unknown>";
            switch( kind_ )
               {
                  case FilterKind::kAlwaysFalse:strKind = "AlwaysFalse";
                  break;
                  case FilterKind::kAlwaysTrue:strKind = "AlwaysTrue";
                  break;
                  case FilterKind::kIsNull:strKind = "IsNull";
                  break;
                  case FilterKind::kIsNotNull:strKind = "IsNotNull";
                  break;
                  case FilterKind::kBoolValue:strKind = "BoolValue";
                  break;
                  case FilterKind::kBigintRange:strKind = "BigintRange";
                  break;
                  case FilterKind::kBigintValuesUsingHashTable:strKind = "BigintValuesUsingHashTable";
                  break;
                  case FilterKind::kBigintValuesUsingBitmask:strKind = "BigintValuesUsingBitmask";
                  break;
                  case FilterKind::kDoubleRange:strKind = "DoubleRange";
                  break;
                  case FilterKind::kFloatRange:strKind = "FloatRange";
                  break;
                  case FilterKind::kBytesRange:strKind = "BytesRange";
                  break;
                  case FilterKind::kBytesValues:strKind = "BytesValues";
                  break;
                  case FilterKind::kBigintMultiRange:strKind = "BigintMultiRange";
                  break;
                  case FilterKind::kMultiRange:strKind = "MultiRange";
                  break;
               };

            return fmt::format(
               "FilterTVL({}, {}, {})", strKind, deterministic_ ? "deterministic" : "nondeterministic", nullAllowed_ ? "null allowed" : "null not allowed"
            );
         }

      protected:
         const bool nullAllowed_;

      private:
         const bool deterministic_;
         const FilterKind kind_;
   };

/// IN-list filter for integral data types. Implemented as a hash table. Good
/// for large number of values that do not fit within a small range.
   template< tvl::VectorProcessingStyle Vec >
   class BigintValuesUsingHashTableTVL final
      : public FilterTVL< Vec >{
      public:
         using register_type = Vec::register_type;
         using FilterTVL< Vec >::nullAllowed_;
      public:
         /// @param min Minimum value.
         /// @param max Maximum value.
         /// @param values A list of unique values that pass the filter. Must contain
         /// at least two entries.
         /// @param nullAllowed Null values are passing the filter if true.
         BigintValuesUsingHashTableTVL(
            int64_t min, int64_t max, const std::vector< int64_t > & values, bool nullAllowed
         )
            : FilterTVL< Vec >( true, nullAllowed, FilterKind::kBigintValuesUsingHashTable ), min_( min ), max_( max ), values_( values ){
            constexpr int32_t kPaddingElements = 4;
            if( min >= max )
               {
                  throw "min must be less than max";
               }
            if( values.size( ) <= 1 )
               {
                  throw "values must contain at least 2 entries";
               }

            // Size the hash table to be 2+x the entry count, e.g. 10 entries
            // gets 1 << log2 of 50 == 32. The filter is expected to fail often so we
            // wish to increase the chance of hitting empty on first probe.
            auto size = 1u<<( uint32_t ) std::log2( values.size( )*5 );
            hashTable_.resize( size+kPaddingElements );
            sizeMask_ = size-1;
            std::fill( hashTable_.begin( ), hashTable_.end( ), kEmptyMarker );
            for( auto value : values )
               {
                  if( value == kEmptyMarker )
                     {
                        containsEmptyMarker_ = true;
                     }else
                     {
                        auto position = ( ( value*M )&( size-1 ) );
                        for( auto i = position; i < position+size; i++ )
                           {
                              uint32_t index = i&sizeMask_;
                              if( hashTable_[ index ] == kEmptyMarker )
                                 {
                                    hashTable_[ index ] = value;
                                    break;
                                 }
                           }
                     }
               }
            // Replicate the last element of hashTable kPaddingEntries times at 'size_' so
            // that one can load a full vector of elements past the last used index.
            for( auto i = 0; i < kPaddingElements; ++i )
               {
                  hashTable_[ sizeMask_+1+i ] = hashTable_[ sizeMask_ ];
               }
            std::sort( values_.begin( ), values_.end( ) );
         }

         BigintValuesUsingHashTableTVL(
            const BigintValuesUsingHashTableTVL & other, bool nullAllowed
         )
            : FilterTVL< Vec >( true, nullAllowed, other.kind( ) ), min_( other.min_ ), max_( other.max_ ), hashTable_( other.hashTable_ ), containsEmptyMarker_( other.containsEmptyMarker_ ), values_( other.values_ ), sizeMask_( other.sizeMask_ ){ }

         std::unique_ptr< FilterTVL< Vec > > clone(
            std::optional< bool > nullAllowed = std::nullopt
         ) const final{
            if( nullAllowed )
               {
                  return std::make_unique< BigintValuesUsingHashTableTVL< Vec > >(
                     * this, nullAllowed.value( )
                  );
               }else
               {
                  return std::make_unique< BigintValuesUsingHashTableTVL< Vec > >( * this );
               }
         }

         bool testInt64( int64_t value ) const final {
            if( containsEmptyMarker_ && value == kEmptyMarker )
               {
                  return true;
               }
            if( value < min_ || value > max_ )
               {
                  return false;
               }
            uint32_t pos = ( value*M )&sizeMask_;
            for( auto i = pos; i <= pos+sizeMask_; i++ )
               {
                  int32_t idx = i&sizeMask_;
                  int64_t l = hashTable_[ idx ];
                  if( l == kEmptyMarker )
                     {
                        return false;
                     }
                  if( l == value )
                     {
                        return true;
                     }
               }
            return false;
         }

         typename Vec::register_type test( typename Vec::register_type x ) final {

            auto rangeMask =
               tvl::between_inclusive< Vec >(
                  x, tvl::set1< Vec >( min_ ), tvl::set1< Vec >( max_ )
               );

            if( tvl::to_integral< Vec >( rangeMask ) == 0 )
               {
                  return tvl::set1< Vec >( 0 );
               }
            if( containsEmptyMarker_ )
               {
                  return FilterTVL< Vec >::test( x );
               }

            auto indices = x*M&sizeMask_;
            auto const emptyMarkerVec = tvl::set1< Vec >( kEmptyMarker );
            auto data = tvl::gather< Vec >(
               emptyMarkerVec, hashTable_.data( ), indices, rangeMask
            );
            // The lanes with kEmptyMarker missed, the lanes matching x hit and the other
            // lanes must check next positions.

            auto result = tvl::equal< Vec >( x, data );
            auto missed = tvl::equal< Vec >( data, emptyMarkerVec );

            uint16_t unresolved = tvl::mask_reduce< Vec >(
               ~tvl::to_integral< Vec >( result )&
               ~tvl::to_integral< Vec >( missed )
            );

            if( !unresolved )
               {
                  return tvl::to_vector< Vec >( result );
               }
            int64_t indicesArray[Vec::vector_element_count()];
            int64_t valuesArray[Vec::vector_element_count()];
            int64_t resultArray[Vec::vector_element_count()];

            * reinterpret_cast<register_type *>(indicesArray) = indices+1;
            * reinterpret_cast<register_type *>(valuesArray) = x;
            * reinterpret_cast<register_type *>(resultArray) = tvl::to_vector< Vec >( result );
            auto allEmpty = tvl::set1< Vec >( kEmptyMarker );

            while( unresolved )
               {
                  auto lane = bits::getAndClearLastSetBit( unresolved );
                  // Loop for each unresolved (not hit and
                  // not empty) until finding hit or empty.
                  int64_t index = indicesArray[ lane ];
                  int64_t value = valuesArray[ lane ];
                  auto allValue = tvl::set1< Vec >( value );
                  for( ;; )
                     {
                        auto line = tvl::loadu< Vec >(
                           hashTable_.data( )+index
                        );

                        if( tvl::to_integral< Vec >( tvl::equal< Vec >( line, allValue ) ) )
                           {
                              resultArray[ lane ] = -1;
                              break;
                           }
                        if( tvl::to_integral< Vec >( tvl::equal< Vec >( line, allEmpty ) ) )
                           {
                              resultArray[ lane ] = 0;
                              break;
                           }
                        index += 4;
                        if( index > sizeMask_ )
                           {
                              index = 0;
                           }
                     }
               }
            return tvl::loadu< Vec >( resultArray );
         }

         bool testInt64Range( int64_t min, int64_t max, bool hasNull ) const final {
            if( hasNull && nullAllowed_ )
               {
                  return true;
               }

            if( min == max )
               {
                  return testInt64( min );
               }

            if( min > max_ || max < min_ )
               {
                  return false;
               }
            auto it = std::lower_bound( values_.begin( ), values_.end( ), min );
            assert( it != values_.end( ) ); // min is already tested to be <= max_.
            if( min == * it )
               {
                  return true;
               }
            return max >= * it;
         }

         std::unique_ptr< FilterTVL< Vec > > mergeWith( const FilterTVL< Vec > * other ) const final {
            switch( other->kind( ) )
               {
                  case FilterKind::kIsNotNull:return std::make_unique< BigintValuesUsingHashTableTVL< Vec > >( * this, false );
                  case FilterKind::kBigintValuesUsingHashTable:
                     {
                        auto otherValues = dynamic_cast<const BigintValuesUsingHashTableTVL< Vec > *>(other);
                        auto min = std::max( min_, otherValues->min( ) );
                        auto max = std::min( max_, otherValues->max( ) );

                        return mergeWith( min, max, other );
                     }
                  throw "unreachable";
               }
         }

         int64_t min( ) const{
            return min_;
         }

         int64_t max( ) const{
            return max_;
         }

         std::string toString( ) const final{
            return fmt::format(
               "BigintValuesUsingHashTableTVL: [{}, {}] {}", min_, max_, nullAllowed_ ? "with nulls" : "no nulls"
            );
         }

      private:
         std::unique_ptr< FilterTVL< Vec > >
         mergeWith( int64_t min, int64_t max, const FilterTVL< Vec > * other ) const {
            bool bothNullAllowed = nullAllowed_ && other->testNull( );

            std::vector< int64_t > valuesToKeep;
            valuesToKeep.reserve( values_.size( ) );
            if( containsEmptyMarker_ && other->testInt64( kEmptyMarker ) )
               {
                  valuesToKeep.emplace_back( kEmptyMarker );
               }

            for( int64_t v : hashTable_ )
               {
                  if( v != kEmptyMarker && other->testInt64( v ) )
                     {
                        valuesToKeep.emplace_back( v );
                     }
               }

            return createBigintValuesTVL< Vec >( valuesToKeep, bothNullAllowed );
         }

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

// Creates a hash or bitmap based IN filter depending on value distribution.
   template< tvl::VectorProcessingStyle Vec >
   std::unique_ptr< FilterTVL< Vec > > createBigintValuesTVL(
      const std::vector< int64_t > & values, bool nullAllowed
   ) {
      int64_t min = values[ 0 ];
      int64_t max = values[ 0 ];
      for( int i = 1; i < values.size( ); ++i )
         {
            if( values[ i ] > max )
               {
                  max = values[ i ];
               }else if( values[ i ] < min )
               {
                  min = values[ i ];
               }
         }
      return std::make_unique< BigintValuesUsingHashTableTVL< Vec > >(
         min, max, values, nullAllowed
      );
   }

} // namespace facebook::velox::common
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
#include "Filter.h"

namespace facebook::velox::common{

   std::string Filter::toString( ) const{
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
         "Filter({}, {}, {})", strKind, deterministic_ ? "deterministic" : "nondeterministic", nullAllowed_ ? "null allowed" : "null not allowed"
      );
   }

   BigintValuesUsingHashTable::BigintValuesUsingHashTable(
      int64_t min, int64_t max, const std::vector< int64_t > & values, bool nullAllowed
   )
      : Filter( true, nullAllowed, FilterKind::kBigintValuesUsingHashTable ), min_( min ), max_( max ), values_( values ){
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

   bool BigintValuesUsingHashTable::testInt64( int64_t value ) const{
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

   register_type BigintValuesUsingHashTable::test( register_type x ){

      auto rangeMask =
         tvl::between_inclusive< vecext >(
            x, tvl::set1< vecext >( min_ ), tvl::set1< vecext >( max_ )
         );

      if( tvl::to_integral< vecext >( rangeMask ) == 0 )
         {
            return tvl::set1< vecext >( 0 );
         }
      if( containsEmptyMarker_ )
         {
            return Filter::test( x );
         }

      auto indices = x*M&sizeMask_;
      auto const emptyMarkerVec = tvl::set1< vecext >( kEmptyMarker );
      auto data = tvl::gather< vecext >(
         emptyMarkerVec, hashTable_.data( ), indices, rangeMask
      );
      // The lanes with kEmptyMarker missed, the lanes matching x hit and the other
      // lanes must check next positions.

      auto result = tvl::equal< vecext >( x, data );
      auto missed = tvl::equal< vecext >( data, emptyMarkerVec );

      uint16_t unresolved = tvl::mask_reduce< vecext >(
         ~tvl::to_integral< vecext >( result )&
         ~tvl::to_integral< vecext >( missed )
      );

      if( !unresolved )
         {
            return tvl::to_vector< vecext >( result );
         }
      int64_t indicesArray[vecext::vector_element_count()];
      int64_t valuesArray[vecext::vector_element_count()];
      int64_t resultArray[vecext::vector_element_count()];

      * reinterpret_cast<register_type *>(indicesArray) = indices+1;
      * reinterpret_cast<register_type *>(valuesArray) = x;
      * reinterpret_cast<register_type *>(resultArray) = tvl::to_vector< vecext >( result );
      auto allEmpty = tvl::set1< vecext >( kEmptyMarker );

      while( unresolved )
         {
            auto lane = bits::getAndClearLastSetBit( unresolved );
            // Loop for each unresolved (not hit and
            // not empty) until finding hit or empty.
            int64_t index = indicesArray[ lane ];
            int64_t value = valuesArray[ lane ];
            auto allValue = tvl::set1< vecext >( value );
            for( ;; )
               {
                  auto line = tvl::loadu< vecext >(
                     hashTable_.data( )+index
                  );

                  if( tvl::to_integral< vecext >( tvl::equal< vecext >( line, allValue ) ) )
                     {
                        resultArray[ lane ] = -1;
                        break;
                     }
                  if( tvl::to_integral< vecext >( tvl::equal< vecext >( line, allEmpty ) ) )
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
      return tvl::loadu< vecext >( resultArray );
   }

   bool BigintValuesUsingHashTable::testInt64Range(
      int64_t min, int64_t max, bool hasNull
   ) const{
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

   std::unique_ptr< Filter > createBigintValues(
      const std::vector< int64_t > & values, bool nullAllowed
   ){
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
      return std::make_unique< BigintValuesUsingHashTable >(
         min, max, values, nullAllowed
      );
   }

   namespace{
      int compareRanges( const char * lhs, size_t length, const std::string & rhs ){
         int size = std::min( length, rhs.length( ) );
         int compare = memcmp( lhs, rhs.data( ), size );
         if( compare )
            {
               return compare;
            }
         return length-rhs.size( );
      }
   } // namespace



   namespace{
      int32_t binarySearch( const std::vector< int64_t > & values, int64_t value ){
         auto it = std::lower_bound( values.begin( ), values.end( ), value );
         if( it == values.end( ) || * it != value )
            {
               return -std::distance( values.begin( ), it )-1;
            }else
            {
               return std::distance( values.begin( ), it );
            }
      }
   } // namespace


   std::unique_ptr< Filter > BigintValuesUsingHashTable::mergeWith(
      const Filter * other
   ) const{
      switch( other->kind( ) )
         {
            case FilterKind::kIsNotNull:return std::make_unique< BigintValuesUsingHashTable >( * this, false );
            case FilterKind::kBigintValuesUsingHashTable:
               {
                  auto otherValues = dynamic_cast<const BigintValuesUsingHashTable *>(other);
                  auto min = std::max( min_, otherValues->min( ) );
                  auto max = std::min( max_, otherValues->max( ) );

                  return mergeWith( min, max, other );
               }
            throw "unreachable";
         }
   }

   std::unique_ptr< Filter > BigintValuesUsingHashTable::mergeWith(
      int64_t min, int64_t max, const Filter * other
   ) const{
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

      return createBigintValues( valuesToKeep, bothNullAllowed );
   }

   namespace{
// compareResult = left < right for upper, right < left for lower
      bool mergeExclusive( int compareResult, bool left, bool right ){
         return compareResult == 0 ? ( left || right )
                                   : ( compareResult < 0 ? left : right );
      }
   } // namespace


} // namespace facebook::velox::common

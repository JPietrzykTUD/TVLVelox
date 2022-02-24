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
#include "FilterTVL.h"

namespace facebook::velox::common{


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


   namespace{
// compareResult = left < right for upper, right < left for lower
      bool mergeExclusive( int compareResult, bool left, bool right ){
         return compareResult == 0 ? ( left || right )
                                   : ( compareResult < 0 ? left : right );
      }
   } // namespace


} // namespace facebook::velox::common

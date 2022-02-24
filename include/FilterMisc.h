#pragma once
#include <folly/Range.h>
#include <folly/container/F14Set.h>
#include "StringView.h"

namespace facebook ::velox::common{

   enum class FilterKind{
      kAlwaysFalse, kAlwaysTrue, kIsNull, kIsNotNull, kBoolValue, kBigintRange, kBigintValuesUsingHashTable, kBigintValuesUsingBitmask, kDoubleRange, kFloatRange, kBytesRange, kBytesValues, kBigintMultiRange, kMultiRange, };



// Helper for applying filters to different types
   template< typename TFilterTVL, typename T >
      static inline bool applyFilter( TFilterTVL & filter, T value ){
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

   template< typename TFilterTVL >
      static inline bool applyFilter( TFilterTVL & filter, folly::StringPiece value ){
         return filter.testBytes( value.data( ), value.size( ) );
      }

   template< typename TFilterTVL >
      static inline bool applyFilter( TFilterTVL & filter, StringView value ){
         return filter.testBytes( value.data( ), value.size( ) );
      }

}
/**********************************************************************************************
 * This file is part of the TVL Project.                                                      *
 * Copyright (C) 2022 by TVL-Team                                                             *
 *                                                                                            *
 * This file is part of TVL - a template simd library.                                        *
 *                                                                                            *
 * This program is free software: you can redistribute it and/or modify it under the          *
 * terms of the GNU General Public License as published by the Free Software Foundation,      *
 * either version 3 of the License, or (at your option) any later version.                    *
 *                                                                                            *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;  *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  *
 * See the GNU General Public License for more details.                                       *
 *                                                                                            *
 * You should have received a copy of the GNU General Public License along with this program. *
 * If not, see <http://www.gnu.org/licenses/>.                                                *
 **********************************************************************************************/
 //todo: Apache2
/*
 * @file lib/static/utils/type_helper.hpp
 * @date 17.02.2022
 * @brief TODO.
 */
#ifndef TUD_D2RG_TVL_LIB_STATIC_UTILS_TYPE_HELPER_HPP
#define TUD_D2RG_TVL_LIB_STATIC_UTILS_TYPE_HELPER_HPP

#include <cstddef>
#include <type_traits>
#include <typeinfo>
#include <cxxabi.h>
#include <memory>
#include <string>
#include <cstdlib>
#include <limits>

namespace tvl {
   template< class T >
   std::string type_name( ) {
      typedef typename std::remove_reference< T >::type TR;
      std::unique_ptr< char, void ( * )( void * ) > own (
         abi::__cxa_demangle( typeid( TR ).name( ), nullptr,nullptr, nullptr ),
         std::free
      );
      std::string r = own != nullptr ? own.get( ) : typeid( TR ).name( );
      if( std::is_const< TR >::value ) {
         r += " const";
      }
      if( std::is_volatile< TR >::value ) {
         r += " volatile";
         }
      if( std::is_lvalue_reference< T >::value ) {
         r += "&";
      } else if( std::is_rvalue_reference< T >::value ) {
         r += "&&";
      }
      return r;
   }
   #define TYPENAME( x ) type_name< decltype( x ) >( )

   
   template< typename T >
   struct is_tuple_impl : std::false_type { };   
   template< typename... Ts >
   struct is_tuple_impl< std::tuple< Ts... > > : std::true_type { };
   template< typename T >
   struct is_tuple : is_tuple_impl< std::remove_cv_t< T > > { };

   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_STATIC_UTILS_TYPE_HELPER_HPP
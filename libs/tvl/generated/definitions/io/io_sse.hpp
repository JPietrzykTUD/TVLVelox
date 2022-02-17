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
 * @file lib/generated/definitions/io/io_sse.hpp
 * @date 17.02.2022
 * @brief I/O primitives. Implementation for sse
 */
#ifndef TUD_D2RG_TVL_LIB_GENERATED_DEFINITIONS_IO_IO_SSE_HPP
#define TUD_D2RG_TVL_LIB_GENERATED_DEFINITIONS_IO_IO_SSE_HPP

#include "../../declarations/io.hpp"

namespace tvl {
   namespace details {
      template< ImplementationDegreeOfFreedom Idof >
      struct load_impl< simd< int64_t, sse  >, Idof > {
         using Vec = simd< int64_t, sse  >;
         static constexpr bool native_supported() {
            return true;
         }
   /*
    * @brief Loads data from aligned memory into a vector register.
    * @details todo.
    * @param memory Aligned memory which should be transferred into a vector register.
    * @return Vector containing the loaded data.
    */
         [[nodiscard]] 
         TVL_FORCE_INLINE static typename Vec::register_type apply(
            typename Vec::base_type const *  memory
         ) {return _mm_load_si128( reinterpret_cast< __m128i const * >( memory ) );
         }
      };
   } // end of namespace details for template specialization of load_impl for sse using int64_t.
   
   namespace details {
      template< ImplementationDegreeOfFreedom Idof >
      struct loadu_impl< simd< int64_t, sse  >, Idof > {
         using Vec = simd< int64_t, sse  >;
         static constexpr bool native_supported() {
            return true;
         }
   /*
    * @brief Loads data from (un)aligned memory into a vector register.
    * @details todo.
    * @param memory (Un)aligned memory which should be transferred into a vector register.
    * @return Vector containing the loaded data.
    */
         [[nodiscard]] 
         TVL_FORCE_INLINE static typename Vec::register_type apply(
            typename Vec::base_type const *  memory
         ) {return _mm_loadu_si128( reinterpret_cast< __m128i const * >( memory ) );
         }
      };
   } // end of namespace details for template specialization of loadu_impl for sse using int64_t.
   
   namespace details {
      template< ImplementationDegreeOfFreedom Idof >
      struct set1_impl< simd< int64_t, sse  >, Idof > {
         using Vec = simd< int64_t, sse  >;
         static constexpr bool native_supported() {
            return true;
         }
   /*
    * @brief Broadcasts a single value into all lanes of a vector register.
    * @details todo.
    * @param value Value which should be broadcasted.
    * @return Vector containing the same value in all lanes.
    */
         [[nodiscard]] 
         TVL_FORCE_INLINE static typename Vec::register_type apply(
            typename Vec::base_type  value
         ) {return _mm_set1_epi64x( value );
         }
      };
   } // end of namespace details for template specialization of set1_impl for sse using int64_t.
   
   namespace details {
      template< ImplementationDegreeOfFreedom Idof >
      struct gather_impl< simd< int64_t, sse  >, Idof > {
         using Vec = simd< int64_t, sse  >;
         static constexpr bool native_supported() {
            return false;
         }
   /*
    * @brief Transfers data from arbitrary locations into a vector register.
    * @details todo.
    * @param source Vector register containing values which should be preserved depending on the mask (if mask[i] == 0).
    * @param memory (Start)pointer of the memory (which is used as base for address calculation).
    * @param index Offsets (in base_type granularity) relative to the start pointer.
    * @param mask Mask indicating which lanes should be gathered.
    * @return Vector containing gathered data.
    */
         [[nodiscard]] TVL_NO_NATIVE_SUPPORT_WARNING
         TVL_FORCE_INLINE static typename Vec::register_type apply(
            typename Vec::register_type  source, 
            typename Vec::base_type const *  memory, 
            typename Vec::register_type  index, 
            typename Vec::mask_type  mask
         ) {
            static_assert( !std::is_same_v< Idof, native >, "The primitive gather is not supported by your hardware natively while it is forced by using native" );
            return _mm_set_epi64x( ( mask[ 1 ] == 0 ) ? source[ 1 ] : memory[ index[ 1 ] ], ( mask[ 0 ] == 0 ) ? source[ 0 ] : memory[ index[ 0 ] ] );
         }
      };
   } // end of namespace details for template specialization of gather_impl for sse using int64_t.
   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_GENERATED_DEFINITIONS_IO_IO_SSE_HPP
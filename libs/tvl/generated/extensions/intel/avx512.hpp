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
 * @file lib/generated/extensions/intel/avx512.hpp
 * @date 17.02.2022
 * @brief 
 */
#ifndef TUD_D2RG_TVL_LIB_GENERATED_EXTENSIONS_INTEL_AVX512_HPP
#define TUD_D2RG_TVL_LIB_GENERATED_EXTENSIONS_INTEL_AVX512_HPP

#include "immintrin.h"

namespace tvl {
   struct avx512 {
      using default_size_in_bits = std::integral_constant< std::size_t, 512 >;
      template< Arithmetic BaseType, std::size_t VectorSizeInBits = default_size_in_bits::value >
      struct types {
         using register_t __attribute__ ((
            __vector_size__ (
               VectorSizeInBits/sizeof(
                  TVL_DEP_TYPE(
                     (std::is_integral_v< BaseType >),
                     long long,
                     TVL_DEP_TYPE(
                        (sizeof( BaseType ) == 4),
                        float,
                        double
                     )
                  )
               )
            ), 
            __may_alias__, 
            __aligned__(VectorSizeInBits/sizeof(char))
            )) =
            TVL_DEP_TYPE(
               (std::is_integral_v< BaseType >),
               long long,
               TVL_DEP_TYPE(
                  (sizeof( BaseType ) == 4),
                  float,
                  double
               )
            );
         using mask_t =
            TVL_DEP_TYPE(
            ( VectorSizeInBits == 512 ),
            TVL_DEP_TYPE(
               ( sizeof( register_t ) / sizeof( BaseType ) ) == 64,
               __mmask64,
               TVL_DEP_TYPE(
                  ( sizeof( register_t ) / sizeof( BaseType ) ) == 32,
                  __mmask32,
                  TVL_DEP_TYPE(
                     ( sizeof( register_t ) / sizeof( BaseType ) ) == 16,
                     __mmask16,
                     __mmask8
                  )
               )
            ),
            TVL_DEP_TYPE(
               ( VectorSizeInBits == 256 ),
               TVL_DEP_TYPE(
                  ( sizeof( register_t ) / sizeof( BaseType ) ) == 32,
                  __mmask32,
                  TVL_DEP_TYPE(
                     ( sizeof( register_t ) / sizeof( BaseType ) ) == 16,
                     __mmask16,
                     __mmask8
                  )
               ),
               TVL_DEP_TYPE(
                  ( sizeof( register_t ) / sizeof( BaseType ) ) == 16,
                  __mmask16,
                  __mmask8
               )
            )
         );
      };
   };
   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_GENERATED_EXTENSIONS_INTEL_AVX512_HPP
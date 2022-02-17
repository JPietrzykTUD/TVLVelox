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
 * @file lib/generated/extensions/intel/avx2.hpp
 * @date 17.02.2022
 * @brief 
 */
#ifndef TUD_D2RG_TVL_LIB_GENERATED_EXTENSIONS_INTEL_AVX2_HPP
#define TUD_D2RG_TVL_LIB_GENERATED_EXTENSIONS_INTEL_AVX2_HPP

#include "immintrin.h"

namespace tvl {
   struct avx2 {
      using default_size_in_bits = std::integral_constant< std::size_t, 256 >;
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
            register_t;
      };
   };
   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_GENERATED_EXTENSIONS_INTEL_AVX2_HPP
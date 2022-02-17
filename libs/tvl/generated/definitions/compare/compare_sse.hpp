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
 * @file lib/generated/definitions/compare/compare_sse.hpp
 * @date 17.02.2022
 * @brief Compare primitives. Implementation for sse
 */
#ifndef TUD_D2RG_TVL_LIB_GENERATED_DEFINITIONS_COMPARE_COMPARE_SSE_HPP
#define TUD_D2RG_TVL_LIB_GENERATED_DEFINITIONS_COMPARE_COMPARE_SSE_HPP

#include "../../declarations/compare.hpp"

namespace tvl {
   namespace details {
      template< ImplementationDegreeOfFreedom Idof >
      struct equal_impl< simd< int64_t, sse  >, Idof > {
         using Vec = simd< int64_t, sse  >;
         static constexpr bool native_supported() {
            return true;
         }
   /*
    * @brief Compares two vector registers for equality.
    * @details todo.
    * @param vec_a Left vector.
    * @param vec_b Right vector.
    * @return Vector mask type indicating whether vec_a[*]==vec_b[*].
    */
         [[nodiscard]] 
         TVL_FORCE_INLINE static typename Vec::mask_type apply(
            typename Vec::register_type  vec_a, 
            typename Vec::register_type  vec_b
         ) {return _mm_cmpeq_epi64( vec_a, vec_b );
         }
      };
   } // end of namespace details for template specialization of equal_impl for sse using int64_t.
   
   namespace details {
      template< ImplementationDegreeOfFreedom Idof >
      struct between_inclusive_impl< simd< int64_t, sse  >, Idof > {
         using Vec = simd< int64_t, sse  >;
         static constexpr bool native_supported() {
            return true;
         }
   /*
    * @brief Checks if the values of a vector are in a specific range (min[*] <= d[*] <= max[*]).
    * @details todo.
    * @param vec_data Data vector.
    * @param vec_min Minimum vector.
    * @param vec_max Maximum vector.
    * @return Vector mask type indicating whether the data is in the given range.
    */
         [[nodiscard]] 
         TVL_FORCE_INLINE static typename Vec::mask_type apply(
            typename Vec::register_type  vec_data, 
            typename Vec::register_type  vec_min, 
            typename Vec::register_type  vec_max
         ) {return _mm_andnot_si128( _mm_cmpgt_epi64( vec_min, vec_data ), _mm_andnot_si128( _mm_cmpgt_epi64( vec_data, vec_max ), _mm_set1_epi64x(-1)));
         }
      };
   } // end of namespace details for template specialization of between_inclusive_impl for sse using int64_t.
   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_GENERATED_DEFINITIONS_COMPARE_COMPARE_SSE_HPP
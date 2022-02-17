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
 * @file lib/generated/definitions/calc/calc_avx512.hpp
 * @date 17.02.2022
 * @brief Arithmetic primitives. Implementation for avx512
 */
#ifndef TUD_D2RG_TVL_LIB_GENERATED_DEFINITIONS_CALC_CALC_AVX512_HPP
#define TUD_D2RG_TVL_LIB_GENERATED_DEFINITIONS_CALC_CALC_AVX512_HPP

#include "../../declarations/calc.hpp"

namespace tvl {
   namespace details {
      template< ImplementationDegreeOfFreedom Idof >
      struct add_impl< simd< uint64_t, avx512  >, Idof > {
         using Vec = simd< uint64_t, avx512  >;
         static constexpr bool native_supported() {
            return false;
         }
   /*
    * @brief Adds two vector registers.
    * @details todo.
    * @param vec_a First vector.
    * @param vec_b Second vector.
    * @return Vector containing result of the addition.
    */
         [[nodiscard]] TVL_NO_NATIVE_SUPPORT_WARNING
         TVL_FORCE_INLINE static typename Vec::register_type apply(
            typename Vec::register_type  vec_a, 
            typename Vec::register_type  vec_b
         ) {
            static_assert( !std::is_same_v< Idof, native >, "The primitive add is not supported by your hardware natively while it is forced by using native" );
            return _mm512_add_epi64(vec_a, vec_b);
         }
      };
   } // end of namespace details for template specialization of add_impl for avx512 using uint64_t.
   
   namespace details {
      template< ImplementationDegreeOfFreedom Idof >
      struct add_impl< simd< uint32_t, avx512  >, Idof > {
         using Vec = simd< uint32_t, avx512  >;
         static constexpr bool native_supported() {
            return true;
         }
   /*
    * @brief Adds two vector registers.
    * @details todo.
    * @param vec_a First vector.
    * @param vec_b Second vector.
    * @return Vector containing result of the addition.
    */
         [[nodiscard]] 
         TVL_FORCE_INLINE static typename Vec::register_type apply(
            typename Vec::register_type  vec_a, 
            typename Vec::register_type  vec_b
         ) {return _mm512_add_epi32(vec_a, vec_b);
         }
      };
   } // end of namespace details for template specialization of add_impl for avx512 using uint32_t.
   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_GENERATED_DEFINITIONS_CALC_CALC_AVX512_HPP
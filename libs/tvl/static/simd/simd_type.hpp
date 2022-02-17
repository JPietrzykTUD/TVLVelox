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
 * @file lib/static/simd/simd_type.hpp
 * @date 17.02.2022
 * @brief TODO.
 */
#ifndef TUD_D2RG_TVL_LIB_STATIC_SIMD_SIMD_TYPE_HPP
#define TUD_D2RG_TVL_LIB_STATIC_SIMD_SIMD_TYPE_HPP

#include "../utils/type_concepts.hpp"
#include "../utils/type_helper.hpp"
#include "simd_type_concepts.hpp"

namespace tvl {
   template<
      Arithmetic BaseType,
      TargetExtension< BaseType > TargetExtensionType,
      std::size_t VectorSizeInBits = TargetExtensionType::default_size_in_bits::value
   >
   struct simd {
      using base_type = BaseType;
      using target_extension = TargetExtensionType;

      using register_type = typename TargetExtensionType::template types< BaseType, VectorSizeInBits >::register_t;
      using mask_type = typename TargetExtensionType::template types< BaseType, VectorSizeInBits >::mask_t;

      static constexpr /*should be consteval, but clang does not eat this */ std::size_t vector_size_b() {
         return VectorSizeInBits;
      }
      static constexpr std::size_t vector_size_B() {
         return sizeof( register_type );
      }
      static constexpr std::size_t vector_element_count() {
         return sizeof( register_type ) / sizeof( base_type );
      }
      static constexpr std::size_t vector_alignment() {
         if constexpr( vector_size_B() > 32 ) {
            return 64;
         } else {
            return vector_size_B();
         }
      }
      static constexpr std::size_t vector_mask_ratio() {
         return ( sizeof( mask_type ) * 8 ) / vector_element_count();
      }
      static constexpr std::size_t mask_shift() {
         return vector_element_count();
      }
   };

   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_STATIC_SIMD_SIMD_TYPE_HPP
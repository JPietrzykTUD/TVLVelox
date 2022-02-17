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
 * @file lib/static/simd/simd_type_concepts.hpp
 * @date 17.02.2022
 * @brief TODO.
 */
#ifndef TUD_D2RG_TVL_LIB_STATIC_SIMD_SIMD_TYPE_CONCEPTS_HPP
#define TUD_D2RG_TVL_LIB_STATIC_SIMD_SIMD_TYPE_CONCEPTS_HPP

#include <type_traits>
#include <concepts>
#include "../utils/type_concepts.hpp"

namespace tvl {
   template< typename T, typename U >
   concept TargetExtension = Arithmetic< U > && requires {
      typename T::default_size_in_bits;
      typename T:: template types< U >;
      typename T:: template types< U >::register_t;
      typename T:: template types< U >::mask_t;
   };
   template< typename T >
   concept VectorProcessingStyle = requires {
      typename T::base_type;
      typename T::target_extension;
      typename T::register_type;
      typename T::mask_type;
      { T::vector_size_b() } -> std::same_as< std::size_t >;
      { T::vector_size_B() } -> std::same_as< std::size_t >;
      { T::vector_element_count() } -> std::same_as< std::size_t >;
      { T::vector_alignment() } -> std::same_as< std::size_t >;
      { T::vector_mask_ratio() } -> std::same_as< std::size_t >;
      { T::mask_shift() } -> std::same_as< std::size_t >;
   } &&
      Arithmetic< typename T::base_type > &&
      TargetExtension< typename T::target_extension, typename T::base_type > &&
      ( T::vector_size_b() > 0 ) &&
      ( T::vector_size_B() > 0 ) &&
      ( T::vector_element_count() > 0 ) &&
      ( T::vector_alignment() > 0 ) &&
      ( T::vector_mask_ratio() > 0 ) &&
      ( T::mask_shift() > 0 );

   struct native{};
   struct workaround{};
   template< class T >
   concept ImplementationDegreeOfFreedom = std::is_same_v< T, native > || std::is_same_v< T, workaround >;

   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_STATIC_SIMD_SIMD_TYPE_CONCEPTS_HPP
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
 * @file lib/generated/declarations/compare.hpp
 * @date 17.02.2022
 * @brief Compare primitives.
 */
#ifndef TUD_D2RG_TVL_LIB_GENERATED_DECLARATIONS_COMPARE_HPP
#define TUD_D2RG_TVL_LIB_GENERATED_DECLARATIONS_COMPARE_HPP


namespace tvl {
   namespace details {
      template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof >
      struct equal_impl{};
   } // end namespace details
   /*
    * @brief Compares two vector registers for equality.
    * @details todo.
    * @param vec_a Left vector.
    * @param vec_b Right vector.
    * @return Vector mask type indicating whether vec_a[*]==vec_b[*].
    */
   template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof = workaround >
   [[nodiscard]] 
   TVL_FORCE_INLINE typename Vec::mask_type equal(
      typename Vec::register_type  vec_a, 
      typename Vec::register_type  vec_b
   ) {
      return details::equal_impl< Vec, Idof >::apply(
         vec_a, vec_b
      );
   }
   
   namespace details {
      template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof >
      struct between_inclusive_impl{};
   } // end namespace details
   /*
    * @brief Checks if the values of a vector are in a specific range (min[*] <= d[*] <= max[*]).
    * @details todo.
    * @param vec_data Data vector.
    * @param vec_min Minimum vector.
    * @param vec_max Maximum vector.
    * @return Vector mask type indicating whether the data is in the given range.
    */
   template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof = workaround >
   [[nodiscard]] 
   TVL_FORCE_INLINE typename Vec::mask_type between_inclusive(
      typename Vec::register_type  vec_data, 
      typename Vec::register_type  vec_min, 
      typename Vec::register_type  vec_max
   ) {
      return details::between_inclusive_impl< Vec, Idof >::apply(
         vec_data, vec_min, vec_max
      );
   }
   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_GENERATED_DECLARATIONS_COMPARE_HPP
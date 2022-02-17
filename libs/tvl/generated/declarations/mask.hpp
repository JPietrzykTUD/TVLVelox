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
 * @file lib/generated/declarations/mask.hpp
 * @date 17.02.2022
 * @brief Mask related primitives.
 */
#ifndef TUD_D2RG_TVL_LIB_GENERATED_DECLARATIONS_MASK_HPP
#define TUD_D2RG_TVL_LIB_GENERATED_DECLARATIONS_MASK_HPP


namespace tvl {
   namespace details {
      template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof >
      struct to_integral_impl{};
   } // end namespace details
   /*
    * @brief Forms an integral value from the most significant bits of every lane in a vector mask register.
    * @details todo.
    * @param vec_mask Vector mask register containing mask style data.
    * @return Integral value representing the vector mask register.
    */
   template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof = workaround >
   [[nodiscard]] 
   TVL_FORCE_INLINE typename Vec::base_type to_integral(
      typename Vec::mask_type  vec_mask
   ) {
      return details::to_integral_impl< Vec, Idof >::apply(
         vec_mask
      );
   }
   
   namespace details {
      template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof >
      struct get_msb_impl{};
   } // end namespace details
   /*
    * @brief Retrieves the most significant bit of every element in a vector register and sets the corresponding bit in a base type.
    * @details todo.
    * @param vec Vector mask register containing mask style data.
    * @return Integral value representing the vector mask register.
    */
   template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof = workaround >
   [[nodiscard]] 
   TVL_FORCE_INLINE typename Vec::base_type get_msb(
      typename Vec::register_type  vec
   ) {
      return details::get_msb_impl< Vec, Idof >::apply(
         vec
      );
   }
   
   namespace details {
      template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof >
      struct to_vector_impl{};
   } // end namespace details
   /*
    * @brief Forms an vector register from an integral where all bits are set in a lane if the corresponding mask bit is set to 1.
    * @details todo.
    * @param mask Vector mask register containing mask style data.
    * @return Integral value representing the vector mask register.
    */
   template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof = workaround >
   [[nodiscard]] 
   TVL_FORCE_INLINE typename Vec::register_type to_vector(
      typename Vec::mask_type  mask
   ) {
      return details::to_vector_impl< Vec, Idof >::apply(
         mask
      );
   }
   
   namespace details {
      template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof >
      struct mask_reduce_impl{};
   } // end namespace details
   /*
    * @brief Masks out every non relevant bit.
    * @details todo.
    * @param mask Integral value containing n (set) bits.
    * @return Integral value with only relevant bits set (or not).
    */
   template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof = workaround >
   [[nodiscard]] 
   TVL_FORCE_INLINE typename Vec::base_type mask_reduce(
      typename Vec::base_type  mask
   ) {
      return details::mask_reduce_impl< Vec, Idof >::apply(
         mask
      );
   }
   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_GENERATED_DECLARATIONS_MASK_HPP
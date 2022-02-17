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
 * @file lib/generated/declarations/calc.hpp
 * @date 17.02.2022
 * @brief Arithmetic primitives.
 */
#ifndef TUD_D2RG_TVL_LIB_GENERATED_DECLARATIONS_CALC_HPP
#define TUD_D2RG_TVL_LIB_GENERATED_DECLARATIONS_CALC_HPP

#include "immintrin.h"

namespace tvl {
   namespace details {
      template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof >
      struct add_impl{};
   } // end namespace details
   /*
    * @brief Adds two vector registers.
    * @details todo.
    * @param vec_a First vector.
    * @param vec_b Second vector.
    * @return Vector containing result of the addition.
    */
   template< VectorProcessingStyle Vec, ImplementationDegreeOfFreedom Idof = workaround >
   [[nodiscard]] 
   TVL_FORCE_INLINE typename Vec::register_type add(
      typename Vec::register_type  vec_a, 
      typename Vec::register_type  vec_b
   ) {
      return details::add_impl< Vec, Idof >::apply(
         vec_a, vec_b
      );
   }
   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_GENERATED_DECLARATIONS_CALC_HPP
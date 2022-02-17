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
 * @file lib/static/utils/type_concepts.hpp
 * @date 17.02.2022
 * @brief TODO.
 */
#ifndef TUD_D2RG_TVL_LIB_STATIC_UTILS_TYPE_CONCEPTS_HPP
#define TUD_D2RG_TVL_LIB_STATIC_UTILS_TYPE_CONCEPTS_HPP

#include "type_helper.hpp"

namespace tvl {
   template< typename T >
   concept Arithmetic = std::is_arithmetic_v< T >;

   template< typename T >
   concept Tuple = is_tuple< T >::value;

   
} // end of namespace tvl

#endif //TUD_D2RG_TVL_LIB_STATIC_UTILS_TYPE_CONCEPTS_HPP
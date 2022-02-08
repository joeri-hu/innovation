/**
 * @file class-traits.h
 * @brief Type traits for user-defined class types.
 * 
 * @version 1.0
 * @date December 2021
 * 
 * @authors INNO Project-group (Semester A+B, 2021)
 * @author Joeri Kok (joeri.j.kok@student.hu.nl)
 * @author Rick Horeman (rick.horeman@student.hu.nl)
 * @author Richard Janssen (richard.janssen@student.hu.nl)
 * @author Koen Eijkelenboom (koen.eijkelenboom@student.hu.nl)
 * @author Tim Hardeman (tim.hardeman@student.hu.nl)
 * 
 * @copyright GPL-3.0 License
 */
#ifndef CFG_CONFIG_TRAITS_CLASS_TRAITS_H
#define CFG_CONFIG_TRAITS_CLASS_TRAITS_H

#include "iterator-traits.h"

#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @remark Checks if a type qualifies as a data type.
 * 
 * @details A type is considered to be a data type if it has a standard layout and is
 * trivially copyable.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct is_data_type
    : std::conjunction<
        std::is_standard_layout<T>,
        std::is_trivially_copyable<T>> {};

/**
 * @var is_data_type_v
 * 
 * @brief Helper variable template for the @ref is_data_type type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_data_type_v
    = bool{is_data_type<T>{}};

/**
 * @remark Checks if a type qualifies as a unary type.
 * 
 * @details A type is considered to be a unary type if it can be created with a default
 * constructor as well as a copy constructor.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct is_unary_type
    : std::conjunction<
        std::is_default_constructible<T>,
        std::is_copy_constructible<T>> {};

/**
 * @var is_unary_type_v
 * 
 * @brief Helper variable template for the @ref is_unary_type type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_unary_type_v
    = bool{is_unary_type<T>{}};

/**
 * @remark Checks if a type qualifies as a contiguous container type.
 * 
 * @details A type is considered to be a contiguous container type if it qualifies as an
 * unary type and has a random access iterator.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct is_contiguous_container
    : std::conjunction<
        is_unary_type<T>,
        has_random_access_iter<T>> {};

/**
 * @var is_contiguous_container_v
 * 
 * @brief Helper variable template for the @ref is_contiguous_container type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_contiguous_container_v
    = bool{is_contiguous_container<T>{}};

} // namespace cfg

#endif

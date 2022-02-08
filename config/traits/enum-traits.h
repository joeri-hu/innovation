/**
 * @file enum-traits.h
 * @brief Type traits for user-defined enumeration types.
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
#ifndef CFG_CONFIG_TRAITS_ENUM_TRAITS_H
#define CFG_CONFIG_TRAITS_ENUM_TRAITS_H

#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @remark Checks if a type qualifies as a scoped enumeration type.
 * 
 * @details A type is considered to be a scoped enumeration type if it qualifies as an
 * enumeration type that cannot be implicitly converted to its underlying type.
 * 
 * @tparam T Type to check.
 * 
 * @{
 */
template<typename T, bool = std::is_enum_v<T>>
struct is_scoped_enum : std::false_type {};

template<typename T>
struct is_scoped_enum<T, true>
    : std::negation<std::is_convertible<T, std::underlying_type_t<T>>> {};
/** @} */

/**
 * @var is_scoped_enum_v
 * 
 * @brief Helper variable template for the @ref is_scoped_enum type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_scoped_enum_v
    = bool{is_scoped_enum<T>{}};

/**
 * @remark Checks if a type qualifies as a bitwise enumeration type.
 * 
 * @details Bitwise enumeration types are scoped enumeration types that are suitable for
 * performing bitwise operations on. A type is considered to be a bitwise enumeration
 * type if it qualifies as a scoped enumeration type of which its underlying type is an
 * unsigned integral type.
 * 
 * @tparam T Type to check.
 * 
 * @{
 */
template<typename T, bool = is_scoped_enum_v<T>>
struct is_bitwise_enum : std::false_type {};

template<typename T>
struct is_bitwise_enum<T, true>
    : std::is_unsigned<std::underlying_type_t<T>> {};
/** @} */

/**
 * @var is_bitwise_enum_v
 * 
 * @brief Helper variable template for the @ref is_bitwise_enum type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_bitwise_enum_v
    = bool{is_bitwise_enum<T>{}};

} // namespace cfg

#endif

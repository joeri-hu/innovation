/**
 * @file type-traits.h
 * @brief General type traits.
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
#ifndef CFG_CONFIG_TRAITS_TYPE_TRAITS_H
#define CFG_CONFIG_TRAITS_TYPE_TRAITS_H

#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @remark Checks if a type matches any type from a given set of types.
 * 
 * @tparam T Type to check.
 * @tparam Ts Set of types to check against.
 */
template<typename T, typename... Ts>
struct is_same_any
    : std::disjunction<std::is_same<T, Ts>...> {};

/**
 * @var is_same_any_v
 * 
 * @brief Helper variable template for the @ref is_same_any_v type trait.
 * 
 * @tparam T Type to check.
 * @tparam Ts Set of types to check against.
 */
template<typename T, typename... Ts>
inline constexpr auto is_same_any_v
    = bool{is_same_any<T, Ts...>{}};

/**
 * @remark Checks if a type matches all types from an arbitrary set of types.
 * 
 * @tparam T Type to check.
 * @tparam Ts Set of types to check against.
 */
template<typename T, typename... Ts>
struct is_same_all
    : std::conjunction<std::is_same<T, Ts>...> {};

/**
 * @var is_same_all_v
 * 
 * @brief Helper variable template for the @ref is_same_all_v type trait.
 * 
 * @tparam T Type to check.
 * @tparam Ts Set of types to check against.
 */
template<typename T, typename... Ts>
inline constexpr auto is_same_all_v
    = bool{is_same_all<T, Ts...>{}};

} // namespace cfg

#endif

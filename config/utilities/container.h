/**
 * @file container.h
 * @brief Container related utility functions/variables/aliases.
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
#ifndef CFG_CONFIG_UTILITIES_CONTAINER_H
#define CFG_CONFIG_UTILITIES_CONTAINER_H

#include <array>
#include <cstddef>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @typedef array
 * 
 * @brief Helper alias template for the std::array container.
 * 
 * @details Allows for creating and referring to std::array containers when dealing with
 * integral types that are signed.
 * 
 * @tparam T Type of the elements.
 * @tparam N Number of elements.
 */
template<typename T, auto N,
    typename = std::enable_if_t<(N >= 0)>,
    typename = std::enable_if_t<std::is_same_v<
        std::common_type_t<decltype(N), std::size_t>, std::size_t>>>
using array = std::array<T, static_cast<std::size_t>(N)>;

} // namespace cfg

#endif

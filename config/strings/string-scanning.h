/**
 * @file string-scanning.h
 * @brief Helper functions for scanning through strings, comparing strings, and other
 * common string operations.
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
#ifndef CFG_CONFIG_STRINGS_STRING_SCANNING_H
#define CFG_CONFIG_STRINGS_STRING_SCANNING_H

#include <algorithm>
#include <cctype>
#include <string_view>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Checks if a string contains one or more special characters.
 * 
 * @details Any character not within range [0-9][a-z][A-Z], is considered to be a
 * special character. The characters specified by the 'exceptions' parameter are exempted
 * from this rule.
 * 
 * @param[in] source String to check for special characters.
 * @param[in] exceptions Characters that are not considered to be special. The default
 * value of this parameter is: "()-_".
 */
[[nodiscard]]
inline auto contains_special_character(
    std::string_view source,
    std::string_view exceptions = "()-_"
) -> bool {
    return not std::all_of(source.cbegin(), source.cend(),
        [&](unsigned char character) -> bool {
            return std::isalnum(character)
                or exceptions.find_first_of(character)
                    != std::string_view::npos;
        }
    );
}

} // namespace cfg

#endif

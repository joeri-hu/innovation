/**
 * @file string-conversions.h
 * @brief Helper functions for converting strings to arithmetic types.
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
#ifndef CFG_CONFIG_STRINGS_STRING_CONVERSIONS_H
#define CFG_CONFIG_STRINGS_STRING_CONVERSIONS_H

#include "zstring-view.h"

#include <traits/iterator-traits.h>

#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Converts a range of elements to a string of characters, formatted in
 * hexadecimal notation.
 * 
 * @detail Each element within the source container requires two elements of storage in
 * the destination container. An element is split into two parts and each part is
 * converted to a single hexadecimal digit.
 * 
 * @tparam InputIt Input-iterator type of the source container.
 * @tparam OutputIt Output-iterator type of the destination container.
 * 
 * @param[in] first Iterator to the beginning of the source range.
 * @param[in] last Iterator to the end of the source range.
 * @param[out] first_dest Iterator to the beginning of the destination range.
 * 
 * @return Iterator to one past the last converted element of the destination range.
 */
template<typename InputIt, typename OutputIt,
    typename = std::enable_if_t<is_input_iter_v<InputIt>>,
    typename = std::enable_if_t<is_iterator_v<OutputIt>>>
constexpr auto convert_to_hex(InputIt first, InputIt last, OutputIt first_dest)
-> OutputIt {
    auto const map = "0123456789abcdef";
    auto const mask = 0b1111'1111u;

    while (first != last) {
        *first_dest++ = map[(*first & mask) >> 4];
        *first_dest++ = map[*first++ & (mask >> 4)];
    }
    return first_dest;
}

/**
 * @brief Converts all elements of a contaienr to a string of characters, formatted in
 * hexadecimal notation.
 * 
 * @details This function acts as a helper-function of the @ref convert_hex function that
 * operates on iterators. It takes the required iterators from the source and destination
 * containers and passes them along.
 * 
 * @tparam InputCont Type of the source container.
 * @tparam OutputCont Type of the destination container.
 * 
 * @param[in] source Source container to convert the elements from.
 * @param[out] dest Destination container to convert the elements to.
 * 
 * @return Iterator to one past the last converted element of the destination container.
 */
template<typename InputCont, typename OutputCont,
    typename = std::enable_if_t<has_input_iter_v<InputCont>>,
    typename = std::enable_if_t<has_iterator_v<OutputCont>>>
constexpr auto convert_to_hex(InputCont const& source, OutputCont& dest) {
    using std::begin, std::end;
    return convert_to_hex(begin(source), end(source), begin(dest));
}

/**
 * @brief Converts the entire string-value from a container or view to an arithmetic
 * value.
 * 
 * @details Refer to the documentation of std::from_chars on cppreference.com for more
 * details.
 * 
 * @tparam Container Type of the container or view to convert its value from.
 * @tparam T Type of the arithmetic value to which the string-value is converted.
 * 
 * @param[in] input Reference to a container or view containing the string-value.
 * @param[out] value Stores the converted value, if the parsing was successful.
 * @param[in] base Base of the converted value, which should be between 2 and 36.
 * 
 * @return A std::from_chars_result object, which contains information about the
 * conversion.
 */
template<typename Container, typename T,
    typename = std::enable_if_t<has_random_access_iter_v<Container>>,
    typename = std::enable_if_t<std::is_arithmetic_v<T>>>
[[nodiscard]]
auto from_chars(Container const& input, T& value, int base = 10)
-> std::from_chars_result {
    using std::data, std::size;
    return std::from_chars(data(input), data(input) + size(input), value, base);
}

/**
 * @brief Converts a character to a boolean value.
 * 
 * @param[in] character Character to convert.
 * 
 * @return Only returns true if the character is equal to '1'.
 */
[[nodiscard]]
constexpr auto bool_from_char(char character) -> bool
{ return character == '1'; }

/**
 * @brief Converts a zero-terminated string to a boolean value.
 * 
 * @param[in] zstring Zero-terminated string to convert.
 * 
 * @return Only returns true if the first character is equal to '1'. The other characters
 * are simply ignored.
 */
[[nodiscard]]
constexpr auto bool_from_zstring(zstring_view zstring) -> bool
{ return bool_from_char(zstring.front()); }

/**
 * @brief Converts a zero-terminated string to a 32-bit signed integer value.
 * 
 * @param[in] zstring Zero-terminated string to convert.
 * 
 * @return The decimal equivalent of the converted string. If the string could not be
 * converted correctly, the return value is zero.
 */
[[nodiscard]]
inline auto int32_from_zstring(zstring_view zstring) -> std::int32_t
{ return std::int32_t{std::strtol(zstring.data(), nullptr, 10)}; }

/**
 * @brief Converts a zero-terminated string to a 32-bit unsigned integer value.
 * 
 * @param[in] zstring Zero-terminated string to convert.
 * 
 * @return The decimal equivalent of the converted string. If the string could not be
 * converted correctly, the return value is zero.
 */
[[nodiscard]]
inline auto uint32_from_zstring(zstring_view zstring) -> std::uint32_t
{ return std::uint32_t{std::strtoul(zstring.data(), nullptr, 10)}; }

/**
 * @brief Converts a zero-terminated string to a boolean value.
 * 
 * @param[in] value Zero-terminated string to convert.
 * @param[out] result Receives a value of true if the first character is equal to '1'.
 * The other characters are simply ignored.
 */
constexpr auto convert(zstring_view value, bool& result) -> void
{ result = bool_from_zstring(value); }

/**
 * @brief Converts a zero-terminated string to a 32-bit signed integer value.
 * 
 * @param[in] value Zero-terminated string to convert.
 * @param[out] result Receives the decimal equivalent of the converted string. If the
 * string could not be converted correctly, the result is zero.
 */
inline auto convert(zstring_view value, std::int32_t& result) -> void
{ result = int32_from_zstring(value); }

/**
 * @brief Converts a zero-terminated string to a 32-bit unsigned integer value.
 * 
 * @param[in] value Zero-terminated string to convert.
 * @param[out] result Receives the decimal equivalent of the converted string. If the
 * string could not be converted correctly, the result is zero.
 */
inline auto convert(zstring_view value, std::uint32_t& result) -> void
{ result = uint32_from_zstring(value); }

} // namespace cfg

#endif

/**
 * @file validation-rules.h
 * @brief Rules used for validating values.
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
#ifndef CFG_CONFIG_CHECKING_VALIDATION_RULES_H
#define CFG_CONFIG_CHECKING_VALIDATION_RULES_H

#include <errors/error-types.h>
#include <strings/string-conversions.h>
#include <strings/string-scanning.h>
#include <strings/zstring-view.h>
#include <traits/enum-traits.h>
#include <utilities/enum.h>

#include <cctype>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <optional>
#include <string_view>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @struct validate_result
 * 
 * @brief Data type that is returned by the @ref validate_value function.
 * 
 * @details A validation result consists of two data-members. Optional data and an
 * optional validation error. Depending on the outcome of the @ref validate_value
 * function, either optional data-member could be empty.
 * 
 * @tparam T Type of the validated data.
 */
template<typename T>
struct validate_result {
    /**
     * @brief Implicitly converts a validation result to another validation result with
     * differently typed validated data.
     */
    template<typename U>
    [[nodiscard]]
    constexpr operator validate_result<U>() const
    { return {std::optional<U>{data}, error}; }

    /**
     * @brief Compares two validation results for (in)equality.
     * 
     * @param[in] lhs Validation result on the left-hand side of the operator.
     * @param[in] rhs Validation result on the right-hand side of the operator.
     * 
     * @return Two validation results are considered to be equal when both their optional
     * data and optional error match.
     * 
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(
        validate_result const& lhs, validate_result const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(
        validate_result const& lhs, validate_result const& rhs) -> bool
    { return lhs.data == rhs.data and lhs.error == rhs.error; }
    /** @} */

    std::optional<T> data;                 /**< Optional validated data. */
    std::optional<validation_error> error; /**< Optional validation error. */
};

/**
 * @brief Validates the range of an arithmetic value.
 * 
 * @details If the value is within the range specified by the @p min and @p max
 * thresholds, the validation is successful and no error is returned. Boolean values are
 * excluded from the range check.
 * 
 * @tparam T The arithmetic type of the value to validate.
 * 
 * @param[in] value View to a string that possibly contains an arithmetic value.
 * @param[in] min Threshold for the minimum value of the converted result.
 * @param[in] max Threshold for the maximum value of the converted result.
 * 
 * @return A @link validate_result validation result @endlink of which its data member
 * contains the validated value. If the validation was successfull, the optional error is
 * empty. Otherwise, it contains a validation error.
 */
template<typename T,
    typename = std::enable_if_t<std::is_arithmetic_v<T>>>
[[nodiscard]]
constexpr auto validate_value_range(
    T value,
    [[maybe_unused]] T min = std::numeric_limits<T>::min(),
    [[maybe_unused]] T max = std::numeric_limits<T>::max()
) -> validate_result<T> {
    if constexpr (not std::is_same_v<T, bool>) {
        if (value < min) return {value, validation_error::below_min_threshold};
        if (value > max) return {value, validation_error::above_max_threshold};
    }
    return {value, std::nullopt};
}

/**
 * @brief Validates the arithmetic value of a given string-view.
 * 
 * @details If the value of the string-view contains a number that fits within the range
 * specified by the @p min and @p max thresholds, the validation is successful and no
 * error is returned. When validating a boolean value, the @p min and @p max arguments
 * are ignored.
 * 
 * @tparam T Type which indicates how the string-view value should be interpreted. This
 * type is required to be an arithmetic type.
 * 
 * @param[in] value View to a string that possibly represents an arithmetic value.
 * @param[in] min Threshold for the minimum value of the converted result.
 * @param[in] max Threshold for the maximum value of the converted result.
 * 
 * @return A @link validate_result validation result @endlink that consists of optional
 * data and an optional validation error. If the validation was successfull, the optional
 * error is empty and the optional data contains a value. Otherwise, a validation error
 * is returned and data only contains a value when the conversion was successful.
 */
template<typename T,
    typename = std::enable_if_t<std::is_arithmetic_v<T>>>
[[nodiscard]]
constexpr auto validate_value(
    std::string_view value,
    [[maybe_unused]] T min = std::numeric_limits<T>::min(),
    [[maybe_unused]] T max = std::numeric_limits<T>::max()
) -> validate_result<T> {
    if (value.empty()) return {std::nullopt, validation_error::missing_value};

    auto const is_type_bool = std::is_same_v<T, bool>;
    auto result = std::conditional_t<is_type_bool, std::uint8_t, T>{};
    auto [ptr, error] = from_chars(value, result);

    if (error == std::errc::invalid_argument)
        return {std::nullopt, validation_error::contains_invalid_character};
    if (error == std::errc::result_out_of_range)
        return {std::nullopt, validation_error::out_of_type_range};

    if constexpr (is_type_bool) {
        if (result > 1) return {std::nullopt, validation_error::out_of_type_range};
    }
    return validate_value_range<T>(result, min, max);
}

/**
 * @brief Validates the name of a string.
 *  
 * @details If the name is not empty and contains no special characters, the validation
 * is successful and no error is returned.
 * 
 * @param[in] name String that contains a name.
 * 
 * @return An optional validation error.
 */
[[nodiscard]]
inline auto validate_name(std::string_view name)
-> validate_result<std::string_view> {
    if (name.empty())
        return {std::nullopt, validation_error::missing_value};
    if (contains_special_character(name))
        return {std::nullopt, validation_error::contains_invalid_character};

    return {name, std::nullopt};
}

/**
 * @brief Validates a USB-detection option.
 *
 * @details If the option is not empty and matches one of the possible USB-detection
 * modes, the validation is successful and no error is returned.
 *
 * @param[in] option String that contains an USB option.
 *
 * @return An optional validation error.
 */
template<typename UsbDetect,
    typename = std::enable_if_t<is_scoped_enum_v<UsbDetect>>>
[[nodiscard]]
constexpr auto validate_usb(std::string_view option)
-> validate_result<std::int32_t> {
    if (option.empty())       return {std::nullopt, validation_error::missing_value};
    if (option == "on")       return {to_underlying(UsbDetect::ON), std::nullopt};
    if (option == "interval") return {to_underlying(UsbDetect::INTERVAL), std::nullopt};
    if (option == "off")      return {to_underlying(UsbDetect::OFF), std::nullopt};

    return {std::nullopt, validation_error::invalid_option};
}

/**
 * @deprecated This function has been deprecated, use @ref validate_value instead.
 * 
 * @brief Validates the boolean value of a string.
 * 
 * @details If the string-value contains '0' or '1', the validation is successful and no
 * error is returned.
 * 
 * @param[in] value String that contains a possible boolean value.
 * 
 * @return An optional validation error.
 */
[[deprecated, nodiscard]]
constexpr auto validate_bool(zstring_view value)
-> std::optional<validation_error> {
    if (value.empty() or value.front() == '\0')
        return validation_error::missing_value;
    if (value[1] != '\0')
        return validation_error::exceeds_max_length;
    if (not std::isdigit(static_cast<unsigned char>(value.front())))
        return value.front() == '-'
            ? validation_error::negative_value
            : validation_error::contains_invalid_character;
    if (value.front() != '0' and value.front() != '1')
        return validation_error::above_type_range;

    return std::nullopt;
}

/**
 * @deprecated This function has been deprecated, use @ref validate_value instead.
 * 
 * @brief Validates the unsigned 32-bit integer value of a string.
 * 
 * @details If the string-value contains a number that fits within the range specified by
 * the @p min and @p max arguments, the validation is successful and no error is
 * returned.
 * 
 * @param[in] value String that contains a possible unsigned integer value.
 * @param[in] min Threshold for the minimum value of the converted result.
 * @param[in] max Threshold for the maximum value of the converted result.
 * 
 * @return An optional validation error.
 */
[[deprecated, nodiscard]]
inline auto validate_uint32(
    zstring_view value,
    std::uint32_t min = 0,
    std::uint32_t max = 99'999'999
) -> std::optional<validation_error> {
    if (value.empty()) return validation_error::missing_value;

    // note: errno is a macro defined in <cerrno>
    errno = 0;
    char* value_end;
    auto const result = std::uint32_t{std::strtoul(value.data(), &value_end, 10)};

    if (value_end == value.data() or *value_end != '\0')
        return validation_error::contains_invalid_character;
    if (errno == ERANGE)
        return validation_error::above_type_range;

    if (result < min) return validation_error::below_min_threshold;
    if (result > max) return validation_error::above_max_threshold;

    return std::nullopt;
}

/**
 * @deprecated This function has been deprecated, use @ref validate_value instead.
 * 
 * @brief Validates the signed 32-bit integer value of a string.
 * 
 * @details If the string-value contains a number that fits within the range specified by
 * the @p min and @p max arguments, the validation is successful and no error is
 * returned.
 * 
 * @param[in] value String that contains a possible signed integer value.
 * @param[in] min Threshold for the minimum value of the converted result.
 * @param[in] max Threshold for the maximum value of the converted result.
 * 
 * @return An optional validation error.
 */
[[deprecated, nodiscard]]
inline auto validate_int32(
    zstring_view value,
    std::int32_t min = -99'999'999,
    std::int32_t max = +99'999'999
) -> std::optional<validation_error> {
    if (value.empty()) return validation_error::missing_value;

    // note: errno is a macro defined in <cerrno>
    errno = 0;
    char* value_end;
    auto const result = std::int32_t{std::strtol(value.data(), &value_end, 10)};

    if (value_end == value.data() or *value_end != '\0')
        return validation_error::contains_invalid_character;
    if (errno == ERANGE)
        return result < 0
            ? validation_error::below_type_range
            : validation_error::above_type_range;

    if (result < min) return validation_error::below_min_threshold;
    if (result > max) return validation_error::above_max_threshold;

    return std::nullopt;
}

} // namespace cfg

#endif

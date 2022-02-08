/**
 * @file setting-validators.h
 * @brief Helper functions that dispatch calls to validation functions.
 * 
 * @version 1.0
 * @date January 2022
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
#ifndef CFG_CONFIG_SETTINGS_SETTING_VALIDATORS_H
#define CFG_CONFIG_SETTINGS_SETTING_VALIDATORS_H

#include "setting.h"

#include <checking/validation-mode.h>
#include <checking/validators.h>

#include <string_view>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Dispatches a call to a @link validators.h validation function @endlink while
 * providing a uniform call-signature.
 * 
 * @details A different call-signature is wrapped around a @link validators.h validation
 * function @endlink and its return value is converted to a @link validate_result
 * validation result @endlink that stores a @ref setting_data object. The excess
 * arguments are simply discarded. This function allows @ref setting objects to be of the
 * same type while using differently typed validators and validation results.
 * 
 * @tparam Validator Reference to a @link validators.h validation function @endlink that
 * takes a string-view argument and returns a type that can be implicitly converted to a
 * @link validate_result validation result @endlink that stores a @ref setting_data
 * object.
 * @tparam Ts Arbitrary number of optional argument types.
 * 
 * @param[in] value String-value to validate.
 * 
 * @return @link validate_result Validation result @endlink that stores a @ref
 * setting_data object.
 */
template<auto (&Validator)(std::string_view), typename... Ts>
[[nodiscard]]
constexpr auto dispatch_validator(std::string_view value, validation_mode, Ts...)
    -> validate_result<setting_data>
{ return Validator(value); }

/**
 * @brief Dispatches a call to one of the @link validators.h validation functions
 * @endlink while providing a uniform call-signature.
 * 
 * @details Which validation function is invoked depends on the given validation mode.
 * 
 * @tparam FileValidator Reference to a @link validators.h validation function @endlink
 * with a string-view argument that is invoked when validating a config file.
 * @tparam MsgValidator Reference to a @link validators.h validation function @endlink
 * with a string-view argument that is invoked when validating a config message.
 * @tparam Ts Arbitrary number of optional argument types.
 * 
 * @param[in] value Data-value to validate.
 * @param[in] mode Indicates which validation mode is in effect.
 * 
 * @return @link validate_result Validation result @endlink that stores a @ref
 * setting_data object.
 */
template<
    auto (&FileValidator)(std::string_view),
    auto (&MsgValidator)(std::string_view),
    typename... Ts>
[[nodiscard]]
constexpr auto dispatch_validator(std::string_view value, validation_mode mode, Ts...)
-> validate_result<setting_data> {
    return invoke_validator(mode,
        [&]{ return FileValidator(value); },
        [&]{ return MsgValidator(value); });
}

/**
 * @brief Dispatches a call to a @ref validate_value function while providing a uniform
 * call-signature.
 * 
 * @details This function allows @ref setting objects to be of the same type while using
 * differently typed @ref validate_value functions. The return value is converted to a
 * @link validate_result validation result @endlink that stores a @ref setting_data
 * object.
 * 
 * Which @link validators.h validation function @endlink will be invoked depends on the
 * given validation mode. When validating the value of a config message, the underlying
 * bits will first be converted to an integral value that matches the instantiated type
 * of this function.
 * 
 * @tparam T Integral type of the @p MinMax arguments passed to a @ref validate_value
 * function.
 * @tparam MinMax Optional thresholds for the minimum and maximum value of the converted
 * result. The default values are defined by the @ref validate_value @ref or
 * validate_value_range function.
 * 
 * @param[in] value Data-value to validate.
 * 
 * @return @link validate_result Validation result @endlink that stores a @ref
 * setting_data object.
 */
template<typename T, std::enable_if_t<std::is_integral_v<T>, T>... MinMax>
[[nodiscard]]
constexpr auto validate(std::string_view value, validation_mode mode)
-> validate_result<setting_data> {
    return invoke_validator(mode,
        [&]{ return validate_value<T>(value, MinMax...); },
        [&]{ return validate_value_range<T>(convert_bits<T>(value), MinMax...); });
}

} // namespace cfg

#endif

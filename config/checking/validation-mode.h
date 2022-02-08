/**
 * @file validation-mode.h
 * @brief Validation modes used for controlling the behavior of validation mechanisms.
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
#ifndef CFG_CONFIG_CHECKING_VALIDATION_MODE_H
#define CFG_CONFIG_CHECKING_VALIDATION_MODE_H

#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @enum validation_mode
 * 
 * @brief Enumeration of the validation modes.
 * 
 * @details Values obtained from a config file require to be validated differently than
 * values obtained from a config message. This enumeration provides a way to make the
 * distinction.
 */
enum class validation_mode {
    config_file,    /**< Indicates to perform validation on config file values. */
    config_message, /**< Indicates to perform validation on config message values. */
};

/**
 * @brief Invokes one of the given validators based on the given validation mode.
 * 
 * @details This function is used to map a validator to a validation mode. If the given
 * validation mode is not recognized, the validator for config file values is used.
 * 
 * @tparam FileFunc Callable type used for validating config file values.
 * @tparam MsgFunc Callable type used for validating config message values.
 * 
 * @param[in] mode Indicates which validation mode is in effect.
 * @param[in] file_action Action to invoke when validating config file values.
 * @param[in] message_action Action to invoke when validating config message values.
 * 
 * @return Matches the return value of either validator.
 */
template<typename FileFunc, typename MsgFunc,
    typename = std::enable_if_t<std::is_invocable_v<FileFunc>>,
    typename = std::enable_if_t<std::is_invocable_v<MsgFunc>>>
constexpr auto invoke_validator(
    validation_mode mode,
    FileFunc const& file_action,
    MsgFunc const& message_action
) -> decltype(auto) {
    switch (mode) {
    case validation_mode::config_file:    return file_action();
    case validation_mode::config_message: return message_action();
    default: return file_action();
    }
}

} // namespace cfg

#endif

/**
 * @file error-types.h
 * @brief Types of all the different errors.
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
#ifndef CFG_CONFIG_ERRORS_ERROR_TYPES_H
#define CFG_CONFIG_ERRORS_ERROR_TYPES_H

#include <ff.h>

#include <cstdint>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @namespace error
 * 
 * @brief Contains everything related to errors.
 */
namespace error {

/**
 * @enum category
 * 
 * @brief Enumeration of the error categories.
 */
enum class category : std::uint32_t {
    unspecified, /**< Default error category. */
    parsing,     /**< Indicates an error occurred during the parsing phase. */
    validation,  /**< Indicates an error occurred during the validation phase. */
    verification /**< Indicates an error occurred during the verification phase. */
};

/**
 * @namespace error
 * 
 * @brief Contains all the different error types.
 */
namespace type {

/**
 * @enum parsing
 * 
 * @brief Enumeration of the parsing error identifiers.
 */
enum class parsing : std::uint32_t {
    unspecified,              /**< Default parsing error. */
    missing_opening_tag,      /**< Indicates an opening tag is missing. */
    missing_closing_tag,      /**< Indicates a closing tag is missing. */
    exceeds_max_value_length, /**< Indicates the value within a tag is too long. */
    empty_config,             /**< Indicates the config file is empty. */
    no_tags_found,            /**< Indicates the config file contains no tags. */
    invalid_message_pointer,  /**< Indicates the config message pointer is invalid. */
    insufficient_message_size /**< Indicates the config message buffer is too small. */
};

/**
 * @enum validation
 * 
 * @brief Enumeration of the validation error identifiers.
 */
enum class validation : std::uint32_t {
    unspecified,                /**< Default validation error. */
    setting_unset,              /**< Indicates the value of a setting was never set. */
    contains_invalid_character, /**< Indicates a value contains an invalid character. */
    missing_value,              /**< Indicates a value is empty. */
    negative_value,             /**< Indicates a value is a negative number. */
    exceeds_max_length,         /**< Indicates a value is too long. */
    out_of_type_range,          /**< Indicates a value will cause overflow. */
    below_type_range,           /**< Indicates a value will cause negative overflow. */
    above_type_range,           /**< Indicates a value will cause positive overflow. */
    below_min_threshold,        /**< Indicates a value falls below its threshold. */
    above_max_threshold,        /**< Indicates a value exceeds its threshold. */
    invalid_option              /**< Indicates a value was not a valid option */
};

/**
 * @enum verification
 *
 * @brief Enumeration of the verification error identifiers.
 */
enum class verification : std::uint32_t {
    unspecified,                /**< Default verification error. */
    no_trigger_enabled,         /**< Indicates that no trigger has been enabled. */
    no_data_destination_enabled /**< Indicates no data-destination has been enabled. */
};

/**
 * @enum Input/Output
 * 
 * @brief Enumeration of the I/O errors.
 */
enum class io {
    file_not_found = FR_NO_FILE,      /**< Indicates the file could not be found. */
    path_not_found = FR_NO_PATH,      /**< Indicates the path could not be found. */
    invalid_name   = FR_INVALID_NAME, /**< Indicates the filename is invalid. */
    file_too_large                    /**< Indicates the file is too large. */
};

} // namespace type
} // namespace error

/**
 * @typedef parsing_error
 * 
 * @brief Shorter notation to refer to the type of a parsing error.
 */
using parsing_error = error::type::parsing;

/**
 * @typedef validation_error
 * 
 * @brief Shorter notation to refer to the type of a validation error.
 */
using validation_error = error::type::validation;

/**
 * @typedef verification_error
 *
 * @brief Shorter notation to refer to the type of a verification error.
 */
using verification_error = error::type::verification;

/**
 * @typedef io_error
 * 
 * @brief Shorter notation to refer to the I/O error.
 */
using io_error = error::type::io;

} // namespace cfg

#endif

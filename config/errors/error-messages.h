/**
 * @file error-messages.h
 * @brief Error messages linked to the error types.
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
#ifndef CFG_CONFIG_ERRORS_ERROR_MESSAGES_H
#define CFG_CONFIG_ERRORS_ERROR_MESSAGES_H

#include "error-types.h"

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Gets the error message that is mapped to an I/O error.
 * 
 * @param[in] error_id Identifier of the @ref io_error.
 * 
 * @return Pointer to a literal string.
 */
constexpr auto get_error_message(io_error error_id) {
    switch (error_id) {
    case io_error::file_not_found:   return "file could not be found";
    case io_error::path_not_found:   return "path could not be found";
    case io_error::invalid_name:     return "file path format is invalid";
    case io_error::file_too_large:   return "file is too large";
    default:                         return "unknown file I/O error";
    }
}

} // namespace cfg

#endif

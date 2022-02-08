/**
 * @file error-handler.h
 * @brief Component for storing and reporting errors.
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
#ifndef CFG_CONFIG_ERRORS_ERROR_HANDLER_H
#define CFG_CONFIG_ERRORS_ERROR_HANDLER_H

#include "error-code.h"
#include "error-types.h"

#include <checking/verification-identifiers.h>
#include <parsing/file-pointer.h>
#include <settings/setting-identifiers.h>
#include <strings/zstring-view.h>
#include <utilities/container.h>
#include <utilities/enum.h>
#include <traits/enum-traits.h>

// warning: implicitly includes 'all.h'
#include <logger.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @class error_handler
 * 
 * @brief Stores and logs error codes.
 * 
 * @tparam MaxErrors Maximum number of errors an error-handler can store.
 */
template<int MaxErrors,
    typename = std::enable_if_t<(MaxErrors > 0)>>
class error_handler {
    /**
     * @typedef container
     * 
     * @brief Shorter notation to refer to the type of the error-buffer.
     */
    using container = array<error::code, MaxErrors>;

    /**
     * @typedef iterator
     * 
     * @brief Shorter notation to refer to the error-buffer's iterator type.
     */
    using iterator = typename container::iterator;

    /**
     * @typedef const_iterator
     * 
     * @brief Shorter notation to refer to the error-buffer's const iterator type.
     */
    using const_iterator = typename container::const_iterator;

public:
    /**
     * @brief Adds a parsing error with a given data value.
     * 
     * @tparam T Integral type of the data-segment of the error-code.
     * 
     * @param[in] error_id Identifier of the parsing error.
     * @param[in] data Contents of the data-segment of the error-code, only the 24 least
     * significant bits are used. The default data-value is set to zero.
     */
    template<typename T = std::int_fast32_t,
        typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr auto add_error(parsing_error error_id, T data = 0) -> void
    { add_error({error_id, data}); }

    /**
     * @brief Adds a parsing error, with file pointer data, to the internal error-buffer.
     * 
     * @details The column and line number of the file pointer are mapped to the 12
     * higher and lower order bits of the error data segment, respectively.
     * 
     * @param[in] error_id Identifier of the parsing error.
     * @param[in] pos Position of the file pointer.
     */
    constexpr auto add_error(parsing_error error_id, file_ptr pos) -> void {
        auto error_code = error::code{error_id};
        error_code.data.high = pos.column();
        error_code.data.low  = pos.line();
        add_error(error_code);
    }

    /**
     * @brief Adds a parsing error with user-defined data to the internal error-buffer.
     * 
     * @details The characters are mapped to the corresponding bytes of the error data
     * segment. Starting at the lower order bits.
     * 
     * @param[in] error_id Identifier of the parsing error.
     * @param[in] char1 First character to use as part of the error data segment.
     * @param[in] char2 Second character to use as part of the error data segment.
     * @param[in] char3 Third character to use as part of the error data segment.
     */
    constexpr auto add_error(
        parsing_error error_id,
        char char1, char char2, char char3
    ) -> void {
        auto error_code = error::code{error_id};
        error_code.data.byte1 = char1;
        error_code.data.byte2 = char2;
        error_code.data.byte3 = char3;
        add_error(error_code);
    }

    /**
     * @brief Adds an error type with some identifier to the internal error-buffer.
     * 
     * @tparam ErrorType Scoped enumeration type of the error code.
     * @tparam Identifier Scoped enumeration type of the identifier.
     * 
     * @param[in] error Identifier of the validation error.
     * @param[in] id Identifier of a scoped enumeration.
     */
    template<typename ErrorType, typename Identifier,
        typename = std::enable_if_t<is_bitwise_enum_v<ErrorType>>,
        typename = std::enable_if_t<is_scoped_enum_v<Identifier>>>
    constexpr auto add_error(ErrorType error, Identifier id) -> void
    { add_error({error, to_underlying(id)}); }

    /**
     * @brief Adds an error code to the internal error-buffer.
     * 
     * @details If the error-buffer is full, the error code that was added last will be
     * overwritten.
     * 
     * @param[in] error_code Error code to add.
     */
    constexpr auto add_error(error::code error_code) -> void {
        *top_error = error_code;
        if (is_error_limit_reached()) return;
        ++top_error;
    }

    /**
     * @brief Logs all of the errors stored within the internal error-buffer.
     * 
     * @details Error codes are formatted in a hexadecimal notation.
     * 
     * @param[in] error_msg Optional error message that precedes the error codes.
     */
    auto log_errors(zstring_view error_msg) const -> void {
        if (is_error_limit_reached()) return;
        if (not error_msg.empty()) {
            aether_log << error_msg.data();
        }
        auto offset = 0;
        std::array<char, 16u * MaxErrors> message;
        std::for_each(errors.begin(), const_iterator{top_error},
            [&](auto const error_code) {
                auto const chars = std::sprintf(
                    message.data() + offset, "  %#08lX\n", error_code.value());
                offset += (chars < 0) ? 0 : chars;
            }
        );
        aether_log << message.data();
    }

    /**
     * @brief Clears the internal error-buffer.
     */
    constexpr auto clear_errors() -> void
    { top_error = errors.begin(); }

    /**
     * @brief Checks if the internal error-buffer is not empty.
     */
    [[nodiscard]]
    constexpr auto contains_errors() const -> bool
    { return top_error != errors.cbegin(); }

    /**
     * @brief Checks if the internal error-buffer is full.
     */
    [[nodiscard]]
    constexpr auto is_error_limit_reached() const -> bool
    { return top_error == errors.cend(); }

    /**
     * @brief Gets the maximum number of errors the internal error-buffer can store.
     * 
     * @return Maximum number of errors that can be stored.
     */
    [[nodiscard]]
    constexpr auto error_capacity() const -> int
    { return MaxErrors; }

    /**
     * @brief Gets the number of errors currently stored in the internal error-buffer.
     * 
     * @return Number of errors that are currently stored.
     */
    [[nodiscard]]
    constexpr auto error_count() const -> int
    { return std::distance(errors.cbegin(), const_iterator{top_error}); }

private:
    container errors{};                 /**< Stores the error codes. */
    iterator top_error{errors.begin()}; /**< Keeps track of the error codes. */
};

} // namespace cfg

#endif

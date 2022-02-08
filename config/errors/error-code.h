/**
 * @file error-code.h
 * @brief Error code that stores error information.
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
#ifndef CFG_CONFIG_ERRORS_ERROR_CODE_H
#define CFG_CONFIG_ERRORS_ERROR_CODE_H

#include "error-types.h"

#include <traits/enum-traits.h>
#include <traits/type-traits.h>
#include <utilities/bitwise.h>
#include <utilities/enum.h>

#include <cstdint>
#include <type_traits>

/**
 * @namespace cfg::error
 * 
 * @brief Contains everything related to errors, within the configuration namespace.
 */
namespace cfg::error {

/**
 * @brief Constants that describe various specifications of the error code segments.
 * 
 * @details These values are used for performing bit manipulations on an error code.
 * 
 * @{
 * 
 * @namespace segment_size
 * 
 * @brief Contains the size of each error code segment.
 */
namespace segment_size {
    inline constexpr auto category = int{3};
    inline constexpr auto type     = int{5};
    inline constexpr auto data     = int{24};
}

/**
 * @namespace segment_mask
 * 
 * @brief Contains the bitmask of each error code segment.
 */
namespace segment_mask {
    inline constexpr auto category = std::uint32_t{make_bitmask(segment_size::category)};
    inline constexpr auto type     = std::uint32_t{make_bitmask(segment_size::type)};
    inline constexpr auto data     = std::uint32_t{make_bitmask(segment_size::data)};
}

/**
 * @namespace data_size
 * 
 * @brief Contains the size of each error data segment.
 */
namespace data_size {
    inline constexpr auto byte  = int{8};
    inline constexpr auto half  = int{12};
    inline constexpr auto dbyte = int{byte * 2};
    inline constexpr auto word  = int{half * 2};
}

/**
 * @namespace data_mask
 * 
 * @brief Contains the bitmask of each error data segment.
 */
namespace data_mask {
    inline constexpr auto byte  = std::uint32_t{make_bitmask(data_size::byte)};
    inline constexpr auto half  = std::uint32_t{make_bitmask(data_size::half)};
    inline constexpr auto dbyte = std::uint32_t{make_bitmask(data_size::dbyte)};
    inline constexpr auto word  = std::uint32_t{make_bitmask(data_size::word)};
}
/** @} */

/**
 * @class code
 *
 * @brief Stores error information in a compact and concise way. Provides an interface
 * that allows the error data to be configured separately.
 * 
 * @details An error code is divided into three different segments. The first segment
 * represents the error category and consists of 3 bits. The second segment represents
 * the error type and consists of 5 bits. The third and final segment represents the
 * error data and consists of 24 bits. The error data can contain additional information
 * that is relevant to a specific error type.
 * 
 * Error codes can be constructed directly while providing all of the necessary
 * information in one go. It is also possible to construct an error code with just its
 * error type, allowing the error data to be set at a later point in time.
 */
class code {
private:
    /**
     * @brief Constructs an error code with a given error category and error identifier.
     * 
     * @details The error category, followed by the error identifier, are shifted to the
     * higher order bits of the error code. This leaves the remaining part of the higher
     * order bits and all of the lower order bits free to be used by the error data.
     * 
     * @tparam ErrorType Type of the error code.
     * 
     * @param[in] category_id Category of the error code.
     * @param[in] error_id Identifier of the error code.
     */
    template<typename ErrorType,
        typename = std::enable_if_t<is_bitwise_enum_v<ErrorType>>>
    constexpr code(category category_id, ErrorType error_id):
        data{{[&]{
            auto bits = std::uint32_t{category_id & segment_mask::category};
            bits <<= segment_size::type;
            bits  |= error_id & segment_mask::type;
            bits <<= segment_size::data;
            return bits;
        }()}}
    {}

    /**
     * @struct full_segment
     * 
     * @brief Provides an abstraction for the entire internal value of the error code.
     */
    struct full_segment {
        std::uint32_t bits; /**< Actual contents of the error code. */
    };

    /**
     * @struct data_segment
     * 
     * @brief Provides an abstraction of the data segment of an error code.
     * 
     * @details A data segment is implemented in terms of a full segment. It provides an
     * abstraction to interact with the data segment of an error code or a specific part
     * thereof. 
     * 
     * @tparam Mask Indicates the group of bits that can be set or accessed.
     * @tparam Offset Indicates the location of the group of bits within the error code.
     */
    template<std::uint32_t Mask, int Offset = 0,
        typename = std::enable_if_t<(Mask <= segment_mask::data)>,
        typename = std::enable_if_t<(Offset < segment_size::data)>>
    struct data_segment : private full_segment {
        /**
         * @brief Assigns part of the data segment, which depends on the configured
         * bitmask and offset.
         * 
         * @param[in] error_data New value of the data segment part. Only the bits that
         * match the bitmask are used.
         * 
         * @return Reference to the data segment object.
         */
        constexpr auto operator=(std::int_fast32_t error_data) -> data_segment& {
            bits &= ~(Mask << Offset);
            bits |= (static_cast<std::uint32_t>(error_data) & Mask) << Offset;
            return *this;
        }

        /**
         * @brief Implicitly converts the relevant bits of a specific data segment to an
         * unsigned 32-bit integer value.
         * 
         * @details The part that is relevant depends on the bitmask and offset of the
         * data segment and is shifted to the lower order bits of the unsigned 32-bit
         * integer value.
         */
        [[nodiscard]]
        constexpr operator std::uint32_t() const
        { return (bits >> Offset) & Mask; }
    };

    /**
     * @union data_format
     * 
     * @brief Defines the format of the data segment of an error code.
     * 
     * @details The data segment of an error code consists of 24 bits and is divided into
     * different parts. Each part represents a group of bits at a specific location.
     * These parts can be accessed and set individually without affecting other parts of
     * the error code. The parts of the data segment are laid out as follows:
     * 
     *   - High part  (bits 12-23)
     *   - Low part   (bits  0-11)
     *   - Byte3 part (bits 16-23)
     *   - Byte2 part (bits  8-15)
     *   - Byte1 part (bits  0-7)
     */
    union data_format {
        /**
         * @brief Assigns the data segment of an error code with a new value.
         * 
         * @param[in] error_data New data segment value. Only the lower 24 bits are used.
         * 
         * @return Reference to the data format object.
         */
        constexpr auto operator=(std::int_fast32_t error_data) -> data_format& {
            data = error_data;
            return *this;
        }

        /**
         * @brief Provides access to the 24 data bits.
         */
        [[nodiscard]]
        constexpr operator std::uint32_t() const
        { return data; }

        full_segment value;                                    /**< All 32 error bits. */
        data_segment<segment_mask::data> data;                 /**< All 24 data bits. */
        data_segment<data_mask::half, data_size::half> high;   /**< High 12 data bits. */
        data_segment<data_mask::half> low;                     /**< Low 12 data bits. */
        data_segment<data_mask::byte, data_size::dbyte> byte3; /**< High 8 data bits. */
        data_segment<data_mask::byte, data_size::byte> byte2;  /**< Mid 8 data bits. */
        data_segment<data_mask::byte> byte1;                   /**< Low 8 data bits. */
    };

public:
    /**
     * @brief Constructs a default error code.
     * 
     * @details A default constructed error code is zero-initialized. This mean that both
     * the category and type of the error are unspecified.
     */
    constexpr code() = default;

    /**
     * @brief Constructs a parsing error code with a given error identifier.
     * 
     * @param[in] error_id Identifier of the error code.
     */
    constexpr explicit code(type::parsing error_id)
        : code{category::parsing, error_id} {}

    /**
     * @brief Constructs a validation error code with a given error identifier.
     * 
     * @param[in] error_id Identifier of the error code.
     */
    constexpr explicit code(type::validation error_id)
        : code{category::validation, error_id} {}

    /**
     * @brief Constructs a verification error code with a given error identifier.
     * 
     * @param[in] error_id Identifier of the error code.
     */
    constexpr explicit code(type::verification error_id)
        : code{category::verification, error_id} {}

    /**
     * @brief Constructs an error code with a given error identifier and error data.
     * 
     * @details The error category depends on the type of the error. Both the error
     * category and error identifier are shifted to the higher order bits, respectively.
     * The remaining bits are set by the error data.
     * 
     * @tparam ErrorType Type of the error code.
     * 
     * @param[in] error_id Identifier of the error code.
     * @param[in] error_data Data value of the error code.
     */
    template<typename ErrorType,
        typename = std::enable_if_t<is_bitwise_enum_v<ErrorType>>>
    constexpr code(ErrorType error_id, std::int_fast32_t error_data)
        : code{error_id}
    { data.value.bits |= static_cast<std::uint32_t>(error_data) & segment_mask::data; }

    /**
     * @brief Returns the unsigned 32-bit integer value of the error code.
     */
    [[nodiscard]]
    constexpr auto value() const -> std::uint32_t
    { return data.value.bits; }

    /**
     * @brief Implicitly converts an error code to its unsigned 32-bit integer value.
     */
    [[nodiscard]]
    constexpr operator std::uint32_t() const
    { return value(); }

    /**
     * @brief Compares two error codes for (in)equality.
     * 
     * @param[in] lhs Error code on the left-hand side of the operator.
     * @param[in] rhs Error code on the right-hand side of the operator.
     * 
     * @return Two error codes are considered to be equal when their unsigned 32-bit
     * integer values match.
     * 
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(code const& lhs, code const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(code const& lhs, code const& rhs) -> bool
    { return lhs.value() == rhs.value(); }
    /** @} */

    data_format data{}; /**< Data segment of the error code. */
};

} // namespace cfg::error

#endif

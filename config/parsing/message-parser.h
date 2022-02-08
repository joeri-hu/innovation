/**
 * @file message-parser.h
 * @brief Parsing-mechanism for processing config messages.
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
#ifndef CFG_CONFIG_PARSING_MESSAGE_PARSER_H
#define CFG_CONFIG_PARSING_MESSAGE_PARSER_H

#include "config-parser.h"

#include <errors/error-handler.h>
#include <errors/error-types.h>
#include <traits/class-traits.h>
#include <traits/iterator-traits.h>
#include <utilities/bitwise.h>
#include <utilities/range.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @class message_data
 * 
 * @brief Data-type used for handling config messages, consisting of a pointer to a range
 * of bytes and a size.
 */
class message_data {
public:
    /**
     * @brief Default constructs a message-data object.
     */
    constexpr message_data() = default;

    /**
     * @brief Constructs a message-data object with a given data pointer and size.
     */
    constexpr message_data(std::byte* data, std::uint_least8_t size)
        : data_{data}, size_{size} {}

    /**
     * @brief Gets the data pointer.
     */
    [[nodiscard]]
    constexpr auto data() const -> std::byte const*
    { return data_; }

    /**
     * @brief Gets the size of message data.
     */
    [[nodiscard]]
    constexpr auto size() const -> std::uint_least8_t
    { return size_; }

    /**
     * @brief Compares two message-data objects for (in)equality.
     * 
     * @param[in] lhs Message-data object on the left-hand side of the operator.
     * @param[in] rhs Message-data object on the right-hand side of the operator.
     * 
     * @return Two message-data objects are considered to be equal when their data
     * pointer and size matches.
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(
        message_data const& lhs, message_data const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(
        message_data const& lhs, message_data const& rhs) -> bool
    { return lhs.data_ == rhs.data_ and lhs.size_ == rhs.size_; }
    /** @} */

private:
    std::byte* data_{};         /**< Pointer to the data of the message. */
    std::uint_least8_t size_{}; /**< Size of of the message data. */
};

/**
 * @class message_parser
 * 
 * @brief Parses config messages in the form of byte ranges and maps the extracted bits
 * to the value-buffer of each setting.
 * 
 * @tparam SettingIter Iterator type of the settings container.
 * @tparam MaxSettings Maximum number of settings to operate on.
 */
template<typename SettingIter, int MaxSettings,
    typename = std::enable_if_t<is_random_access_iter_v<SettingIter>>,
    typename = std::enable_if_t<(MaxSettings > 0)>>
class message_parser : public config_parser<message_parser<SettingIter, MaxSettings>> {
    /**
     * @typedef base_type
     * 
     * @brief Shorter notation to refer to the type of the base class.
     */
    using base_type = config_parser<message_parser<SettingIter, MaxSettings>>;

    /**
     * @{
     * @brief Grants the public interface access to its implementation.
     */
    template<typename Config>
    friend constexpr auto base_type::parse_config(Config const&) -> void;
    friend auto base_type::report_parsing_errors() const -> void;
    friend constexpr auto base_type::has_parsing_errors() const -> bool;
    /** @} */

public:
    /**
     * @var max_settings
     * 
     * @brief Maximum number of settings that a message parser can operate on.
     */
    static constexpr auto max_settings = int{MaxSettings};

    /**
     * @brief Default constructs a message parser.
     */
    constexpr message_parser() = default;

    /**
     * @brief Constructs a message parser with a range of settings to operate on.
     * 
     * @tparam Settings Container type that stores its contents in a contiguous sequence.
     * 
     * @param[in,out] settings Container with settings which will have their values set
     * based on the contents of the parsed config message.
     */
    template<typename Settings,
        typename = std::enable_if_t<is_contiguous_container_v<Settings>>>
    constexpr explicit message_parser(Settings& settings)
        : settings_{settings} {}

    /**
     * @brief Clears all of the parsing errors.
     */
    constexpr auto clear_parsing_errors() -> void
    { err_handler.clear_errors(); }

    /**
     * @brief Sets the new range of settings to operate on.
     * 
     * @details If the distance of the new range of settings exceeds the MaxSettings
     * value, the new range is ignored and no changes are made.
     * 
     * @param[in] settings New range of settings to operate on. This can also be a
     * reference to a container type, due to the extensive constructors of the range
     * class.
     */
    constexpr auto set_settings(range<SettingIter> settings) -> void {
        auto const distance = settings.distance();
        if (distance <= 0 or distance > MaxSettings) return;
        settings_ = settings;
    }

private:
    /**
     * @brief Parses a config message.
     * 
     * @details Checks if the config message appears to be valid and iterates through all
     * of the settings. The predefined span of bits of each setting is extracted from the
     * config message and written to their internal value buffer. If the bitspan of a
     * setting has a size of zero, it is simply ignored and its value remains unaltered.
     * 
     * @param[in] config Config message to validate.
     */
    constexpr auto parse_config_impl(message_data config) -> void {
        validate_config_message(config);
        if (err_handler.contains_errors()) return;

        for (auto& setting_obj : settings_) {
            if (auto const bits = setting_obj.config_bits(); bits.size() != 0) {
                setting_obj.set_value(extract_bits(config.data(), bits));
            }
        }
    }

    /**
     * @brief Validates a config message.
     * 
     * @details Checks if the data pointer of the config message is valid and if its size
     * is sufficiently large.
     * 
     * @note Depending on the robustness and capabalities of the LoRaWAN protocol, it
     * could be useful to implement a checksum mechanism similar to CRC32.
     * 
     * @param[in] config Config message to validate.
     */
    constexpr auto validate_config_message(message_data config) -> void {
        if (config.data() == nullptr) {
            err_handler.add_error(parsing_error::invalid_message_pointer);
        }
        if (config.size() < bitspan::byte_boundary) {
            err_handler.add_error(
                parsing_error::insufficient_message_size, config.size());
        }
    }

    /**
     * @brief Checks if any error has occurred during the parsing of a config message.
     */
    [[nodiscard]]
    constexpr auto has_parsing_errors_impl() const -> bool
    { return err_handler.contains_errors(); }

    /**
     * @brief Reports any error that might have occurred during the parsing of a
     * config message.
     * 
     * @details If there are no parsing errors to report, the logging request is simply
     * ignored.
     */
    auto report_parsing_errors_impl() const -> void {
        err_handler.log_errors(
            "[ERROR]Some errors occurred while parsing the config message:\n");
    }

    error_handler<2> err_handler; /**< Handles potential parsing-errors. */
    range<SettingIter> settings_; /**< Range of settings to operate on. */
};

/**
 * @remark Allows a message parser to be constructed from a container type.
 * 
 * @tparam Settings Container type that stores its contents in a contiguous sequence.
 */
template<typename Settings,
    typename = std::enable_if_t<is_contiguous_container_v<Settings>>>
message_parser(Settings&)
    -> message_parser<iterator_type<Settings>, std::tuple_size<Settings>{}>;

} // namespace cfg

#endif

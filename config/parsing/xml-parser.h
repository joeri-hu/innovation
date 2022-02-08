/**
 * @file xml-parser.h
 * @brief Parsing-mechanism for processing XML-formatted data.
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
#ifndef CFG_CONFIG_PARSING_XML_PARSER_H
#define CFG_CONFIG_PARSING_XML_PARSER_H

#include "config-parser.h"
#include "file-pointer.h"

#include <errors/error-handler.h>
#include <errors/error-types.h>
#include <traits/class-traits.h>
#include <traits/iterator-traits.h>
#include <utilities/algorithm.h>
#include <utilities/container.h>
#include <utilities/range.h>

#define SAXML_NO_MALLOC
#define SAXML_MAX_STRING_LENGTH 64
#include <libraries/saxml.h>

#include <array>
#include <cstddef>
#include <cstdio>
#include <string_view>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @class xml_parser
 * 
 * @brief Parses XML-formatted data with the use of the SAXML C-library and maps each
 * parsed value to a matching setting.
 * 
 * @tparam SettingIter Iterator type of the settings container.
 * @tparam MaxSettings Maximum number of settings to operate on.
 */
template<typename SettingIter, int MaxSettings,
    typename = std::enable_if_t<is_random_access_iter_v<SettingIter>>,
    typename = std::enable_if_t<(MaxSettings > 0)>>
class xml_parser : public config_parser<xml_parser<SettingIter, MaxSettings>> {
    /**
     * @typedef base_type
     * 
     * @brief Shorter notation to refer to the type of the base class.
     */
    using base_type = config_parser<xml_parser<SettingIter, MaxSettings>>;

    /**
     * @typedef settings_range
     * 
     * @brief Type of the range of settings.
     */
    using settings_range = range<SettingIter>;

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
     * @brief Maximum number of settings that an XML parser can operate on.
     */
    static constexpr auto max_settings = int{MaxSettings};

    /**
     * @var max_tag_depth
     * 
     * @brief Maximum tag-depth of all the settings.
     */
    static constexpr auto max_tag_depth
        = int{settings_range::value_type::max_tag_depth};

    /**
     * @brief Default constructs an XML parser.
     */
    constexpr xml_parser() = default;

    /**
     * @brief Constructs an XML parser with a range of settings to operate on.
     * 
     * @tparam Settings Container type that stores its contents in a contiguous sequence.
     * 
     * @param[in,out] settings Container with settings. Their tags will be searched and
     * their values will be set based on the contents of the parsed XML data.
     */
    template<typename Settings,
        typename = std::enable_if_t<is_contiguous_container_v<Settings>>>
    constexpr explicit xml_parser(Settings& settings)
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
    constexpr auto set_settings(settings_range settings) -> void {
        auto const distance = settings.distance();
        if (distance <= 0 or distance > MaxSettings) return;
        settings_ = settings;
    }

private:
    /**
     * @brief Parses XML-formatted data.
     * 
     * @details Walks through each character of the provided configuration data. The
     * SAXML library performs the core parsing procedures. Every raised SAX-event is
     * dispatched to the corresponding member functions prefixed with a 'handle' name.
     * 
     * Any potential parsing error is added to the error-handler. A file-pointer is used
     * to keep track of the line and column position within the XML file.
     * 
     * @param[in] config XML-formatted data to parse.
     */
    constexpr auto parse_config_impl(std::string_view config) -> void {
        if (config.empty()) {
            err_handler.add_error(parsing_error::empty_config, position);
            return;
        }
        reset_parsing();
        execute_parsing(config);
        verify_parsing();
    }

    /**
     * @brief Resets all the state that changes during the parsing of the XML file.
     */
    constexpr auto reset_parsing() -> void {
        position.reset();
        err_handler.clear_errors();
        cfg::fill_n(tag_levels, settings_.distance(), std::int8_t{});
        handle_tag_called = false;
    }

    /**
     * @brief Executes the parsing process.
     * 
     * @details Interacts with the SAXML library to carry out the core parsing mechanics
     * with the provided XML-formatted data.
     * 
     * @param[in] config XML-formatted data to parse.
     */
    auto execute_parsing(std::string_view config) -> void {
        auto saxml_context = tSaxmlContext{
            this,
            invoke<&xml_parser::handle_tag>,
            invoke<&xml_parser::handle_tag_end>,
            nullptr,
            invoke<&xml_parser::handle_content>,
            nullptr
        };
        auto saxml = saxml_Initialize(&saxml_context, SAXML_MAX_STRING_LENGTH);

        for (auto const character : config) {
            saxml_HandleCharacter(saxml, character);
            update_position(character);
        }
    }

    /**
     * @brief Invokes member-function calls based on a given type-erased object.
     *
     * @details This function is used to interface with the SAXML C-library without
     * exposing shared state to the global namespace.
     *
     * @tparam MemberFunc Pointer to a member-function used for handling SAX-events.
     * 
     * @param[in] object Instance of an XML parser object to operate on.
     * @param[in] value Zero-terminated string referring to the parsed data of interest.
     */
    template<void (xml_parser::*MemberFunc)(char const*)>
    static constexpr auto invoke(void* object, char const* value) -> void
    { (static_cast<xml_parser*>(object)->*MemberFunc)(value); }

    /**
     * @brief Updates the position of the file-pointer based on the given character.
     * 
     * @details If the character is a carriage return, it is simply ignored.
     * 
     * @param[in] character Character that is being parsed.
     */
    constexpr auto update_position(char character) -> void {
        if (character == '\n') {
            position.next_line();
        } else if (character != '\r') {
            position.next_column();
        }
    }

    /**
     * @brief Verifies the parsing process.
     * 
     * @details A tag-depth is being tracked during parsing. If this tag-depth does not
     * match zero exactly, or if no tags were found, a matching parsing-error is added
     * to the error-handler.
     */
    constexpr auto verify_parsing() -> void {
        if (tag_depth > 0) {
            err_handler.add_error(parsing_error::missing_closing_tag, tag_depth);
        } else if (tag_depth < 0) {
            err_handler.add_error(parsing_error::missing_opening_tag, -tag_depth);
        }
        if (not handle_tag_called) {
            err_handler.add_error(parsing_error::no_tags_found, position);
        }
    }

    /**
     * @brief Checks if any error has occurred during the parsing of an XML file.
     */
    [[nodiscard]]
    constexpr auto has_parsing_errors_impl() const -> bool
    { return err_handler.contains_errors(); }

    /**
     * @brief Reports any error that might have occurred during the parsing of a config
     * file.
     * 
     * @details If there are no parsing errors to report, the logging request is simply
     * ignored.
     */
    auto report_parsing_errors_impl() const -> void {
        err_handler.log_errors(
            "[ERROR]Some errors occurred while parsing the config file:\n");
    }

    /**
     * @brief Handles SAX-events whenever an XML-tag is being parsed.
     * 
     * @details Whenever the parsed tag-name matches the tag-name of a setting at the
     * right tag-depth, that setting is selected as the new target setting. The target
     * setting will be of interest for other event-handlers.
     * 
     * If there are no tags found in the XML-file, this function is never called and
     * the 'handle_tag_called' flag remains set to false.
     * 
     * @param[in] tag Name of the tag that is being parsed.
     */
    constexpr auto handle_tag(char const* tag) -> void {
        if (tag_depth < max_tag_depth) {
            for (auto [it, end, idx] = settings_.enumerate(); it != end; ++it, ++idx) {
                if (not tag_depth_matches(idx))     continue;
                if (not tag_name_matches(tag, idx)) continue;
                increase_tag_level(idx);
                select_setting(idx);
            }
        }
        ++tag_depth;
        handle_tag_called = true;
    }

    /**
     * @brief Checks if the current tag-depth matches the tracked tag-level of a given
     * setting.
     * 
     * @param[in] index Index of the setting to check.
     */
    [[nodiscard]]
    constexpr auto tag_depth_matches(std::uint_fast16_t index) const -> bool
    { return tag_levels[index] == tag_depth; }

    /**
     * @brief Checks if the tag-name matches the tag-name of a given setting at the
     * current tag-depth.
     * 
     * @param[in] tag Tag name to compare.
     * @param[in] index Index of the setting to check.
     */
    [[nodiscard]]
    constexpr auto tag_name_matches(
        std::string_view tag, std::uint_fast16_t index) const -> bool
    { return settings_[index].tag(tag_depth) == tag; }

    /**
     * @brief Increases the tracked tag-level of a given setting.
     * 
     * @details This function is part of the tag-matching algorithm that reduces
     * unnecessary string comparisons.
     * 
     * @param[in] index Index of the setting of which its tag-level is increased.
     */
    constexpr auto increase_tag_level(std::uint_fast16_t index) -> void
    { tag_levels[index] = static_cast<std::int8_t>(tag_depth + 1); }

    /**
     * @brief Selects a setting as the target.
     * 
     * @details This function selects the given setting as the target setting which can
     * be of interest to some event-handlers.
     * 
     * @param[in] index Index of the setting to select.
     */
    constexpr auto select_setting(std::uint_least16_t index) -> void
    { target_setting = index; }

    /**
     * @brief Handles SAX-events whenever a closing tag is being parsed.
     * 
     * @details Simply decrements the current tag depth.
     */
    constexpr auto handle_tag_end(char const*) -> void
    { --tag_depth; }

    /**
     * @brief Handles SAX-events whenever the contents of an XML-tag is being parsed.
     * 
     * @details Sets the value of the targeted setting to the parsed content as long as
     * the content matches the right tag. If the size of the content exceeds the size of
     * a setting's value-buffer, the content is only copied partially and a matching
     * parser-error is added to the error-handler.
     * 
     * @param[in] content Zero-terminated string that refers to the actual contents of an
     * XML-tag.
     */
    constexpr auto handle_content(char const* content) -> void {
        if (not tag_depth_matches(target_setting))    return;
        if (not is_final_tag_reached(target_setting)) return;

        auto const string_value = std::string_view{content};
        if (string_value.size() > settings_range::value_type::max_value_size) {
            err_handler.add_error(parsing_error::exceeds_max_value_length, position);
        }
        settings_[target_setting].set_value(string_value);
        reset_tag_level(target_setting);
    }

    /**
     * @brief Checks if the tag-depth refers to the final tag of a given setting.
     * 
     * @param[in] index Index of the setting.
     */
    constexpr auto is_final_tag_reached(std::uint_fast16_t index) -> bool {
        return tag_depth == max_tag_depth
            or (tag_depth < max_tag_depth and
                settings_[index].is_tag_empty(tag_depth));
    }

    /**
     * @brief Sets the reached tag-depth level of a given setting to zero.
     * 
     * @param[in] index Index of the setting.
     */
    constexpr auto reset_tag_level(std::uint_fast16_t index) -> void
    { tag_levels[index] = 0; }

    /**
     * @brief Handles SAX-events whenever the attribute of an XML-tag is being parsed.
     * 
     * @note There is no implementation provided, as there is currently no use for it.
     */
    constexpr auto handle_attribute(char const*) -> void {}

    settings_range settings_;                     /**< Range of all the settings. */
    file_ptr position;                            /**< Position of the file-pointer. */
    error_handler<MaxSettings> err_handler;       /**< Handles parsing-errors. */
    array<std::int8_t, MaxSettings> tag_levels{}; /**< Tracks tag levels of settings. */
    std::uint_least16_t target_setting{};         /**< Index of the selected setting. */
    std::int_least8_t tag_depth{};                /**< Tracks the depth of a tag. */
    bool handle_tag_called{};                     /**< Tracks if any tag was parsed. */
};

/**
 * @remark Allows an XML parser to be constructed from a container type.
 * 
 * @tparam Settings Container type that stores its contents in a contiguous sequence.
 */
template<typename Settings,
    typename = std::enable_if_t<is_contiguous_container_v<Settings>>>
xml_parser(Settings&)
    -> xml_parser<iterator_type<Settings>, std::tuple_size<Settings>{}>;

} // namespace cfg

#endif

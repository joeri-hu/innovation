/**
 * @file config-handler.h
 * @brief Mechanism for processing configuration files or messages.
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
#ifndef CFG_CONFIG_CORE_CONFIG_HANDLER_H
#define CFG_CONFIG_CORE_CONFIG_HANDLER_H

#include "main-config.h"

#include <checking/default-verification-rules.h>
#include <checking/validation-mode.h>
#include <errors/error-handler.h>
#include <settings/default-settings.h>
#include <settings/setting-handler.h>
#include <traits/class-traits.h>
#include <traits/enum-traits.h>
#include <utilities/container.h>
#include <utilities/file-io.h>

#include <array>
#include <cstddef>
#include <optional>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @class config_handler
 * 
 * @brief Processes a configuration file or message and maps the parsed values to a main
 * configuration object that is used for controlling various internal systems.
 * 
 * @details Configuration files are parsed with a parser-implementation that uses a data
 * interchange format library. Configuration messages are parsed by extracting and
 * converting ranges of bit sequences to integral values.
 * 
 * The parsed values are stored in a value-buffer contained within each setting. These
 * settings are validated and set/applied to a main configuration object that is used for
 * controlling various other parts of the program internally.
 * 
 * Potential parsing and validation errors are tracked by an error-handler and can be
 * reported respectively. After all is said and done, the main configuration object can
 * be verified with a set of verification rules to prevent any misconfigurations.
 * 
 * @tparam Parser Template type of the concrete parser implementation.
 * @tparam MainConfig Configuration-object type that controls various internal systems.
 * @tparam Settings Container type that stores the settings in a contiguous sequence.
 */
template<
    template<typename, auto, typename...> typename Parser,
    typename MainConfig = main_config,
    typename Settings = decltype(get_default_settings()),
    typename = std::enable_if_t<is_data_type_v<MainConfig>>,
    typename = std::enable_if_t<is_contiguous_container_v<Settings>>>
class config_handler {
public:
    /**
     * @brief Default-constructs a config-handler.
     */
    config_handler() = default;

    /**
     * @brief Constructs a config-handler with a main configuration object and a
     * container of settings.
     * 
     * @param[in] main_cfg Main configuration object.
     * @param[in] settings Container of settings.
     */
    config_handler(MainConfig const& main_cfg, Settings const& settings)
        : main_cfg_{main_cfg}, settings_{settings} {}

    /**
     * @brief Processes a configuration file or message.
     * 
     * @details The contents of a configuration file or message is parsed and the values
     * that are relevant are copied to the value-buffer of each matching setting. Each
     * setting will then be validated and applied by the setting-handler. The settings
     * that are validated successfully are mapped to the corresponding options within
     * the main configuration object.
     * 
     * @tparam ConfigData Type of the config file or message data.
     * 
     * @param[in] data Actual data of the config file or message.
     */
    template<typename ConfigData>
    constexpr auto process_config(ConfigData const& data) -> void {
        parser.parse_config(data);
        setting_handlr.apply_valid_settings(main_cfg_);
    }

    /**
     * @brief Verifies the settings of the main configuration object.
     * 
     * @details Each verification rule verifies if the settings of the main configuration
     * object are correct. Any time any sort of misconfiguration is detected, a matching
     * verification error is added to the applicable error-buffer.
     * 
     * @tparam VerifyRules Container type of the verification rules.
     * 
     * @param[in] rules Container of verification rules stored in a contiguous sequence.
     * 
     * @return The resulting verification consists of an error-handler that contains zero
     * or more verifiction errors.
     */
    template<typename VerifyRules = decltype(get_default_verification_rules()),
        typename = std::enable_if_t<is_contiguous_container_v<VerifyRules>>>
    constexpr auto verify_main_config(
        VerifyRules const& rules = get_default_verification_rules())
    {
        auto verification = error_handler<std::tuple_size<VerifyRules>{}>{};
        for (auto const& rule : rules) {
            if (auto const error_id = rule.verify(main_cfg_); error_id) {
                verification.add_error(*error_id, rule.id());
            }
        }
        return verification;
    }

    /**
     * @brief Checks if any parsing or validation error has occurred during the
     * processing of the configuration file.
     * 
     * @return If any type of error has occurred during the parsing or validation
     * process, this function returns true. Otherwise, this function returns false and
     * the processing of a configuration file could be considered successful.
     */
    [[nodiscard]]
    constexpr auto has_config_errors() const -> bool {
        return parser.has_parsing_errors()
            or setting_handlr.has_validation_errors();
    }

    /**
     * @brief Reports any parsing or validation error that might have occurred during
     * the processing of a configuration file.
     * 
     * @details If there are no parsing or validation errors to report, the request is
     * simply ignored.
     */
    auto report_config_errors() const -> void {
        parser.report_parsing_errors();
        setting_handlr.report_validation_errors();
    }

    /**
     * @brief Gets the main configuration object.
     * 
     * @return Const-reference to the main configuration object.
     */
    [[nodiscard]]
    constexpr auto get_main_config() const -> MainConfig const&
    { return main_cfg_; }

    /**
     * @brief Resets the main configuration object to its initial values.
     * 
     * @details This function can be used to reset the values of the main configuration
     * object before processing another configuration file. 
     */
    constexpr auto reset_main_config() -> void
    { main_cfg_.reset(); }

    /**
     * @brief Sets new values to the main configuration object.
     * 
     * @details When processing a configuration file, the internally stored main
     * configuration object is provided with new values. Depending on your setup and
     * run-time environment, some values of the main configuration object remain
     * unaltered and can be used to fall back on whenever a setting could not be applied.
     * 
     * @param[in] main_cfg New values for the main configuration object.
     */
    constexpr auto set_main_config(MainConfig const& main_cfg) -> void
    { main_cfg_ = main_cfg; }

    /**
     * @brief Sets new settings to the settings container.
     * 
     * @param[in] settings New container of settings.
     */
    template<typename StatusIndicator,
        typename = std::enable_if_t<is_scoped_enum_v<StatusIndicator>>>
    constexpr auto set_status_indicator(StatusIndicator status) -> void
    { main_cfg_.framework.status = status; }

    /**
     * @brief Sets new settings to the settings container.
     * 
     * @param[in] settings New container of settings.
     */
    constexpr auto set_settings(Settings const& settings) -> void
    { settings_ = settings; }

private:
    /**
     * @var setting_count
     * 
     * @brief Numbers of settings stored by the settings container.
     */
    static constexpr auto setting_count = int{std::tuple_size<Settings>{}};

    /**
     * @typedef setting_iter
     * 
     * @brief Iterator type of the settings container.
     */
    using setting_iter = typename Settings::iterator;

    /**
     * @typedef setting_handler_t
     * 
     * @brief Shorter notation to refer to the setting-handler type.
     */
    using setting_handler_t = setting_handler<setting_iter, setting_count>;

    /**
     * @typedef parser_t
     * 
     * @brief Shorter notation to refer to the provided parser-implementation type.
     */
    using parser_t = Parser<setting_iter, setting_count>;

    MainConfig main_cfg_{};                       /**< Main configuration object. */
    Settings settings_{get_default_settings()};   /**< Container of settings. */
    parser_t parser{settings_};                   /**< Concrete parser implementation. */
    setting_handler_t setting_handlr{
        settings_, validation_mode::config_file}; /**< Validates and applies settings. */
};

} // namespace cfg

#endif

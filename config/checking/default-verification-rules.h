/**
 * @file default-verification-rules.h
 * @brief Default verification rules used for verifying configuration constraints.
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
#ifndef CFG_CONFIG_CHECKING_DEFAULT_VERIFICATION_RULES_H
#define CFG_CONFIG_CHECKING_DEFAULT_VERIFICATION_RULES_H

#include "default-verification-ids.h"
#include "verification-rule.h"

#include <core/main-config.h>
#include <errors/error-types.h>
#include <traits/class-traits.h>

#include <optional>
#include <type_traits>

/**
 * @namespace cfg
 *
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Verifies if a trigger configuration has at least one data-destination enabled.
 * 
 * @tparam TriggerConfig Type of the data-structure that stores a trigger configuration.
 * 
 * @param[in] trigger_config Configuration of a trigger.
 *
 * @return If the given trigger is set to enabled, either the option to write data via LoRaWAN
 * or to the SD-card is required to turned on. Not upholding this condition
 * 
 * @return If the given trigger is set to enabled but has no data-destination configured,
 * a @ref verification_error::no_data_destination_enabled is returned. Otherwise, the
 * optional error is empty.
 */
template<typename TriggerConfig, typename... Ts,
    typename = std::enable_if_t<is_data_type_v<TriggerConfig>>>
[[nodiscard]]
constexpr auto verify_data_destination(TriggerConfig const& config)
-> std::optional<verification_error> {
    if (not config.enable)    return std::nullopt;
    if (config.write_to.lora) return std::nullopt;
    if (config.write_to.sd)   return std::nullopt;

    return verification_error::no_data_destination_enabled;
}

/**
 * @brief Checks if any of the time, light, acceleration, or orientation triggers are
 * enabled within the given trigger configuration.
 * 
 * @tparam TriggerConfig Type of the data-structure that stores a trigger-config.
 * 
 * @param[in] trigger_config Configuration of a trigger.
 */
template<typename TriggerConfig, typename... Ts,
    typename = std::enable_if_t<is_data_type_v<TriggerConfig>>>
constexpr auto is_any_trigger_enabled(TriggerConfig const& config) -> bool {
    return config.time.enable
        or config.light.enable
        or config.acceleration.enable
        or config.orientation.enable;
}

/**
 * @brief Gets the default verification rules.
 *
 * @details Each verification rule consists of a verification identifier and an invocable
 * verify action. When adding a new default verification rule, ensure it is provided with
 * a new corresponding default verification identifier as well.
 *
 * @return Array of verification rules.
 */
[[nodiscard]]
constexpr auto get_default_verification_rules() {
    using id = verification_identifier;

    constexpr auto verification_rules = std::array{
        verification_rule{
            id::trigger_requirement,
            +[](main_config const& config) {
                using status = std::optional<verification_error>;
                return is_any_trigger_enabled(config.framework.trigger)
                    ? status{std::nullopt}
                    : status{verification_error::no_trigger_enabled};
            }},
        verification_rule{
            id::time_trigger,
            +[](main_config const& config)
            { return verify_data_destination(config.framework.trigger.time); }},
        verification_rule{
            id::light_trigger,
            +[](main_config const& config)
            { return verify_data_destination(config.framework.trigger.light); }},
        verification_rule{
            id::acceleration_trigger,
            +[](main_config const& config)
            { return verify_data_destination(config.framework.trigger.acceleration); }},
        verification_rule{
            id::orientation_trigger,
            +[](main_config const& config)
            { return verify_data_destination(config.framework.trigger.orientation); }
        }
    };
    return verification_rules;
}

} // namespace cfg

#endif

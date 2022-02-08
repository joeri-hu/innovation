/**
 * @file setting-handler.h
 * @brief Responsible for validating and applying settings.
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
#ifndef CFG_CONFIG_SETTINGS_SETTING_HANDLER_H
#define CFG_CONFIG_SETTINGS_SETTING_HANDLER_H

#include "setting.h"
#include "setting-identifiers.h"

#include <checking/validation-mode.h>
#include <errors/error-handler.h>
#include <errors/error-types.h>
#include <traits/class-traits.h>
#include <traits/iterator-traits.h>
#include <utilities/range.h>

#include <array>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @class setting_handler
 * 
 * @brief Validates and applies a range of settings.
 * 
 * @details Possibly unset or invalid settings are tracked with the use of error-codes.
 * 
 * @tparam Iterator Iterator type of the settings container.
 * @tparam MaxSettings Maximum number of settings to operate on.
 */
template<typename Iterator, int MaxSettings,
    typename = std::enable_if_t<is_random_access_iter_v<Iterator>>,
    typename = std::enable_if_t<(MaxSettings > 0)>>
class setting_handler {
public:
    /**
     * @brief Default constructs a setting handler.
     */
    constexpr setting_handler() = default;

    /**
     * @brief Constructs a setting-handler with a range of settings to operate on.
     * 
     * @tparam Settings Container type that stores its contents in a contiguous sequence.
     * 
     * @param[in,out] settings Container with settings. Each of these settings can be
     * validated and applied by invoking their self-contained validator and action
     * object, respectively.
     * @param[in] mode Validation mode that specifies how each setting should be
     * validated.
     */
    template<typename Settings,
        typename = std::enable_if_t<is_contiguous_container_v<Settings>>>
    constexpr setting_handler(Settings& settings, validation_mode mode)
        : settings_{settings}, mode_{mode} {}

    /**
     * @brief Validates each setting by invoking their self-contained validator.
     * 
     * @details If a setting is determined to be invalid, a matching validation error is
     * added to one of the applicable error-buffers (depending on the necessity of the
     * setting and the severity of the validation error).
     */
    constexpr auto validate_settings() -> void {
        for (auto const& setting_obj : settings_) {
            if (auto const error = setting_obj.validate(mode_); error) {
                handle_invalid_setting(setting_obj, *error);
            }
        }
    }

    /**
     * @brief Applies the self-contained action of each setting.
     * 
     * @tparam MainConfig Data-structure type of the configuration object.
     * 
     * @param[in,out] config Configuration object from which values can be read and
     * to which values can be written.
     */
    template<typename MainConfig,
        typename = std::enable_if_t<is_data_type_v<MainConfig>>>
    constexpr auto apply_settings(MainConfig& config) const -> void {
        for (auto const& setting_obj : settings_) {
            setting_obj.apply(config);
        }
    }

    /**
     * @brief Applies the settings after successfully validating them.
     * 
     * @details Each setting from the range of settings is first validated. If a setting
     * is determined to be valid, it is applied (by indirectly invoking its action-
     * object). Otherwise, when a setting is not valid, a validation error is added to
     * one of the applicable error-buffers.
     * 
     * @tparam MainConfig Data-structure type of the configuration object.
     * 
     * @param[in,out] config Configuration object which can be used by a setting's
     * validator to read values from. It can also be used by its action-object to write
     * converted values to.
     */
    template<typename MainConfig,
        typename = std::enable_if_t<is_data_type_v<MainConfig>>>
    constexpr auto apply_valid_settings(MainConfig& config) -> void {
        for (auto const& setting_obj : settings_) {
            if (auto const error = setting_obj.validate(mode_); error) {
                handle_invalid_setting(setting_obj, *error);
            } else {
                setting_obj.apply(config);
            }
        }
    }

    /**
     * @brief Reports any error that might have occurred during the validation process.
     * 
     * @details Validation errors caused by settings that have not been set and settings
     * with invalid values are reported separately. If there are no errors to report, the
     * request is simply ignored.
     */
    auto report_validation_errors() const -> void {
        report_unset_setting_errors();
        report_invalid_value_errors();
    }

    /**
     * @brief Reports errors of settings that have not been set.
     * 
     * @details Unset settings are considered to be errors with a lower severity. If
     * there are no unset-setting errors, the logging request is simply ignored.
     */
    auto report_unset_setting_errors() const -> void
    { unset_setting_errors.log_errors("[WARNING]Some settings were not set:\n"); }

    /**
     * @brief Reports errors of settings of which their value is determined to be
     * invalid.
     * 
     * @details Settings with invalid values are considered to be errors with a higher
     * severity. If there are no invalid-value errors, the logging request is simply
     * ignored.
     */
    auto report_invalid_value_errors() const -> void
    { invalid_value_errors.log_errors("[ERROR]Some values are not valid:\n"); }

    /**
     * @brief Clears all of the buffered validation errors.
     * 
     * @details This applies to the unset-setting error-buffer, as well as the invalid-
     * value error-buffer.
     */
    constexpr auto clear_errors() -> void {
        unset_setting_errors.clear_errors();
        invalid_value_errors.clear_errors();
    }

    /**
     * @brief Checks if any unset-setting or invalid-value error has occurred during the
     * validation process.
     * 
     * @return If any validated setting that is marked as required has not been set with
     * a value or its value is invalid, this function returns true. Otherwise, this
     * function returns false.
     */
    [[nodiscard]]
    constexpr auto has_validation_errors() const -> bool {
        return has_unset_setting_errors()
            or has_invalid_value_errors();
    }

    /**
     * @details Checks if any unset-setting error has occurred during the validation
     * process.
     * 
     * @return If any validated setting that is marked as required has not been set with
     * a value, this function returns true. Otherwise, this function returns false.
     */
    [[nodiscard]]
    constexpr auto has_unset_setting_errors() const -> bool
    { return unset_setting_errors.contains_errors(); }

    /**
     * @brief Checks if any invalid-value error has occurred during the validation
     * process.
     * 
     * @return If any validated setting has an invalid value, this function returns true.
     * Otherwise, this function returns false.
     */
    [[nodiscard]]
    constexpr auto has_invalid_value_errors() const -> bool
    { return invalid_value_errors.contains_errors(); }

    /**
     * @brief Gets the validation mode.
     */
    constexpr auto get_validation_mode() const -> validation_mode
    { return mode_; }

    /**
     * @brief Sets the validation mode.
     * 
     * @param[in] mode New validation mode.
     */
    constexpr auto set_validation_mode(validation_mode mode) -> void
    { mode_ = mode; }

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
    constexpr auto set_settings(range<Iterator> settings) -> void {
        auto const distance = settings.distance();
        if (distance <= 0 or distance > MaxSettings) return;
        settings_ = settings;
    }

private:
    /**
     * @typedef setting_t
     * 
     * @brief Type of the object of a setting.
     */
    using setting_t = typename range<Iterator>::value_type;

    /**
     * @brief Handles a setting that was not validated successfully.
     * 
     * @details If the invalid was not set and is marked as optional, no action is
     * performed and the validation error is discarded. Otherwise, the validation error
     * is added to the corresponding error-buffer.
     * 
     * @param[in] setting_obj Object of an invalid setting.
     * @param[in] error_id Identifier of the validation error.
     */
    constexpr auto handle_invalid_setting(
        setting_t const& setting_obj,
        validation_error error_id
    ) -> void {
        switch (error_id) {
        case validation_error::setting_unset:
            if (setting_obj.type() == setting_type::optional) return;
            unset_setting_errors.add_error(error_id, setting_obj.id());
            break;
        default:
            invalid_value_errors.add_error(error_id, setting_obj.id());
            break;
        }
    }

    range<Iterator> settings_;                       /**< Range of all the settings. */
    error_handler<MaxSettings> unset_setting_errors; /**< Stores unset-setting errors. */
    error_handler<MaxSettings> invalid_value_errors; /**< Stores invalid-value errors. */
    validation_mode mode_{validation_mode::config_file}; /**< Mode of the validator. */
};

/**
 * @remark Allows a setting-handler to be constructed from a container type.
 * 
 * @tparam Settings Container type that stores its contents in a contiguous sequence.
 */
template<typename Settings,
    typename = std::enable_if_t<is_contiguous_container_v<Settings>>>
setting_handler(Settings&, validation_mode)
    -> setting_handler<iterator_type<Settings>, std::tuple_size<Settings>{}>;

} // namespace cfg

#endif

/**
 * @file default-settings.h
 * @brief Default mapping of configuration settings to XML-content.
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
#ifndef CFG_CONFIG_SETTINGS_DEFAULT_SETTINGS_H
#define CFG_CONFIG_SETTINGS_DEFAULT_SETTINGS_H

#include "default-setting-ids.h"
#include "setting.h"
#include "setting-validators.h"

#include <checking/validation-rules.h>
#include <core/main-config.h>
#include <parsing/node.h>
#include <utilities/bitwise.h>

// warning: implicitly includes 'all.h'
#include <Framework/LowPowerFramework/Low_power_framework.hpp>

#include <cstdint>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Gets the default settings.
 * 
 * @details Specifies which predefined path of tag-names is mapped to which predefined
 * data-member of a @ref main_configuration object. Each @ref setting is provided with a
 * validator and a specific action, which either could be something generic or fully
 * customized. The bitspan of a setting indicates which part of a config message contains
 * the value to obtain.
 * 
 * The definitons of the @link default-setting-ids.h default settings @endlink are
 * strongly coupled to the default @link setting_identifier setting-identfiers @endlink.
 * When adding a new default setting, it should have a new corresponding default
 * identifier as well. 
 * 
 * @return Array of @ref setting objects.
 */
[[nodiscard]]
constexpr auto get_default_settings() {
    using id = setting_identifier;

    constexpr auto aether = node{"aether"};

    constexpr auto properties = aether / "properties";
    constexpr auto usb        = aether / "usb";
    constexpr auto trigger    = aether / "trigger";

    constexpr auto time          = trigger / "time";
    constexpr auto time_sensors  = time / "activate-sensors";
    constexpr auto light         = trigger / "light";
    constexpr auto light_sensors = light / "activate-sensors";
    constexpr auto acceleration  = trigger / "acceleration";
    constexpr auto accel_sensors = acceleration / "activate-sensors";
    constexpr auto orientation   = trigger / "orientation";
    constexpr auto orien_sensors = orientation / "activate-sensors";

    constexpr auto settings = make_settings(
        setting{
            id::device_name,
            properties / "name",
            setting_type::optional,
            dispatch_validator<validate_name>,
            +[](setting_data data, main_config& config)
            { zcopy_max(data.string, config.device_name.size(), config.device_name); }},
        setting{
            id::usb_detection,
            usb / "detection",
            bitspan::make<24, 2>,
            dispatch_validator<validate_usb<USB_DETECTION>>,
            +[](setting_data data, main_config& config)
            { config.framework.usb_detection = static_cast<USB_DETECTION>(data.int32); }},
        setting{
            id::usb_interval_ms,
            usb / "detection-interval-ms",
            bitspan::make<32, 32>,
            validate<std::uint32_t, 1'000>,
            +[](setting_data data, main_config& config)
            { config.framework.usb_detection_interval_ms = data.uint32; }},
        setting{
            id::time_trigger_enabled,
            time / "enabled",
            bitspan::make<26>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.time.enable = data.flag; }},
        setting{
            id::time_trigger_interval,
            time / "interval-ms",
            bitspan::make<64, 32>,
            validate<std::uint32_t, 1'000>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.time.interval_ms = data.uint32; }},
        setting{
            id::time_trigger_thp,
            time_sensors / "thp",
            bitspan::make<8>,
            validate<bool>,
            +[](setting_data data, main_config& config) {
                if (config.framework.trigger.time.enable) {
                    config.framework.bme280.measure_temperature = data.flag;
                    config.framework.bme280.measure_humidity    = data.flag; 
                    config.framework.bme280.measure_pressure    = data.flag;
                    config.framework.trigger.time.measure.thp   = data.flag;
                } else {
                    config.framework.bme280.measure_temperature = false;
                    config.framework.bme280.measure_humidity    = false; 
                    config.framework.bme280.measure_pressure    = false;
                    config.framework.trigger.time.measure.thp   = false;
                }
            }},
        setting{
            id::time_trigger_acc_gyro,
            time_sensors / "accel-gyro",
            bitspan::make<9>,
            validate<bool>,
            +[](setting_data data, main_config& config) {
                if (config.framework.trigger.time.enable) {
                    config.framework.bmx160.measure_accelerometer    = data.flag;
                    config.framework.bmx160.measure_gyroscope        = data.flag;
                    config.framework.trigger.time.measure.accel_gyro = data.flag;
                } else {
                    config.framework.bmx160.measure_accelerometer    = false;
                    config.framework.bmx160.measure_gyroscope        = false;
                    config.framework.trigger.time.measure.accel_gyro = false;
                }
            }},
        setting{
            id::time_trigger_magnetometer,
            time_sensors / "magnet",
            bitspan::make<10>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.time.enable) { 
                    config.framework.bmx160.measure_magnetometer = data.flag;
                    config.framework.trigger.time.measure.magnet = data.flag;
                } else {
                    config.framework.bmx160.measure_magnetometer = false;
                    config.framework.trigger.time.measure.magnet = false;
                }
            }},
        setting{
            id::time_trigger_light_intensity,
            time_sensors / "light",
            bitspan::make<11>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.time.enable) { 
                    config.framework.veml6030.measure_light     = data.flag;
                    config.framework.trigger.time.measure.light = data.flag;
                } else {
                    config.framework.veml6030.measure_light     = false;
                    config.framework.trigger.time.measure.light = false;
                }
            }},
        setting{
            id::time_trigger_lora_priority,
            time / "write-to" / "lorawan-priority",
            bitspan::make<128, 2>,
            validate<std::int8_t, 0, 3>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.time.lorawan_priority = data.int8; }},
        setting{
            id::time_trigger_write_to_lora,
            time / "write-to" / "lora",
            bitspan::make<130>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.time.write_to.lora = data.flag; }},
        setting{
            id::time_trigger_write_to_sd,
            time / "write-to" / "sd",
            bitspan::make<131>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.time.write_to.sd = data.flag; }},
        setting{
            id::light_trigger_enabled,
            light / "enabled",
            bitspan::make<27>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.light.enable = data.flag; }},
        setting{
            id::light_trigger_low_threshold,
            light / "low-threshold",
            bitspan::make<112, 16>,
            validate<std::uint16_t>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.light.low_threshold = data.uint16; }},
        setting{
            id::light_trigger_high_threshold,
            light / "high-threshold",
            bitspan::make<96, 16>,
            validate<std::uint16_t>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.light.high_threshold = data.uint16; }},
        setting{
            id::light_trigger_thp,
            light_sensors / "thp",
            bitspan::make<12>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.light.enable) {
                    config.framework.trigger.light.measure.thp = data.flag;
                } else {
                    config.framework.trigger.light.measure.thp = false;
                }
            }},
        setting{
            id::light_trigger_acc_gyro,
            light_sensors / "accel-gyro",
            bitspan::make<13>,
            validate<bool>,
            +[](setting_data data, main_config& config) {
                if (config.framework.trigger.light.enable) {
                    config.framework.trigger.light.measure.accel_gyro = data.flag;
                } else {
                    config.framework.trigger.light.measure.accel_gyro = false;
                }
            }},
        setting{
            id::light_trigger_magnetometer,
            light_sensors / "magnet",
            bitspan::make<14>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.light.enable) { 
                    config.framework.trigger.light.measure.magnet = data.flag;
                } else {
                    config.framework.trigger.light.measure.magnet = false;
                }
            }},
        setting{
            id::light_trigger_light_intensity,
            light_sensors / "light",
            bitspan::make<15>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.light.enable) { 
                    config.framework.trigger.light.measure.light = data.flag;
                } else {
                    config.framework.trigger.light.measure.light = false;
                }
            }},
        setting{
            id::light_trigger_lora_priority,
            light / "write-to" / "lorawan-priority",
            bitspan::make<132, 2>,
            validate<std::int8_t, 0, 3>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.light.lorawan_priority = data.int8; }},
        setting{
            id::light_trigger_write_to_lora,
            light / "write-to" / "lora",
            bitspan::make<134>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.light.write_to.lora = data.flag; }},
        setting{
            id::light_trigger_write_to_sd,
            light / "write-to" / "sd",
            bitspan::make<135>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.light.write_to.sd = data.flag; }},
        setting{
            id::acceleration_trigger_enabled,
            acceleration / "enabled",
            bitspan::make<28>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.acceleration.enable = data.flag; }},
        setting{
            id::acceleration_trigger_thp,
            accel_sensors / "thp",
            bitspan::make<16>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.acceleration.enable) {
                    config.framework.trigger.acceleration.measure.thp = data.flag;
                } else {
                    config.framework.trigger.acceleration.measure.thp = false;
                }
            }},
        setting{
            id::acceleration_trigger_acc_gyro,
            accel_sensors / "accel-gyro",
            bitspan::make<17>,
            validate<bool>,
            +[](setting_data data, main_config& config) {
                if (config.framework.trigger.acceleration.enable) {
                    config.framework.trigger.acceleration.measure.accel_gyro = data.flag;
                } else {
                    config.framework.trigger.acceleration.measure.accel_gyro = false;
                }
            }},
        setting{
            id::acceleration_trigger_magnetometer,
            accel_sensors / "magnet",
            bitspan::make<18>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.acceleration.enable) { 
                    config.framework.trigger.acceleration.measure.magnet = data.flag;
                } else {
                    config.framework.trigger.acceleration.measure.magnet = false;
                }
            }},
        setting{
            id::acceleration_trigger_light_intensity,
            accel_sensors / "light",
            bitspan::make<19>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.acceleration.enable) { 
                    config.framework.trigger.acceleration.measure.light = data.flag;
                } else {
                    config.framework.trigger.acceleration.measure.light = false;
                }
            }},
        setting{
            id::acceleration_trigger_lora_priority,
            acceleration / "write-to" / "lorawan-priority",
            bitspan::make<136, 2>,
            validate<std::int8_t, 0, 3>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.acceleration.lorawan_priority = data.int8; }},
        setting{
            id::acceleration_trigger_write_to_lora,
            acceleration / "write-to" / "lora",
            bitspan::make<138>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.acceleration.write_to.lora = data.flag; }},
        setting{
            id::acceleration_trigger_write_to_sd,
            acceleration / "write-to" / "sd",
            bitspan::make<139>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.acceleration.write_to.sd = data.flag; }},
        setting{
            id::orientation_trigger_enabled,
            orientation / "enabled",
            bitspan::make<29>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.orientation.enable = data.flag; }},
        setting{
            id::orientation_trigger_thp,
            orien_sensors / "thp",
            bitspan::make<20>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.orientation.enable) {
                    config.framework.trigger.orientation.measure.thp = data.flag;
                } else {
                    config.framework.trigger.orientation.measure.thp = false;
                }
            }},
        setting{
            id::orientation_trigger_acc_gyro,
            orien_sensors / "accel-gyro",
            bitspan::make<21>,
            validate<bool>,
            +[](setting_data data, main_config& config) {
                if (config.framework.trigger.orientation.enable) {
                    config.framework.trigger.orientation.measure.accel_gyro = data.flag;
                } else {
                    config.framework.trigger.orientation.measure.accel_gyro = false;
                }
            }},
        setting{
            id::orientation_trigger_magnetometer,
            orien_sensors / "magnet",
            bitspan::make<22>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.orientation.enable) { 
                    config.framework.trigger.orientation.measure.magnet = data.flag;
                } else {
                    config.framework.trigger.orientation.measure.magnet = false;
                }
            }},
        setting{
            id::orientation_trigger_light_intensity,
            orien_sensors / "light",
            bitspan::make<23>,
            validate<bool>,
            +[](setting_data data, main_config& config) { 
                if (config.framework.trigger.orientation.enable) { 
                    config.framework.trigger.orientation.measure.light = data.flag;
                } else {
                    config.framework.trigger.orientation.measure.light = false;
                }
            }},
        setting{
            id::orientation_trigger_lora_priority,
            orientation / "write-to" / "lorawan-priority",
            bitspan::make<140, 2>,
            validate<std::int8_t, 0, 3>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.orientation.lorawan_priority = data.int8; }},
        setting{
            id::orientation_trigger_write_to_lora,
            orientation / "write-to" / "lora",
            bitspan::make<142>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.orientation.write_to.lora = data.flag; }},
        setting{
            id::orientation_trigger_write_to_sd,
            orientation / "write-to" / "sd",
            bitspan::make<143>,
            validate<bool>,
            +[](setting_data data, main_config& config)
            { config.framework.trigger.orientation.write_to.sd = data.flag; }
        }
    );
    return settings;
}

} // namespace cfg

#endif

/**
 * @file main-config.h
 * @brief Main configuration object for controlling various internal systems.
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
#ifndef CFG_CONFIG_CORE_MAIN_CONFIG_H
#define CFG_CONFIG_CORE_MAIN_CONFIG_H

#include "config-comparison.h"

#include <errors/error-handler.h>
#include <strings/string-conversions.h>
#include <traits/class-traits.h>

// warning: implicitly includes 'all.h'
#include <logger.hpp>
#include <Framework/AEtherData.h>
#include <Framework/LowPowerFramework/Low_power_framework.hpp>
#include <LoRaWAN/App/Commissioning.h>

#include <array>
#include <algorithm>
#include <cstdio>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @struct main_configuration
 * 
 * @brief Contains all of the values that control various other internal parts of the
 * program.
 * 
 * @details The main configuration object ultimately describes how this program should
 * behave. Having an outside source indirectly alter its values allows this program to
 * become more dynamic of nature.
 * 
 * Part of this class is a framework configuration object. This object is used to
 * interact with the internal mechanics of Nathan Houwaart's framework system.
 * 
 * @tparam FrameworkConfig Type of the framework configuration object.
 */
template<typename FrameworkConfig = LowPowerFrameworkConfig,
    typename = std::enable_if_t<is_data_type_v<FrameworkConfig>>>
struct main_configuration {
    /**
     * @var max_name_size
     * 
     * @brief Maximum size of the device name.
     * 
     * @details This value needs to be large enough to be able to store the converted
     * value of the DevEUI of the builtin LoRaWAN chip.
     */
    static constexpr auto max_name_size = std::size_t{
        std::max(std::size_t{32}, (std::size(LORAWAN_DEVICE_EUI_D) * 2) + 1)
    };

    /**
     * @brief Constructs the default main configuration.
     * 
     * @details The default device name is set to DevEUI of the builtin LoRaWAN chip.
     */
    constexpr main_configuration() {
        *convert_to_hex(LORAWAN_DEVICE_EUI_D, device_name) = '\0';

        framework.status = StatusIndicator::operational;

        framework.usb_detection = USB_DETECTION::INTERVAL;
        framework.usb_detection_interval_ms = 10'000;

        framework.bme280.low_power   = true;
        framework.bmx160.low_power   = true;
        framework.veml6030.low_power = true;

        framework.trigger.time.enable              = true;
        framework.trigger.time.interval_ms         = 20'000;
        framework.trigger.time.measure.thp         = true;
        framework.trigger.time.measure.accel_gyro  = true;
        framework.trigger.time.measure.magnet      = true;
        framework.trigger.time.measure.light       = true;
        framework.trigger.time.lorawan_priority    = 4;
        framework.trigger.time.write_to.lora       = true;
        framework.trigger.time.write_to.sd         = true;

        framework.trigger.light.enable             = true;
        framework.trigger.light.low_threshold      = 1'000;
        framework.trigger.light.high_threshold     = 20'000;
        framework.trigger.light.measure.thp        = true;
        framework.trigger.light.measure.accel_gyro = true;
        framework.trigger.light.measure.magnet     = true;
        framework.trigger.light.measure.light      = true;
        framework.trigger.light.lorawan_priority   = 4;
        framework.trigger.light.write_to.lora      = true;
        framework.trigger.light.write_to.sd        = true;

        framework.trigger.acceleration.enable             = true;
        framework.trigger.acceleration.measure.thp        = true;
        framework.trigger.acceleration.measure.accel_gyro = true;
        framework.trigger.acceleration.measure.magnet     = true;
        framework.trigger.acceleration.measure.light      = true;
        framework.trigger.acceleration.lorawan_priority   = 4;
        framework.trigger.acceleration.write_to.lora      = true;
        framework.trigger.acceleration.write_to.sd        = true;
        
        framework.trigger.orientation.enable              = true;
        framework.trigger.orientation.measure.thp         = true;
        framework.trigger.orientation.measure.accel_gyro  = true;
        framework.trigger.orientation.measure.magnet      = true;
        framework.trigger.orientation.measure.light       = true;
        framework.trigger.orientation.lorawan_priority    = 4;
        framework.trigger.orientation.write_to.lora       = true;
        framework.trigger.orientation.write_to.sd         = true;
    }

    /**
     * @brief Resets the main configuration to its default values.
     */
    constexpr auto reset() -> void
    { *this = {}; }

    /**
     * @brief Compares two configuration objects for (in)equality.
     * 
     * @param[in] lhs Configuration object on the left-hand side of the operator.
     * @param[in] rhs Configuration object on the right-hand side of the operator.
     * 
     * @return Two main configuration objects are considered to be equal when all of
     * their data-members match.
     * 
     * @{
     */
    [[nodiscard]]
    friend auto operator!=(
        main_configuration const& lhs, main_configuration const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend auto operator==(
        main_configuration const& lhs,
        main_configuration const& rhs
    ) -> bool {
        return lhs.device_name == rhs.device_name
            and lhs.framework == rhs.framework;
    }
    /** @} */

    std::array<char, max_name_size> device_name; /**< Buffer storing the device name. */
    FrameworkConfig framework{};                 /**< Framework configuration object. */
};

/**
 * @typedef main_config
 * 
 * @brief Shorter notation to refer to the main configuration class.
 */
using main_config = main_configuration<>;

/**
 * @remark Ensures the main configuration object to be a data type.
 */
static_assert(is_data_type_v<main_config>);

/**
 * @brief Logs all of the values of a main configuration object.
 * 
 * @warning This function consumes a decent chunk of available stack space. Be aware of
 * using it when most of the stack is already being occupied by something else. Also,
 * ensure to manually update the buffer-size of message to log, accordingly.
 * 
 * @param[in] config Main configuration object to log.
 */
inline auto log_main_config(main_config const& config) -> void {
    std::array<char, 1024> message;
    std::sprintf(message.data(),
        "[INFO]Active config contents:\n"
        "  Name: %s\n"
        "  USB settings\n"
        "    detection: %s\n"
        "    interval-ms: %lu\n"
        "  Time trigger\n"
        "    enabled: %i\n"
        "    interval-ms: %lu\n"
        "    Sensors\n"
        "      thp: %i\n"
        "      accel-gyro: %i\n"
        "      magnet: %i\n"
        "      light: %i\n"
        "    Write to\n"
        "      lorawan-priority: %hi\n"
        "      lora: %i\n"
        "      sd: %i\n"
        "  Light trigger\n"
        "    enabled: %i\n"
        "    low-threshold: %hu\n"
        "    high-threshold: %hu\n"
        "    Sensors\n"
        "      thp: %i\n"
        "      accel-gyro: %i\n"
        "      magnet: %i\n"
        "      light: %i\n"
        "    Write to\n"
        "      lorawan-priority: %hi\n"
        "      lora: %i\n"
        "      sd: %i\n"
        "  Acceleration trigger\n"
        "    enabled: %i\n"
        "    Sensors\n"
        "      thp: %i\n"
        "      accel-gyro: %i\n"
        "      magnet: %i\n"
        "      light: %i\n"
        "    Write to\n"
        "      lorawan-priority: %hi\n"
        "      lora: %i\n"
        "      sd: %i\n"
        "  Orientation trigger\n"
        "    enabled: %i\n"
        "    Sensors\n"
        "      thp: %i\n"
        "      accel-gyro: %i\n"
        "      magnet: %i\n"
        "      light: %i\n"
        "    Write to\n"
        "      lorawan-priority: %hi\n"
        "      lora: %i\n"
        "      sd: %i\n",
        config.device_name.data(),
        config.framework.usb_detection == USB_DETECTION::ON  ? "on"  :
        config.framework.usb_detection == USB_DETECTION::OFF ? "off" : "interval",
        config.framework.usb_detection_interval_ms,

        config.framework.trigger.time.enable,
        config.framework.trigger.time.interval_ms,
        config.framework.trigger.time.measure.thp,
        config.framework.trigger.time.measure.accel_gyro,
        config.framework.trigger.time.measure.magnet,
        config.framework.trigger.time.measure.light,
        config.framework.trigger.time.lorawan_priority,
        config.framework.trigger.time.write_to.lora,
        config.framework.trigger.time.write_to.sd,
        
        config.framework.trigger.light.enable,
        config.framework.trigger.light.low_threshold,
        config.framework.trigger.light.high_threshold,
        config.framework.trigger.light.measure.thp,
        config.framework.trigger.light.measure.accel_gyro,
        config.framework.trigger.light.measure.magnet,
        config.framework.trigger.light.measure.light,
        config.framework.trigger.light.lorawan_priority,
        config.framework.trigger.light.write_to.lora,
        config.framework.trigger.light.write_to.sd,

        config.framework.trigger.acceleration.enable,
        config.framework.trigger.acceleration.measure.thp,
        config.framework.trigger.acceleration.measure.accel_gyro,
        config.framework.trigger.acceleration.measure.magnet,
        config.framework.trigger.acceleration.measure.light,
        config.framework.trigger.acceleration.lorawan_priority,
        config.framework.trigger.acceleration.write_to.lora,
        config.framework.trigger.acceleration.write_to.sd,

        config.framework.trigger.orientation.enable,
        config.framework.trigger.orientation.measure.thp,
        config.framework.trigger.orientation.measure.accel_gyro,
        config.framework.trigger.orientation.measure.magnet,
        config.framework.trigger.orientation.measure.light,
        config.framework.trigger.orientation.lorawan_priority,
        config.framework.trigger.orientation.write_to.lora,
        config.framework.trigger.orientation.write_to.sd
    );
    aether_log << message.data();
}

} // namespace cfg

#endif

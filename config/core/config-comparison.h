/**
 * @file config-comparison.h
 * @brief Comparison functions for various configuration objects from Nathan Houwaart's
 * framework.
 * 
 * @details These comparison functions are provided to promote the configuration objects
 * from semi-regular types to regular types, adhering to the value semantics of C++.
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
#ifndef CFG_CONFIG_CORE_CONFIG_COMPARISON_H
#define CFG_CONFIG_CORE_CONFIG_COMPARISON_H

// warning: implicitly includes 'all.h'
#include <LowPowerFramework/Low_power_framework.hpp>
#include <Sensor/Sensor.hpp>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Compares two BMX160 configuration objects for (in)equality.
 * 
 * @param[in] lhs Configuration object on the left-hand side of the operator.
 * @param[in] rhs Configuration object on the right-hand side of the operator.
 * 
 * @return Two configuration object are considered to be equal when all of their
 * data-members match.
 * 
 * @{
 */
[[nodiscard]]
constexpr auto operator==(
    BMX160Config const& lhs,
    BMX160Config const& rhs
) -> bool {
    return lhs.measure_accelerometer == rhs.measure_accelerometer
        and lhs.measure_gyroscope == rhs.measure_gyroscope
        and lhs.measure_magnetometer == rhs.measure_magnetometer
        and lhs.low_power == rhs.low_power
        and lhs.detect_shocks == rhs.detect_shocks;
}

[[nodiscard]]
constexpr auto operator!=(
    BMX160Config const& lhs,
    BMX160Config const& rhs) -> bool
{ return not (lhs == rhs); }
/** @} */

/**
 * @brief Compares two BME280 configuration objects for (in)equality.
 * 
 * @param[in] lhs Configuration object on the left-hand side of the operator.
 * @param[in] rhs Configuration object on the right-hand side of the operator.
 * 
 * @return Two configuration object are considered to be equal when all of their
 * data-members match.
 * 
 * @{
 */
[[nodiscard]]
constexpr auto operator==(
    BME280Config const& lhs,
    BME280Config const& rhs
) -> bool {
    return lhs.measure_pressure == rhs.measure_pressure
        and lhs.measure_temperature == rhs.measure_temperature
        and lhs.measure_humidity == rhs.measure_humidity
        and lhs.low_power == rhs.low_power;
}

[[nodiscard]]
constexpr auto operator!=(
    BME280Config const& lhs,
    BME280Config const& rhs) -> bool
{ return not (lhs == rhs); }
/** @} */

/**
 * @brief Compares two VEML6030 configuration objects for (in)equality.
 * 
 * @param[in] lhs Configuration object on the left-hand side of the operator.
 * @param[in] rhs Configuration object on the right-hand side of the operator.
 * 
 * @return Two configuration object are considered to be equal when all of their
 * data-members match.
 * 
 * @{
 */
[[nodiscard]]
constexpr auto operator==(
    VEML6030Config const& lhs,
    VEML6030Config const& rhs
) -> bool {
    return lhs.measure_light == rhs.measure_light
        and lhs.low_power == rhs.low_power;
}

[[nodiscard]]
constexpr auto operator!=(
    VEML6030Config const& lhs,
    VEML6030Config const& rhs) -> bool
{ return not (lhs == rhs); }
/** @} */

/**
 * @brief Compares two LowPowerFrameworkConfig configuration objects for (in)equality.
 * 
 * @param[in] lhs Configuration object on the left-hand side of the operator.
 * @param[in] rhs Configuration object on the right-hand side of the operator.
 * 
 * @return Two configuration object are considered to be equal when all of their
 * data-members match.
 * 
 * @{
 */
[[nodiscard]]
constexpr auto operator==(
    LowPowerFrameworkConfig const& lhs,
    LowPowerFrameworkConfig const& rhs
) -> bool {
    return lhs.usb_detection == rhs.usb_detection
        and lhs.usb_detection_interval_ms == rhs.usb_detection_interval_ms
        and lhs.bmx160 == rhs.bmx160
        and lhs.bme280 == rhs.bme280
        and lhs.veml6030 == rhs.veml6030
        and lhs.trigger == rhs.trigger;
}

[[nodiscard]]
constexpr auto operator!=(
    LowPowerFrameworkConfig const& lhs,
    LowPowerFrameworkConfig const& rhs) -> bool
{ return not (lhs == rhs); }
/** @} */

} // namespace cfg

#endif

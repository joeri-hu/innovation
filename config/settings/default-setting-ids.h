/**
 * @file default-setting-ids.h
 * @brief Setting identifiers which are needed by the default settings. 
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
#ifndef CFG_CONFIG_SETTINGS_DEFAULT_SETTING_IDS_H
#define CFG_CONFIG_SETTINGS_DEFAULT_SETTING_IDS_H

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @enum setting_identifier
 * 
 * @brief Enumeration of the default settings identifiers.
 * 
 * @details These setting identifiers are strongly coupled to the default settings. This
 * enumeration should be updated accordingly when adding a new default setting.
 */
enum class setting_identifier {
    unspecified,                          /**< Default setting identifier. */
    device_name,                          /**< Controls the internal name of the AEther. */
    usb_detection,                        /**< Controls the USB detection mode. */
    usb_interval_ms,                      /**< Controls the interval of the USB detection. */
    time_trigger_enabled,                 /**< Controls the time trigger. */
    time_trigger_interval,                /**< Controls time triggered measurement interval (in ms). */
    time_trigger_thp,                     /**< Controls time triggered temp, hum and pres measurements. */
    time_trigger_acc_gyro,                /**< Controls time triggered accel and gyro measurements. */
    time_trigger_magnetometer,            /**< Controls time triggered magnetometer measurement. */
    time_trigger_light_intensity,         /**< Controls time triggered light intensity measurement. */
    time_trigger_lora_priority,           /**< Controls time trigger's lorawan priority. */
    time_trigger_write_to_lora,           /**< Controls time triggered sending with lora. */
    time_trigger_write_to_sd,             /**< Controls time triggered writing to sd-card/log. */
    light_trigger_enabled,                /**< Controls the light trigger. */
    light_trigger_low_threshold,          /**< Controls light trigger low threshold (in measured lux). */
    light_trigger_high_threshold,         /**< Controls light trigger high threshold (in measured lux). */
    light_trigger_thp,                    /**< Controls light triggered temp, hum and pres measurements. */
    light_trigger_acc_gyro,               /**< Controls light triggered accel and gyro measurements. */
    light_trigger_magnetometer,           /**< Controls light triggered magnetometer measurement. */
    light_trigger_light_intensity,        /**< Controls light triggered light intensity measurement. */
    light_trigger_lora_priority,          /**< Controls light trigger's lorawan priority. */
    light_trigger_write_to_lora,          /**< Controls light triggered sending with lora. */
    light_trigger_write_to_sd,            /**< Controls light triggered writing to sd-card/log. */
    acceleration_trigger_enabled,         /**< Controls the acceleration trigger. */
    acceleration_trigger_thp,             /**< Controls acceleration triggered temp, hum and pres measurements. */
    acceleration_trigger_acc_gyro,        /**< Controls acceleration triggered accel and gyro measurements. */
    acceleration_trigger_magnetometer,    /**< Controls acceleration triggered magnetometer measurement. */
    acceleration_trigger_light_intensity, /**< Controls acceleration triggered light intensity measurement. */
    acceleration_trigger_lora_priority,   /**< Controls acceleration trigger's lorawan priority. */
    acceleration_trigger_write_to_lora,   /**< Controls acceleration triggered sending with lora. */
    acceleration_trigger_write_to_sd,     /**< Controls acceleration triggered writing to sd-card/log. */
    orientation_trigger_enabled,          /**< Controls the orientation trigger. */
    orientation_trigger_thp,              /**< Controls orientation triggered temp, hum and pres measurements. */
    orientation_trigger_acc_gyro,         /**< Controls orientation triggered accel and gyro measurements. */
    orientation_trigger_magnetometer,     /**< Controls orientation triggered magnetometer measurement. */
    orientation_trigger_light_intensity,  /**< Controls orientation triggered light intensity measurement. */
    orientation_trigger_lora_priority,    /**< Controls orientation trigger's lorawan priority. */
    orientation_trigger_write_to_lora,    /**< Controls orientation triggered sending with lora. */
    orientation_trigger_write_to_sd       /**< Controls orientation triggered writing to sd-card/log. */
};

} // namespace cfg

#endif

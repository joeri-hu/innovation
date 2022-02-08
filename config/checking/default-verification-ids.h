/**
 * @file default-verification-ids.h
 * @brief Verification identifiers which are needed by the verification rules.
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
#ifndef CFG_CONFIG_CHECKING_DEFAULT_VERIFICATION_IDS_H
#define CFG_CONFIG_CHECKING_DEFAULT_VERIFICATION_IDS_H

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @enum verification_identifier
 *
 * @brief Enumeration of the default verification identifiers.
 *
 * @details These verification identifiers are strongly coupled to the default
 * verification rules. This enumeration should be updated accordingly when adding a new
 * default verification rule.
 */
enum class verification_identifier {
    unspecified,          /**< Default verification identifier. */
    trigger_requirement,  /**< Verifies at least one type of trigger is enabled. */
    time_trigger,         /**< Verifies the time trigger will generate output. */
    light_trigger,        /**< Verifies the light trigger will generate output. */
    acceleration_trigger, /**< Verifies the acceleration trigger will generate output. */
    orientation_trigger   /**< Verifies the orientation trigger will generate output. */
};

} // namespace cfg

#endif

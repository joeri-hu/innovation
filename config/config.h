/**
 * @file config.h
 * @brief System for processing configuration files.
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
#ifndef CFG_CONFIG_H
#define CFG_CONFIG_H

#include "core/config-handler.h"
#include "core/main-config.h"
#include "errors/error-handler.h"
#include "errors/error-messages.h"
#include "parsing/xml-parser.h"
#include "parsing/message-parser.h"
#include "strings/zstring-view.h"
#include "utilities/file-io.h"

// warning: implicitly includes 'all.h'
#include <logger.hpp>
#include <Framework/AEtherData.h>

#include <array>
#include <cstdio>
#include <string_view>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Processes configuration files or messages.
 * 
 * @details Configurations are processed with the use of a configuration-handler.
 * Ultimately, a config file or config message is converted to a main-configuration
 * object. The name 'main-configuration' is unfortunately somewhat ambiguous but it is
 * used to refer to the data-structure that the program uses internally to control
 * various other systems.
 * 
 * The config-handler is responsible for parsing the contents of a config file or
 * message, and mapping the parsed values to its container of settings. Each setting is
 * then validated by invoking its validation function. When passing the validation, its
 * (potentially converted) value is then applied to the main-config object. Any potential
 * parsing or validation error is written a log file.
 * 
 * To ensure that no misconfiguration will render the device useless (e.g. having no
 * destination selected for its accumalated data), the main-config object will be
 * verified as a final step. If the main-config object did not pass verification, it will
 * be reset to its default values. Matching verification errors will be logged and the
 * status indicator will be used to indicate some failure occurred.
 * 
 * @tparam ConfigHandler Type of the config-handler.
 * @tparam ConfigData Type of the config file or message.
 * 
 * @param[in] cfg_handler Config-handler that processes config files or messages.
 * @param[in] data Config file or message that needs to be processed.
 * 
 * @return Main-config object used for controlling various internal systems.
 */
template<typename ConfigHandler, typename ConfigData>
auto process_config(ConfigHandler&& cfg_handler, ConfigData const& data) -> main_config {
    cfg_handler.process_config(data);
    if (cfg_handler.has_config_errors()) {
        aether_log << "[ERROR]Config could not be fully processed.\n";
        cfg_handler.report_config_errors();
        cfg_handler.set_status_indicator(StatusIndicator::failure);
    } else {
        aether_log << "[INFO]Config processed successfully!\n";
        auto const verification = cfg_handler.verify_main_config();
        if (verification.contains_errors()) {
            verification.log_errors("[ERROR]Active config did not pass verification:\n");
            cfg_handler.reset_main_config();
            cfg_handler.set_status_indicator(StatusIndicator::failure);
        } else {
            aether_log << "[INFO]Active config passed verification!\n";
        }
    }
    return cfg_handler.get_main_config();
}

/**
 * @brief Processes configuration messages.
 * 
 * @details This function creates a config-handler that consists of a message parser. The
 * message parser processes config messages in the form of raw data (bytes) and maps
 * them to the corresponding value of each setting. For more details, refer to the @ref
 * process_config function.
 * 
 * The functions lora::wait_for_message and lora::receive could be used to obtain data
 * from LoRaWAN. At least, it appears that way, we haven't actually tested anything that
 * is related to receiving data via LoRaWAN due to time constraints. The data can then be
 * converted to a main-config object with this function, as in:
 * 
 * auto main_cfg = process_config_message({
 *     reinterpret_cast<std::byte*>(payload.Buffer), payload.BufferSize});
 * 
 * @param[in] message Contains a pointer to an array of bytes that resembles a config
 * message, and a message size. The size of the message is checked to ensure that the
 * bitspan of each setting refers to some bits within the bounds of the config message.
 * 
 * @return Main-config object used for controlling various internal systems.
 */
inline auto process_config_message(message_data message) -> main_config
{ return process_config(config_handler<message_parser>{}, message); }

/**
 * @brief Processes configuration files.
 * 
 * @details This function loads the given configuration file from disk and uses a config-
 * handler that consists of an XML parser to parse its contents. For more details, refer
 * to the @ref process_config function.
 * 
 * @warning This function consumes a large chunk of available stack space to be able to
 * load the given config file. Be aware of invoking it when most of the stack is already
 * being used for some other purpose.
 * 
 * @param[in] filename Name of the configuration file to process.
 * 
 * @return Main-config object used for controlling various internal systems.
 */
inline auto process_config_file(zstring_view filename) -> main_config {
    auto const max_size = 3'072;
    std::array<char, max_size> config_file;
    auto const [bytes_read, file_error] = load_file(filename, max_size, config_file);

    if (file_error) {
        std::array<char, 128> message;
        std::sprintf(message.data(),
            "[ERROR]Config-file '%s' could not be loaded: %s\n",
            filename.size() > 32 ? "" : filename.data(),
            get_error_message(*file_error)
        );
        aether_log << message.data();
        auto main_cfg = main_config{};
        main_cfg.framework.status = StatusIndicator::failure;
        return main_cfg;
    }
    return process_config(
        config_handler<xml_parser>{},
        std::string_view{config_file.data(), bytes_read}
    );
}

} // namespace cfg

#endif

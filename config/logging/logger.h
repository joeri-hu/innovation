/**
 * @file logger.h
 * @brief Component for recording information to a log file.
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
#ifndef CFG_CONFIG_LOGGING_LOGGER_H
#define CFG_CONFIG_LOGGING_LOGGER_H

#include <ffconf.h>
// warning: implicitly includes 'all.h'
#include <logger.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <string_view>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Makes a new logger object that writes its output to a file with the given name
 * and suffix.
 *
 * @details The name and suffix of the logger are combined into a single filename. When
 * the size of the name and suffix exceed the maximum file size (i.e.: @p FF_MAX_LFN, a
 * macro defined in @ref ffconf.h), the suffix takes precedence. If the name and suffix
 * are empty, a default logger object is constructed. Refer to the documentation of the
 * @ref ::logger class for more details.
 * 
 * @warning Using a @ref ::logger initalized with a size of zero causes undefined behavior.
 *  
 * @param[in] name Name of the file to write logging information to.
 * @param[in] suffix Extension of the filename.
 * 
 * @return New logger object.
 */
[[nodiscard]]
inline auto make_logger(std::string_view name, std::string_view suffix) -> ::logger {
    if (name.empty() and suffix.empty()) return {};

    auto const max_filename_size = std::size_t{FF_MAX_LFN};
    std::array<char, max_filename_size> filename;

    auto const suffix_size = std::min(suffix.size(), max_filename_size - 1);
    auto const name_size = std::min(name.size(), max_filename_size - suffix_size - 1);

    auto filename_end = std::copy_n(name.cbegin(), name_size, filename.begin());
    filename_end = std::copy_n(suffix.cbegin(), suffix_size, filename_end);
    *filename_end = '\0';

    auto const filename_size = std::distance(filename.begin(), std::next(filename_end));
    return {filename.data(), static_cast<std::size_t>(filename_size)};
}

/**
 * @struct logger
 * 
 * @deprecated This class is intended to be superseded by a more complete logger.
 * @{
 */
template<typename Logger>
struct logger {
    template<typename T>
    [[deprecated]]
    friend constexpr auto operator<<(logger const& loggr, T msg) -> logger const& {
        loggr.log << msg;
        return loggr;
    }

    Logger& log;
};

template<typename Logger>
logger(Logger) -> logger<Logger>;
/** @} */

} // namespace cfg

#endif

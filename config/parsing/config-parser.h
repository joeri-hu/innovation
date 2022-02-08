/**
 * @file config-parser.h
 * @brief Interface for parser-implementations.
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
#ifndef CFG_CONFIG_PARSING_CONFIG_PARSER_H
#define CFG_CONFIG_PARSING_CONFIG_PARSER_H

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @interface config_parser
 * 
 * @brief Templated interface for parsing formatted data, following the CRTP-pattern.
 * 
 * @tparam SubParser Derived class which implements the public interface functions.
 */
template<typename SubParser>
class config_parser {
public:
    /**
     * @brief Parses formatted data from a configuration file or message.
     * 
     * @tparam Config Type of the configuration data.
     * 
     * @param[in] config Configuration data to parse.
     */
    template<typename ConfigData>
    constexpr auto parse_config(ConfigData const& config) -> void
    { dispatch().parse_config_impl(config); }

    /**
     * @brief Checks if any parsing error has occurred during the parsing process.
     */
    [[nodiscard]]
    constexpr auto has_parsing_errors() const -> bool
    { return dispatch().has_parsing_errors_impl(); }

    /**
     * @brief Reports any error that might have occurred during the parsing of a
     * configuration file.
     */
    auto report_parsing_errors() const -> void
    { dispatch().report_parsing_errors_impl(); }

protected:
    /**
     * @brief Special member functions.
     * 
     * @details To prevent unintentional slicing, these special member functions are
     * given protected access.
     * 
     * @{
     */
    constexpr config_parser() = default;
    constexpr config_parser(config_parser const&) = default;
    constexpr config_parser(config_parser&&) = default;
    constexpr auto operator=(config_parser const&) -> config_parser& = default;
    constexpr auto operator=(config_parser&&) -> config_parser& = default;
    ~config_parser() = default;
    /** @} */

private:
    /**
     * @{
     * @brief Dispatches function calls to the derived sub-parser.
     */
    constexpr auto dispatch() -> SubParser&
    { return static_cast<SubParser&>(*this); }

    constexpr auto dispatch() const -> SubParser const&
    { return static_cast<SubParser const&>(*this); }
    /** @} */
};

} // namespace cfg

#endif

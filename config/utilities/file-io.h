/**
 * @file file-io.h
 * @brief File input/output related utility functions/variables/aliases.
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
#ifndef CFG_CONFIG_UTILITIES_FILE_IO_H
#define CFG_CONFIG_UTILITIES_FILE_IO_H

#include <errors/error-types.h>
#include <strings/zstring-view.h>
#include <traits/class-traits.h>

// warning: implicitly includes 'all.h'
#include <sdcard.hpp>
#include <FatFs/src/ff.h>

#include <cstddef>
#include <optional>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @struct io_result
 * 
 * @brief Data type that is returned by the @ref load_file function.
 * 
 * @details An I/O result constists of a @p bytes_read data-member and an optional
 * I/O error. Success is indicated by an empty I/O error.
 */
struct io_result {
    /**
     * @brief Compares two file I/O results for (in)equality.
     * 
     * @param[in] lhs File I/O result on the left-hand side of the operator.
     * @param[in] rhs File I/O result on the right-hand side of the operator.
     * 
     * @return Two file I/O results are considered to be equal when both their number of
     * bytes read and optional I/O error matches.
     * 
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(
        io_result const& lhs, io_result const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(
        io_result const& lhs, io_result const& rhs) -> bool
    { return lhs.bytes_read == rhs.bytes_read and lhs.error == rhs.error; }
    /** @} */

    std::uint_least32_t bytes_read; /**< Indicates the number of bytes read. */
    std::optional<io_error> error;  /**< Optional I/O error. */
};

/**
 * @brief Loads a file from the SD-card.
 * 
 * @tparam Container Container type that stores its contents in a contiguous sequence.
 * 
 * @param[in] filename Name of the file to load.
 * @param[out] buffer Container that will store the contents of the loaded file.
 * @param[in] buffer_size Capacity of the buffer.
 * 
 * @return If the file was loaded successfully, returns the number of bytes read.
 * Otherwise, a value of zero is returned.
 */
template<typename Container,
    typename = std::enable_if_t<is_contiguous_container_v<Container>>>
auto load_file(zstring_view filename, std::size_t buffer_size, Container& buffer)
-> io_result {
    struct perilogue {
        perilogue()  { SDCard_Clock_Config(); sd_card::init(); }
        ~perilogue() { sd_card::sleep(); SDCard_Clock_Config(); }
    } const file_io;

    auto bytes_read = unsigned{};
    auto status = sd_card::read_chars(
        std::data(buffer), buffer_size, filename.data(), &bytes_read);

    if (status != FR_OK)
        return {bytes_read, static_cast<io_error>(status)};
    if (bytes_read < buffer_size)
        return {bytes_read, std::nullopt};

    auto file_size = std::uint32_t{};
    status = sd_card::get_filesize(filename.data(), file_size);

    if (status != FR_OK)
        return {bytes_read, static_cast<io_error>(status)};
    if (file_size > buffer_size)
        return {bytes_read, io_error::file_too_large};

    return {bytes_read, std::nullopt};
}

} // namespace cfg

#endif

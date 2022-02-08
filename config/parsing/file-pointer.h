/**
 * @file file-pointer.h
 * @brief File pointer that points to a position within a file.
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
#ifndef CFG_CONFIG_PARSING_FILE_PTR_H
#define CFG_CONFIG_PARSING_FILE_PTR_H

#include <cstdint>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @class file_pointer
 * 
 * @brief Tracks the position of a pointer within a file with the use of a column and
 * line number.
 * 
 * @tparam T Type of the column and line number, which is required to be an integral
 * type.
 */
template<typename T,
    typename = std::enable_if_t<std::is_integral_v<T>>>
class file_pointer {
public:
    /**
     * @brief Constructs a default-initialized file-pointer.
     * 
     * @details Initializes the column and line number to the value defined by the
     * 'begin_pos' constant.
     */
    constexpr file_pointer() = default;

    /**
     * @brief Constructs a file-pointer with a given column and line number.
     * 
     * @param[in] column_number Column number to initialize the file-pointer with.
     * @param[in] line_number Line number to initialize the file-pointer with.
     */
    constexpr file_pointer(T column_number, T line_number)
        : col_nr{column_number}, line_nr{line_number} {}

    /**
     * @brief Advances the column number to the next position.
     */
    constexpr auto next_column() -> void
    { increase_columns(); }

    /**
     * @brief Advances the line number to the next position and resets the column number.
     */
    constexpr auto next_line() -> void {
        increase_lines();
        reset_columns();
    }

    /**
     * @brief Resets the column and line number.
     * 
     * @details The column and line numbers are set to the value defined by the
     * 'begin_pos' constant.
     */
    constexpr auto reset() -> void {
        reset_columns();
        reset_lines();
    }

    /**
     * @brief Gets the column number.
     * 
     * @return Column number.
     */
    [[nodiscard]]
    constexpr auto column() const -> T
    { return col_nr; }

    /**
     * @brief Gets the line number.
     * 
     * @return Line number.
     */
    [[nodiscard]]
    constexpr auto line() const -> T
    { return line_nr; }

    /**
     * @brief Compares two file-pointers for (in)equality.
     * 
     * @param[in] lhs File-pointer on the left-hand side of the operator.
     * @param[in] rhs File-pointer on the right-hand side of the operator.
     * 
     * @return Two file-pointers are considered to be equal when their column and line
     * number matches.
     * 
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(
        file_pointer const& lhs, file_pointer const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(
        file_pointer const& lhs,
        file_pointer const& rhs
    ) -> bool {
        return lhs.col_nr == rhs.col_nr
            and lhs.line_nr == rhs.line_nr;
    }
    /** @} */

private:
    /**
     * @brief Increases the column number with a given step-count.
     * 
     * @details The default vLaue of the step-count is one.
     * 
     * @param[in] step Number of positions to increase the column number with.
     */
    constexpr auto increase_columns(T step = 1) -> void
    { col_nr += step; }

    /**
     * @brief Increases the line number with a given step-count.
     * 
     * @details The default value of the step-count is one.
     * 
     * @param[in] step Number of positions to increase the line number with.
     */
    constexpr auto increase_lines(T step = 1) -> void
    { line_nr += step; }

    /**
     * @brief Resets the column number to the value defined by the 'begin_pos' constant.
     */
    constexpr auto reset_columns() -> void
    { col_nr = begin_pos; }

    /**
     * @brief Resets the line number to the value defined by the 'begin_pos' constant.
     */
    constexpr auto reset_lines() -> void
    { line_nr = begin_pos; }

    static constexpr auto begin_pos = T{1}; /**< Initial file-pointer position. */
    T col_nr{begin_pos};                    /**< Tracks the column number. */
    T line_nr{begin_pos};                   /**< Tracks the line number. */
};

/**
 * @brief Aliases for common file-pointer types.
 * 
 * @{
 */
using file_ptr   = file_pointer<int>;
using file_ptr8  = file_pointer<std::int8_t>;
using file_ptr16 = file_pointer<std::int16_t>;
using file_ptr32 = file_pointer<std::int32_t>;
using file_ptr64 = file_pointer<std::int64_t>;
/** @} */

} // namespace cfg

#endif

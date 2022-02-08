/**
 * @file zstring-view.h
 * @brief View of a zero-terminated contiguous sequence of characters.
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
#ifndef CFG_CONFIG_STRINGS_ZSTRING_VIEW_H
#define CFG_CONFIG_STRINGS_ZSTRING_VIEW_H

#include <string_view>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @struct zstring_view
 * 
 * @brief Provides a view to a zero-terminated string.
 * 
 * @details Zero-terminated string views are implemented in terms of std::string_views
 * and are designed to seamlessly interface with C-string APIs. They are intended to be
 * constructed only from C-strings or other string views that are zero-terminated.
 * Opposed to standard string views, this guarantees that the data() member-function
 * returns a pointer to an underlying character array which is zero-terminated.
 */
struct zstring_view : private std::string_view {
    /**
     * @typedef base_type
     * 
     * @brief Shorter notation to refer to the type of the base class.
     */
    using base_type = std::string_view;

    /**
     * @brief Provides basic type information.
     * 
     * @{
     */
    using typename base_type::value_type;
    using typename base_type::size_type;
    using typename base_type::reference;
    using typename base_type::const_reference;
    using typename base_type::iterator;
    using typename base_type::const_iterator;
    /** @} */

    /**
     * @brief Provides element access.
     * 
     * @{
     */
    using base_type::operator[];
    using base_type::front;
    using base_type::back;
    using base_type::data;
    /** @} */

    /**
     * @brief Provides iterator operations.
     * 
     * @{
     */
    using base_type::begin;
    using base_type::end;
    using base_type::cbegin;
    using base_type::cend;
    /** @} */

    /**
     * @brief Provides capacity operations.
     * 
     * @{
     */
    using base_type::size;
    using base_type::empty;
    /** @} */

    /**
     * @brief Provides common operations.
     * 
     * @{
     */
    using base_type::copy;
    using base_type::substr;
    using base_type::compare;
    using base_type::find;
    using base_type::find_first_of;
    using base_type::find_last_of;
    using base_type::npos;
    /** @} */

    /**
     * @brief Default-constructs a zstring-view object.
     */
    constexpr zstring_view() = default;

    /**
     * @brief Constructs a zero-terminated string view from a zero-terminated C-string.
     * 
     * @details This constructor allows implicit conversions from zero-terminated
     * C-strings to zero-terminated string views.
     * 
     * @param[in] zstring Pointer to a zero-terminated C-string.
     */
    constexpr zstring_view(char const* zstring)
        : base_type(zstring) {}

    /**
     * @brief Constructs a zero-terminated string view with a specific size from a
     * zero-terminated C-string.
     * 
     * @param[in] zstring Pointer to a zero-terminated C-string.
     * @param[in] size Size of the zero-terminated string view.
     */
    constexpr zstring_view(char const* zstring, size_type size)
        : base_type(zstring, size) {}

    /**
     * @brief Constructs a zero-terminated string view from a standard string view that
     * should refer to a zero-terminated string.
     * 
     * @param[in] str_view Standard string view of a zero-terminated string.
     */
    constexpr explicit zstring_view(base_type str_view)
        : base_type(str_view) {}

    /**
     * @brief Converts a zero-terminated string view to a standard string view.
     * 
     * @return Standard string view.
     */
    [[nodiscard]]
    constexpr auto to_string_view() const -> base_type
    { return {data(), size()}; }

    /**
     * @brief Compares two zero-terminated string views for (in)equality.
     * 
     * @param[in] lhs Zero-terminated string view on the left-hand side of the operator.
     * @param[in] rhs Zero-terminated string view on the right-hand side of the operator.
     * 
     * @return Two zero-terminated string view are considered to be equal when they are
     * of equal size and of all their underlying characters match.
     * 
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(
        zstring_view const& lhs, zstring_view const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(
        zstring_view const& lhs, zstring_view const& rhs) -> bool
    { return static_cast<base_type>(lhs) == static_cast<base_type>(rhs); }
    /** @} */
};

} // namespace cfg

#endif

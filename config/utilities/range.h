/**
 * @file range.h
 * @brief Component for interacting with a range of elements.
 * 
 * @version 1.0
 * @date December 2021
 * 
 * @authors INNO Project-group (Semester A+B, 2021)
 * @author Joeri Kok (joeri.j.kok@student.hu.nl)
 * @author Rick Horeman (rick.horeman@student.hu.nl)
 * @author Richard Janssen (richard.janssen@student.hu.nl)
 * @author Tim Hardeman (tim.hardeman@student.hu.nl)
 * @author Koen Eijkelenboom (koen.eijkelenboom@student.hu.nl)
 * 
 * @copyright GPL-3.0 License
 */
#ifndef CFG_CONFIG_UTILITIES_RANGE_H
#define CFG_CONFIG_UTILITIES_RANGE_H

#include <traits/iterator-traits.h>

#include <cstdint>
#include <iterator>
#include <tuple>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @class range
 * 
 * @brief Provides an abstraction to interact with a range of elements.
 * 
 * @details A range consists of two iterators of the same type.
 * 
 * @tparam Iter Type of the iterators.
 */
template<typename Iter,
    typename = std::enable_if_t<is_iterator_v<Iter>>>
class range {
public:
    /**
     * @typedef iterator
     * 
     * @brief Type of the iterators.
     */
    using iterator = Iter;

    /**
     * @typedef difference_type
     * 
     * @brief Type of the difference between the first and last iterator.
     */
    using difference_type = typename std::iterator_traits<Iter>::difference_type;

    /**
     * @typedef value_type
     * 
     * @brief Type of the value pointed to by an iterator.
     */
    using value_type = typename std::iterator_traits<Iter>::value_type;

    /**
     * @typedef pointer
     * 
     * @brief Type of the pointer to a value type.
     */
    using pointer = typename std::iterator_traits<Iter>::pointer;

    /**
     * @typedef reference
     * 
     * @brief Type of the reference to a value type.
     */
    using reference = typename std::iterator_traits<Iter>::reference;

    /**
     * @typedef const_reference
     * 
     * @brief Type of the reference to a constant value type.
     */
    using const_reference = value_type const&;

    /**
     * @brief Default-constructs a range.
     * 
     * @details The iterators are either default or value-initialized and could contain a
     * singular value.
     */
    constexpr range() = default;

    /**
     * @brief Constructs a range from two given iterators.
     */
    constexpr range(Iter begin, Iter end)
        : first{begin}, last{end} {}

    /**
     * @brief Constructs a range from a given container that can be modified.
     * 
     * @details Initializes the first and last iterator to the beginning and end of the
     * given container, respectively.
     * 
     * @tparam Container Type of the container to obtain its range from.
     */
    template<typename Container>
    constexpr explicit range(Container& container):
        first{[&]{ using std::begin; return begin(container); }()},
        last{[&]{ using std::end; return end(container); }()}
    {}

    /**
     * @brief Constructs a range from a given container that cannot be modified.
     * 
     * @details Initializes the first and last iterator to the beginning and end of the
     * given container, respectively.
     * 
     * @tparam Container Type of the container to obtain its range from.
     */
    template<typename Container>
    constexpr explicit range(Container const& container):
        first{[&]{ using std::cbegin; return cbegin(container); }()},
        last{[&]{ using std::cend; return cend(container); }()}
    {}

    /**
     * @brief Prohibits the construction of a range that belongs to an expiring
     * container.
     * 
     * @tparam Container Type of the container to obtain its range from.
     */
    template<typename Container>
    constexpr explicit range(Container&& container) = delete;

    /**
     * @brief Gets the distance of the range.
     * 
     * @return Number of steps that it takes to go from first to last.
     */
    [[nodiscard]]
    constexpr auto distance() const -> difference_type
    { return std::distance(first, last); }

    /**
     * @brief Enumerates the first and last iterator with a count.
     * 
     * @tparam Count Type of the iterations count. The default is std::uint16_t.
     * 
     * @param[in] start Value from which the count starts. The default value is either
     * default or zero-initialized, depending on the count type.
     * 
     * @return Tuple consisting of the first and last iterator with a count initialized
     * to the start value, respectively.
     */
    template<typename Count = std::uint16_t>
    [[nodiscard]]
    constexpr auto enumerate(Count start = {}) const -> std::tuple<Iter, Iter, Count>
    { return {first, last, start}; }

    /**
     * @brief Gets the first iterator, pointing to the beginning of the range.
     * 
     * @return Copy of the first iterator.
     */
    constexpr auto begin() const -> Iter
    { return first; }

    /**
     * @brief Gets the last iterator, pointing to the end of the range.
     * 
     * @return Copy of the last iterator.
     */
    constexpr auto end() const -> Iter
    { return last; }

    /**
     * @brief Accesses an element at the given index.
     * 
     * @details The index to access is relative to the beginning of the range. This
     * function does not perform any bounds checking.
     * 
     * @tparam Index Type of the index, which is required to be an integral type.
     * 
     * @param[in] index Index of the element to access.
     * 
     * @{
     * @return Reference to an element that can be modified.
     */
    template<typename Index,
        typename = std::enable_if_t<std::is_integral_v<Index>>>
    constexpr auto operator[](Index index) -> reference
    { return access(this, index); }

    /**
     * @return Reference to an element that cannot be modified.
     */
    template<typename Index,
        typename = std::enable_if_t<std::is_integral_v<Index>>>
    constexpr auto operator[](Index index) const -> const_reference
    { return access(this, index); }
    /** @} */

    /**
     * @brief Compares two ranges for (in)equality.
     * 
     * @param[in] lhs Range on the left-hand side of the operator.
     * @param[in] rhs Range on the right-hand side of the operator.
     * 
     * @return Ranges are considered to be equal when both of their iterators match.
     * 
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(
        range const& lhs, range const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(
        range const& lhs, range const& rhs) -> bool
    { return lhs.first == rhs.first and lhs.last == rhs.last; }
    /** @} */

private:
    /**
     * @brief Accesses an element at the given index.
     * 
     * @details Provides the shared implementation of both const and non-const qualified
     * subscript operators.
     * 
     * @tparam Range Type of the const or non-const qualified range.
     * @tparam Index Integral type of the index.
     * 
     * @param[in] ptr Pointer to the range of which to access an element.
     * @param[in] idx Index of the element to access.
     * 
     * @return Reference to an element.
     */
    template<typename Range, typename Index>
    static constexpr auto& access(Range ptr, Index idx) {
        auto iter = ptr->begin();
        std::advance(iter, idx);
        return *iter;
    }

    Iter first{}; /**< Iterator that points to the beginning of the range. */
    Iter last{};  /**< Iterator that points to the end of the range. */
};

/**
 * @remark Allows a range to be constructed from a modifiable container type.
 * 
 * @tparam Container Container type with an iterator member-type.
 */
template<typename Container>
range(Container&) -> range<iterator_type<Container>>;

/**
 * @remark Allows a range to be constructed from a non-modifiable container type.
 * 
 * @tparam Container Container type with an iterator member-type.
 */
template<typename Container>
range(Container const&) -> range<const_iterator_type<Container>>;

} // namespace cfg

#endif

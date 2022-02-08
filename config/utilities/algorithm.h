/**
 * @file algorithm.h
 * @brief Algorithms for operating on ranges of elements.
 * 
 * @details Most of the algorithms defined within this file simply provide a compile-time
 * alternative to the standard library algorithms. The implementations of these
 * algorithms are mainly based on the examples provided by the site cppreference.com. For
 * all of these algorithms, there is a function overload that interacts with containers
 * instead of iterators.
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
#ifndef CFG_CONFIG_UTILITIES_ALGORITHM_H
#define CFG_CONFIG_UTILITIES_ALGORITHM_H

#include <traits/iterator-traits.h>

#include <algorithm>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Copies a range of elements from one container to another.
 * 
 * @details Refer to the documentation of std::copy on cppreference.com for more
 * information.
 * 
 * @tparam InputIt Input-iterator type of the source container.
 * @tparam OutputIt Output-iterator type of the destination container.
 * 
 * @param[in] first Iterator to the beginning of the source range.
 * @param[in] last Iterator to the end of the source range.
 * @param[out] first_dest Iterator to the beginning of the destination range.
 * 
 * @return Iterator to one past the last copied element of the destination range.
 */
template<typename InputIt, typename OutputIt,
    typename = std::enable_if_t<is_input_iter_v<InputIt>>,
    typename = std::enable_if_t<is_iterator_v<OutputIt>>>
constexpr auto copy(InputIt first, InputIt last, OutputIt first_dest) -> OutputIt {
    while (first != last) {
        *first_dest++ = *first++;
    }
    return first_dest;
}

/**
 * @brief Copies all elements from one container to another.
 * 
 * @details This function acts as a helper-function of the @ref copy function that
 * operates on iterators. It takes the required iterators from the source and destination
 * containers and passes them along.
 * 
 * @tparam InputCont Type of the source container.
 * @tparam OutputCont Type of the destination container.
 * 
 * @param[in] source Source container to copy the elements from.
 * @param[out] dest Destination container to copy the elements to.
 * 
 * @return Iterator to one past the last copied element of the destination container.
 */
template<typename InputCont, typename OutputCont,
    typename = std::enable_if_t<has_input_iter_v<InputCont>>,
    typename = std::enable_if_t<has_iterator_v<OutputCont>>>
constexpr auto copy(InputCont const& source, OutputCont& dest) {
    using std::begin, std::end;
    return cfg::copy(begin(source), end(source), begin(dest));
}

/**
 * @brief Copies a given number of elements from one container to another.
 * 
 * @details Refer to the documentation of std::copy_n on cppreference.com for more
 * information.
 * 
 * @tparam InputIt Input-iterator type of the source container.
 * @tparam Size Size type of the element count.
 * @tparam OutputIt Output-iterator type of the destination container.
 * 
 * @param[in] first Iterator to the beginning of the source range.
 * @param[in] count Number of elements to copy from the source range.
 * @param[out] result Iterator to beginning of the destination range.
 * 
 * @return Iterator to one past the last copied element of the destination range.
 */
template<typename InputIt, typename Size, typename OutputIt,
    typename = std::enable_if_t<is_input_iter_v<InputIt>>,
    typename = std::enable_if_t<is_iterator_v<OutputIt>>>
constexpr auto copy_n(InputIt first, Size count, OutputIt result) -> OutputIt {
    if (count > 0) {
        *result++ = *first;
        for (auto idx = Size{1}; idx < count; ++idx) {
            *result++ = *++first;
        }
    }
    return result;
}

/**
 * @brief Copies a given number of elements from the beginning of one container to the
 * beginning of another.
 * 
 * @details This function acts as a helper-function of the @ref copy_n function that
 * operates on iterators. It takes the required iterators from the source and destination
 * containers and passes them along.
 * 
 * @tparam InputCont Type of the source container.
 * @tparam Size Size type of the element count.
 * @tparam OutputCont Type of the destination container.
 * 
 * @param[in] source Source container to copy the elements from.
 * @param[in] count Number of elements to copy from the source container.
 * @param[out] dest Destination container to copy the elements to.
 * 
 * @return Iterator to one past the last copied element of the destination range.
 */
template<typename InputCont, typename Size, typename OutputCont,
    typename = std::enable_if_t<has_input_iter_v<InputCont>>,
    typename = std::enable_if_t<has_iterator_v<OutputCont>>>
constexpr auto copy_n(InputCont const& source, Size count, OutputCont& dest) {
    using std::begin;
    return cfg::copy_n(begin(source), count, begin(dest));
}

/**
 * @brief Copies a given number of elements from one container to another, setting the
 * last element to a null-terminator.
 * 
 * @details To ensure the resulting copy is always null-terminated, there are at most N-1
 * elements copied from the source range to the destination range. If the element count
 * is set to one, only a null-terminator will be copied to the destination range. The
 * null-terminator is either a default or zero-initialized element corresponding to the
 * value-type of the destination container.
 * 
 * @tparam InputIt Input-iterator type of the source container.
 * @tparam Size Size type of the element count.
 * @tparam OutputIt Output-iterator type of the destination container.
 * 
 * @param[in] first Iterator to the beginning of the source range.
 * @param[in] count Number of elements to copy to the destination range, including the
 * null-terminator.
 * @param[out] result Iterator to beginning of the destination range.
 * 
 * @return An iterator to the null-terminated element of the destination range, if the
 * element count is greater than zero. Otherwise, the result iterator is returned.
 */
template<typename InputIt, typename Size, typename OutputIt,
    typename = std::enable_if_t<is_input_iter_v<InputIt>>,
    typename = std::enable_if_t<is_iterator_v<OutputIt>>>
constexpr auto zcopy_n(InputIt first, Size count, OutputIt result) -> OutputIt {
    if (count > 0) {
        result = cfg::copy_n(first, count - 1, result);
        *result = typename std::iterator_traits<OutputIt>::value_type{};
    }
    return result;
}

/**
 * @brief Copies at most a given number of elements from the beginning of one container
 * to the beginning of another, setting the last element to a null-terminator.
 * 
 * @details The number of elements copied depends on the given count and the size of the
 * source container. This function determines which value is the lesser of the two, takes
 * the required iterators from the source and destination containers, and passes them
 * along to the @ref zcopy_n function.
 * 
 * @tparam InputCont Type of the source container.
 * @tparam Size Size type of the element count.
 * @tparam OutputCont Type of the destination container.
 * 
 * @param[in] source Source container to copy the elements from.
 * @param[in] count Maximum number of elements to copy to the destination range,
 * including the null-terminator.
 * @param[out] dest Destination container to copy the elements to.
 * 
 * @return An iterator to the null-terminated element of the destination range, if the
 * element count is greater than zero. Otherwise, the result iterator is returned.
 */
template<typename InputCont, typename Size, typename OutputCont,
    typename = std::enable_if_t<has_input_iter_v<InputCont>>,
    typename = std::enable_if_t<has_iterator_v<OutputCont>>>
constexpr auto zcopy_max(InputCont const& source, Size count, OutputCont& dest) {
    using std::begin, std::size;
    auto const count_max = std::min(size(source) + 1, count);
    return zcopy_n(begin(source), count_max, begin(dest));
}

/**
 * @brief Copies a given value to a range of elements within a container.
 * 
 * @details Refer to the documentation of std::fill on cppreference.com for more
 * information.
 * 
 * @tparam ForwardIt Forward-iterator type of the container to fill.
 * @tparam T Type of the value to fill the range of elements with.
 * 
 * @param[out] first Iterator to the beginning of the range of elements.
 * @param[out] last Iterator to the end of the range of elements.
 * @param[in] value Value that is copied to each element within the range.
 */
template<typename ForwardIt, typename T,
    typename = std::enable_if_t<is_forward_iter_v<ForwardIt>>>
constexpr auto fill(ForwardIt first, ForwardIt last, T const& value) -> void {
    for (; first != last; ++first) {
        *first = value;
    }
}

/**
 * @brief Copies a given value to all elements of a container.
 * 
 * @details This function acts as a helper-function of the @ref fill function that
 * operates on iterators. It takes the required iterators from the container to be filled
 * and passes them along.
 * 
 * @tparam ForwardCont Type of the container to fill.
 * @tparam T Type of the value to fill all of the elements with.
 * 
 * @param[out] result Container of the elements to fill.
 * @param[in] value Value that is copied to all of the elements.
 */
template<typename ForwardCont, typename T,
    typename = std::enable_if_t<has_forward_iter_v<ForwardCont>>>
constexpr auto fill(ForwardCont& result, T const& value) -> void {
    using std::begin, std::end;
    return cfg::fill(begin(result), end(result), value);
}

/**
 * @brief Copies a given value to a given number of elements within a container.
 * 
 * @details Refer to the documentation of std::fill_n on cppreference.com for more
 * information.
 * 
 * @tparam OutputIt Output-iterator type of the container to fill.
 * @tparam T Type of the value to fill the number of elements with.
 * 
 * @param[out] first Iterator to the beginning of the range of elements.
 * @param[in] count Number of elements to fill.
 * @param[in] value Value that is copied to the number of elements.
 * 
 * @return Iterator to one past the last copied element of the destination range.
 */
template<typename OutputIt, typename Size, typename T,
    typename = std::enable_if_t<is_iterator_v<OutputIt>>>
constexpr auto fill_n(OutputIt first, Size count, T const& value) -> OutputIt {
    for (auto idx = Size{}; idx < count; ++idx) {
        *first++ = value;
    }
    return first;
}

/**
 * @brief Copies a given value to a number of elements at the beginning of a container.
 * 
 * @details This function acts as a helper-function of the @ref fill_n function that
 * operates on iterators. It takes the required iterators from the container to be filled
 * and passes them along.
 * 
 * @tparam OutputCont Type of the container to fill.
 * @tparam T Type of the value to fill the number of elements with.
 * 
 * @param[out] result Container of the number of elements to fill.
 * @param[in] count Number of elements to fill.
 * @param[in] value Value that is copied to the number of elements.
 * 
 * @return Iterator to one past the last copied element of the destination range.
 */
template<typename OutputCont, typename Size, typename T,
    typename = std::enable_if_t<has_iterator_v<OutputCont>>>
constexpr auto fill_n(OutputCont& result, Size count, T const& value) {
    using std::begin;
    return cfg::fill_n(begin(result), count, value);
}

} // namespace cfg

#endif

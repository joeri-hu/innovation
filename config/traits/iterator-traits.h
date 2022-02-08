/**
 * @file iterator-traits.h
 * @brief Type traits for iterators.
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
#ifndef CFG_CONFIG_TRAITS_ITERATOR_TRAITS_H
#define CFG_CONFIG_TRAITS_ITERATOR_TRAITS_H

#include <iterator>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @typedef iterator_type
 * 
 * @brief Aliases the iterator member-type of a given type.
 * 
 * @tparam T Type of which to obtain its iterator member-type from.
 */
template<typename T>
using iterator_type = typename T::iterator;

/**
 * @typedef const_iterator_type
 * 
 * @brief Aliases the constant iterator member-type of a given type.
 * 
 * @tparam T Type of which to obtain its constant iterator member-type from.
 */
template<typename T>
using const_iterator_type = typename T::const_iterator;
/** @} */

/**
 * @namespace detail
 * 
 * @brief Provides helper/meta functions/types local to this header file.
 */
namespace detail {

/**
 * @brief Helper alias templates.
 * 
 * @{
 * 
 * @typedef category_type
 * 
 * @brief Aliases the iterator category member-type of a given iterator type.
 * 
 * @tparam Iter Iterator type of which to obtain its iterator category from.
 */
template<typename Iter>
using category_type
    = typename std::iterator_traits<Iter>::iterator_category;

/**
 * @remark Checks if a type (in)directly belongs to the iterator category based on the
 * given iterator tag type.
 * 
 * @tparam Iter Type to check.
 * @tparam Tag Type of the iterator tag.
 * 
 * @{
 */
template<typename Iter, typename Tag, typename = void>
struct is_iter_category : std::false_type {};

template<typename Iter, typename Tag>
struct is_iter_category<Iter, Tag, std::void_t<category_type<Iter>>>
    : std::is_convertible<category_type<Iter>, Tag> {};
/** @} */

/**
 * @remark Checks if a type, based on the result of the given type trait template, could
 * be iterated by some iterator type or has a member-type which could.
 * 
 * @tparam T Type to check.
 * @tparam Trait Template of a trait that should determine if a type is iterable in a
 * certain way.
 * 
 * @{
 */
template<typename T, template<typename...> typename Trait, typename = void>
struct is_iterable : std::false_type {};

template<typename T, template<typename...> typename Trait>
struct is_iterable<T, Trait, std::void_t<iterator_type<T>>>
    : Trait<iterator_type<T>> {};

template<typename T, template<typename...> typename Trait>
struct is_iterable<T, Trait, std::enable_if_t<std::is_array_v<T>>>
    : Trait<std::add_pointer_t<std::remove_extent_t<T>>> {};
/** @} */

} // namespace detail

/**
 * @brief Input iterator.
 * 
 * @{
 * 
 * @remark Checks if a type qualifies as an input iterator type.
 * 
 * @details A type is considered to be an input iterator type if its iterator category
 * (in)directly relates to the input iterator tag type.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct is_input_iter
    : detail::is_iter_category<T, std::input_iterator_tag> {};

/**
 * @var is_input_iter_v
 * 
 * @brief Helper variable template for the @ref is_input_iter type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_input_iter_v
    = bool{is_input_iter<T>{}};

/**
 * @remark Checks if a type could be iterated by or has a member-type that qualifies as
 * an input iterator type.
 * 
 * @details Refer to the @ref is_input_iter type trait for more details.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct has_input_iter
    : detail::is_iterable<T, is_input_iter> {};

/**
 * @var has_input_iter_v
 * 
 * @brief Helper variable template for the @ref has_input_iter type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto has_input_iter_v
    = bool{has_input_iter<T>{}};
/** @} */

/**
 * @brief Output iterator.
 * 
 * @{
 * 
 * @remark Checks if a type qualifies as an output iterator type.
 * 
 * @details A type is considered to be an output iterator type if its iterator category
 * (in)directly relates to the output iterator tag type.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct is_output_iter
    : detail::is_iter_category<T, std::output_iterator_tag> {};

/**
 * @var is_output_iter_v
 * 
 * @brief Helper variable template for the @ref is_output_iter type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_output_iter_v
    = bool{is_output_iter<T>{}};

/**
 * @remark Checks if a type could be iterated by or has a member-type that qualifies as
 * an output iterator type.
 * 
 * @details Refer to the @ref is_output_iter type trait for more details.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct has_output_iter
    : detail::is_iterable<T, is_output_iter> {};

/**
 * @var has_output_iter_v
 * 
 * @brief Helper variable template for the @ref has_output_iter type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto has_output_iter_v
    = bool{has_output_iter<T>{}};
/** @} */

/**
 * @brief Forward iterator.
 * 
 * @{
 * 
 * @remark Checks if a type qualifies as a forward iterator type.
 * 
 * @details A type is considered to be a forward iterator type if its iterator category
 * (in)directly relates to the forward iterator tag type.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct is_forward_iter
    : detail::is_iter_category<T, std::forward_iterator_tag> {};

/**
 * @var is_forward_iter_v
 * 
 * @brief Helper variable template for the @ref is_forward_iter type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_forward_iter_v
    = bool{is_forward_iter<T>{}};

/**
 * @remark Checks if a type could be iterated by or has a member-type that qualifies as a
 * forward iterator type.
 * 
 * @details Refer to the @ref is_forward_iter type trait for more details.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct has_forward_iter
    : detail::is_iterable<T, is_forward_iter> {};

/**
 * @var has_forward_iter_v
 * 
 * @brief Helper variable template for the @ref has_forward_iter type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto has_forward_iter_v
    = bool{has_forward_iter<T>{}};
/** @} */

/**
 * @brief Bidirectional iterator.
 * 
 * @{
 * 
 * @remark Checks if a type qualifies as a bidirectional iterator type.
 * 
 * @details A type is considered to be a bidirectional iterator type if its iterator
 * category (in)directly relates to the bidirectional iterator tag type.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct is_bidirectional_iter
    : detail::is_iter_category<T, std::bidirectional_iterator_tag> {};

/**
 * @var is_bidirectional_iter_v
 * 
 * @brief Helper variable template for the @ref is_bidirectional_iter type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_bidirectional_iter_v
    = bool{is_bidirectional_iter<T>{}};

/**
 * @remark Checks if a type could be iterated by or has a member-type that qualifies as a
 * bidirectional iterator type.
 * 
 * @details Refer to the @ref is_bidirectional_iter type trait for more details.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct has_bidirectional_iter
    : detail::is_iterable<T, is_bidirectional_iter> {};

/**
 * @var has_bidirectional_iter_v
 * 
 * @brief Helper variable template for the @ref has_bidirectional_iter type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto has_bidirectional_iter_v
    = bool{has_bidirectional_iter<T>{}};
/** @} */

/**
 * @brief Random-access iterator.
 * 
 * @{
 * 
 * @remark Checks if a type qualifies as a random-access iterator type.
 * 
 * @details A type is considered to be a random-access iterator type if its iterator
 * category (in)directly relates to the random-access iterator tag type.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct is_random_access_iter
    : detail::is_iter_category<T, std::random_access_iterator_tag> {};

/**
 * @var is_random_access_iter_v
 * 
 * @brief Helper variable template for the @ref is_random_access_iter type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_random_access_iter_v
    = bool{is_random_access_iter<T>{}};

/**
 * @remark Checks if a type could be iterated by or has a member-type that qualifies as a
 * random-access iterator type.
 * 
 * @details Refer to the @ref is_random_access_iter type trait for more details.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct has_random_access_iter
    : detail::is_iterable<T, is_random_access_iter> {};

/**
 * @var has_random_access_iter_v
 * 
 * @brief Helper variable template for the @ref has_random_access_iter type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto has_random_access_iter_v
    = bool{has_random_access_iter<T>{}};
/** @} */

/**
 * @brief Iterator.
 * 
 * @{
 * 
 * @remark Checks if a type qualifies as an iterator type.
 * 
 * @details A type is considered to be an iterator type if its iterator category
 * (in)directly relates to the input or output iterator tag type.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct is_iterator
    : std::disjunction<
        is_input_iter<T>,
        is_output_iter<T>> {};

/**
 * @var is_iterator_v
 * 
 * @brief Helper variable template for the @ref is_iterator type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto is_iterator_v
    = bool{is_iterator<T>{}};

/**
 * @remark Checks if a type could be iterated by or has a member-type that qualifies as
 * an iterator type.
 * 
 * @details Refer to the @ref is_iterator type trait for more details.
 * 
 * @tparam T Type to check.
 */
template<typename T>
struct has_iterator
    : detail::is_iterable<T, is_iterator> {};

/**
 * @var has_iterator_v
 * 
 * @brief Helper variable template for the @ref has_iterator type trait.
 * 
 * @tparam T Type to check.
 */
template<typename T>
inline constexpr auto has_iterator_v
    = bool{has_iterator<T>{}};
/** @} */

} // namespace cfg

#endif

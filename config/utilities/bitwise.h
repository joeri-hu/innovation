/**
 * @file bitwise.h
 * @brief Bitwise related utility classes/functions/variables/aliases.
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
#ifndef CFG_CONFIG_UTILITIES_BITWISE_H
#define CFG_CONFIG_UTILITIES_BITWISE_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @class bitspan
 * 
 * @brief Represents a span of bits.
 */
class bitspan {
    /**
     * @typdef uint_c
     * 
     * @brief Helper-type that allows the constructor to range-check its arguments in
     * compile time.
     * 
     * @tparam N Numeric value of the integral-constant.
     */
    template<unsigned N>
    using uint_c = std::integral_constant<unsigned, N>;

public:
    /**
     * @var min_size
     * 
     * @brief Minimum size of a bitspan, in number of bits.
     */
    static constexpr auto min_size = 1u;

    /**
     * @var max_size
     * 
     * @brief Maximum size of a bitspan, in number of bits.
     */
    static constexpr auto max_size = 64u;

    /**
     * @var boundary
     * 
     * @brief Boundary of the bitspan, in number of bytes.
     */
    static constexpr auto byte_boundary = 64u;

    /**
     * @var make
     * 
     * @brief Helper variable that is used for creating bitspans.
     * 
     * @param[in] Pos Start position of the bitspan.
     * @param[in] Size Size of the bitspan in number of bits.
     */
    template<unsigned Pos, unsigned Size = min_size>
    static constexpr auto make = bitspan{uint_c<Pos>{}, uint_c<Size>{}};

    /**
     * @brief Default constructs a bitspan.
     * 
     * @details Zero-initializes both the position and size data-members.
     */
    constexpr bitspan() = default;

    /**
     * @brief Constructs a bitspan.
     * 
     * @details A bitspan has a start position and a size. Its size is constrained to the
     * range defined by the @ref min_size and @ref max_size constants. The span of the
     * bits is required not to exceed the boundary defined by the @ref byte_boundary
     * constant.
     * 
     * @tparam Pos Start position of the bitspan within a 64-byte boundary.
     * @tparam Size Size of the bitspan in number of bits, limited to 64.
     */
    template<unsigned Pos, unsigned Size,
        typename = std::enable_if_t<(Size >= min_size and Size <= max_size)>,
        typename = std::enable_if_t<(Pos + Size <= byte_boundary * 8)>>
    constexpr bitspan(uint_c<Pos>, uint_c<Size>)
        : pos_{Pos}, size_{Size} {}

    /**
     * @brief Gets the osition of the bitspan.
     */
    [[nodiscard]]
    constexpr auto pos() const -> std::uint_least16_t
    { return pos_; }

    /**
     * @brief Gets the size of the bitspan.
     */
    [[nodiscard]]
    constexpr auto size() const -> std::uint_least8_t
    { return size_; }

    /**
     * @brief Compares two bit-ranges for (in)equality.
     * 
     * @param[in] lhs Bit-range on the left-hand side of the operator.
     * @param[in] rhs Bit-range on the right-hand side of the operator.
     * 
     * @return Two bit-ranges are considered to be equal when their position and size
     * matches.
     * 
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(
        bitspan const& lhs, bitspan const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(
        bitspan const& lhs, bitspan const& rhs) -> bool
    { return lhs.pos_ == rhs.pos_ and lhs.size_ == rhs.size_; }
    /** @} */

private:
    std::uint_least16_t pos_{}; /**< Start position of the bitspan. */
    std::uint_least8_t size_{}; /**< Size of the bitspan in number of bits. */
};

/**
 * @brief Converts the bits of some range of bytes or characters to an integral value.
 * 
 * @tparam View Type that points to the bytes or characters.
 * @tparam T Integral type of the converted value.
 * 
 * @param[in] value View to some range of bytes or characters to convert.
 * 
 * @return Converted integral value.
 */
template<typename T, typename View,
    typename = std::enable_if_t<std::is_integral_v<T>>>
[[nodiscard]]
constexpr auto convert_bits(View value) -> T {
    if (std::size(value) > sizeof(T)) return T{};

    auto result = T{};
    std::memcpy(&result, std::data(value), std::size(value));
    return result;
}

/**
 * @brief Extracts a span of bits from a range of bytes or characters.
 * 
 * @warning Ensure that the pointer to the data source is not null and refers to a range
 * of at least N bytes, where N is greater or equal to the byte boundary of a bitspan.
 * 
 * @tparam T Element type of the range of characters or bytes, which is constrained to
 * the size of a byte.
 * 
 * @param[in] source Range of bytes or characters to extract bits from.
 * @param[in] bits Span of bits to extract.
 * 
 * @return Unsigned integral value of at least 64-bits that contains the span of bits
 * extracted from the range of bytes or characters.
 */
template<typename T,
    typename = std::enable_if_t<(sizeof(T) == sizeof(std::byte))>>
[[nodiscard]]
constexpr auto extract_bits(T const source[], bitspan bits)
-> std::uint_fast64_t {
    auto const mask = 0b1111'1111u;
    auto const width = 8u;
    auto result = std::uint_fast64_t{};
    unsigned pos{bits.pos()};
    unsigned size{bits.size()};
    auto offset = pos % width;
    auto const count = (size + offset - 1) / width;
    auto const start = pos / width;

    for (auto idx = 0u; idx < count; ++idx) {
        result |= source[start + idx] & (mask >> offset);
        auto const diff = width - offset;
        pos += diff;
        size -= diff;
        result <<= std::min(size, width);
        offset = pos % width;
    }
    auto const span = width - (offset + size);
    auto const last = source[start + count] & (mask >> offset);
    result |= last >> span;
    return result;
}

/**
 * @brief Makes a bitmask of a given size.
 * 
 * @tparam U Type of the bitmask, which is constrained to be an unsigned integral type.
 * The default type is an unsigned 32-bit integer.
 * 
 * @param[in] size Size of the bitmask, the number of lower bits that will be set to one.
 * 
 * @return Bitmask of an unsigned integral type.
 */
template<typename U = std::uint32_t,
    typename = std::enable_if_t<std::is_unsigned_v<U>>>
[[nodiscard]]
constexpr auto make_bitmask(int size) -> U
{ return ~(~U{} << size); }

} // namespace cfg

#endif

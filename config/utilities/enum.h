/**
 * @file enum.h
 * @brief Enumeration type related utility functions/variables/aliases.
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
#ifndef CFG_CONFIG_UTILITIES_ENUM_H
#define CFG_CONFIG_UTILITIES_ENUM_H

#include <traits/enum-traits.h>

#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @brief Converts an enumeration type to its underlying type.
 * 
 * @details Refer to cppreference.com for more details.
 * 
 * @tparam Enum Enumeration type to convert.
 * 
 * @param[in] enumeration Value of the enumeration type to convert.
 * 
 * @return Integral value of the converted enumeration type.
 */
template<typename Enum,
    typename = std::enable_if_t<std::is_enum_v<Enum>>>
[[nodiscard]]
constexpr auto to_underlying(Enum enumeration)
{ return static_cast<std::underlying_type_t<Enum>>(enumeration); }

/**
 * @brief Bitwise arithmetic operators.
 * 
 * @details Keep in mind that unsigned integer arithmetic can only be performed if the
 * common type of both operands is of an unsigned integral type. This means that at least
 * one operand is required to match the integer promotion rank of an unsigned integer. Be
 * wary of this when operating on bitwise enumerations with a narrow underlying type.
 * 
 * @{
 * 
 * @brief Bitwise AND operators.
 * 
 * @{
 * 
 * @brief Performs a bitwise AND operation.
 * 
 * @details This operator is overloaded for a bitwise enumeration and integral type.
 * 
 * @tparam Enum Bitwise enumeration type of the enumeration value on the left-hand side.
 * @tparam Integral Integral type of the value on the right-hand side.
 * 
 * @param[in] lhs Value of the bitwise enumeration type.
 * @param[in] rhs Value of the integral type.
 * 
 * @return Bitwise AND value of the operands.
 */
template<typename Enum, typename Integral,
    std::enable_if_t<is_bitwise_enum_v<Enum>, int> = 0,
    std::enable_if_t<std::is_integral_v<Integral>, int> = 0>
[[nodiscard]]
constexpr auto operator&(Enum lhs, Integral rhs)
{ return to_underlying(lhs) & rhs; }

/**
 * @brief Performs a bitwise AND operation.
 * 
 * @details This operator is overloaded for an integral and bitwise enumeration type.
 * 
 * @tparam Integral Integral type of the value on the left-hand side.
 * @tparam Enum Bitwise enumeration type of the enumeration value on the right-hand side.
 * 
 * @param[in] lhs Value of the integral type.
 * @param[in] rhs Value of the bitwise enumeration type.
 * 
 * @return Bitwise AND value of the operands.
 */
template<typename Integral, typename Enum,
    std::enable_if_t<std::is_integral_v<Integral>, int> = 0,
    std::enable_if_t<is_bitwise_enum_v<Enum>, int> = 0>
[[nodiscard]]
constexpr auto operator&(Integral lhs, Enum rhs)
{ return rhs & lhs; }

/**
 * @brief Performs a compound assignment by bitwise AND operation.
 * 
 * @details This operator is overloaded for an integral and bitwise enumeration type.
 * 
 * @tparam Integral Integral type of the operand on the left-hand side.
 * @tparam Enum Bitwise enumeration type of the enumeration value on the right-hand side.
 * 
 * @param[in] lhs Reference to the value of the integral type.
 * @param[in] rhs Value of the bitwise enumeration type.
 * 
 * @return Reference to the operand on the left-hand side with the newly assigned bitwise
 * AND value.
 */
template<typename Integral, typename Enum,
    typename = std::enable_if_t<std::is_integral_v<Integral>>,
    typename = std::enable_if_t<is_bitwise_enum_v<Enum>>>
constexpr auto operator&=(Integral& lhs, Enum rhs) -> Integral& {
    lhs &= to_underlying(rhs);
    return lhs;
}
/** @} */

/**
 * @brief Bitwise XOR operators.
 * 
 * @{
 * 
 * @brief Performs a bitwise XOR operation.
 * 
 * @details This operator is overloaded for a bitwise enumeration and integral type.
 * 
 * @tparam Enum Bitwise enumeration type of the enumeration value on the left-hand side.
 * @tparam Integral Integral type of the value on the right-hand side.
 * 
 * @param[in] lhs Value of the bitwise enumeration type.
 * @param[in] rhs Value of the integral type.
 * 
 * @return Bitwise XOR value of the operands.
 */
template<typename Enum, typename Integral,
    std::enable_if_t<is_bitwise_enum_v<Enum>, int> = 0,
    std::enable_if_t<std::is_integral_v<Integral>, int> = 0>
[[nodiscard]]
constexpr auto operator^(Enum lhs, Integral rhs)
{ return to_underlying(lhs) ^ rhs; }

/**
 * @brief Performs a bitwise XOR operation.
 * 
 * @details This operator is overloaded for an integral and bitwise enumeration type.
 * 
 * @tparam Integral Integral type of the value on the left-hand side.
 * @tparam Enum Bitwise enumeration type of the enumeration value on the right-hand side.
 * 
 * @param[in] lhs Value of the integral type.
 * @param[in] rhs Value of the bitwise enumeration type.
 * 
 * @return Bitwise XOR value of the operands.
 */
template<typename Integral, typename Enum,
    std::enable_if_t<std::is_integral_v<Integral>, int> = 0,
    std::enable_if_t<is_bitwise_enum_v<Enum>, int> = 0>
[[nodiscard]]
constexpr auto operator^(Integral lhs, Enum rhs)
{ return rhs ^ lhs; }

/**
 * @brief Performs a compound assignment by bitwise XOR operation.
 * 
 * @details This operator is overloaded for an integral and bitwise enumeration type.
 * 
 * @tparam Integral Integral type of the operand on the left-hand side.
 * @tparam Enum Bitwise enumeration type of the enumeration value on the right-hand side.
 * 
 * @param[in] lhs Reference to the value of the integral type.
 * @param[in] rhs Value of the bitwise enumeration type.
 * 
 * @return Reference to the operand on the left-hand side with the newly assigned bitwise
 * XOR value.
 */
template<typename Integral, typename Enum,
    typename = std::enable_if_t<std::is_integral_v<Integral>>,
    typename = std::enable_if_t<is_bitwise_enum_v<Enum>>>
constexpr auto operator^=(Integral& lhs, Enum rhs) -> Integral& {
    lhs ^= to_underlying(rhs);
    return lhs;
}
/** @} */

/**
 * @brief Bitwise OR operators.
 * 
 * @{
 * 
 * @brief Performs a bitwise OR operation.
 * 
 * @details This operator is overloaded for a bitwise enumeration and integral type.
 * 
 * @tparam Enum Bitwise enumeration type of the enumeration value on the left-hand side.
 * @tparam Integral Integral type of the value on the right-hand side.
 * 
 * @param[in] lhs Value of the bitwise enumeration type.
 * @param[in] rhs Value of the integral type.
 * 
 * @return Bitwise OR value of the operands.
 */
template<typename Enum, typename Integral,
    std::enable_if_t<is_bitwise_enum_v<Enum>, int> = 0,
    std::enable_if_t<std::is_integral_v<Integral>, int> = 0>
[[nodiscard]]
constexpr auto operator|(Enum lhs, Integral rhs)
{ return to_underlying(lhs) | rhs; }

/**
 * @brief Performs a bitwise OR operation.
 * 
 * @details This operator is overloaded for an integral and bitwise enumeration type.
 * 
 * @tparam Integral Integral type of the value on the left-hand side.
 * @tparam Enum Bitwise enumeration type of the enumeration value on the right-hand side.
 * 
 * @param[in] lhs Value of the integral type.
 * @param[in] rhs Value of the bitwise enumeration type.
 * 
 * @return Bitwise OR value of the operands.
 */
template<typename Integral, typename Enum,
    std::enable_if_t<std::is_integral_v<Integral>, int> = 0,
    std::enable_if_t<is_bitwise_enum_v<Enum>, int> = 0>
[[nodiscard]]
constexpr auto operator|(Integral lhs, Enum rhs)
{ return rhs | lhs; }

/**
 * @brief Performs a compound assignment by bitwise OR operation.
 * 
 * @details This operator is overloaded for an integral and bitwise enumeration type.
 * 
 * @tparam Integral Integral type of the operand on the left-hand side.
 * @tparam Enum Bitwise enumeration type of the enumeration value on the right-hand side.
 * 
 * @param[in] lhs Reference to the value of the integral type.
 * @param[in] rhs Value of the bitwise enumeration type.
 * 
 * @return Reference to the operand on the left-hand side with the newly assigned bitwise
 * OR value.
 */
template<typename Integral, typename Enum,
    typename = std::enable_if_t<std::is_integral_v<Integral>>,
    typename = std::enable_if_t<is_bitwise_enum_v<Enum>>>
constexpr auto operator|=(Integral& lhs, Enum rhs) -> Integral& {
    lhs |= to_underlying(rhs);
    return lhs;
}
/**
 * @}
 * @}
 */

} // namespace cfg

#endif

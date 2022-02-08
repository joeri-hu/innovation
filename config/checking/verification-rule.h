/**
 * @file verification-rule.h
 * @brief Defines how a main configuration object should be verified for correctness.
 *
 * @version 1.0
 * @date January 2022
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
#ifndef CFG_CONFIG_CHECKING_VERIFICATION_RULE_H
#define CFG_CONFIG_CHECKING_VERIFICATION_RULE_H

#include "verification-identifiers.h"

#include <errors/error-types.h>
#include <traits/class-traits.h>

#include <optional>
#include <type_traits>
#include <utility>

/**
 * @namespace cfg
 *
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @struct verification_rule
 *
 * @brief Verifies a main configuration object with the use of a given verifier-function.
 * 
 * @tparam Verifier Type of the verifier-function.
 */
template<typename Verifier>
class verification_rule {
public:
    /**
     * @brief Default-constructs a verification_rule.
     */
    constexpr verification_rule() = default;

    /**
     * @brief Constructs a verification rule with an indenfier and a verifier-function.
     * 
     * @param[in] id Identifier of the verification rule.
     * @param[in] verifier Function that performs the verification of a verification
     * rule.
     */
    constexpr verification_rule(
        verification_identifier id,
        Verifier const& verifier
    ):
        id_{id},
        verifier_fn{verifier}
    {}

    /**
     * @brief Verifies a configuration object with its verifier-function.
     * 
     * @tparam MainConfig Data-structure type of the configuration object.
     * @tparam Ts Types of the optional arguments.
     * 
     * @param[in] config Configuration object which is being verified by a verifier.
     * @param[in] args Optional number of arguments that will be forwarded to the
     * verifier.
     * 
     * @return If the verifier-function verified its verification rule successfully, the
     * optional error is empty. Otherwise, a verification error is returned.
     */
    template<typename MainConfig, typename... Ts,
        typename = std::enable_if_t<is_data_type_v<MainConfig>>,
        typename = std::enable_if_t<
            std::is_invocable_v<Verifier, MainConfig const&, Ts&&...>>>
    constexpr auto verify(MainConfig const& config, Ts&&... args) const
        -> std::optional<verification_error>
    { return verifier_fn(config, std::forward<Ts>(args)...); }

    /**
     * @brief Gets the verification identifier of a verification rule.
     */
    [[nodiscard]]
    constexpr auto id() const -> verification_identifier
    { return id_; }

    /**
     * @brief Compares two validation rules for (in)equality.
     * 
     * @param[in] lhs Validation rule on the left-hand side of the operator.
     * @param[in] rhs Validation rule on the right-hand side of the operator.
     * 
     * @return Two validation results are considered to be equal when both their
     * identifiers match.
     * 
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(
        verification_rule const& lhs, verification_rule const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(
        verification_rule const& lhs, verification_rule const& rhs) -> bool
    { return lhs.id_ == rhs.id_; }
    /** @} */

private:
    verification_identifier id_; /**< Identifier of a verification rule. */
    Verifier verifier_fn;        /**< Verifier-function that does the verification. */
};

} // namespace cfg

#endif

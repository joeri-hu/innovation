/**
 * @file setting.h
 * @brief Mapping between values within a configuration file or message and an internal
 * configuration object.
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
#ifndef CFG_CONFIG_SETTINGS_SETTING_H
#define CFG_CONFIG_SETTINGS_SETTING_H

#include "setting-identifiers.h"

#include <errors/error-types.h>
#include <parsing/node.h>
#include <utilities/algorithm.h>
#include <utilities/bitwise.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @enum setting_type
 * 
 * @brief Enumeration of the different setting types.
 * 
 * @details This enumeration is used to indicate the severity/necessity of a setting.
 */
enum class setting_type : bool {
    required, /**< Indicates a setting is required to be set with a value. */
    optional  /**< Indicates a setting is not required to be set with a value. */
};

/**
 * @union setting_data
 * 
 * @brief Provides a generic data-type for the validated and converted value of a @ref
 * setting object.
 * 
 * @details When a @ref setting validates its value with success, the converted result
 * can be cached to be reused by its invocable action object. This union allows a @ref
 * setting to store its converted data with a generic type, improving interoperability.
 */
union setting_data {
    /**
     * @brief Default-constructs a setting-data object.
     */
    setting_data() = default;

    /**
     * @brief Constructs a setting-data object with a string-view.
     * 
     * @details Marks #string as the active member.
     */
    constexpr explicit setting_data(std::string_view data)
        : string{data} {}

    /**
     * @brief Constructs a setting-data object with a 32-bit signed integer.
     * 
     * @details Marks #int32 as the active member.
     */
    constexpr explicit setting_data(std::int32_t data)
        : int32{data} {}

    /**
     * @brief Constructs a setting-data object with a 32-bit unsigned integer.
     * 
     * @details Marks #uint32 as the active member.
     */
    constexpr explicit setting_data(std::uint32_t data)
        : uint32{data} {}

    /**
     * @brief Constructs a setting-data object with a 16-bit signed integer.
     * 
     * @details Marks #int16 as the active member.
     */
    constexpr explicit setting_data(std::int16_t data)
        : int16{data} {}

    /**
     * @brief Constructs a setting-data object with a 16-bit unsigned integer.
     * 
     * @details Marks #uint16 as the active member.
     */
    constexpr explicit setting_data(std::uint16_t data)
        : uint16{data} {}

    /**
     * @brief Constructs a setting-data object with an 8-bit signed integer.
     * 
     * @details Marks #int8 as the active member.
     */
    constexpr explicit setting_data(std::int8_t data)
        : int8{data} {}

    /**
     * @brief Constructs a setting-data object with an 8-bit unsigned integer.
     * 
     * @details Marks #uint8 as the active member.
     */
    constexpr explicit setting_data(std::uint8_t data)
        : uint8{data} {}

    /**
     * @brief Constructs a setting-data object with a boolean.
     * 
     * @details Marks #flag as the active member.
     */
    constexpr explicit setting_data(bool data)
        : flag{data} {}

    /**
     * @brief Assigns new data to a setting-data object.
     * 
     * @details Which member will become active depends on the template type. The
     * selection is done with the use of the overloaded constructors.
     * 
     * @tparam T Type of the new data, which is required to be trivially copy-
     * constructible.
     * 
     * @param[in] data New data to set.
     * 
     * @return Reference to the setting-data object.
     */
    template<typename T,
        typename = std::enable_if_t<std::is_trivially_copy_constructible_v<T>>>
    constexpr auto operator=(T data) -> setting_data& {
        *this = setting_data{data};
        return *this;
    }

    std::string_view string{}; /**< Represents a view to a string. */
    std::int32_t  int32;       /**< Represents a 32-bit signed integer value. */
    std::uint32_t uint32;      /**< Represents a 32-bit unsigned integer value. */
    std::int16_t  int16;       /**< Represents a 16-bit signed integer value. */
    std::uint16_t uint16;      /**< Represents a 16-bit unsigned integer value. */
    std::int8_t   int8;        /**< Represents an 8-bit signed integer value. */
    std::uint8_t  uint8;       /**< Represents an 8-bit unsigned integer value. */
    bool flag;                 /**< Represents a boolean value. */
};

/**
 * @class setting
 * 
 * @brief Provides the mapping between values within a config file or message and a given
 * data-structure that represents a configuration object used for controlling various
 * internal systems.
 * 
 * @details A setting consists of a path of tag-names and an optional bitspan. The path
 * of tag-names represents a location within a config file from which it will read and
 * store a value. The bitspan refers to a range of bits within a config message to read
 * and store a value from. This buffered value can be validated with the provided
 * validator.
 * 
 * The validator can be invoked directly in a user-defined way by first obtaining a
 * reference to it. It can also be invoked indirectly by utilizing the @ref validate
 * member-function. This function expects the validator to return its (converted) value
 * in the form of a @ref setting_data object. The setting-data is cached with the
 * intention to be utilized by the provided action.
 * 
 * As with the validator, the same goes for the provided action. It can either be invoked
 * directly or indirectly. An action is intended to be applied whenever the value of a
 * setting is validated successfully. The @ref apply member-function can be used to pass
 * the cached setting-data to the action. 
 * 
 * @tparam MaxTagDepth Maximum depth of the path of tag-names.
 * @tparam Validator Invocable type of the setting's validation object.
 * @tparam Action Invocable type of the setting's action object.
 */
template<int MaxTagDepth, typename Validator, typename Action,
    typename = std::enable_if_t<(MaxTagDepth > 0)>>
class setting {
public:
    /**
     * @var max_tag_depth
     * 
     * @brief Maximum depth of the path of tag-names.
     */
    static constexpr auto max_tag_depth = int{MaxTagDepth};

    /**
     * @var max_value_size
     * 
     * @brief Maximum number of characters that the buffered value can hold.
     */
    static constexpr auto max_value_size = std::size_t{32};

    /**
     * @typedef value_type
     * 
     * @brief Type of the buffered value.
     */
    using value_type = std::array<std::byte, max_value_size>;

    /**
     * @typedef tag_type
     * 
     * @brief Type of the tag-name, a zero-terminated C-style string.
     */
    using tag_type = typename node_sz<MaxTagDepth>::value_type;

    /**
     * @typedef setting_id
     * 
     * @brief Shorter notation to refer to the setting identifier type.
     */
    using setting_id = setting_identifier;

    /**
     * @brief Default-constructs a setting.
     */
    constexpr setting() = default;

    /**
     * @brief Constructs a setting with the given properties.
     * 
     * @param[in] id Identifier of the setting.
     * @param[in] tags Path of tag-names that indicates where to find the value to store.
     * @param[in] type Type of the setting, which can either be optional or required.
     * @param[in] bits Indicates which part of a config message to store as a value.
     * @param[in] validator Invocable object that validates the setting's stored value.
     * @param[in] action Invocable object intended to map the setting's value to a config
     * object.
     * 
     * @{
     */
    constexpr setting(
        setting_id id,
        node_sz<MaxTagDepth> const& tags,
        setting_type type,
        bitspan bits,
        Validator const& validator,
        Action const& action
    ):
        tags_{tags},
        id_{id},
        validator_fn{validator},
        action_fn{action},
        cfg_bits{bits},
        type_{type}
    {}

    /**
     * @brief Constructs a required setting with the given properties.
     * 
     * @param[in] id Identifier of the setting.
     * @param[in] tags Path of tag-names that indicates where to find the value to store.
     * @param[in] bits Indicates which part of a config message to store as a value.
     * @param[in] validator Invocable object that validates the setting's stored value.
     * @param[in] action Invocable object intended to map the setting's value to a config
     * object.
     */
    constexpr setting(
        setting_id id,
        node_sz<MaxTagDepth> const& tags,
        bitspan bits,
        Validator const& validator,
        Action const& action
    ):
        setting{id, tags, setting_type::required, bits, validator, action}
    {}

    /**
     * @brief Constructs a setting with the given properties without a mapping to a
     * config message.
     * 
     * @details Its bitspan is default-initialized, effectively setting the size of the
     * bitspan to zero.
     * 
     * @param[in] id Identifier of the setting.
     * @param[in] tags Path of tag-names that indicates where to find the value to store.
     * @param[in] type Type of the setting, which can either be optional or required.
     * @param[in] validator Invocable object that validates the setting's stored value.
     * @param[in] action Invocable object intended to map the setting's value to a config
     * object.
     */
    constexpr setting(
        setting_id id,
        node_sz<MaxTagDepth> const& tags,
        setting_type type,
        Validator const& validator,
        Action const& action
    ):
        setting{id, tags, type, bitspan{}, validator, action}
    {}

    /**
     * @brief Constructs a copy from a setting with a different maximum tag-depth.
     * 
     * @details If the maximum tag-depth of the setting to copy from is less than the
     * newly created setting, the remaining tag-names are zero-initialized. Otherwise,
     * when there is not enough room, only the tag-names that can fit are copied.
     * 
     * @tparam MaxTagDepthOther Maximum tag-depth of the setting to copy from.
     * 
     * @param[in] other Setting to copy from.
     */
    template<int MaxTagDepthOther>
    constexpr setting(setting<MaxTagDepthOther, Validator, Action> const& other):
        value{other.get_value()},
        tags_{other.tags()},
        value_view{other.view_value()},
        id_{other.id()},
        validator_fn{other.validator()},
        action_fn{other.action()},
        cfg_bits{other.config_bits()},
        type_{other.type()}
    {}

    /**
     * @brief Validates the stored value and caches the converted data.
     * 
     * @details This function is intended to be used with a validator that returns an
     * optional @ref setting_data object and an optional @link error::type::validation
     * validation error @endlink. The validator is only invoked if this setting contains
     * a value. If the value is not set, a validation error of type 'setting_unset' is
     * returned. Otherwise, the optional @ref setting_data object is cached.
     * 
     * @tparam Ts Types of the optional arguments.
     * 
     * @param[in] args Optional number of arguments that will be forwarded to the
     * validator.
     * 
     * @return An optional @link error::type::validation validation error @endlink. There
     * should be no @link error::type::validation validation error @endlink if the stored
     * value was validated successfully.
     */
    template<typename... Ts,
        typename = std::enable_if_t<
            std::is_invocable_v<Validator, std::string_view, Ts&&...>>>
    constexpr auto validate(Ts&&... args) const -> std::optional<validation_error> {
        if (not is_set()) return validation_error::setting_unset;

        auto const [data, status] = validator_fn(value_view, std::forward<Ts>(args)...);
        cache = data;
        return status;
    }

    /**
     * @brief Applies the action of the invocable action object.
     * 
     * @details The action is invoked with the cached optional @ref setting_data object.
     * 
     * @warning This function should only be used when the @ref validate function was
     * successful, to ensure the cached setting-data contains a value. The user-provided
     * validator and action object are responsible for activating and accessing the same
     * data-member within a @ref setting_data object.
     * 
     * @tparam Ts Types of the optional arguments.
     * 
     * @param[in] args Optional number of arguments that will be forwarded to the action
     * object.
     * 
     * @return Matches the return value of the action object.
     */
    template<typename... Ts,
        typename = std::enable_if_t<
            std::is_invocable_v<Action, setting_data, Ts&&...>>>
    constexpr auto apply(Ts&&... args) const -> decltype(auto)
    { return action_fn(*cache, std::forward<Ts>(args)...); }

    /**
     * @brief Checks whether the tag at a given index is empty.
     * 
     * @param[in] index Index of the possibly empty tag.
     */
    [[nodiscard]]
    constexpr auto is_tag_empty(int index) const -> bool
    { return tags_[index] == tag_type{}; }

    /**
     * @brief Checks whether the setting has been set and stores a value.
     */
    [[nodiscard]]
    constexpr auto is_set() const -> bool
    { return not value_view.empty(); }

    /**
     * @brief Gets the setting's identifier.
     * 
     * @return Identifier of this setting.
     */
    [[nodiscard]]
    constexpr auto id() const -> setting_id
    { return id_; }

    /**
     * @brief Gets the type of setting.
     * 
     * @details The type of a setting refers to its severity/necessity, which can either
     * be optional or required.
     * 
     * @return Type of this setting.
     */
    [[nodiscard]]
    constexpr auto type() const -> setting_type
    { return type_; }

    /**
     * @brief Gets the span of bits that refers to some part within a config message.
     * 
     * @return Bitspan of this setting.
     */
    [[nodiscard]]
    constexpr auto config_bits() const -> bitspan
    { return cfg_bits; }

    /**
     * @brief Gets the name of a tag at a given depth.
     * 
     * @return Tag-name at the requested depth.
     */
    [[nodiscard]]
    constexpr auto tag(std::int_fast8_t depth) const -> tag_type
    { return tags_[depth]; }

    /**
     * @brief Gets all of the tag-names.
     * 
     * @return Const-reference to all of the tag-names.
     */
    [[nodiscard]]
    constexpr auto tags() const -> node_sz<MaxTagDepth> const&
    { return tags_; }

    /**
     * @brief Gets the invocable validator object.
     * 
     * @details The validator object can be accessed to invoke it in a way that differs
     * from the @ref validate function.
     * 
     * @return Const-reference to the validator.
     */
    [[nodiscard]]
    constexpr auto validator() const -> Validator const&
    { return validator_fn; }

    /**
     * @brief Gets the invocable action object.
     * 
     * @details The action object can be accessed to invoke it in a way that differs from
     * the @ref apply function.
     * 
     * @return Const-reference to the action.
     */
    [[nodiscard]]
    constexpr auto action() const -> Action const&
    { return action_fn; }

    /**
     * @brief Gets a view of the buffered value.
     * 
     * @return String-view of the buffered value. If no value is set, the string-view is
     * empty.
     */
    [[nodiscard]]
    constexpr auto view_value() const -> std::string_view
    { return value_view; }

    /**
     * @brief Gets the buffered value.
     * 
     * @return Const-reference to the value-buffer.
     */
    [[nodiscard]]
    constexpr auto get_value() const -> value_type const&
    { return value; }

    /**
     * @brief Sets the buffered value to the contents of a given string.
     * 
     * @details Copies at most N number of elements from the given @p content string to
     * the buffer of the stored value, where N is determined by #max_value_size. The
     * buffered value is not null-terminated.
     * 
     * @param[in] content Contents of a string.
     */
    constexpr auto set_value(std::string_view content) -> void {
        auto const value_size = std::min(content.size(), max_value_size);
        auto const content_data = reinterpret_cast<std::byte const*>(content.data());
        cfg::copy_n(content_data, value_size, value.data());
        value_view = {reinterpret_cast<char*>(value.data()), value_size};
    }

    /**
     * @brief Sets the buffered value to the binary equivalence of a given integral value.
     * 
     * @param[in] content Content of an integral value.
     */
    constexpr auto set_value(std::uint_fast64_t content) -> void {
        static_assert(sizeof(content) <= max_value_size);
        std::memcpy(value.data(), &content, sizeof(content));
        value_view = {reinterpret_cast<char*>(value.data()), sizeof(content)};
    }

    /**
     * @brief Compares two settings for (in)equality.
     * 
     * @param[in] lhs Setting on the left-hand side of the operator.
     * @param[in] rhs Setting on the right-hand side of the operator.
     * 
     * @return Two settings are considered to be equal when their identifiers match.
     * 
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(
        setting const& lhs, setting const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(
        setting const& lhs, setting const& rhs) -> bool
    { return lhs.id_ == rhs.id_; }
    /** @} */

private:
    value_type value{};                          /**< Value to be stored. */
    node_sz<MaxTagDepth> tags_{};                /**< Path of all the tag-names. */
    mutable std::optional<setting_data> cache{}; /**< Caches a converted value. */
    std::string_view value_view{};               /**< View of the buffered value. */
    setting_id id_{setting_id::unspecified};     /**< Identifier of this setting. */
    Validator validator_fn{};                    /**< Validates the buffered value. */
    Action action_fn{};                          /**< Applies an action. */
    bitspan cfg_bits{};                          /**< Span of config message bits. */
    setting_type type_{};                        /**< Type of the setting.  */
};

/**
 * @remark Deduces the maximum tag-depth from a path of tag-names.
 * 
 * @tparam MaxTagDepth Maximum depth of the path of tag-names.
 * @tparam Validator Invocable type of the setting's validation object.
 * @tparam Action Invocable type of the setting's action object.
 * 
 * @{
 */
template<int MaxTagDepth, typename Validator, typename Action>
setting(setting_identifier, node_sz<MaxTagDepth>, setting_type, Validator, Action)
    -> setting<MaxTagDepth, Validator, Action>;

template<int MaxTagDepth, typename Validator, typename Action>
setting(setting_identifier, node_sz<MaxTagDepth>, bitspan, Validator, Action)
    -> setting<MaxTagDepth, Validator, Action>;

template<int MaxTagDepth, typename Validator, typename Action>
setting(setting_identifier, node_sz<MaxTagDepth>, setting_type, bitspan, Validator, Action)
    -> setting<MaxTagDepth, Validator, Action>;
/** @} */

/**
 * @brief Makes an array of settings from a given number of settings.
 * 
 * @details Each setting can have a different depth of tag-names. The setting with the
 * greatest tag-depth determines the tag-depth of the array of settings. Settings that
 * are copied to a setting with a greater tag-depth, will have their excess tag-names
 * default constructed or value-initialized.
 * 
 * @tparam TagDepths Number of tag-names that each setting contains.
 * @tparam Validator Invocable type of the setting's validation object.
 * @tparam Action Invocable type of the setting's action object.
 * 
 * @param[in] settings Arbitrary number of setting objects with potentially different
 * tag-depths.
 * 
 * @return Array of setting objects.
 */
template<int... TagDepths, typename Validator, typename Action>
[[nodiscard]]
constexpr auto make_settings(setting<TagDepths, Validator, Action> const&... settings)
-> std::array<setting<std::max({TagDepths...}), Validator, Action>, sizeof...(TagDepths)>
{ return {settings...}; }

} // namespace cfg

#endif

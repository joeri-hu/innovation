/**
 * @file node.h
 * @brief Container-type for storing tag names.
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
#ifndef CFG_CONFIG_PARSING_NODE_H
#define CFG_CONFIG_PARSING_NODE_H

#include <utilities/algorithm.h>
#include <utilities/container.h>

#include <type_traits>

/**
 * @namespace cfg
 * 
 * @brief Contains everything related to the processing of configuration files.
 */
namespace cfg {

/**
 * @struct node
 * 
 * @brief Stores a given number of tags of a specific type.
 * 
 * @details Nodes are implemented in terms of arrays and can be copied from other nodes
 * that have different depths.
 * 
 * @tparam Tag Type of the tags. These are ideally string-views or something similar.
 * @tparam Depth Number of tags that can be stored.
 */
template<typename Tag, int Depth,
    typename = std::enable_if_t<(Depth > 0)>>
struct node : private array<Tag, Depth> {
    /**
     * @var depth
     * 
     * @brief Number of tags this container can store.
     */
    static constexpr auto depth = int{Depth};

    /**
     * @typedef base_type
     * 
     * @brief Shorter notation to refer to the type of the base class.
     */
    using base_type = array<Tag, Depth>;

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
    using base_type::cbegin;
    using base_type::end;
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
     * @brief Provides a default constructor.
     */
    using base_type::base_type;

    /**
     * @brief Constructs a node from a variable amount of tags.
     * 
     * @details A variable amount of tags can consist of either objects of tag-types or
     * objects of other nodes. Other nodes are explicitly casted to their underlying
     * tag-type, which will return their last tag.
     * 
     * @tparam Tags Variable amount of tag-types that need to be convertible to the
     * tag-type of this node.
     * 
     * @param[in] tags Variable amount of tags or other nodes.
     */
    template<typename... Tags,
        typename = std::enable_if_t<
            std::conjunction_v<std::is_convertible<Tags, Tag>...>>>
    constexpr explicit node(Tags const&... tags)
        : base_type{static_cast<Tag>(tags)...} {}

    /**
     * @brief Constructs a node from another node.
     * 
     * @details Copies the tags from the other node. If the other node has more tags than
     * the destination node, only those tags for which there is space are copied. Copying
     * starts at the first tag of the source node. If the other node has fewer tags than
     * the destination node, the remaining tags of the destination node are default/
     * value-initialized.
     * 
     * @tparam DepthOther Number of tags that the other node stores.
     * 
     * @param[in] other Other node to copy its tags from.
     */
    template<int DepthOther>
    constexpr node(node<Tag, DepthOther> const& other) {
        if constexpr (DepthOther < Depth) {
            cfg::fill(cfg::copy(other, *this), end(), Tag{});
        } else if constexpr (DepthOther > Depth) {
            cfg::copy_n(other, Depth, *this);
        }
    }

    /**
     * @brief Constructs a node from other nodes.
     * 
     * @details Copies the tags from all other nodes. This constructor is restricted to
     * ensure that the combined number of tags from the other nodes is equal to the
     * number of tags of the destination node.
     * 
     * @tparam DepthOthers Number of tags that each other node stores.
     * 
     * @param[in] others Other nodes to copy its tags from.
     */
    template<int... DepthOthers,
        typename = std::enable_if_t<((DepthOthers + ...) == Depth)>>
    constexpr node(node<Tag, DepthOthers> const&... others) {
        auto dest = begin();
        ((dest = cfg::copy(others, dest)), ...);
    }

    /**
     * @brief Constructs a child node from a parent node.
     * 
     * @details Copies the tags from a parent node and stores the additional tag at the
     * back of the container. This constructor is restricted to only copy from another
     * node which has one tag less. 
     * 
     * @tparam DetphParent Number of tags that the parent node stores.
     * 
     * @param[in] parent Other node to copy its tags from.
     * @param[in] tag Additional tag to be stored at the back of the container.
     */
    template<int DepthParent,
        typename = std::enable_if_t<(DepthParent == (Depth - 1))>>
    constexpr node(node<Tag, DepthParent> const& parent, Tag const& tag) {
        cfg::copy(parent, *this);
        back() = tag;
    }

    /**
     * @brief Explicitly converts a node to the last tag that it stores.
     */
    [[nodiscard]]
    constexpr explicit operator const_reference() const
    { return back(); }

    /**
     * @brief Binds a new tag to a node, similarly to nesting directory paths.
     * 
     * @param[in] parent Node to copy its tags from.
     * @param[in] tag Tag to add to the end of the newly created node.
     * 
     * @return Newly created node object with a tag-depth one greater than the parent
     * node.
     */
    [[nodiscard]]
    friend constexpr auto operator/(
        node const& parent, Tag const& tag) -> node<Tag, (Depth + 1)>
    { return {parent, tag}; }

    /**
     * @brief Chains two nodes together, similarly to nesting directory paths.
     * 
     * @tparam DepthOther Number of tags the node on the right-hand side stores.
     * 
     * @param[in] lhs Node on the left-hand side of the operator to copy its tags from.
     * @param[in] rhs Node on the right-hand side of the operator to copy its tags from.
     * 
     * @return Newly created node object with a tag-depth equal to the combined tag-depth
     * of both nodes.
     */
    template<int DepthOther>
    [[nodiscard]]
    friend constexpr auto operator/(node const& lhs, node<Tag, DepthOther> const& rhs)
        -> node<Tag, (Depth + DepthOther)>
    { return {lhs, rhs}; }

    /**
     * @brief Accesses a tag at the given index.
     * 
     * @details These overloaded subscript operators allow the use of signed integer
     * indices. This is accomplished by simply converting the index to the required size
     * type of a std::array. Care should be taken to prevent undesired results, as this
     * function does not perform any range or bounds checking.
     * 
     * @param[in] index Index of the tag to access.
     * 
     * @return Reference to a tag that can be modified.
     */
    constexpr auto operator[](int index) -> reference
    { return (*this)[static_cast<size_type>(index)]; }

    /**
     * @return Reference to a tag that cannot be modified.
     */
    constexpr auto operator[](int index) const -> const_reference
    { return (*this)[static_cast<size_type>(index)]; }

    /**
     * @brief Compares two nodes for (in)equality.
     * 
     * @param[in] lhs Node on the left-hand side of the operator.
     * @param[in] rhs Node on the right-hand side of the operator.
     * 
     * @return Two nodes are considered to be equal when all of their tags match.
     * 
     * @{
     */
    [[nodiscard]]
    friend constexpr auto operator!=(
        node const& lhs, node const& rhs) -> bool
    { return not (lhs == rhs); }

    [[nodiscard]]
    friend constexpr auto operator==(
        node const& lhs, node const& rhs) -> bool
    { return static_cast<base_type>(lhs) == static_cast<base_type>(rhs); }
    /** @} */
};

/**
 * @remark Deduces the tag-type from a variable amount of tags.
 * 
 * @details The type of the first tag is used for instantiating a node. Any tag-type that
 * follows is required to be convertible to the first tag-type. The depth of the node is
 * determined by counting all of the tags that are used to construct a new node with.
 * 
 * @tparam Tag Tag-type of the node to instantiate.
 * @tparam Tags Variable amount of other tag-types.
 */
template<typename Tag, typename... Tags,
    typename = std::enable_if_t<
        std::conjunction_v<std::is_convertible<Tags, Tag>...>>>
node(Tag, Tags...) -> node<Tag, static_cast<int>(1 + sizeof...(Tags))>;

/**
 * @remark Deduces the tag-type and tag-depth from a variable amount of other nodes.
 * 
 * @details The new tag-depth is determined by combining the tag-depth from all of the
 * other nodes.
 * 
 * @tparam Tag Tag-type of the node to instantiate.
 * @tparam Depths Variable amount of tag-depths from the other nodes.
 */
template<typename Tag, int... Depths>
node(node<Tag, Depths>...) -> node<Tag, (Depths + ...)>;

/**
 * @typedef node_sz
 * 
 * @brief Alias for nodes with tags of zero-terminated string types.
 * 
 * @tparam Depth Number of tags that the node stores.
 */
template<int Depth>
using node_sz = node<char const*, Depth>;

} // namespace cfg

#endif

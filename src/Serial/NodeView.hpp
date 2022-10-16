#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace serial {
class Node;

enum class NodeType : uint8_t {
    // Type of node value.
    Object,
    Array,
    String,
    Boolean,
    Integer,
    Decimal,
    Null,
    // Used in tokenizers.
    Unknown,
    Token,
    EndOfFile,
};
using NodeValue = std::string;

using NodeProperty = std::pair<std::string, Node>;
using NodeProperties = std::vector<NodeProperty>;

/**
 * @brief Class that is returned from a {@link Node} when getting constant properties. This represents a key tree from a parent,
 * this allows reads of large trees with broken nodes to not need to generate new content.
 */
class NodeConstView {
protected:
    friend class Node;

    using Key = std::variant<std::string, uint32_t>;

    NodeConstView() noexcept = default;
    NodeConstView(const Node *parent, Key key, const Node *object);
    NodeConstView(const NodeConstView *parent, Key key);

public:
    bool hasObject() const noexcept { return _object != nullptr; }
    const Node *object() const { return _object; }

    explicit operator bool() const noexcept { return hasObject(); }
    explicit operator const Node &() const { return *_object; }

    const Node &operator*() const { return *_object; }
    const Node *operator->() const { return _object; }

    NodeConstView operator[](const std::string &key) const;
    NodeConstView operator[](uint32_t index) const;

protected:
    const Node *_parent = nullptr;
    const Node *_object = nullptr;
    std::vector<Key> _keys;
};

/**
 * @brief Class that extends the usage of {@link NodeConstView} to mutable nodes.
 */
class NodeView : public NodeConstView {
protected:
    friend class Node;

    NodeView() noexcept = default;
    NodeView(Node *parent, Key key, Node *object);
    NodeView(NodeView *parent, Key key);

public:
    Node *object();

    explicit operator Node &() { return *object(); }

    Node &operator*() { return *object(); }
    Node *operator->() { return object(); }

    NodeView operator[](const std::string &key);
    NodeView operator[](uint32_t index);
};

}

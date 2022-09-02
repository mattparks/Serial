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
    friend class Node;
protected:
    using Key = std::variant<std::string, uint32_t>;

    NodeConstView() = default;
    NodeConstView(const Node *parent, Key key, const Node *object);
    NodeConstView(const NodeConstView *parent, Key key);

public:
    bool hasObject() const noexcept { return _object != nullptr; }
    const Node *object() const { return _object; }

    explicit operator bool() const noexcept { return hasObject(); }
    operator const Node &() const { return *_object; }

    const Node &operator*() const { return *_object; }
    const Node *operator->() const { return _object; }

    template<typename T>
    T get() const;
    template<typename T>
    T getWithFallback(const T &fallback) const;
    template<typename T>
    bool get(T &dest) const;
    template<typename T, typename K>
    bool getWithFallback(T &dest, const K &fallback) const;
    template<typename T>
    bool get(T &&dest) const;
    template<typename T, typename K>
    bool getWithFallback(T &&dest, const K &fallback) const;
    
    NodeConstView propertyWithBackup(const std::string &key, const std::string &backupKey) const;
    NodeConstView propertyWithValue(const std::string &key, const NodeValue &propertyValue) const;

    NodeConstView operator[](const std::string &key) const;
    NodeConstView operator[](uint32_t index) const;

    NodeProperties properties() const;

    NodeType type() const;
    
protected:
    const Node *_parent = nullptr;
    const Node *_object = nullptr;
    std::vector<Key> _keys;
};
}

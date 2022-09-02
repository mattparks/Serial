#pragma once

#include "NodeConstView.hpp"

namespace serial {
/**
 * @brief Class that extends the usage of {@link NodeConstView} to mutable nodes.
 */
class NodeView : public NodeConstView {
    friend class Node;
protected:
    NodeView() = default;
    NodeView(Node *parent, Key key, Node *object);
    NodeView(NodeView *parent, Key key);

public:
    Node *object();

    operator Node &() { return *object(); }

    Node &operator*() { return *object(); }
    Node *operator->() { return object(); }

    template<typename T>
    void set(const T &value);
    template<typename T>
    void set(T &&value);
    
    NodeView propertyWithBackup(const std::string &key, const std::string &backupKey);
    NodeView propertyWithValue(const std::string &key, const NodeValue &propertyValue);

    NodeView operator[](const std::string &key);
    NodeView operator[](uint32_t index);

    NodeView operator=(const NodeConstView &) = delete;
    NodeView operator=(const NodeView &) = delete;
    template<typename T>
    Node &operator=(const T &rhs);
    template<typename T>
    Node &operator=(T &&rhs);

    NodeProperties &properties();
};
}

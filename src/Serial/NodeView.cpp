#include "NodeView.hpp"

#include "Node.hpp"

namespace serial {
NodeView::NodeView(Node *parent, Key key, Node *object) :
    NodeConstView(parent, std::move(key), object) {
}

NodeView::NodeView(NodeView *parent, Key key) :
    NodeConstView(parent, std::move(key)) {
}

Node *NodeView::object() {
    if (!hasObject()) {
        // This will build the tree of nodes from the return _keys tree.
        for (const auto &key : _keys) {
            if (const auto name = std::get_if<std::string>(&key))
                _object = &const_cast<Node *>(_parent)->add(*name);
            else if (const auto index = std::get_if<std::uint32_t>(&key))
                _object = &const_cast<Node *>(_parent)->add(*index);
            else
                throw std::runtime_error("Key for node return is neither a int or a string");
            
            // Because the last key will set parent to the value parent usage should be avoided.
            _parent = _object;
        }

        _keys.erase(_keys.begin(), _keys.end() - 1);
    }

    return const_cast<Node *>(_object);
}

NodeView NodeView::propertyWithBackup(const std::string &key, const std::string &backupKey) {
    if (!hasObject())
        return {this, key};
    return const_cast<Node *>(_object)->propertyWithBackup(key, backupKey);
}

NodeView NodeView::propertyWithValue(const std::string &key, const NodeValue &propertyValue) {
    if (!hasObject())
        return {this, key};
    return const_cast<Node *>(_object)->propertyWithValue(key, propertyValue);
}

NodeView NodeView::operator[](const std::string &key) {
    if (!hasObject())
        return {this, key};
    return const_cast<Node *>(_object)->operator[](key);
}

NodeView NodeView::operator[](uint32_t index) {
    if (!hasObject())
        return {this, index};
    return const_cast<Node *>(_object)->operator[](index);
}

NodeProperties &NodeView::properties() {
    if (!hasObject())
        return object()->properties();
    return const_cast<Node *>(_object)->properties();
}
}

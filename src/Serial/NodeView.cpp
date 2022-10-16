#include "NodeView.hpp"

#include "Node.hpp"

namespace serial {
NodeConstView::NodeConstView(const Node *parent, Key key, const Node *object) :
    _parent(parent),
    _object(object),
    _keys{std::move(key)} {
}

NodeConstView::NodeConstView(const NodeConstView *parent, Key key) :
    _parent(parent->_parent),
    _keys(parent->_keys) {
    _keys.emplace_back(std::move(key));
}

NodeConstView NodeConstView::operator[](const std::string &key) const {
    if (!hasObject())
        return {this, key};
    return _object->operator[](key);
}

NodeConstView NodeConstView::operator[](uint32_t index) const {
    if (!hasObject())
        return {this, index};
    return _object->operator[](index);
}

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

}

#include "NodeConstView.hpp"

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

NodeConstView NodeConstView::propertyWithBackup(const std::string &key, const std::string &backupKey) const {
    if (!hasObject())
        return {this, key};
    return _object->propertyWithBackup(key, backupKey);
}

NodeConstView NodeConstView::propertyWithValue(const std::string &key, const NodeValue &propertyValue) const {
    if (!hasObject())
        return {this, key};
    return _object->propertyWithValue(key, propertyValue);
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

NodeProperties NodeConstView::properties() const {
    if (!hasObject())
        return NodeProperties{};
    return _object->properties();
}

NodeType NodeConstView::type() const {
    if (!hasObject())
        return NodeType::Null;
    return _object->type();
}
}

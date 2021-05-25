#include "NodeConstView.hpp"

#include "Node.hpp"

namespace serial {
NodeConstView::NodeConstView(const Node *parent, NodeKey key, const Node *value) :
	parent(parent),
	value(value),
	keys{std::move(key)} {
}

NodeConstView::NodeConstView(const NodeConstView *parent, NodeKey key) :
	parent(parent->parent),
	keys(parent->keys) {
	keys.emplace_back(std::move(key));
}

NodeConstView NodeConstView::GetPropertyWithBackup(const NodeKey &key, const NodeKey &backupKey) const {
	if (!has_value())
		return {this, key};
	return value->GetPropertyWithBackup(key, backupKey);
}

NodeConstView NodeConstView::GetPropertyWithValue(const NodeKey &key, const std::string &propertyValue) const {
	if (!has_value())
		return {this, key};
	return value->GetPropertyWithValue(key, propertyValue);
}

NodeConstView NodeConstView::operator[](const NodeKey &key) const {
	if (!has_value())
		return {this, key};
	return value->operator[](key);
}

NodePropertiesMap NodeConstView::GetProperties() const {
	if (!has_value())
		return NodePropertiesMap{};
	return value->GetProperties();
}

NodeType NodeConstView::GetType() const {
	if (!has_value())
		return NodeType::Null;
	return value->GetType();
}
}

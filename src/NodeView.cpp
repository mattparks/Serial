#include "NodeView.hpp"

#include "Node.hpp"

namespace serial {
NodeView::NodeView(Node *parent, NodeKey key, Node *value) :
	NodeConstView(parent, std::move(key), value) {
}

NodeView::NodeView(NodeView *parent, NodeKey key) :
	NodeConstView(parent, std::move(key)) {
}

Node *NodeView::get() {
	if (!has_value()) {
		// This will build the tree of nodes from the return keys tree.
		for (const auto &key : keys) {
			value = &const_cast<Node *>(parent)->AddProperty(key);
			// Because the last key will set parent to the value parent usage should be avoided.
			parent = value;
		}

		keys.erase(keys.begin(), keys.end() - 1);
	}

	return const_cast<Node *>(value);
}

/*NodeView NodeView::GetPropertyWithBackup(const NodeKey &key, const NodeKey &backupKey) {
	if (!has_value())
		return {this, key};
	return const_cast<Node *>(value)->GetPropertyWithBackup(key, backupKey);
}

NodeView NodeView::GetPropertyWithValue(const NodeKey &key, const std::string &propertyValue) {
	if (!has_value())
		return {this, key};
	return const_cast<Node *>(value)->GetPropertyWithValue(key, propertyValue);
}*/

NodeView NodeView::operator[](const NodeKey &key) {
	if (!has_value())
		return {this, key};
	return const_cast<Node *>(value)->operator[](key);
}

std::map<NodeKey, Node> &NodeView::GetProperties() {
	if (!has_value())
		return get()->GetProperties();
	return const_cast<Node *>(value)->GetProperties();
}
}

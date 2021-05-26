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
	NodeView(Node *parent, NodeKey key, Node *value);
	NodeView(NodeView *parent, NodeKey key);

public:
	Node *get();

	operator Node &() { return *get(); }

	Node &operator*() { return *get(); }
	Node *operator->() { return get(); }

	template<typename T>
	void Set(const T &value);
	template<typename T>
	void Set(T &&value);
	
	NodeView GetPropertyWithBackup(const NodeKey &key, const NodeKey &backupKey);
	NodeView GetPropertyWithValue(const NodeKey &key, const NodeValue &propertyValue);

	NodeView operator[](const NodeKey &key);

	NodeView operator=(const NodeConstView &) = delete;
	NodeView operator=(const NodeView &) = delete;
	template<typename T>
	Node &operator=(const T &rhs);
	template<typename T>
	Node &operator=(T &&rhs);

	NodePropertiesMap &GetProperties();
};
}

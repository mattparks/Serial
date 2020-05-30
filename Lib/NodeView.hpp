#pragma once

#include "NodeConstView.hpp"

namespace acid {
/**
 * @brief Class that extends the usage of {@link NodeConstView} to mutable nodes.
 */
class NodeView : public NodeConstView {
	friend class Node;
protected:
	NodeView() = default;
	NodeView(Node *parent, Key key, Node *value);
	NodeView(NodeView *parent, Key key);

public:
	Node *get();

	operator Node &() { return *get(); }

	Node &operator*() { return *get(); }
	Node *operator->() { return get(); }

	template<typename T>
	void SetName(const T &value);

	template<typename T>
	void Set(const T &value);

	std::vector<NodeView> GetProperties(const std::string &name);
	NodeView GetPropertyWithBackup(const std::string &name, const std::string &backupName);
	NodeView GetPropertyWithValue(const std::string &propertyName, const std::string &propertyValue);

	NodeView operator[](const std::string &key);
	NodeView operator[](uint32_t index);

	// TODO: Support copying nodes.
	NodeView operator=(const NodeConstView &) = delete;
	//NodeView operator=(NodeConstView &&) = delete;
	NodeView operator=(const NodeView &) = delete;
	//NodeView operator=(NodeView &&) = delete;
	template<typename T>
	Node &operator=(const T &rhs);
	template<typename T>
	Node &operator=(T &&rhs);

	std::vector<Node> &GetProperties();

	void SetName(const std::string &name);
};
}

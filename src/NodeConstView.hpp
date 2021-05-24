#pragma once

#include <cstdint>
#include <variant>
#include <string>
#include <map>
#include <vector>

namespace serial {
class Node;

using NodeKey = std::variant<std::string, uint32_t>; // name or index

enum class NodeType : uint8_t {
	Object, Array, String, Boolean, Integer, Decimal, Null, Token, Unknown
};

/**
 * @brief Class that is returned from a {@link Node} when getting constant properties. This represents a key tree from a parent,
 * this allows reads of large trees with broken nodes to not need to generate new content.
 */
class NodeConstView {
	friend class Node;
protected:
	NodeConstView() = default;
	NodeConstView(const Node *parent, NodeKey key, const Node *value);
	NodeConstView(const NodeConstView *parent, NodeKey key);

public:
	bool has_value() const noexcept { return value != nullptr; }
	const Node *get() const { return value; }

	explicit operator bool() const noexcept { return has_value(); }
	operator const Node &() const { return *value; }

	const Node &operator*() const { return *value; }
	const Node *operator->() const { return value; }

	template<typename T>
	T Get() const;
	template<typename T>
	T GetWithFallback(const T &fallback) const;
	template<typename T>
	bool Get(T &dest) const;
	template<typename T, typename K>
	bool GetWithFallback(T &dest, const K &fallback) const;
	template<typename T>
	bool Get(T &&dest) const;
	template<typename T, typename K>
	bool GetWithFallback(T &&dest, const K &fallback) const;
	
	NodeConstView GetPropertyWithBackup(const NodeKey &key, const NodeKey &backupKey) const;
	NodeConstView GetPropertyWithValue(const NodeKey &key, const std::string &propertyValue) const;

	NodeConstView operator[](const NodeKey &key) const;

	std::map<NodeKey, Node> GetProperties() const;

	NodeType GetType() const;
	
protected:
	const Node *parent = nullptr;
	const Node *value = nullptr;
	std::vector<NodeKey> keys;
};
}

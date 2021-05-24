#pragma once

#include <ostream>

#include "NodeFormat.hpp"
#include "NodeView.hpp"

namespace serial {
/**
 * @brief Class that is used to represent a tree of UFT-8 values, used in serialization.
 */
class Node final {
public:
	Node() = default;
	Node(const Node &node) = default;
	Node(Node &&node) noexcept = default;

	template<typename T, typename = std::enable_if_t<std::is_convertible_v<T *, NodeFormat *>>>
	void ParseString(std::string_view string);
	template<typename T, typename = std::enable_if_t<std::is_convertible_v<T *, NodeFormat *>>>
	void WriteStream(std::ostream &stream, NodeFormat::Format format = NodeFormat::Minified) const;

	template<typename T, typename _Elem = char, typename = std::enable_if_t<std::is_convertible_v<T *, NodeFormat *>>>
	void ParseStream(std::basic_istream<_Elem> &stream);
	template<typename T, typename _Elem = char, typename = std::enable_if_t<std::is_convertible_v<T *, NodeFormat *>>>
	std::basic_string<_Elem> WriteString(NodeFormat::Format format = NodeFormat::Minified) const;

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
	template<typename T>
	void Set(const T &value);
	template<typename T>
	void Set(T &&value);
	
	/**
	 * Clears all properties from this node.
	 */
	void Clear();

	/**
	 * Gets if the node has a value, or has properties that have values.
	 * @return If the node is internally valid.
	 */
	bool IsValid() const;

	template<typename T>
	Node &Append(const T &value);
	template<typename ...Args>
	Node &Append(const Args &...args);
	
	//Node &Merge(Node &&node);

	bool HasProperty(const NodeKey &key) const;
	NodeConstView GetProperty(const NodeKey &key) const;
	NodeView GetProperty(const NodeKey &key);
	Node &AddProperty(const Node &node);
	Node &AddProperty(Node &&node = {});
	Node &AddProperty(const NodeKey &key, const Node &node);
	Node &AddProperty(const NodeKey &key, Node &&node = {});
	Node RemoveProperty(const NodeKey &key);
	Node RemoveProperty(const Node &node);

	NodeConstView GetPropertyWithBackup(const NodeKey &key, const NodeKey &backupKey) const;
	NodeConstView GetPropertyWithValue(const NodeKey &key, const std::string &propertyValue) const;
	NodeView GetPropertyWithBackup(const NodeKey &key, const NodeKey &backupKey);
	NodeView GetPropertyWithValue(const NodeKey &key, const std::string &propertyValue);

	NodeConstView operator[](const NodeKey &key) const;
	NodeView operator[](const NodeKey &key);

	Node &operator=(const Node &rhs) = default;
	Node &operator=(Node &&rhs) noexcept = default;
	Node &operator=(const NodeConstView &rhs);
	Node &operator=(NodeConstView &&rhs);
	Node &operator=(NodeView &rhs);
	Node &operator=(NodeView &&rhs);
	template<typename T>
	Node &operator=(const T &rhs);

	bool operator==(const Node &rhs) const;
	bool operator!=(const Node &rhs) const;
	bool operator<(const Node &rhs) const;

	const std::map<NodeKey, Node> &GetProperties() const { return properties; }
	std::map<NodeKey, Node> &GetProperties() { return properties; }

	const std::string &GetValue() const { return value; }
	void SetValue(std::string value) { this->value = std::move(value); }

	const NodeType &GetType() const { return type; }
	void SetType(NodeType type) { this->type = type; }

protected:
	std::map<NodeKey, Node> properties;
	std::string value;
	NodeType type = NodeType::Object;
};
}

#include "Node.inl"
#include "NodeConstView.inl"
#include "NodeView.inl"

#pragma once

#include <vector>
#include <string>

namespace acid
{
/**
 * @brief Class that is used to represent a tree of values, used in file-object serialization.
 */
class Node
{
public:
	enum class Type
	{
		String, Object, Array, Boolean, Number, Null, Unknown
	};

	using Property = std::pair<std::string, Node>;

	Node() = default;

	Node(std::string value);

	Node(std::string value, std::vector<Property> &&properties);

	template<typename T>
	T As() const;

	template<typename T>
	T As(const T &fallback) const;

	template<typename T>
	T GetValue() const;

	template<typename T>
	void SetValue(const T &value);

	//const Type &GetType() const { return m_type; }

	Node *GetParent() const { return m_parent; }

	std::string GetName() const;

	void SetName(const std::string &name);

	template<typename T>
	Node &Append(T value);

	template<typename ...Args>
	Node &Append(Args ... args);

	bool HasProperty(const std::string &name) const;

	const Node &GetProperty(const std::string &name) const;

	Node &GetProperty(const std::string &name);

	Node &AddProperty(const std::string &name = "", Node &&node = {});

	void RemoveProperty(const std::string &name);

	const std::vector<Property> &GetProperties() const { return m_properties; };

	template <typename T>
	Node &operator=(const T &rhs);

	const Node &operator[](const std::string &key) const;

	Node &operator[](const std::string &key);

	const Node &operator[](const uint32_t &index) const;

	Node &operator[](const uint32_t &index);

	bool operator==(const Node &other) const;

	bool operator!=(const Node &other) const;

	bool operator<(const Node &other) const;

protected:
	std::string m_value;
	//Type m_type{};
	Node *m_parent{};
	std::vector<Property> m_properties;
};
}

#include "Node.inl"

#pragma once

#include "NodeReturn.hpp"

namespace acid {
/**
 * @brief Class that is used to represent a tree of values, used in serialization.
 */
class ACID_EXPORT Node {
public:
	enum class Type {
		Object,
		Array,
		String,
		Boolean,
		Number,
		Null,
		Unknown
	};

	enum class Format {
		Beautified,
		Minified
	};

	using Property = std::pair<std::string, Node>;

	Node() = default;
	Node(const Node &node);
	Node(Node &&node) = default;
	explicit Node(std::string value, Type type = Type::String);
	Node(std::string value, std::vector<Property> &&properties);

	virtual ~Node() = default;

	template<typename _Elem = char>
	void Load(std::basic_istream<_Elem> &stream);
	template<typename _Elem = char>
	void Load(const std::basic_string<_Elem> &string);
	template<typename _Elem = char>
	void Write(std::basic_ostream<_Elem> &stream, Format format = Format::Beautified) const;
	template<typename _Elem = char>
	std::basic_string<_Elem> Write(Format format = Format::Beautified) const;

	template<typename T>
	T Get() const;
	template<typename T>
	T Get(const T &fallback) const;
	template<typename T>
	void Get(T &dest) const;
	template<typename T, typename K>
	void Get(T &dest, const K &fallback) const;
	template<typename T>
	void Set(const T &value);

	/**
	 * Removes this node from it's parent.
	 **/
	void Remove();

	/**
	 * Clears all properties from this node.
	 **/
	void Clear();

	/**
	 * Gets if the node has a value, or has properties that have values.
	 * @return If the node is internally valid.
	 **/
	bool IsValid() const;

	template<typename T>
	Node &Append(const T &value);
	template<typename ...Args>
	Node &Append(const Args &...args);

	bool HasProperty(const std::string &name) const;
	NodeReturn GetProperty(const std::string &name) const;
	NodeReturn GetProperty(uint32_t index) const;
	Node &AddProperty();
	Node &AddProperty(const std::string &name, Node &&node);
	Node &AddProperty(uint32_t index, Node &&node);
	void RemoveProperty(const std::string &name);
	void RemoveProperty(const Node &node);

	// TODO: Remove before flight
	void AddSize(std::size_t &size) const;

	NodeReturn operator[](const std::string &key) const;
	NodeReturn operator[](uint32_t index) const;

	Node &operator=(const Node &node);
	Node &operator=(Node &&node) = default;
	template<typename T>
	Node &operator=(const T &rhs);

	bool operator==(const Node &other) const;
	bool operator!=(const Node &other) const;
	bool operator<(const Node &other) const;

	const std::vector<Property> &GetProperties() const { return m_properties; }
	std::vector<Property> &GetProperties() { return m_properties; }

	std::string GetName() const;
	void SetName(const std::string &name);

	const std::string &GetValue() const { return m_value; }
	void SetValue(const std::string &value) { m_value = value; }

	const Type &GetType() const { return m_type; }
	void SetType(Type type) { m_type = type; }

	Node *GetParent() const { return m_parent; }

protected:
	virtual void LoadStructure(const std::string &string);
	virtual void WriteStructure(std::ostream &stream, Format format) const;
	
	std::vector<Property> m_properties;
	std::string m_value;
	Type m_type = Type::Object;
	Node *m_parent = nullptr;
};
}

#include "Node.inl"
#include "NodeReturn.inl"

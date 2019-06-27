#pragma once

#include <vector>
#include <string>
#include <variant>
#include <stdexcept>

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

	enum class Format
	{
		Beautified, Minified
	};

	using Property = std::pair<std::string, Node>;

	class Return
	{
	public:
		Return() = default;

		Return(Node const *parent, std::variant<std::string, int32_t> key, Node const *value) :
			m_parent{const_cast<Node *>(parent)},
			m_key{std::move(key)},
			m_value{const_cast<Node *>(value)}
		{
		}

		bool has_value() const noexcept
		{
			return m_value != nullptr;
		}

		Node *get() const
		{
			if (!has_value())
			{
				if (auto name{std::get_if<std::string>(&m_key)}; name)
				{
					m_value = &m_parent->AddProperty(*name);
				}
				else if (auto index{std::get_if<int32_t>(&m_key)}; index)
				{
					m_value = &m_parent->AddProperty(*index);
				}
				else
				{
					throw std::runtime_error("Could not get a node from the return");
				}
			}

			return m_value;
		}

		explicit operator bool() const noexcept { return has_value(); }

		operator Node &() const { return *get(); }

		Node &operator*() const { return *get(); }

		Node *operator->() const { return get(); }

		template <typename T>
		Node &operator=(const T &rhs) const
		{
			return *get() = rhs;
		}

		Return operator[](const std::string &key) const
		{
			/*if (!has_value())
			{
				return {this, key, nullptr};
			}*/

			return get()->operator[](key);
		}

		Return operator[](const uint32_t &index) const
		{
			/*if (!has_value())
			{
				return {this, index, nullptr};
			}*/

			return get()->operator[](index);
		}

	private:
		Node *m_parent{};
		std::variant<std::string, int32_t> m_key;
		mutable Node *m_value{};
	};

	Node() = default;

	Node(std::string value);

	Node(std::string value, const Type &type);

	Node(std::string value, std::vector<Property> &&properties);

	virtual ~Node() = default;

	virtual void Load(std::istream &inStream);

	virtual void Write(std::ostream &outStream, const Format &format = Format::Beautified) const;

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

	template<typename T>
	T GetValue() const;

	template<typename T>
	void SetValue(const T &value);

	const Type &GetType() const { return m_type; }

	void SetType(const Type &type) { m_type = type; }

	Node *GetParent() const { return m_parent; }

	std::string GetName() const;

	void SetName(const std::string &name);

	template<typename T>
	Node &Append(T value);

	template<typename ...Args>
	Node &Append(Args ... args);

	bool HasProperty(const std::string &name) const;

	Return GetProperty(const std::string &name) const;

	Return GetProperty(const uint32_t &index) const;

	Node &AddProperty(const std::string &name = "", Node &&node = {});

	Node &AddProperty(const uint32_t &index, Node &&node = {});

	void RemoveProperty(const std::string &name);

	const std::vector<Property> &GetProperties() const { return m_properties; };

	template <typename T>
	Node &operator=(const T &rhs);

	Return operator[](const std::string &key) const;

	Return operator[](const uint32_t &index) const;

	bool operator==(const Node &other) const;

	bool operator!=(const Node &other) const;

	bool operator<(const Node &other) const;

protected:
	std::string m_value;
	Type m_type{};
	Node *m_parent{};
	std::vector<Property> m_properties;
};
}

#include "Node.inl"

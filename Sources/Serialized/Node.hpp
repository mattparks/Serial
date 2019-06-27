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
			m_keys{std::move(key)},
			m_value{const_cast<Node *>(value)}
		{
		}

		Return(Return *parent, std::variant<std::string, int32_t> key) :
			m_parent{parent->m_parent},
			m_keys{parent->m_keys}
		{
			m_keys.emplace_back(std::move(key));
		}

		bool has_value() const noexcept
		{
			return m_value != nullptr;
		}

		Node *get()
		{
			if (!has_value())
			{
				// This will build the tree of nodes from the return keys tree.
				for (const auto &key : m_keys)
				{
					if (auto name{std::get_if<std::string>(&key)}; name)
					{
						m_value = &m_parent->AddProperty(*name);
					}
					else if (auto index{std::get_if<int32_t>(&key)}; index)
					{
						m_value = &m_parent->AddProperty(*index);
					}
					else
					{
						throw std::runtime_error("Key for node return is neither a int or a string");
					}

					// Because the last key will set parent to the value parent usage should be avoided.
					m_parent = m_value;
				}

				m_keys.erase(m_keys.begin(), m_keys.end() - 1);
			}

			return m_value;
		}

		explicit operator bool() const noexcept { return has_value(); }

		operator Node &() { return *get(); }

		Node &operator*() { return *get(); }

		Node *operator->() { return get(); }

		template <typename T>
		Node &operator=(const T &rhs)
		{
			return *get() = rhs;
		}

		Return operator[](const std::string &key)
		{
			if (!has_value())
			{
				return {this, key};
			}

			return get()->operator[](key);
		}

		Return operator[](const uint32_t &index)
		{
			if (!has_value())
			{
				return {this, index};
			}

			return get()->operator[](index);
		}

	private:
		Node *m_parent{};
		std::vector<std::variant<std::string, int32_t>> m_keys;
		Node *m_value{};
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

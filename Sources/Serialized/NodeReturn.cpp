#include "NodeReturn.hpp"

#include "Node.hpp"

namespace acid
{
NodeReturn::NodeReturn(Node const *parent, std::variant<std::string, int32_t> key, Node const *value) :
	m_parent{const_cast<Node *>(parent)},
	m_keys{std::move(key)},
	m_value{const_cast<Node *>(value)}
{
}

NodeReturn::NodeReturn(NodeReturn *parent, std::variant<std::string, int32_t> key) :
	m_parent{parent->m_parent},
	m_keys{parent->m_keys}
{
	m_keys.emplace_back(std::move(key));
}

bool NodeReturn::has_value() const noexcept
{
	return m_value != nullptr;
}

Node *NodeReturn::get()
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

std::string NodeReturn::GetName() const
{
	return *std::get_if<std::string>(&m_keys.back());
}

void NodeReturn::SetName(const std::string &name)
{
	if (!has_value())
	{
		m_keys.back() = name;
		return;
	}

	m_value->SetName(name);
}

NodeReturn NodeReturn::operator[](const std::string &key)
{
	if (!has_value())
	{
		return {this, key};
	}

	return get()->operator[](key);
}

NodeReturn NodeReturn::operator[](const uint32_t &index)
{
	if (!has_value())
	{
		return {this, index};
	}

	return get()->operator[](index);
}
}

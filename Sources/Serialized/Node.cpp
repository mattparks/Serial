#include "Node.hpp"

namespace acid
{
Node::Node(std::string value) :
	m_value{std::move(value)}
{
}

Node::Node(std::string value, std::vector<Property> &&properties) :
	m_value{std::move(value)},
	m_properties{std::move(properties)}
{
	for (auto &[propertyName, property] : m_properties)
	{
		property.m_parent = this;
	}
}

std::string Node::GetName() const
{
	if (m_parent == nullptr)
	{
		throw std::runtime_error("Cannot get name if parent is null");
	}

	for (const auto &[propertyName, property] : m_parent->m_properties)
	{
		if (property == *this)
		{
			return propertyName;
		}
	}

	return "";
}

void Node::SetName(const std::string &name)
{
	if (m_parent == nullptr)
	{
		throw std::runtime_error("Cannot set name if parent is null");
	}

	for (auto it{m_parent->m_properties.begin()}; it < m_parent->m_properties.end(); ++it)
	{
		if (it->second == *this)
		{
			it->first = name;
			return;
		}
	}
}

bool Node::HasProperty(const std::string &name) const
{
	for (const auto &[propertyName, property] : m_properties)
	{
		if (propertyName == name)
		{
			return true;
		}
	}

	return false;
}

const Node &Node::GetProperty(const std::string &name) const
{
	for (auto &property : m_properties)
	{
		if (property.first == name)
		{
			return property.second;
		}
	}

	throw std::runtime_error("Could not get property");
}

Node &Node::GetProperty(const std::string &name)
{
	for (auto &property : m_properties)
	{
		if (property.first == name)
		{
			return property.second;
		}
	}

	throw std::runtime_error("Could not get property");
}

Node &Node::AddProperty(const std::string &name, Node &&node)
{
	node.m_parent = this;
	return m_properties.emplace_back(name, std::move(node)).second;
}

void Node::RemoveProperty(const std::string &name)
{
	//node.m_parent = nullptr;
	m_properties.erase(std::remove_if(m_properties.begin(), m_properties.end(), [name](const auto &n)
	{
		return n.first == name;
	}), m_properties.end());
}

const Node &Node::operator[](const std::string &key) const
{
	return GetProperty(key);
}

Node &Node::operator[](const std::string &key)
{
	if (!HasProperty(key))
	{
		return AddProperty(key);
	}

	return GetProperty(key);
}

const Node &Node::operator[](const uint32_t &index) const
{
	return m_properties[index].second;
}

Node &Node::operator[](const uint32_t &index)
{
	m_properties.resize(std::max(m_properties.size(), static_cast<std::size_t>(index + 1)));
	return m_properties[index].second;
}

bool Node::operator==(const Node &other) const
{
	return m_value == other.m_value && m_properties.size() == other.m_properties.size() && 
		std::equal(m_properties.begin(), m_properties.end(), other.m_properties.begin(), [](const auto &left, const auto &right)
		{
			return left == right;
		});
}

bool Node::operator!=(const Node &other) const
{
	return !(*this == other);
}

bool Node::operator<(const Node &other) const
{
	return m_value < other.m_value || m_properties < other.m_properties;
}
}

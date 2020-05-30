#include "Node.hpp"

#include <algorithm>

namespace acid {
const Node::Format Node::Format::Beautified = Format(2, '\n', ' ', true);
const Node::Format Node::Format::Minified = Format(0, '\0', '\0', false);

static const Node NullNode = Node("null", Node::Type::Null);

Node::Node(const std::string &name, const Node &node) :
	Node(node) {
	SetName(name);
}

Node::Node(std::string value, Type type) :
	value(std::move(value)),
	type(type) {
}

Node::Node(std::string value, std::vector<Node> &&properties) :
	properties(std::move(properties)),
	value(std::move(value)) {
}

void Node::Clear() {
	properties.clear();
}

bool Node::IsValid() const {
	switch (type) {
	case Type::Token:
	case Type::Unknown:
		return false;
	case Type::Object:
	case Type::Array:
		return !properties.empty();
	case Type::Null:
		return true;
	default:
		return !value.empty();
	}
}

bool Node::HasProperty(const std::string &name) const {
	for (const auto &property : properties) {
		if (property.name == name)
			return true;
	}

	return false;
}

NodeConstView Node::GetProperty(const std::string &name) const {
	for (const auto &property : properties) {
		if (property.name == name)
			return {this, name, &property};
	}

	return {this, name, nullptr};
}

NodeConstView Node::GetProperty(uint32_t index) const {
	if (index < properties.size())
		return {this, index, &properties[index]};

	return {this, index, nullptr};
}

// TODO: Duplicate
NodeView Node::GetProperty(const std::string &name) {
	for (auto &property : properties) {
		if (property.name == name)
			return {this, name, &property};
	}

	return {this, name, nullptr};
}

// TODO: Duplicate
NodeView Node::GetProperty(uint32_t index) {
	if (index < properties.size())
		return {this, index, &properties[index]};

	return {this, index, nullptr};
}

Node &Node::AddProperty() {
	return properties.emplace_back();
}

Node &Node::AddProperty(const std::string &name, Node &&node) {
	node.name = name;
	return properties.emplace_back(std::move(node));
}

Node &Node::AddProperty(const std::string &name) {
	Node node;
	node.name = name;
	return properties.emplace_back(std::move(node));
}

Node &Node::AddProperty(uint32_t index, Node &&node) {
	properties.resize(std::max(properties.size(), static_cast<std::size_t>(index + 1)), NullNode);
	return properties[index] = std::move(node);
}

Node &Node::AddProperty(uint32_t index) {
	properties.resize(std::max(properties.size(), static_cast<std::size_t>(index + 1)), NullNode);
	return properties[index];
}

void Node::RemoveProperty(const std::string &name) {
	//node.parent = nullptr;
	properties.erase(std::remove_if(properties.begin(), properties.end(), [name](const auto &n) {
		return n.name == name;
	}), properties.end());
}

void Node::RemoveProperty(const Node &node) {
	//node.parent = nullptr;
	properties.erase(std::remove_if(properties.begin(), properties.end(), [node](const auto &n) {
		return n == node;
	}), properties.end());
}

std::vector<NodeConstView> Node::GetProperties(const std::string &name) const {
	std::vector<NodeConstView> properties;

	for (const auto &property : this->properties) {
		if (property.name == name)
			properties.emplace_back(NodeConstView(this, name, &property));
	}

	return properties;
}

NodeConstView Node::GetPropertyWithBackup(const std::string &name, const std::string &backupName) const {
	if (auto p1 = GetProperty(name))
		return p1;
	if (auto p2 = GetProperty(backupName))
		return p2;
	return {this, name, nullptr};
}

NodeConstView Node::GetPropertyWithValue(const std::string &propertyName, const std::string &propertyValue) const {
	for (const auto &property : properties) {
		auto properties1 = property.GetProperties(propertyName);
		if (properties1.empty())
			return {this, propertyName, nullptr};

		for (auto &property1 : properties1) {
			if (property1 && property1->GetValue() == propertyValue)
				return {this, propertyName, &property};
		}
	}

	return {this, propertyName, nullptr};
}

// TODO: Duplicate
std::vector<NodeView> Node::GetProperties(const std::string &name) {
	std::vector<NodeView> properties;

	for (auto &property : this->properties) {
		if (property.name == name)
			properties.emplace_back(NodeView(this, name, &property));
	}

	return properties;
}

// TODO: Duplicate
NodeView Node::GetPropertyWithBackup(const std::string &name, const std::string &backupName) {
	if (auto p1 = GetProperty(name))
		return p1;
	if (auto p2 = GetProperty(backupName))
		return p2;
	return {this, name, nullptr};
}

// TODO: Duplicate
NodeView Node::GetPropertyWithValue(const std::string &propertyName, const std::string &propertyValue) {
	for (auto &property : properties) {
		auto properties1 = property.GetProperties(propertyName);
		if (properties1.empty())
			return {this, propertyName, nullptr};

		for (auto &property1 : properties1) {
			if (property1 && property1->GetValue() == propertyValue)
				return {this, propertyName, &property};
		}
	}

	return {this, std::string(propertyName), nullptr};
}

NodeConstView Node::operator[](const std::string &key) const {
	return GetProperty(key);
}

NodeConstView Node::operator[](uint32_t index) const {
	return GetProperty(index);
}

// TODO: Duplicate
NodeView Node::operator[](const std::string &key) {
	return GetProperty(key);
}

// TODO: Duplicate
NodeView Node::operator[](uint32_t index) {
	return GetProperty(index);
}

Node &Node::operator=(Node &&rhs) noexcept {
	properties = std::move(rhs.properties);
	//name = std::move(rhs.name);
	value = std::move(rhs.value);
	type = std::move(rhs.type);
	return *this;
}

bool Node::operator==(const Node &rhs) const {
	return value == rhs.value && properties.size() == rhs.properties.size() &&
		std::equal(properties.begin(), properties.end(), rhs.properties.begin(), [](const auto &left, const auto &right) {
			return left == right;
		});
}

bool Node::operator!=(const Node &rhs) const {
	return !operator==(rhs);
}

bool Node::operator<(const Node &rhs) const {
	if (name < rhs.name) return true;
	if (rhs.name < name) return false;

	if (value < rhs.value) return true;
	if (rhs.value < value) return false;

	if (properties < rhs.properties) return true;
	if (rhs.properties < properties) return false;
	
	return false;
}
}

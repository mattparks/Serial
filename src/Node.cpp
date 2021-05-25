#include "Node.hpp"

#include <algorithm>

namespace serial {
void Node::Clear() {
	properties.clear();
}

bool Node::IsValid() const {
	switch (type) {
	case NodeType::Token:
	case NodeType::Unknown:
		return false;
	case NodeType::Object:
	case NodeType::Array:
		return !properties.empty();
	case NodeType::Null:
		return true;
	default:
		return !value.empty();
	}
}

bool Node::HasProperty(const NodeKey &key) const {
	return properties.find(key) != properties.end();
}

NodeConstView Node::GetProperty(const NodeKey &key) const {
	if (auto it = properties.find(key); it != properties.end())
		return {this, key, &it->second};
	return {this, key, nullptr};
}

// TODO: Duplicate
NodeView Node::GetProperty(const NodeKey &key) {
	if (auto it = properties.find(key); it != properties.end())
		return {this, key, &it->second};
	return {this, key, nullptr};
}

Node &Node::AddProperty(const Node &node) {
	// TODO: generate a real key
	return properties.emplace(properties.size(), node).first->second;
}

Node &Node::AddProperty(Node &&node) {
	// TODO: generate a real key
	return properties.emplace(properties.size(), std::move(node)).first->second;
}

Node &Node::AddProperty(const NodeKey &key, const Node &node) {
	return properties.emplace(key, node).first->second;
}

Node &Node::AddProperty(const NodeKey &key, Node &&node) {
	return properties.emplace(key, std::move(node)).first->second;
}

Node Node::RemoveProperty(const NodeKey &key) {
	if (auto it = properties.find(key); it != properties.end()) {
		auto result = std::move(it->second);
		properties.erase(it);
		return result;
	}
	return {};
}

Node Node::RemoveProperty(const Node &node) {
	for (auto it = properties.begin(); it != properties.end(); ) {
		if (it->second == node) {
			auto result = std::move(it->second);
			it = properties.erase(it);
			return result;
		}
		++it;
	}
	return {};
}

NodeConstView Node::GetPropertyWithBackup(const NodeKey &key, const NodeKey &backupKey) const {
	if (auto p1 = GetProperty(key))
		return p1;
	if (auto p2 = GetProperty(backupKey))
		return p2;
	return {this, key, nullptr};
}

NodeConstView Node::GetPropertyWithValue(const NodeKey &key, const std::string &propertyValue) const {
	// TODO: previous implementation was confusing, why does this method exist?
	for (const auto &[propertyKey, property] : properties) {
		if (auto property1 = property.GetProperty(key); property1->GetValue() == propertyValue)
			return {this, key, &property};
		return {this, key, nullptr};
	}

	return {this, key, nullptr};
}

// TODO: Duplicate
NodeView Node::GetPropertyWithBackup(const NodeKey &key, const NodeKey &backupKey) {
	if (auto p1 = GetProperty(key))
		return p1;
	if (auto p2 = GetProperty(backupKey))
		return p2;
	return {this, key, nullptr};
}

// TODO: Duplicate
NodeView Node::GetPropertyWithValue(const NodeKey &key, const std::string &propertyValue) {
	for (auto &[propertyKey, property] : properties) {
		if (auto property1 = property.GetProperty(key); property1->GetValue() == propertyValue)
			return {this, key, &property};
		return {this, key, nullptr};
	}

	return {this, key, nullptr};
}

NodeConstView Node::operator[](const NodeKey &key) const {
	return GetProperty(key);
}

// TODO: Duplicate
NodeView Node::operator[](const NodeKey &key) {
	return GetProperty(key);
}

Node &Node::operator=(const NodeConstView &rhs) {
	return operator=(*rhs);
}

Node &Node::operator=(NodeConstView &&rhs) {
	return operator=(*rhs);
}

Node &Node::operator=(NodeView &rhs) {
	return operator=(*rhs);
}

Node &Node::operator=(NodeView &&rhs) {
	return operator=(*rhs);
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
	if (value < rhs.value) return true;
	if (rhs.value < value) return false;

//	if (properties < rhs.properties) return true;
//	if (rhs.properties < properties) return false;

	return false;
}
}

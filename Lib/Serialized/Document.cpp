#include "Document.hpp"

namespace acid {
static const Document NullDocument = Document("null", Document::Type::Null);

Document::Document(std::string value, Type type) :
	m_value(std::move(value)),
	m_type(type) {
}

Document::Document(std::string value, std::vector<Document> &&properties) :
	m_properties(std::move(properties)),
	m_value(std::move(value)) {
}

void Document::Clear() {
	m_properties.clear();
}

bool Document::IsValid() const {
	switch (m_type) {
	case Type::Object:
	case Type::Array:
		return !m_properties.empty();
	default:
		return !m_value.empty();
	}
}

bool Document::HasProperty(const std::string &name) const {
	for (const auto &property : m_properties) {
		if (property.m_name == name) {
			return true;
		}
	}

	return false;
}

DocumentReturn Document::GetProperty(const std::string &name) const {
	for (const auto &property : m_properties) {
		if (property.m_name == name) {
			return {this, name, &property};
		}
	}

	return {this, name, nullptr};
}

DocumentReturn Document::GetProperty(uint32_t index) const {
	if (index < m_properties.size()) {
		return {this, index, &m_properties[index]};
	}

	return {this, index, nullptr};
}

Document &Document::AddProperty() {
	return m_properties.emplace_back();
}

Document &Document::AddProperty(const std::string &name, Document &&node) {
	node.m_name = name;
	return m_properties.emplace_back(std::move(node));
}

Document &Document::AddProperty(uint32_t index, Document &&node) {
	m_properties.resize(std::max(m_properties.size(), static_cast<std::size_t>(index + 1)), NullDocument);
	return m_properties[index] = std::move(node);
}

void Document::RemoveProperty(const std::string &name) {
	//node.m_parent = nullptr;
	m_properties.erase(std::remove_if(m_properties.begin(), m_properties.end(), [name](const auto &n) {
		return n.GetName() == name;
	}), m_properties.end());
}

void Document::RemoveProperty(const Document &node) {
	//node.m_parent = nullptr;
	m_properties.erase(std::remove_if(m_properties.begin(), m_properties.end(), [node](const auto &n) {
		return n == node;
	}), m_properties.end());
}

DocumentReturn Document::operator[](const std::string &key) const {
	return GetProperty(key);
}

DocumentReturn Document::operator[](uint32_t index) const {
	return GetProperty(index);
}

bool Document::operator==(const Document &other) const {
	return m_value == other.m_value && m_properties.size() == other.m_properties.size() &&
		std::equal(m_properties.begin(), m_properties.end(), other.m_properties.begin(), [](const auto &left, const auto &right) {
			return left == right;
		});
}

bool Document::operator!=(const Document &other) const {
	return !(*this == other);
}

bool Document::operator<(const Document &other) const {
	if (m_name < other.m_name) return true;
	if (other.m_name < m_name) return false;

	if (m_value < other.m_value) return true;
	if (other.m_value < m_value) return false;

	if (m_properties < other.m_properties) return true;
	if (other.m_properties < m_properties) return false;
	
	return false;
}
}

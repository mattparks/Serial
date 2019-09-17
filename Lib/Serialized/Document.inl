#pragma once

#include "Document.hpp"

#include <map>
#include <filesystem>
#include <algorithm>

#include "Helpers/ConstExpr.hpp"
#include "Helpers/String.hpp"
#include "Resources/Resource.hpp"

namespace acid {
template<typename T>
T Document::Get() const {
	T value;
	*this >> value;
	return value;
}

template<typename T>
T Document::Get(const T &fallback) const {
	if (!IsValid()) {
		return fallback;
	}

	return Get<T>();
}

template<typename T>
void Document::Get(T &dest) const {
	if (IsValid()) {
		*this >> dest;
	}
}

template<typename T, typename K>
void Document::Get(T &dest, const K &fallback) const {
	if (IsValid()) {
		*this >> dest;
		return;
	}

	dest = fallback;
}

template<typename T>
void Document::Set(const T &value) {
	*this << value;
}

template<typename T>
Document &Document::Append(const T &value) {
	AddProperty() << value;
	return *this;
}

template<typename ...Args>
Document &Document::Append(const Args &...args) {
	(Append(args), ...);
	return *this;
}

template<typename T>
Document &Document::operator=(const T &rhs) {
	Set(rhs);
	return *this;
}

/*const Document &operator>>(const Document &node, std::nullptr_t &object)
{
	object = nullptr;
	return node;
}*/

inline Document &operator<<(Document &node, const std::nullptr_t &object) {
	node.SetValue("null");
	node.SetType(Document::Type::Null);
	return node;
}

inline const Document &operator>>(const Document &node, bool &object) {
	object = String::From<bool>(node.GetValue());
	return node;
}

inline Document &operator<<(Document &node, const bool &object) {
	node.SetValue(String::To(object));
	node.SetType(Document::Type::Boolean);
	return node;
}

template<typename T>
std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, const Document &> operator>>(const Document &node, T &object) {
	object = String::From<T>(node.GetValue());
	return node;
}

template<typename T>
std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, Document &> operator<<(Document &node, const T &object) {
	node.SetValue(String::To(object));
	node.SetType(Document::Type::Number);
	return node;
}

/*template<typename T>
std::enable_if_t<std::is_class_v<T> || std::is_pointer_v<T>, const Document &> operator>>(const Document &node, T &object)
{
	node >> ConstExpr::AsRef(object);
	return node;
}*/

template<typename T>
std::enable_if_t<std::is_class_v<T> || std::is_pointer_v<T>, Document &> operator<<(Document &node, const T &object) {
	if (ConstExpr::AsPtr(object) == nullptr) {
		return node << nullptr;
	}

	node << ConstExpr::AsRef(object);
	return node;
}

template<typename T>
const Document &operator>>(const Document &node, std::unique_ptr<T> &object) {
	object = std::make_unique<T>();
	node >> *object;
	return node;
}

template<typename T>
Document &operator<<(Document &node, const std::unique_ptr<T> &object) {
	if (object == nullptr) {
		return node << nullptr;
	}

	node << *object;
	return node;
}

template<typename T>
const Document &operator>>(const Document &node, std::shared_ptr<T> &object) {
	// TODO: Abstract Resource streams out from shared_ptr.
	if constexpr (std::is_base_of_v<Resource, T>) {
		object = T::Create(node);
		return node;
	} else {
		object = std::make_shared<T>();
		node >> *object;
		return node;
	}
}

template<typename T>
Document &operator<<(Document &node, const std::shared_ptr<T> &object) {
	if (object == nullptr) {
		return node << nullptr;
	}

	node << *object;
	return node;
}

/*inline const Document &operator>>(const Document &node, char *&string) {
	std::strcpy(string, node.GetValue().c_str());
	return node;
}

inline Document &operator<<(Document &node, const char *string) {
	node.SetValue(string);
	node.SetType(Document::Type::String);
	return node;
}*/

inline const Document &operator>>(const Document &node, std::string &string) {
	string = node.GetValue();
	return node;
}

inline Document &operator<<(Document &node, const std::string &string) {
	node.SetValue(string);
	node.SetType(Document::Type::String);
	return node;
}

inline const Document &operator>>(const Document &node, std::filesystem::path &object) {
	object = node.GetValue();
	return node;
}

inline Document &operator<<(Document &node, const std::filesystem::path &object) {
	auto str = object.string();
	std::replace(str.begin(), str.end(), '\\', '/');
	node.SetValue(str);
	node.SetType(Document::Type::String);
	return node;
}

template<typename T, typename K>
const Document &operator>>(const Document &node, std::pair<T, K> &pair) {
	pair.first = String::From<T>(node.GetName());
	node >> pair.second;
	return node;
}

template<typename T, typename K>
Document &operator<<(Document &node, const std::pair<T, K> &pair) {
	node.SetName(String::To(pair.first));
	node << pair.second;
	return node;
}

template<typename T>
const Document &operator>>(const Document &node, std::optional<T> &optional) {
	if (node.GetValue() != "null") {
		T x;
		node >> x;
		optional = std::move(x);
	} else {
		optional = std::nullopt;
	}

	return node;
}

template<typename T>
Document &operator<<(Document &node, const std::optional<T> &optional) {
	if (optional) {
		return node << *optional;
	}

	return node << nullptr;
}

template<typename T>
const Document &operator>>(const Document &node, std::vector<T> &vector) {
	vector = {};
	vector.reserve(node.GetProperties().size());

	for (const auto &property : node.GetProperties()) {
		T x;
		property >> x;
		vector.emplace_back(std::move(x));
	}

	return node;
}

template<typename T>
Document &operator<<(Document &node, const std::vector<T> &vector) {
	for (const auto &x : vector) {
		node.AddProperty() << x;
	}

	node.SetType(Document::Type::Array);
	return node;
}

template<typename T, typename K>
const Document &operator>>(const Document &node, std::map<T, K> &map) {
	map = {};

	for (const auto &property : node.GetProperties()) {
		std::pair<T, K> pair;
		property >> pair;
		map.emplace(std::move(pair));
	}

	return node;
}

template<typename T, typename K>
Document &operator<<(Document &node, const std::map<T, K> &map) {
	for (const auto &x : map) {
		node.AddProperty() << x;
	}

	node.SetType(Document::Type::Array);
	return node;
}
}

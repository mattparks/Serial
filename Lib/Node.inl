#pragma once

#include "Node.hpp"

#include <algorithm>
#include <codecvt>
#include <cstring>
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>
#include <map>
#include <set>
#include <sstream>

namespace acid {
namespace priv {
template<typename T> struct is_optional : std::false_type {};
template<typename T> struct is_optional<std::optional<T>> : std::true_type {};
template<typename T> inline constexpr bool is_optional_v = is_optional<T>::value;

/**
 * Converts a type to a string.
 * @tparam T The type to convert from.
 * @param val The value to convert.
 * @return The value as a string.
 */
template<typename T>
static std::string To(T val) {
	if constexpr (std::is_same_v<std::string, T> || std::is_same_v<const char *, T>) {
		return val;
	} else if constexpr (std::is_enum_v<T>) {
		typedef typename std::underlying_type<T>::type safe_type;
		return std::to_string(static_cast<safe_type>(val));
	} else if constexpr (std::is_same_v<bool, T>) {
		return val ? "true" : "false";
	} else if constexpr (std::is_same_v<std::nullptr_t, T>) {
		return "null";
	} else if constexpr (is_optional_v<T>) {
		if (!val.has_value())
			return "null";
		return To(*val);
	} else {
		return std::to_string(val);
	}
}

/**
 * Converts a string to a type.
 * @tparam T The type to convert to.
 * @param str The string to convert.
 * @return The string as a value.
 */
template<typename T>
static T From(const std::string &str) {
	if constexpr (std::is_same_v<std::string, T>) {
		return str;
	} else if constexpr (std::is_enum_v<T>) {
		typedef typename std::underlying_type<T>::type safe_type;
		return static_cast<T>(From<safe_type>(str));
	} else if constexpr (std::is_same_v<bool, T>) {
		return str == "true" || From<std::optional<int32_t>>(str) == 1;
	} else if constexpr (is_optional_v<T>) {
		typedef typename T::value_type base_type;
		base_type temp;
		std::istringstream iss(str);

		if ((iss >> temp).fail())
			return std::nullopt;
		return temp;
	} else {
		long double temp;
		std::istringstream iss(str);
		iss >> temp;
		return static_cast<T>(temp);
	}
}
}

template<typename NodeParser, typename ... Args>
void Node::ParseString(std::string_view string, Args ... args) {
	NodeParser::ParseString(*this, string, args...);
}

template<typename NodeParser, typename ... Args>
void Node::WriteStream(std::ostream &stream, Format format, Args... args) const {
	NodeParser::WriteStream(*this, stream, format, args...);
}

template<typename NodeParser, typename _Elem, typename ... Args>
void Node::ParseStream(std::basic_istream<_Elem> &stream, Args ... args) {
	// We must read as UTF8 chars.
	if constexpr (!std::is_same_v<_Elem, char>) {
#ifndef _MSC_VER
		throw std::runtime_error("Cannot dynamicly parse wide streams on GCC or Clang");
#else
		stream.imbue(std::locale(stream.getloc(), new std::codecvt_utf8<char>));
#endif
	}

	// Reading into a string before iterating is much faster.
	std::string s(std::istreambuf_iterator<_Elem>(stream), {});
	ParseString<NodeParser>(s, args...);
}

template<typename NodeParser, typename _Elem, typename ... Args>
std::basic_string<_Elem> Node::WriteString(const Format &format, Args ... args) const {
	std::basic_stringstream<_Elem> stream;
	WriteStream<NodeParser>(stream, format, args...);
	return stream.str();
}

template<typename T>
T Node::Get() const {
	T value;
	*this >> value;
	return value;
}

template<typename T>
T Node::Get(const T &fallback) const {
	if (!IsValid())
		return fallback;
	
	return Get<T>();
}

template<typename T>
bool Node::Get(T &dest) const {
	if (!IsValid())
		return false;
	
	*this >> dest;
	return true;
}

template<typename T, typename K>
bool Node::Get(T &dest, const K &fallback) const {
	if (!IsValid()) {
		dest = fallback;
		return false;
	}

	*this >> dest;
	return true;
}

template<typename T>
void Node::Set(const T &value) {
	*this << value;
}

template<typename T>
Node &Node::Append(const T &value) {
	AddProperty() << value;
	return *this;
}

template<typename ...Args>
Node &Node::Append(const Args &...args) {
	(Append(args), ...);
	return *this;
}

template<typename T>
Node &Node::operator=(const T &rhs) {
	Set(rhs);
	return *this;
}

inline const Node &operator>>(const Node &node, Node &object) {
	object = node;
	return node;
}

inline Node &operator<<(Node &node, const Node &object) {
	node = object;
	return node;
}

/*const Node &operator>>(const Node &node, std::nullptr_t &object) {
	object = nullptr;
	return node;
}*/

inline Node &operator<<(Node &node, const std::nullptr_t &object) {
	node.SetValue("");
	node.SetType(Node::Type::Null);
	return node;
}

template<typename T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
Node &operator<<(Node &node, const T object) {
	if (!object)
		return node << nullptr;

	node << *object;
	return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::unique_ptr<T> &object) {
	object = std::make_unique<T>();
	node >> *object;
	return node;
}

template<typename T>
Node &operator<<(Node &node, const std::unique_ptr<T> &object) {
	if (!object)
		return node << nullptr;

	node << *object;
	return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::shared_ptr<T> &object) {
	object = std::make_shared<T>();
	node >> *object;
	return node;
}

template<typename T>
Node &operator<<(Node &node, const std::shared_ptr<T> &object) {
	if (!object)
		return node << nullptr;

	node << *object;
	return node;
}

inline const Node &operator>>(const Node &node, bool &object) {
	object = priv::From<bool>(node.GetValue());
	return node;
}

inline Node &operator<<(Node &node, bool object) {
	node.SetValue(priv::To(object));
	node.SetType(Node::Type::Boolean);
	return node;
}

template<typename T, std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, int> = 0>
const Node &operator>>(const Node &node, T &object) {
	object = priv::From<T>(node.GetValue());
	return node;
}

template<typename T, std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, int> = 0>
Node &operator<<(Node &node, T object) {
	node.SetValue(priv::To(object));
	node.SetType(std::is_floating_point_v<T> ? Node::Type::Decimal : Node::Type::Integer);
	return node;
}

inline const Node &operator>>(const Node &node, char *&string) {
	std::strcpy(string, node.GetValue().c_str());
	return node;
}

inline Node &operator<<(Node &node, const char *string) {
	node.SetValue(string);
	node.SetType(Node::Type::String);
	return node;
}

//inline const Node &operator>>(const Node &node, std::string_view &string)

inline Node &operator<<(Node &node, std::string_view string) {
	node.SetValue(string.data());
	node.SetType(Node::Type::String);
	return node;
}

inline const Node &operator>>(const Node &node, std::string &string) {
	string = node.GetValue();
	return node;
}

inline Node &operator<<(Node &node, const std::string &string) {
	node.SetValue(string);
	node.SetType(Node::Type::String);
	return node;
}

inline const Node &operator>>(const Node &node, std::filesystem::path &object) {
	object = node.GetValue();
	return node;
}

inline Node &operator<<(Node &node, const std::filesystem::path &object) {
	auto str = object.string();
	std::replace(str.begin(), str.end(), '\\', '/');
	node.SetValue(str);
	node.SetType(Node::Type::String);
	return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::optional<T> &optional) {
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
Node &operator<<(Node &node, const std::optional<T> &optional) {
	if (optional)
		return node << *optional;

	return node << nullptr;
}

template<typename T>
const Node &operator>>(const Node &node, std::vector<T> &vector) {
	vector.clear();
	vector.reserve(node.GetProperties().size());

	for (const auto &[propertyName, property] : node.GetProperties()) {
		T x;
		property >> x;
		vector.emplace_back(std::move(x));
	}

	return node;
}

template<typename T>
Node &operator<<(Node &node, const std::vector<T> &vector) {
	for (const auto &x : vector)
		node.AddProperty() << x;

	node.SetType(Node::Type::Array);
	return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::set<T> &vector) {
	vector.clear();

	for (const auto &[propertyName, property] : node.GetProperties()) {
		T x;
		property >> x;
		vector.emplace(std::move(x));
	}

	return node;
}

template<typename T>
Node &operator<<(Node &node, const std::set<T> &vector) {
	for (const auto &x : vector)
		node.AddProperty() << x;

	node.SetType(Node::Type::Array);
	return node;
}

template<typename T, typename K>
const Node &operator>>(const Node &node, std::map<T, K> &map) {
	map.clear();

	for (const auto &[propertyName, property] : node.GetProperties()) {
		std::pair<T, K> pair;
		pair.first = priv::From<T>(propertyName);
		property >> pair.second;
		map.emplace(std::move(pair));
	}

	return node;
}

template<typename T, typename K>
Node &operator<<(Node &node, const std::map<T, K> &map) {
	for (const auto &x : map) {
		node.AddProperty(priv::To(x.first)) << x.second;
	}

	node.SetType(Node::Type::Array);
	return node;
}
}

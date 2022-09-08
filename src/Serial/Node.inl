#pragma once

#include "Node.hpp"

#include <array>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <forward_list>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Utils.hpp"

namespace serial {
template<typename T, typename>
void Node::parseString(std::string_view string) {
    T::Load(*this, string);
}

template<typename T, typename>
void Node::writeStream(std::ostream &stream, NodeFormat::Format format) const {
    T::Write(*this, stream, format);
}

// TODO: Duplicate parseStream/writeString templates from NodeFormat.
template<typename T, typename _Elem, typename>
void Node::parseStream(std::basic_istream<_Elem> &stream) {
#if !defined(_MSC_VER) && !defined(__EMSCRIPTEN__)
    // We must read as UTF8 chars.
    if constexpr (!std::is_same_v<_Elem, char>)
        stream.imbue(std::locale(stream.getloc(), new std::codecvt_utf8<char>));
#endif

    // Reading into a string before iterating is much faster.
    std::string s(std::istreambuf_iterator<_Elem>(stream), {});
    return T::Load(*this, s);
}

template<typename T, typename _Elem, typename>
std::basic_string<_Elem> Node::writeString(NodeFormat::Format format) const {
    std::basic_ostringstream<_Elem> stream;
    T::Write(*this, stream, format);
    return stream.str();
}

template<typename T>
T Node::get() const {
    T value;
    *this >> value;
    return value;
}

template<typename T>
T Node::getWithFallback(const T &fallback) const {
    if (!valid())
        return fallback;
    
    return get<T>();
}

template<typename T>
bool Node::get(T &dest) const {
    if (!valid())
        return false;
    
    *this >> dest;
    return true;
}

template<typename T, typename K>
bool Node::getWithFallback(T &dest, const K &fallback) const {
    if (!valid()) {
        dest = fallback;
        return false;
    }

    *this >> dest;
    return true;
}

template<typename T>
bool Node::get(T &&dest) const {
    if (!valid())
        return false;

    *this >> dest;
    return true;
}

template<typename T, typename K>
bool Node::getWithFallback(T &&dest, const K &fallback) const {
    if (!valid()) {
        dest = fallback;
        return false;
    }

    *this >> dest;
    return true;
}

template<typename T>
void Node::set(const T &value) {
    *this << value;
}

template<typename T>
void Node::set(T &&value) {
    *this << value;
}

template<typename T>
Node &Node::append(const T &value) {
    add() << value;
    return *this;
}

template<typename ...Args>
Node &Node::append(const Args &...args) {
    (append(args), ...);
    return *this;
}

template<typename T>
Node &Node::operator=(const T &rhs) {
    set(rhs);
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
    node.value("");
    node.type(NodeType::Null);
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
    object = utils::From<bool>(node.value());
    return node;
}

inline Node &operator<<(Node &node, bool object) {
    node.value(utils::To(object));
    node.type(NodeType::Boolean);
    return node;
}

template<typename T, std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, int> = 0>
const Node &operator>>(const Node &node, T &object) {
    object = utils::From<T>(node.value());
    return node;
}

template<typename T, std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, int> = 0>
Node &operator<<(Node &node, T object) {
    node.value(utils::To(object));
    node.type(std::is_floating_point_v<T> ? NodeType::Decimal : NodeType::Integer);
    return node;
}

inline const Node &operator>>(const Node &node, char *&string) {
    std::strcpy(string, node.value().c_str());
    return node;
}

inline Node &operator<<(Node &node, const char *string) {
    node.value(string);
    node.type(NodeType::String);
    return node;
}

//inline const Node &operator>>(const Node &node, std::string_view &string)

inline Node &operator<<(Node &node, std::string_view string) {
    node.value(std::string(string));
    node.type(NodeType::String);
    return node;
}

inline const Node &operator>>(const Node &node, std::string &string) {
    string = node.value();
    return node;
}

inline Node &operator<<(Node &node, const std::string &string) {
    node.value(string);
    node.type(NodeType::String);
    return node;
}

inline const Node &operator>>(const Node &node, std::wstring &string) {
    string = utils::ConvertUtf16(node.value());
    return node;
}

inline Node &operator<<(Node &node, const std::wstring &string) {
    node.value(utils::ConvertUtf8(string));
    node.type(NodeType::String);
    return node;
}

inline const Node &operator>>(const Node &node, std::filesystem::path &object) {
    object = node.value();
    return node;
}

inline Node &operator<<(Node &node, const std::filesystem::path &object) {
    auto str = object.string();
    std::replace(str.begin(), str.end(), '\\', '/');
    node.value(str);
    node.type(NodeType::String);
    return node;
}

template<typename T, typename K>
const Node &operator>>(const Node &node, std::chrono::duration<T, K> &duration) {
    T x;
    node >> x;
    duration = std::chrono::duration<T, K>(x);
    return node;
}

template<typename T, typename K>
Node &operator<<(Node &node, const std::chrono::duration<T, K> &duration) {
    return node << duration.count();
}

template<typename T>
const Node &operator>>(const Node &node, std::chrono::time_point<T> &timePoint) {
    typename std::chrono::time_point<T>::duration x;
    node >> x;
    timePoint = std::chrono::time_point<T>(x);
    return node;
}

template<typename T>
Node &operator<<(Node &node, const std::chrono::time_point<T> &timePoint) {
    return node << timePoint.time_since_epoch();
}

template<typename T, typename K>
const Node &operator>>(const Node &node, std::pair<T, K> &pair) {
    node["first"].get(pair.first);
    node["second"].get(pair.second);
    return node;
}

template<typename T, typename K>
Node &operator<<(Node &node, const std::pair<T, K> &pair) {
    node["first"].set(pair.first);
    node["second"].set(pair.second);
    return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::optional<T> &optional) {
    if (node.type() != NodeType::Null) {
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
    vector.reserve(node.properties().size());

    for (const auto &[propertyName, property] : node.properties())
        property >> vector.emplace_back();

    return node;
}

template<typename T>
Node &operator<<(Node &node, const std::vector<T> &vector) {
    for (const auto &x : vector)
        node.add() << x;

    node.type(NodeType::Array);
    return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::set<T> &set) {
    set.clear();
    auto where = set.end();

    for (const auto &[propertyName, property] : node.properties()) {
        T x;
        property >> x;
        where = set.insert(where, std::move(x));
    }

    return node;
}

template<typename T>
Node &operator<<(Node &node, const std::set<T> &set) {
    for (const auto &x : set)
        node.add() << x;

    node.type(NodeType::Array);
    return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::unordered_set<T> &set) {
    set.clear();
    auto where = set.end();

    for (const auto &[propertyName, property] : node.properties()) {
        T x;
        property >> x;
        where = set.insert(where, std::move(x));
    }

    return node;
}

template<typename T>
Node &operator<<(Node &node, const std::unordered_set<T> &set) {
    for (const auto &x : set)
        node.add() << x;

    node.type(NodeType::Array);
    return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::multiset<T> &set) {
    set.clear();
    auto where = set.end();

    for (const auto &[propertyName, property] : node.properties()) {
        T x;
        property >> x;
        where = set.insert(where, std::move(x));
    }

    return node;
}

template<typename T>
Node &operator<<(Node &node, const std::multiset<T> &set) {
    for (const auto &x : set)
        node.add() << x;

    node.type(NodeType::Array);
    return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::unordered_multiset<T> &set) {
    set.clear();
    auto where = set.end();

    for (const auto &[propertyName, property] : node.properties()) {
        T x;
        property >> x;
        where = set.insert(where, std::move(x));
    }

    return node;
}

template<typename T>
Node &operator<<(Node &node, const std::unordered_multiset<T> &set) {
    for (const auto &x : set)
        node.add() << x;

    node.type(NodeType::Array);
    return node;
}

template<typename T, std::size_t Size>
const Node &operator>>(const Node &node, std::array<T, Size> &array) {
    array = {};

    for (auto [i, propertyPair] : utils::Enumerate(node.properties()))
        propertyPair.second >> array[i];

    return node;
}

template<typename T, std::size_t Size>
Node &operator<<(Node &node, const std::array<T, Size> &array) {
    for (const auto &x : array)
        node.add() << x;

    node.type(NodeType::Array);
    return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::list<T> &list) {
    list.clear();

    for (const auto &[propertyName, property] : node.properties()) {
        T x;
        property >> x;
        list.emplace_back(std::move(x));
    }

    return node;
}

template<typename T>
Node &operator<<(Node &node, const std::list<T> &list) {
    for (const auto &x : list)
        node.add() << x;

    node.type(NodeType::Array);
    return node;
}

template<typename T>
const Node &operator>>(const Node &node, std::forward_list<T> &list) {
    list.clear();

    for (auto it = node.properties().rbegin(); it != node.properties().rend(); ++it) {
        T x;
        *it >> x;
        list.emplace_front(std::move(x));
    }

    return node;
}

template<typename T>
Node &operator<<(Node &node, const std::forward_list<T> &list) {
    for (const auto &x : list)
        node.add() << x;

    node.type(NodeType::Array);
    return node;
}

template<typename T, typename K>
const Node &operator>>(const Node &node, std::map<T, K> &map) {
    map.clear();
    auto where = map.end();

    for (const auto &[propertyName, property] : node.properties()) {
        std::pair<T, K> pair;
        pair.first = utils::From<T>(propertyName);
        property >> pair.second;
        where = map.insert(where, std::move(pair));
    }

    return node;
}

template<typename T, typename K>
Node &operator<<(Node &node, const std::map<T, K> &map) {
    for (const auto &pair : map)
        node.add(utils::To(pair.first)) << pair.second;

    node.type(NodeType::Object);
    return node;
}

template<typename T, typename K>
const Node &operator>>(const Node &node, std::unordered_map<T, K> &map) {
    map.clear();
    auto where = map.end();

    for (const auto &[propertyName, property] : node.properties()) {
        std::pair<T, K> pair;
        pair.first = utils::From<T>(propertyName);
        property >> pair.second;
        where = map.insert(where, std::move(pair));
    }

    return node;
}

template<typename T, typename K>
Node &operator<<(Node &node, const std::unordered_map<T, K> &map) {
    for (const auto &pair : map)
        node.add(utils::To(pair.first)) << pair.second;

    node.type(NodeType::Object);
    return node;
}

template<typename T, typename K>
const Node &operator>>(const Node &node, std::multimap<T, K> &map) {
    map.clear();
    auto where = map.end();

    for (const auto &[propertyName, property] : node.properties()) {
        std::pair<T, K> pair;
        pair.first = utils::From<T>(propertyName);
        property >> pair.second;
        where = map.insert(where, std::move(pair));
    }

    return node;
}

template<typename T, typename K>
Node &operator<<(Node &node, const std::multimap<T, K> &map) {
    for (const auto &pair : map)
        node.add(utils::To(pair.first)) << pair.second;

    node.type(NodeType::Object);
    return node;
}

template<typename T, typename K>
const Node &operator>>(const Node &node, std::unordered_multimap<T, K> &map) {
    map.clear();
    auto where = map.end();

    for (const auto &[propertyName, property] : node.properties()) {
        std::pair<T, K> pair;
        pair.first = utils::From<T>(propertyName);
        property >> pair.second;
        where = map.insert(where, std::move(pair));
    }

    return node;
}

template<typename T, typename K>
Node &operator<<(Node &node, const std::unordered_multimap<T, K> &map) {
    for (const auto &pair : map)
        node.add(utils::To(pair.first)) << pair.second;

    node.type(NodeType::Object);
    return node;
}
}

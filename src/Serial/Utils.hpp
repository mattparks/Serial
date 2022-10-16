#pragma once

#include <algorithm>
#include <iterator>
#include <tuple>
#include <string_view>
#include <sstream>
#include <list>
#include <vector>
#include <optional>

namespace serial::utils {
template<typename T> struct is_optional : std::false_type {};
template<typename T> struct is_optional<std::optional<T>> : std::true_type {};
template<typename T> inline constexpr bool is_optional_v = is_optional<T>::value;

template<typename Container> struct is_container : std::false_type {};
template<typename... Ts> struct is_container<std::list<Ts...>> : std::true_type {};
template<typename... Ts> struct is_container<std::vector<Ts...>> : std::true_type {};
template<typename T> inline constexpr bool is_container_v = is_container<T>::value;

std::string ReplaceAll(std::string str, std::string_view token, std::string_view to);

std::string_view Trim(std::string_view str, std::string_view whitespace);

bool IsWhitespace(char c) noexcept;

bool IsNumber(std::string_view str) noexcept;

std::string FixEscapedChars(std::string str);

std::string UnfixEscapedChars(std::string str);

template<typename T>
std::string ToString(T val) {
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
        if (!val)
            return "null";
        return ToString(*val);
    } else if constexpr (std::is_same_v<char, T>) {
        return std::string(1, val);
    } else {
        return std::to_string(val);
    }
}

template<typename T>
T FromString(const std::string &str) {
    if constexpr (std::is_same_v<std::string, T>) {
        return str;
    } else if constexpr (std::is_enum_v<T>) {
        typedef typename std::underlying_type<T>::type safe_type;
        return static_cast<T>(FromString<safe_type>(str));
    } else if constexpr (std::is_same_v<bool, T>) {
        return str == "true" || FromString<std::optional<int32_t>>(str) == 1;
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

std::string ConvertUtf8(const std::wstring_view &string);

char ConvertUtf8(wchar_t c);

std::wstring ConvertUtf16(const std::string_view &string);

wchar_t ConvertUtf16(char c);

/**
 * http://reedbeta.com/blog/python-like-enumerate-in-cpp17/
 */
template<typename T,
    typename TIter = decltype(std::begin(std::declval<T>())),
    typename = decltype(std::end(std::declval<T>()))>
constexpr auto Enumerate(T &&iterable) {
    struct iterator {
        size_t i;
        TIter iter;
        bool operator!=(const iterator &rhs) const { return iter != rhs.iter; }
        void operator++() { ++i; ++iter; }
        auto operator*() const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper {
        T iterable;
        auto begin() { return iterator{0, std::begin(iterable)}; }
        auto end() { return iterator{0, std::end(iterable)}; }
    };
    return iterable_wrapper{std::forward<T>(iterable)};
}

}

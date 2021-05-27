#pragma once

#include <algorithm>
#include <string_view>
#include <sstream>
#include <list>
#include <vector>
#include <optional>

namespace serial {
template<typename T> struct is_optional : std::false_type {};
template<typename T> struct is_optional<std::optional<T>> : std::true_type {};
template<typename T> inline constexpr bool is_optional_v = is_optional<T>::value;

template <typename Container> struct is_container : std::false_type {};
template <typename... Ts> struct is_container<std::list<Ts...>> : std::true_type {};
template <typename... Ts> struct is_container<std::vector<Ts...>> : std::true_type {};
template<typename T> inline constexpr bool is_container_v = is_container<T>::value;

class String {
public:
	String() = delete;

	static std::string ReplaceAll(std::string str, std::string_view token, std::string_view to) {
		auto pos = str.find(token);
		while (pos != std::string::npos) {
			str.replace(pos, token.size(), to);
			pos = str.find(token, pos + token.size());
		}

		return str;
	}

	static std::string_view Trim(std::string_view str, std::string_view whitespace) {
		auto strBegin = str.find_first_not_of(whitespace);
		if (strBegin == std::string::npos)
			return "";

		auto strEnd = str.find_last_not_of(whitespace);
		auto strRange = strEnd - strBegin + 1;
		return str.substr(strBegin, strRange);
	}
	
	static bool IsWhitespace(char c) noexcept {
		return c == ' ' || c == '\n' || c == '\r' || c == '\t';
	}

	static bool IsNumber(std::string_view str) noexcept {
		return std::all_of(str.cbegin(), str.cend(), [](auto c) {
			return (c >= '0' && c <= '9') || c == '.' || c == '-';
		});
	}

	static std::string FixEscapedChars(std::string str) {
		static const std::vector<std::pair<char, std::string_view>> replaces = {{'\\', "\\\\"}, {'\n', "\\n"}, {'\r', "\\r"}, {'\t', "\\t"}, {'\"', "\\\""}};

		for (const auto &[from, to] : replaces) {
			auto pos = str.find(from);
			while (pos != std::string::npos) {
				str.replace(pos, 1, to);
				pos = str.find(from, pos + 2);
			}
		}

		return str;
	}

	static std::string UnfixEscapedChars(std::string str) {
		static const std::vector<std::pair<std::string_view, char>> replaces = {{"\\n", '\n'}, {"\\r", '\r'}, {"\\t", '\t'}, {"\\\"", '\"'}, {"\\\\", '\\'}};

		for (const auto &[from, to] : replaces) {
			auto pos = str.find(from);
			while (pos != std::string::npos) {
				if (pos != 0 && str[pos - 1] == '\\')
					str.erase(str.begin() + --pos);
				else
					str.replace(pos, from.size(), 1, to);
				pos = str.find(from, pos + 1);
			}
		}

		return str;
	}

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
		} else if constexpr (std::is_same_v<char, T>) {
			return std::string(1, val);
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
};
}

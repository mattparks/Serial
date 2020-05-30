#pragma once

#include <ostream>

#include "NodeView.hpp"

namespace acid {
/**
 * @brief Class that is used to represent a tree of UFT-8 values, used in serialization.
 */
class Node final {
public:
	using Type = NodeConstView::Type;
	/// key, members
	using Properties = NodeConstView::Properties;

	/**
	 * @brief Class that is used to print a char, and ignore null char.
	 */
	class NullableChar {
	public:
		constexpr NullableChar(char val) : val(val) {}

		constexpr operator const char &() const noexcept { return val; }

		friend std::ostream &operator<<(std::ostream &stream, const NullableChar &c) {
			if (c.val != '\0') stream << c.val;
			return stream;
		}

		char val;
	};

	/**
	 * @brief Class that represents a configurable output format.
	 */
	class Format {
	public:
		constexpr Format(int8_t spacesPerIndent, char newLine, char space, bool inlineArrays) :
			spacesPerIndent(spacesPerIndent),
			newLine(newLine),
			space(space),
			inlineArrays(inlineArrays) {
		}

		/**
		 * Creates a string for the indentation level.
		 * @param indent The node level to get indentation for.
		 * @return The indentation string.
		 */
		std::string GetIndents(int8_t indent) const {
			return std::string(spacesPerIndent * indent, ' ');
		}

		/// Writes a node with full padding.
		static const Format Beautified;
		/// Writes a node with no padding.
		static const Format Minified;

		int8_t spacesPerIndent;
		NullableChar newLine, space;
		bool inlineArrays;
	};

	class Formatter {
	public:
		virtual ~Formatter() = default;

		virtual void ParseString(Node &node, std::string_view string) const = 0;
		virtual void WriteStream(const Node &node, std::ostream &stream) const = 0;
	};

	class Token {
	public:
		Token() = default;
		Token(Type type, std::string_view view) :
			type(type),
			view(view) {
		}

		/**
		 * Compares if two tokens have the same type and string contents.
		 * @param rhs The other token to compare.
		 * @return If the tokens are equal.
		 */
		bool operator==(const Token &rhs) const {
			return type == rhs.type && view == rhs.view.data();
		}

		bool operator!=(const Token &rhs) const {
			return !operator==(rhs);
		}

		Type type;
		std::string_view view;
	};

	Node() = default;
	Node(const Node &node) = default;
	Node(Node &&node) = default;

	void ParseString(std::string_view string, const Formatter &formatter);
	void WriteStream(std::ostream &stream, const Formatter &formatter) const;

	template<typename _Elem = char>
	void ParseStream(std::basic_istream<_Elem> &stream, const Formatter &formatter);
	template<typename _Elem = char>
	std::basic_string<_Elem> WriteString(const Formatter &formatter) const;

	template<typename T>
	T Get() const;
	template<typename T>
	T Get(const T &fallback) const;
	template<typename T>
	bool Get(T &dest) const;
	template<typename T, typename K>
	bool Get(T &dest, const K &fallback) const;
	template<typename T>
	void Set(const T &value);

	/**
	 * Clears all properties from this node.
	 */
	void Clear();

	/**
	 * Gets if the node has a value, or has properties that have values.
	 * @return If the node is internally valid.
	 */
	bool IsValid() const;

	template<typename T>
	Node &Append(const T &value);
	template<typename ...Args>
	Node &Append(const Args &...args);

	bool HasProperty(const std::string &name) const;
	NodeConstView GetProperty(const std::string &name) const;
	NodeConstView GetProperty(uint32_t index) const;
	NodeView GetProperty(const std::string &name);
	NodeView GetProperty(uint32_t index);
	Node &AddProperty();
	Node &AddProperty(const std::string &name, Node &&node);
	Node &AddProperty(const std::string &name);
	Node &AddProperty(uint32_t index, Node &&node);
	Node &AddProperty(uint32_t index);
	void RemoveProperty(const std::string &name);
	void RemoveProperty(const Node &node);

	std::vector<NodeConstView> GetProperties(const std::string &name) const;
	NodeConstView GetPropertyWithBackup(const std::string &name, const std::string &backupName) const;
	NodeConstView GetPropertyWithValue(const std::string &name, const std::string &value) const;
	std::vector<NodeView> GetProperties(const std::string &name);
	NodeView GetPropertyWithBackup(const std::string &name, const std::string &backupName);
	NodeView GetPropertyWithValue(const std::string &name, const std::string &value);

	NodeConstView operator[](const std::string &key) const;
	NodeConstView operator[](uint32_t index) const;
	NodeView operator[](const std::string &key);
	NodeView operator[](uint32_t index);

	Node &operator=(const Node &) = default;
	Node &operator=(Node &&) = default;
	Node &operator=(const NodeConstView &rhs);
	Node &operator=(NodeConstView &&rhs);
	Node &operator=(NodeView &rhs);
	Node &operator=(NodeView &&rhs);
	template<typename T>
	Node &operator=(const T &rhs);

	bool operator==(const Node &rhs) const;
	bool operator!=(const Node &rhs) const;
	bool operator<(const Node &rhs) const;

	const Properties &GetProperties() const { return properties; }
	Properties &GetProperties() { return properties; }

	const std::string &GetValue() const { return value; }
	void SetValue(std::string value) { this->value = std::move(value); }

	const Type &GetType() const { return type; }
	void SetType(Type type) { this->type = type; }

protected:
	Properties properties;
	std::string value;
	Type type = Type::Object;
};
}

#include "Node.inl"
#include "NodeConstView.inl"
#include "NodeView.inl"

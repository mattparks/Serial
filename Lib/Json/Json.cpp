#include "Json.hpp"

namespace acid {
namespace priv {
bool IsWhitespace(char c) noexcept {
	return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

bool IsNumber(std::string_view str) noexcept {
	// This is the fastest way to tell if a string holds a decimal number.
	return std::all_of(str.cbegin(), str.cend(), [](const auto c) {
		return (c >= '0' && c <= '9') || c == '.' || c == '-';
	});
}

std::string FixEscapedChars(std::string str) {
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

std::string UnfixEscapedChars(std::string str) {
	static const std::vector<std::pair<std::string_view, char>> replaces = {{"\\n", '\n'}, {"\\r", '\r'}, {"\\t", '\t'}, {"\\\"", '\"'}, {"\\\\", '\\'}};

	for (const auto &[from, to] : replaces) {
		auto pos = str.find(from);
		while (pos != std::string::npos) {
			if (pos != 0 && str[pos - 1] == '\\')
				str.erase(str.begin() + --pos);
			else
				str.replace(pos, from.size(), 1, to);
			pos = str.find(from, pos + 1 + from.size());
		}
	}

	return str;
}
}

void Json::ParseString(Node &node, std::string_view string) {
	// Tokenizes the string view into small views that are used to build a Node tree.
	std::vector<Node::Token> tokens;

	std::size_t tokenStart = 0;
	enum class QuoteState : char {
		None = '\0', Single = '\'', Double = '"'
	} quoteState = QuoteState::None;

	// Iterates over all the characters in the string view.
	for (std::size_t index = 0; index < string.length(); ++index) {
		auto c = string[index];
		// If the previous character was a backslash the quote will not break the string.
		if (c == '\'' && quoteState != QuoteState::Double && string[index - 1] != '\\')
			quoteState = quoteState == QuoteState::None ? QuoteState::Single : QuoteState::None;
		else if (c == '"' && quoteState != QuoteState::Single && string[index - 1] != '\\')
			quoteState = quoteState == QuoteState::None ? QuoteState::Double : QuoteState::None;

		// When not reading a string tokens can be found.
		// While in a string whitespace and tokens are added to the strings view.
		if (quoteState == QuoteState::None) {
			if (priv::IsWhitespace(c)) {
				// On whitespace start save current token.
				AddToken(std::string_view(string.data() + tokenStart, index - tokenStart), tokens);
				tokenStart = index + 1;
			} else if (c == ':' || c == '{' || c == '}' || c == ',' || c == '[' || c == ']') {
				// Tokens used to read json nodes.
				AddToken(std::string_view(string.data() + tokenStart, index - tokenStart), tokens);
				tokens.emplace_back(Node::Type::Token, std::string_view(string.data() + index, 1));
				tokenStart = index + 1;
			}
		}
	}

	if (tokens.empty())
		throw std::runtime_error("No tokens found in document");

	// Converts the tokens into nodes.
	int32_t k = 0;
	Convert(node, tokens, 0, k);
}

void Json::WriteStream(const Node &node, std::ostream &stream, Node::Format format) {
	stream << (node.GetType() == Node::Type::Array ? '[' : '{') << format.newLine;
	AppendData(node, stream, format, 1);
	stream << (node.GetType() == Node::Type::Array ? ']' : '}');
}

void Json::AddToken(std::string_view view, std::vector<Node::Token> &tokens) {
	if (view.length() != 0) {
		// Finds the node value type of the string and adds it to the tokens vector.
		if (view == "null") {
			tokens.emplace_back(Node::Type::Null, std::string_view());
		} else if (view == "true" || view == "false") {
			tokens.emplace_back(Node::Type::Boolean, view);
		} else if (priv::IsNumber(view)) {
			// This is a quick hack to get if the number is a decimal.
			if (view.find('.') != std::string::npos) {
				if (view.size() >= std::numeric_limits<long double>::digits)
					throw std::runtime_error("Decimal number is too long");
				tokens.emplace_back(Node::Type::Decimal, view);
			} else {
				if (view.size() >= std::numeric_limits<uint64_t>::digits)
					throw std::runtime_error("Integer number is too long");
				tokens.emplace_back(Node::Type::Integer, view);
			}
		} else { // if (view.front() == view.back() == '\"')
			tokens.emplace_back(Node::Type::String, view.substr(1, view.length() - 2));
		}
	}
}

void Json::Convert(Node &current, const std::vector<Node::Token> &tokens, int32_t i, int32_t &r) {
	if (tokens[i] == Node::Token(Node::Type::Token, "{")) {
		auto k = i + 1;

		while (tokens[k] != Node::Token(Node::Type::Token, "}")) {
			auto key = tokens[k].view;
			if (k + 2 >= tokens.size())
				throw std::runtime_error("Missing end of {} array");
			if (tokens[k + 1].view != ":")
				throw std::runtime_error("Missing object colon");
			k += 2;
			Convert(current.AddProperty(std::string(key)), tokens, k, k);
			if (tokens[k].view == ",")
				k++;
		}

		current.SetType(Node::Type::Object);
		r = k + 1;
	} else if (tokens[i] == Node::Token(Node::Type::Token, "[")) {
		auto k = i + 1;

		while (tokens[k] != Node::Token(Node::Type::Token, "]")) {
			if (k >= tokens.size())
				throw std::runtime_error("Missing end of [] object");
			Convert(current.AddProperty(), tokens, k, k);
			if (tokens[k].view == ",")
				k++;
		}

		current.SetType(Node::Type::Array);
		r = k + 1;
	} else {
		std::string str(tokens[i].view);
		if (tokens[i].type == Node::Type::String)
			str = priv::UnfixEscapedChars(str);
		current.SetValue(str);
		current.SetType(tokens[i].type);
		r = i + 1;
	}
}

void Json::AppendData(const Node &source, std::ostream &stream, Node::Format format, int32_t indent) {
	auto indents = format.GetIndents(indent);

	// Only output the value if no properties exist.
	if (source.GetProperties().empty()) {
		if (source.GetType() == Node::Type::String)
			stream << '\"' << priv::FixEscapedChars(source.GetValue()) << '\"';
		else if (source.GetType() == Node::Type::Null)
			stream << "null";
		else
			stream << source.GetValue();
	}

	// Output each property.
	for (auto it = source.GetProperties().begin(); it < source.GetProperties().end(); ++it) {
		stream << indents;
		// Output name for property if it exists.
		if (!it->first.empty()) {
			stream << '\"' << it->first << "\":" << format.space;
		}

		bool isArray = false;
		if (!it->second.GetProperties().empty()) {
			// If all properties have no names, then this must be an array.
			for (const auto &property2 : it->second.GetProperties()) {
				if (property2.first.empty()) {
					isArray = true;
					break;
				}
			}

			stream << (isArray ? '[' : '{') << format.newLine;
		} else if (it->second.GetType() == Node::Type::Object) {
			stream << '{';
		} else if (it->second.GetType() == Node::Type::Array) {
			stream << '[';
		}

		// If a node type is a primitive type.
		static constexpr auto IsPrimitive = [](Node::Type type) {
			return type != Node::Type::Object && type != Node::Type::Array && type != Node::Type::Unknown;
		};

		// Shorten primitive array output length.
		if (isArray && format.inlineArrays && !it->second.GetProperties().empty() && IsPrimitive(it->second.GetProperties()[0].second.GetType())) {
			stream << format.GetIndents(indent + 1);
			// New lines are printed a a space, no spaces are ever emitted by primitives.
			AppendData(it->second, stream, Node::Format(0, ' ', '\0', false), indent);
			stream << '\n';
		} else {
			AppendData(it->second, stream, format, indent + 1);
		}

		if (!it->second.GetProperties().empty()) {
			stream << indents << (isArray ? ']' : '}');
		} else if (it->second.GetType() == Node::Type::Object) {
			stream << '}';
		} else if (it->second.GetType() == Node::Type::Array) {
			stream << ']';
		}

		// Separate properties by comma.
		if (it != source.GetProperties().end() - 1)
			stream << ',';
		// No new line if the indent level is zero (if primitive array type).
		stream << (indent != 0 ? format.newLine : format.space);
	}
}
}

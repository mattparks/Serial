#pragma once

#include "NodeConstView.hpp"
#include "NodeFormat.hpp"

namespace serial {
class JsonTokenizer {
public:
	explicit JsonTokenizer(std::string_view string) : string(string) {}

	const NodeFormat::Token &Next() {
		while (!EndOfInput()) {
			auto ch = ReadChar();

			switch (ch) {
			case ' ':
			case '\n':
			case '\r':
			case '\t':
				break; // Skip whitespace.
			case '\0':
				break; // End of file.
			case '{':
			case '}':
			case '[':
			case ']':
			case ':':
			case ',':
				return token = {NodeType::Token, std::string_view(string.data() + index - 1, 1)};
			case '\'':
			case '\"':
				return ReadQuotedLiteral(ch);
			default:
				// Assume '-' and '+' are not used as tokens elsewhere.
				if (std::isdigit(ch) || ch == '-' || ch == '+') return ReadNumber();

				std::size_t begin = index - 1;
				std::size_t end = begin;

				do {
					end++;
				} while (std::isalnum(ch = ReadChar()) || ch == '_');

				UnreadChar();

				std::string_view view(string.data() + begin, end - begin);
				if (view == "null")
					return token = {NodeType::Null, view};
				if (view == "true" || view == "false")
					return token = {NodeType::Boolean, view};
				break; // return token = {NodeType::Unknown, view.substr(1, view.length() - 2)};
			}
		}

		return token = {NodeType::EndOfFile, std::string_view(string.data() + index - 1, 1)};
	}

	const NodeFormat::Token &Current() const { return token; }

	bool EndOfInput() const { return index == string.length(); }

private:
	char ReadChar() { return string[index++]; }
	void UnreadChar() { index--; }

	const NodeFormat::Token &ReadQuotedLiteral(const char delimiter) {
		std::size_t begin = index;
		std::size_t end = begin;

		while (ReadChar() != delimiter || string[index - 2] == '\\') {
			end++;
		}

		return token = {NodeType::String, std::string_view(string.data() + begin, end - begin)};
	}

	const NodeFormat::Token &ReadNumber() {
		std::size_t begin = index - 1;
		std::size_t end = begin + 1;
		bool isDouble = false;

		auto ch = ReadChar();

		while (true) {
			if (ch == '.') {
				if (isDouble) break;
				isDouble = true;
			} else if (ch == 'e' || ch == 'E') {
				isDouble = true;
				ch = ReadChar();
				if (ch == '+' || ch == '-') {
					end++;
				} else {
					UnreadChar();
				}
			} else if (!std::isdigit(ch)) {
				break;
			}
			end++;
			ch = ReadChar();
		}

		UnreadChar();

		return token = {isDouble ? NodeType::Decimal : NodeType::Integer, std::string_view(string.data() + begin, end - begin)};
	}

	std::string_view string;
	std::size_t index = 0;

	NodeFormat::Token token;
};
}

#pragma once

#include "NodeConstView.hpp"
#include "NodeFormat.hpp"

namespace serial {
class XmlTokenizer {
public:
	explicit XmlTokenizer(std::string_view string) : string(string) {}

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
			case '<':
				ch = ReadChar();
				if (ch == '!') {
					ch = ReadChar();
					if (ch == '-') {
						ch = ReadChar();
						if (ch == '-') return token = {NodeType::Token, std::string_view(string.data() + index - 4, 4)};
						UnreadChar();
						return token = {NodeType::Token, std::string_view(string.data() + index - 3, 3)};
					}
					UnreadChar();
				}
				if (ch == '/') return token = {NodeType::Token, std::string_view(string.data() + index - 2, 2)};
				UnreadChar();
			case '>':
				return token = {NodeType::Token, std::string_view(string.data() + index - 1, 1)};
			case '/':
				ch = ReadChar();
				if (ch == '>') return token = {NodeType::Token, std::string_view(string.data() + index - 2, 2)};
				UnreadChar();
				return token = {NodeType::Token, std::string_view(string.data() + index - 1, 1)};
			case '-':
				ch = ReadChar();
				if (ch == '-') {
					ch = ReadChar();
					if (ch == '>') return token = {NodeType::Token, std::string_view(string.data() + index - 3, 3)};
					UnreadChar();
					return token = {NodeType::Token, std::string_view(string.data() + index - 2, 2)};
				}
				UnreadChar();
				break;
			case '!':
			case '?':
			case '=':
				return token = {NodeType::Token, std::string_view(string.data() + index - 1, 1)};
			case '\'':
			case '\"':
				return ReadQuotedLiteral(ch);
			default:
				std::size_t begin = index - 1;
				std::size_t end = begin;

				do {
					end++;
				} while (std::isalnum(ch = ReadChar()) || ch == '-' || ch == '_' || ch == ':');

				UnreadChar();

				return token = {NodeType::Unknown, std::string_view(string.data() + begin, end - begin)};
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

	std::string_view string;
	std::size_t index = 0;

	NodeFormat::Token token;
};
}

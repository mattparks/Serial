#pragma once

#include <codecvt>
#include <string_view>

#include "Json.hpp"

namespace acid {
template<typename _Elem>
void Json::Load(std::basic_istream<_Elem> &stream) {
	std::vector<std::pair<Type, std::string>> tokens;

	std::string current;
	bool inString = false;

	// We must read into a UTF8 char.
	stream.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<char>));
	// Reading into a string before iterating is much faster.
	std::string s(std::istreambuf_iterator<_Elem>(stream), {});

	// Read stream until end of file.
	for (auto c : s) {
		// On start of string switch in/out of stream space and ignore this char.
		if (c == '"' || c == '\'') {
			inString ^= 1;
			//continue;
		}

		// When not reading a string tokens can be found.
		if (!inString) {
			// Tokens used to read json nodes.
			if (std::string_view(":{},[]").find(c) != std::string::npos) {
				AddToken(tokens, current);
				tokens.emplace_back(Type::Unknown, std::string{c});
				continue;
			}

			// On whitespace save current as a string.
			if (String::IsWhitespace(c)) {
				AddToken(tokens, current);
				continue;
			}
		}

		// Add this char to the current builder stream.
		current += c;
	}

	// Converts the list of tokens into nodes.
	int32_t k = 0;
	Convert(*this, tokens, 0, k);
}

template<typename _Elem>
void Json::Load(const std::basic_string<_Elem> &string) {
	std::basic_stringstream<_Elem> stream(string);
	Load(stream);
}

template<typename _Elem>
void Json::Write(std::basic_ostream<_Elem> &stream, Format format) const {
	if (format == Format::Minified) {
		stream << '{';
	} else {
		stream << "{\n";
	}

	AppendData(*this, stream, 1, format);
	stream << '}';
}

template<typename _Elem>
std::basic_string<_Elem> Json::Write(Format format) const {
	std::basic_stringstream<_Elem> stream;
	Write(stream, format);
	return stream.str();
}

template<typename _Elem>
void Json::AppendData(const Node &source, std::basic_ostream<_Elem> &stream, int32_t indentation, Format format) {
	// Creates a string for the indentation level.
	std::string indents(2 * indentation, ' ');

	// Only output the value if no properties exist.
	if (source.GetProperties().empty()) {
		auto value = String::FixReturnTokens(source.GetValue());

		if (source.GetType() == Type::String) {
			stream << '\"' << value << '\"';
		} else {
			stream << value;
		}
	}

	// Output each property.
	for (auto it = source.GetProperties().begin(); it < source.GetProperties().end(); ++it) {
		std::string openString;
		std::string closeString;

		// Gets array or object type braces.
		if (!it->second.GetProperties().empty()) {
			openString = "{";
			closeString = "}";

			// Sets the braces to an array style if all properties have no names.
			for (const auto &[propertyName2, property2] : it->second.GetProperties()) {
				if (propertyName2.empty()) {
					openString = "[";
					closeString = "]";
					break;
				}
			}

			if (format != Format::Minified) {
				openString += '\n';
				closeString.insert(0, indents);
			}
		} else if (it->second.GetType() == Type::Object) {
			openString = "{";
			closeString = "}";
		} else if (it->second.GetType() == Type::Array) {
			openString = "[";
			closeString = "]";
		}

		// Separate properties by comma.
		if (it != source.GetProperties().end() - 1) {
			closeString += ',';
		}

		if (format != Format::Minified) {
			stream << indents;
		}

		// Output name for property if it exists.
		if (!it->first.empty()) {
			stream << "\"" << it->first << "\":";

			if (format != Format::Minified) {
				stream << ' ';
			}
		}

		// Appends the current stream with the property data.
		stream << openString;
		AppendData(it->second, stream, indentation + 1, format);
		stream << closeString;

		if (format != Format::Minified) {
			stream << '\n';
		}
	}
}
}

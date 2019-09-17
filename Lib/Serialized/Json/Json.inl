#pragma once

#include "Json.hpp"

#include "Helpers/String.hpp"

namespace acid {
template<typename _Elem>
void Json<_Elem>::LoadArray(Document &document, const _Elem *str, std::size_t length) {
	auto startTime = std::chrono::high_resolution_clock::now();
	
	std::vector<Token> tokens;

	const char *start = str;
	bool inString = false;
	bool inEscape = false;

	// Read stream until end of file.
	for (std::size_t i = 0; i < length; i++) {
		char c = str[i];
		bool lastEscape = inEscape;
		// On start of string switch in/out of stream space and ignore this char.
		if (!inEscape) {
			if ((c == '"' || c == '\''))
				inString ^= 1;
			if (c == '\\')
				inEscape = true;
		}
		if (lastEscape)
			inEscape = false;

		// When not reading a string tokens can be found.
		if (!inString) {
			// Tokens used to read json nodes.
			if (std::string_view(":{},[]").find(c) != std::string::npos) {
				AddToken(start, str + i, tokens);
				tokens.emplace_back(str + i, str + i + 1, Document::Type::Unknown);
				start = str + i + 1;
				continue;
			}

			// On whitespace save current as a string.
			if (String::IsWhitespace(c)) {
				AddToken(start, str + i, tokens);
				start = str + i + 1;
				continue;
			}
		}
	}

	auto lengthTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
	std::cout << "Token vector memory size " << (tokens.size() * sizeof(Token)) << "bytes\n";
	std::cout << "Tokenized in " << lengthTime.count() << "ms\n";
	startTime = std::chrono::high_resolution_clock::now();
	
	// Converts the list of tokens into nodes.
	int32_t k = 0;
	Convert(document, tokens, 0, k);

	lengthTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
	std::cout << "Lexed in " << lengthTime.count() << "ms\n";
}

template<typename _Elem>
void Json<_Elem>::WriteStream(const Document &document, std::basic_ostream<_Elem> &stream, Document::Format format) const {
	if (format == Document::Format::Minified) {
		stream << '{';
	} else {
		stream << "{\n";
	}

	AppendData(document, stream, 1, format);
	stream << '}';
}

std::string_view GetString(const char *start, const char *end) {
	return std::string_view(start, end - start);
}

template<typename _Elem>
void Json<_Elem>::AddToken(const char *start, const char *end, std::vector<Token> &tokens) {
	if (start != end) {
		auto view = GetString(start, end);
		// Finds the node value type of the string and adds it to the tokens vector.
		if (String::IsNumber(view)) {
			tokens.emplace_back(start, end, Document::Type::Number);
		} else if (view == "null") {
			tokens.emplace_back(start, end, Document::Type::Null);
		} else if (view == "true" || view == "false") {
			tokens.emplace_back(start, end, Document::Type::Boolean);
		} else {
			// if (current.front() == current.back() == '\"')
			tokens.emplace_back(start + 1, end - 1, Document::Type::String);
		}
	}
}

template<typename _Elem>
void Json<_Elem>::Convert(Document &current, const std::vector<Token> &v, int32_t i, int32_t &r) {
	if (v[i].GetChar() == '{') {
		auto k = i + 1;

		while (v[k].GetChar() != '}') {
			auto key = v[k].GetString();
			k += 2; // k + 1 should be ':'
			Convert(current.AddProperty(key, {}), v, k, k);

			if (v[k].GetChar() == ',') {
				k++;
			}
		}

		current.SetType(Document::Type::Object);
		r = k + 1;
	} else if (v[i].GetChar() == '[') {
		auto k = i + 1;

		while (v[k].GetChar() != ']') {
			Convert(current.AddProperty(), v, k, k);

			if (v[k].GetChar() == ',') {
				k++;
			}
		}

		current.SetType(Document::Type::Array);
		r = k + 1;
	} else {
		current.SetValue(String::UnfixReturnTokens(v[i].GetString()));
		current.SetType(v[i].type);
		r = i + 1;
	}
}

template<typename _Elem>
void Json<_Elem>::AppendData(const Document &source, std::basic_ostream<_Elem> &stream, int32_t indentation, Document::Format format) {
	// Creates a string for the indentation level.
	std::string indents(2 * indentation, ' ');

	// Only output the value if no properties exist.
	if (source.GetProperties().empty()) {
		auto value = String::FixReturnTokens(source.GetValue());

		if (source.GetType() == Document::Type::String) {
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
		if (!it->GetProperties().empty()) {
			openString = "{";
			closeString = "}";

			// Sets the braces to an array style if all properties have no names.
			for (const auto &property2 : it->GetProperties()) {
				if (property2.GetName().empty()) {
					openString = "[";
					closeString = "]";
					break;
				}
			}

			if (format != Document::Format::Minified) {
				openString += '\n';
				closeString.insert(0, indents);
			}
		} else if (it->GetType() == Document::Type::Object) {
			openString = "{";
			closeString = "}";
		} else if (it->GetType() == Document::Type::Array) {
			openString = "[";
			closeString = "]";
		}

		// Separate properties by comma.
		if (it != source.GetProperties().end() - 1) {
			closeString += ',';
		}

		if (format != Document::Format::Minified) {
			stream << indents;
		}

		// Output name for property if it exists.
		if (!it->GetName().empty()) {
			stream << "\"" << it->GetName() << "\":";

			if (format != Document::Format::Minified) {
				stream << ' ';
			}
		}

		// Appends the current stream with the property data.
		stream << openString;
		AppendData(*it, stream, indentation + 1, format);
		stream << closeString;

		if (format != Document::Format::Minified) {
			stream << '\n';
		}
	}
}
}

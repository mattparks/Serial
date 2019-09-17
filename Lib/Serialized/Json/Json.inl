#pragma once

#include "Json.hpp"

#include "Helpers/String.hpp"

namespace acid {
template<typename _Elem>
void Json<_Elem>::LoadString(Document &document, std::basic_string_view<_Elem> string) {
	auto startTime = std::chrono::high_resolution_clock::now();
	
	std::vector<Token> tokens;

	const _Elem *tokenStart = &string[0];
	bool inString = false;

	// Read stream until end of file.
	for (const _Elem *index = string.data(); index < string.data() + string.length(); ++index) {
		// On start of string switch in/out of stream space and ignore this char.
		// If the previous char was a backslash this char should already be in a string.
		if ((*index == '"' || *index == '\'') && *(index - 1) != '\\')
			inString ^= 1;

		// When not reading a string tokens can be found.
		if (!inString) {
			// Tokens used to read json nodes.
			if (std::string_view(":{},[]").find(*index) != std::string::npos) {
				AddToken(tokenStart, index, tokens);
				tokens.emplace_back(index, index + 1, Document::Type::Unknown);
				tokenStart = index + 1;
				continue;
			}

			// On whitespace save current as a string.
			if (String::IsWhitespace(*index)) {
				AddToken(tokenStart, index, tokens);
				tokenStart = index + 1;
				continue;
			}
		}
	}

	auto lengthTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
	std::cout << "Tokenized in " << lengthTime.count() << "ms\n";
	startTime = std::chrono::high_resolution_clock::now();
	
	// Converts the list of tokens into nodes.
	int32_t k = 0;
	Convert(document, tokens, 0, k);

	//std::vector<std::pair<Document::Type, std::string>> debugTokens;
	//for (const auto &token : tokens) {
	//	debugTokens.emplace_back(token.type, token.GetString());
	//}

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
		current.SetValue(v[i].GetString());
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
		if (source.GetType() == Document::Type::String)
			stream << std::quoted(source.GetValue());
		else
			stream << source.GetValue();
	}

	// Output each property.
	for (auto it = source.GetProperties().begin(); it < source.GetProperties().end(); ++it) {
		if (format != Document::Format::Minified)
			stream << indents;

		// Output name for property if it exists.
		if (!it->GetName().empty()) {
			stream << std::quoted(it->GetName()) << ':';
			if (format != Document::Format::Minified)
				stream << ' ';
		}

		bool isArray = false;
		if (!it->GetProperties().empty()) {
			// If all properties have no names, then this must be an array.
			for (const auto &property2 : it->GetProperties()) {
				if (property2.GetName().empty()) {
					isArray = true;
					break;
				}
			}

			stream << (isArray ? '[' : '{');
			if (format != Document::Format::Minified)
				stream << '\n';
		} else if (it->GetType() == Document::Type::Object) {
			stream << '{';
		} else if (it->GetType() == Document::Type::Array) {
			stream << '[';
		}

		AppendData(*it, stream, indentation + 1, format);

		if (!it->GetProperties().empty()) {
			if (format != Document::Format::Minified)
				stream << indents;
			stream << (isArray ? ']' : '}');
		} else if (it->GetType() == Document::Type::Object) {
			stream << '}';
		} else if (it->GetType() == Document::Type::Array) {
			stream << ']';
		}
		
		// Separate properties by comma.
		if (it != source.GetProperties().end() - 1)
			stream << ',';

		if (format != Document::Format::Minified)
			stream << '\n';
	}
}
}

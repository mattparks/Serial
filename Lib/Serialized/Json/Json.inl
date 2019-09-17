#pragma once

#include "Json.hpp"

#include "Helpers/String.hpp"

namespace acid {
template<typename _Elem>
void Json<_Elem>::LoadString(Document &document, std::basic_string_view<_Elem> string) {
	auto start = std::chrono::high_resolution_clock::now();

	std::vector<std::pair<Document::Type, std::string>> tokens;

	std::string current;
	bool inString = false;
	bool inEscape = false;

	// Read stream until end of file.
	for (auto c : string) {
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
				AddToken(tokens, current);
				tokens.emplace_back(Document::Type::Unknown, std::string{c});
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

	auto length = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Tokenized in " << length.count() << "ms\n";
	start = std::chrono::high_resolution_clock::now();
	
	// Converts the list of tokens into nodes.
	int32_t k = 0;
	Convert(document, tokens, 0, k);

	length = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Lexed in " << length.count() << "ms\n";
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

template<typename _Elem>
void Json<_Elem>::AddToken(std::vector<std::pair<Document::Type, std::string>> &tokens, std::string &current) {
	if (!current.empty()) {
		// Finds the node value type of the string and adds it to the tokens vector.
		if (String::IsNumber(current)) {
			tokens.emplace_back(Document::Type::Number, current);
		} else if (current == "null") {
			tokens.emplace_back(Document::Type::Null, current);
		} else if (current == "true" || current == "false") {
			tokens.emplace_back(Document::Type::Boolean, current);
		} else {
			// if (current.front() == current.back() == '\"')
			tokens.emplace_back(Document::Type::String, current.substr(1, current.size() - 2));
		}
	}

	// Clears the current summation stream.
	current.clear();
}

template<typename _Elem>
void Json<_Elem>::Convert(Document &current, const std::vector<std::pair<Document::Type, std::string>> &v, int32_t i, int32_t &r) {
	if (v[i].second == "{") {
		auto k = i + 1;

		while (v[k].second != "}") {
			auto key = v[k].second;
			k += 2; // k + 1 should be ':'
			Convert(current.AddProperty(key, {}), v, k, k);

			if (v[k].second == ",") {
				k++;
			}
		}

		current.SetType(Document::Type::Object);
		r = k + 1;
	} else if (v[i].second == "[") {
		auto k = i + 1;

		while (v[k].second != "]") {
			Convert(current.AddProperty(), v, k, k);

			if (v[k].second == ",") {
				k++;
			}
		}

		current.SetType(Document::Type::Array);
		r = k + 1;
	} else {
		current.SetValue(String::UnfixReturnTokens(v[i].second));
		current.SetType(v[i].first);
		r = i + 1;
	}
}
}

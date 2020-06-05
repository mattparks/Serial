#include "Xml.hpp"

#include <sstream>

#include "String.hpp"

namespace serial {
void Xml::ParseString(Node &node, std::string_view string) {
	// Tokenizes the string view into small views that are used to build a Node tree.
	std::vector<Node::Token> tokens;

	std::size_t tokenStart = 0;
	enum class QuoteState : char {
		None = '\0', Single = '\'', Double = '"'
	} quoteState = QuoteState::None;
	enum class TagState : char {
		None = '\0', Open = '<', Close = '>'
	} tagState = TagState::None;

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
			if (tagState == TagState::Open && String::IsWhitespace(c)) {
				// On whitespace start save current token.
				AddToken(std::string_view(string.data() + tokenStart, index - tokenStart), tokens);
				tokenStart = index + 1;
			} else if (c == '=' || c == '<' || c == '>' || c == '?' || c == '/') {
				if (c == '<')
					tagState = TagState::Open;
				else if (c == '>')
					tagState = TagState::Close;
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

void Xml::WriteStream(const Node &node, std::ostream &stream, Node::Format format) {
	stream << R"(<?xml version="1.0" encoding="utf-8"?>)" << format.newLine;
	// TODO: Taken from body of AppendData properties loop to write parent node tags.
	stream << '<' << node.GetName();
	for (const auto &property : node.GetProperties()) {
		if (property.GetName().rfind('-', 0) != 0) continue;
		stream << " " << property.GetName().substr(1) << "=\"" << property.GetValue() << "\"";
	}
	stream << '>' << format.newLine;
	AppendData(node, stream, format, 1);
	stream << "</" << node.GetName() << ">";
}

void Xml::AddToken(std::string_view view, std::vector<Node::Token> &tokens) {
	if (view.length() != 0)
		tokens.emplace_back(Node::Type::String, view);
}

void Xml::Convert(Node &current, const std::vector<Node::Token> &tokens, int32_t i, int32_t &r) {
	// {"<", "name", "-property", "-", "value", ... , ">",}
	// {"value",}
	// {"</", "name", ">"}

	if (tokens[i] == Node::Token(Node::Type::Token, "<")) {
		auto k = i + 1;
		auto key = tokens[k].view;
		k++;

		while (tokens[k] != Node::Token(Node::Type::Token, ">")) {
			k++;
		}

		if (key == "?")
			Convert(current, tokens, k, k);

		current.SetType(Node::Type::Object);
		r = k + 1;
	} else {
		std::string str(tokens[i].view);
		if (tokens[i].type == Node::Type::String)
			str = String::UnfixEscapedChars(str);
		current.SetValue(str);
		current.SetType(tokens[i].type);
		r = i + 1;
	}
}

void Xml::AppendData(const Node &node, std::ostream &stream, Node::Format format, int32_t indent) {
	stream << node.GetValue();

	auto indents = format.GetIndents(indent);
	// Output each property.
	for (auto it = node.GetProperties().begin(); it < node.GetProperties().end(); ++it) {
		if (it->GetName().rfind('-', 0) == 0) continue;

		// Skip property tag for arrays and move onto appending its properties.
		if (it->GetType() == Node::Type::Array) {
			AppendData(*it, stream, format, indent);
			continue;
		}

		// If the node is an array, then all properties will inherit the array name.
		auto name = node.GetType() == Node::Type::Array ? node.GetName() : it->GetName();
		stream << indents << '<' << name;

		// Add attributes to opening tag.
		int attributeCount = 0;
		for (const auto &property : it->GetProperties()) {
			if (property.GetName().rfind('-', 0) != 0) continue;
			stream << " " << property.GetName().substr(1) << "=\"" << property.GetValue() << "\"";
			attributeCount++;
		}

		// When the property has a value or children recursively append them, otherwise shorten tag ending.
		if (it->GetProperties().size() - attributeCount != 0 || !it->GetValue().empty()) {
			stream << '>';

			if (it->GetProperties().size() - attributeCount != 0) {
				stream << format.newLine;
				AppendData(*it, stream, format, indent + 1);
				stream << indents;
			} else {
				AppendData(*it, stream, format, indent + 1);
			}
			stream << "</" << name << '>' << format.newLine;
		} else {
			stream << "/>" << format.newLine;
		}
	}
}
}

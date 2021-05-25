#include "Xml.hpp"

#include "XmlTokenizer.hpp"

namespace serial {
void Xml::Load(Node &node, std::string_view string) {
	XmlTokenizer tokenizer(string);
#if 0
	std::vector<Token> tokens;
	while (!tokenizer.EndOfInput())
		tokens.emplace_back(tokenizer.Next());
#else
	tokenizer.Next();
	Convert(node, tokenizer);
#endif
}

void Xml::Write(const Node &node, std::ostream &stream, Format format) {
	// TODO: if no XMLDecl write default: R"(<?xml version="1.0" encoding="utf-8"?>)"

	for (const auto &[propertyKey, property] : node.GetProperties()) {
		AppendData(propertyKey, property, stream, format, 0);
	}
}

void Xml::Convert(Node &current, XmlTokenizer &tokenizer) {
	// Ignore comments.
	if (tokenizer.Current() == Token(NodeType::Token, "<!--")) {
		tokenizer.Next();
		while (tokenizer.Next() != Token(NodeType::Token, "-->")) {
		}
		tokenizer.Next();
	}

	// Only start to parse if we are at the start of a tag.
	if (tokenizer.Current() != Token(NodeType::Token, "<"))
		return;
	tokenizer.Next();

	// The next token after the open tag is the name.
	std::string name(tokenizer.Current().view);
	// First token in tag might have been prolog or XMLDecl char, name will be in the following token.
	if (tokenizer.Current() == Token(NodeType::Token, "?") || tokenizer.Current() == Token(NodeType::Token, "!")) {
		name += tokenizer.Next().view;
	}
	tokenizer.Next();

	// Create the property that will contain the attributes and children found in the tag.
	auto &property = CreateProperty(current, name);
	Token lastToken;
	while (tokenizer.Current() != Token(NodeType::Token, ">") && tokenizer.Current() != Token(NodeType::Token, "/>")) {
		// Attributes are added as properties.
		if (tokenizer.Current() == Token(NodeType::Token, "=")) {
			tokenizer.Next();
			property.AddProperty("-" + std::string(lastToken.view)) = tokenizer.Current().view;
		}
		lastToken = tokenizer.Current();
		tokenizer.Next();
	}
	// Inline tag has no children.
	if (tokenizer.Current() == Token(NodeType::Token, "/>")) {
		tokenizer.Next();
		return;
	}
	tokenizer.Next();

	// More tags will follow after prolog and XMLDecl.
	// Prolog tag must end with ?>, XMLDecl does not need any token before closing the tag.
	if (lastToken == Token(NodeType::Token, "?") || name[0] == '!') {
		Convert(current, tokenizer);
		return;
	}

	// Continue through all children until the end tag is found.
	while (true) {
		if (tokenizer.Current() == Token(NodeType::Token, "</")) {
			if (tokenizer.Next().view != name)
				continue;
			tokenizer.Next(); // '>'
			tokenizer.Next(); // eof or '<'
			break;
		}
		
		if (tokenizer.Current().type == NodeType::String) {
			property = tokenizer.Current().view;
			tokenizer.Next();
		} else {
			Convert(property, tokenizer);
		}
	}

#if 0
	// Only start to parse if we are at the start of a tag.
	if (tokens[k] != Token(NodeType::Token, "<"))
		return;
	k++;

	// Ignore comments.
	if (tokens[k] == Token(NodeType::Token, "!") && tokens[k + 1] == Token(NodeType::Token, "--") == 0) {
		k += 2;
		while (tokens[k + 1] != Token(NodeType::Token, ">") && tokens[k] == Token(NodeType::Token, "--"))
			k++;
		k += 2;
		Convert(current, tokens, k);
		return;
	}

	// The next token after the open tag is the name.
	std::string name(tokens[k].view);
	// First token in tag might have been prolog or XMLDecl char, name will be in the following token.
	if (tokens[k] == Token(NodeType::Token, "?") || tokens[k] == Token(NodeType::Token, "!")) {
		name += tokens[k + 1].view;
		k++;
	}
	k++;

	// Create the property that will contain the attributes and children found in the tag.
	auto &property = CreateProperty(current, name);

	while (tokens[k] != Token(NodeType::Token, ">")) {
		// Attributes are added as properties.
		if (tokens[k] == Token(NodeType::Token, "=")) {
			property.AddProperty("-" + std::string(tokens[k - 1].view)) = tokens[k + 1].view.substr(1, tokens[k + 1].view.size() - 2);
			k++;
		}
		k++;
	}
	k++;

	// More tags will follow after prolog and XMLDecl.
	if (tokens[k - 2] == Token(NodeType::Token, "?") || name[0] == '!') {
		Convert(current, tokens, k);
		return;
	}
	// Inline tag has no children.
	if (tokens[k - 2] == Token(NodeType::Token, "/"))
		return;
	// Continue through all children until the end tag is found.
	while (!(tokens[k] == Token(NodeType::Token, "<") && tokens[k + 1] == Token(NodeType::Token, "/") && tokens[k + 2].view == name)) {
		if (tokens[k].type == NodeType::String) {
			property = tokens[k].view;
			k++;
		} else {
			// TODO: If the token at k is not a '<' this will cause a infinite loop, or if k + 2 > tokens.size() vector access will be violated.
			Convert(property, tokens, k);
		}
	}
	k += 4;
#endif
}

Node &Xml::CreateProperty(Node &current, const std::string &name) {
	// Combine duplicate tags.
	if (auto duplicate = current[name]) {
		// If the node is already an array add the new property to it.
		if (duplicate->GetType() == NodeType::Array)
			return duplicate->AddProperty();

		// Copy the duplicate node so we can add it to the new array.
		auto original = current.RemoveProperty(duplicate);
		auto &array = current.AddProperty(name);
		array.SetType(NodeType::Array);
		array.AddProperty(std::move(original));
		return array.AddProperty();
	}

	return current.AddProperty(name);
}

void Xml::AppendData(const NodeKey &nodeKey, const Node &node, std::ostream &stream, Format format, int32_t indent) {
	auto nodeName = std::get_if<std::string>(&nodeKey);
	if (!nodeName || nodeName->rfind('-', 0) == 0) return;

	if (node.GetType() == NodeType::Array) {
		// If the node is an array, then all properties will inherit the array name.
		for (const auto &[propertyKey, property] : node.GetProperties())
			AppendData(nodeKey, property, stream, format, indent);
		return;
	}

	auto indents = format.GetIndents(indent);
	const auto &name = *nodeName;
	stream << indents << '<' << name;

	// Add attributes to opening tag.
	int attributeCount = 0;
	for (const auto &[propertyKey, property] : node.GetProperties()) {
		auto propertyName = std::get_if<std::string>(&propertyKey);
		if (!propertyName || propertyName->rfind('-', 0) != 0) continue;
		stream << " " << propertyName->substr(1) << "=\"" << property.GetValue() << "\"";
		attributeCount++;
	}

	// When the property has a value or children recursively append them, otherwise shorten tag ending.
	if (node.GetProperties().size() - attributeCount != 0 || !node.GetValue().empty()) {
		stream << '>';
		stream << node.GetValue();

		if (node.GetProperties().size() - attributeCount != 0) {
			stream << format.newLine;
			// Output each property.
			for (const auto &[propertyKey, property] : node.GetProperties())
				AppendData(propertyKey, property, stream, format, indent + 1);
			stream << indents;
		} else {
			// Output each property.
			for (const auto &[propertyKey, property] : node.GetProperties())
				AppendData(propertyKey, property, stream, format, indent + 1);
		}
		stream << "</" << name << '>' << format.newLine;
	} else if (name[0] == '?') {
		stream << "?>" << format.newLine;
	} else if (name[0] == '!') {
		stream << '>' << format.newLine;
	} else {
		stream << "/>" << format.newLine;
	}
}
}

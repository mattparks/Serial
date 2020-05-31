#include "Xml.hpp"

#include <sstream>

#include "String.hpp"

namespace serial {
void Xml::ParseString(Node &node, std::string_view string) {
}

void Xml::WriteStream(const Node &node, std::ostream &stream, Node::Format format) {
	stream << R"(<?xml version="1.0" encoding="utf-8"?>)" << format.newLine;
	AppendData(node, stream, format, 0);
}

void Xml::AddToken(std::string_view view, std::vector<Node::Token> &tokens) {
}

void Xml::Convert(Node &current, const std::vector<Node::Token> &tokens, int32_t i, int32_t &r) {
}

void Xml::AppendData(const Node &node, std::ostream &stream, Node::Format format, int32_t indent) {
	auto indents = format.GetIndents(indent);

	int attributeCount = 0;
	std::stringstream nameAttributes;
	nameAttributes << node.GetName();

	for (const auto &property : node.GetProperties()) {
		if (property.GetName().rfind('-', 0) != 0) continue;
		nameAttributes << " " << property.GetName().substr(1) << "=\"" << property.GetValue() << "\"";
		attributeCount++;
	}

	auto nameAndAttribs = nameAttributes.str();
	nameAndAttribs = String::Trim(nameAndAttribs);

	stream << indents;

	if (node.GetProperties().size() - attributeCount == 0 && node.GetValue().empty()) {
		stream << "<" << nameAndAttribs << "/>" << format.newLine;
		return;
	}

	stream << "<" << nameAndAttribs << ">";
	if (!node.GetValue().empty()) {
		//stream << format.newLine << format.GetIndents(indent + 1);
		stream << node.GetValue();
		//stream << format.newLine << indents;
	}

	if (node.GetProperties().size() - attributeCount != 0) {
		stream << format.newLine;

		for (const auto &property : node.GetProperties()) {
			if (property.GetName().rfind('-', 0) != 0)
				AppendData(property, stream, format, indent + 1);
		}

		stream << indents;
	}

	stream << "</" << node.GetName() << '>' << format.newLine;
}
}

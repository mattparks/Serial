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

	auto tagName = String::ReplaceAll(node.GetName(), " ", "_");

	int attributeCount = 0;
	std::stringstream nameAttributes;
	nameAttributes << tagName;

	for (const auto &property : node.GetProperties()) {
		if (property.GetName().rfind('_', 0) != 0) continue;
		nameAttributes << " " << property.GetName().substr(1) << "=\"" << property.GetValue() << "\"";
		attributeCount++;
	}

	auto nameAndAttribs = String::Trim(nameAttributes.str());

	stream << indents;

	if (node.GetName()[0] == '?') {
		stream << "<" << nameAndAttribs << "?>" << format.newLine;

		for (const auto &property : node.GetProperties()) {
			if (property.GetName().rfind('_', 0) != 0)
				AppendData(property, stream, format, indent);
		}

		return;
	}

	if (node.GetProperties().size() - attributeCount == 0 && node.GetValue().empty()) {
		stream << "<" << nameAndAttribs << "/>" << format.newLine;
		return;
	}

	stream << "<" << nameAndAttribs << ">";
	if (!node.GetValue().empty()) {
		stream << format.newLine << format.GetIndents(indent + 1);
		stream << node.GetValue();
	}

	if (!node.GetProperties().empty()) {
		stream << format.newLine;

		for (const auto &property : node.GetProperties()) {
			if (property.GetName().rfind('_', 0) != 0)
				AppendData(property, stream, format, indent + 1);
		}

		stream << indents;
	}

	stream << "</" << tagName << '>' << format.newLine;
}
}

#include "Json.hpp"

#include "JsonTokenizer.hpp"

#define ATTRIBUTE_TEXT_SUPPORT 1

namespace serial {
void Json::Load(Node &node, std::string_view string) {
	JsonTokenizer tokenizer(string);
#if 0
	std::vector<Token> tokens;
	while (!tokenizer.EndOfInput())
		tokens.emplace_back(tokenizer.Next());
#else
	tokenizer.Next();
	Convert(node, tokenizer);
#endif
}

void Json::Write(const Node &node, std::ostream &stream, Format format) {
	stream << (node.GetType() == NodeType::Array ? '[' : '{') << format.newLine;
	AppendData(node, stream, format, 1);
	stream << (node.GetType() == NodeType::Array ? ']' : '}');
}

void Json::Convert(Node &current, JsonTokenizer &tokenizer) {
	if (tokenizer.Current() == Token(NodeType::Token, "{")) {
		tokenizer.Next();

		while (tokenizer.Current() != Token(NodeType::Token, "}")) {
			auto key = tokenizer.Current().view;
			//if (k + 2 >= tokens.size())
			//	throw std::runtime_error("Missing end of {} array");
			if (tokenizer.Next().view != ":")
				throw std::runtime_error("Missing object colon");
			tokenizer.Next();
#if ATTRIBUTE_TEXT_SUPPORT
			// Write value string into current value, then continue parsing properties into current.
			if (key == "#text")
				Convert(current, tokenizer);
			else
#endif
				Convert(current.AddProperty(std::string(key)), tokenizer);
			if (tokenizer.Current().view == ",")
				tokenizer.Next();
		}
		tokenizer.Next();

		current.SetType(NodeType::Object);
	} else if (tokenizer.Current() == Token(NodeType::Token, "[")) {
		tokenizer.Next();

		while (tokenizer.Current() != Token(NodeType::Token, "]")) {
			//if (k >= tokens.size())
			//	throw std::runtime_error("Missing end of [] object");
			Convert(current.AddProperty(), tokenizer);
			if (tokenizer.Current().view == ",")
				tokenizer.Next();
		}
		tokenizer.Next();

		current.SetType(NodeType::Array);
	} else {
		std::string str(tokenizer.Current().view);
		if (tokenizer.Current().type == NodeType::String)
			str = String::UnfixEscapedChars(str);
		current.SetValue(str);
		current.SetType(tokenizer.Current().type);
		tokenizer.Next();
	}
}

void Json::AppendData(const Node &node, std::ostream &stream, Format format, int32_t indent) {
	auto indents = format.GetIndents(indent);

	// Only output the value if no properties exist.
	if (node.GetProperties().empty()) {
		if (node.GetType() == NodeType::String)
			stream << '\"' << String::FixEscapedChars(node.GetValue()) << '\"';
		else if (node.GetType() == NodeType::Null)
			stream << "null";
		else
			stream << node.GetValue();
	}

#if ATTRIBUTE_TEXT_SUPPORT
	// If the Json Node has both properties and a value, value will be written as a "#text" property.
	// XML is the only format that allows a Node to have both a value and properties.
	if (!node.GetProperties().empty() && !node.GetValue().empty()) {
		stream << indents;
		stream << "\"#text\":" << format.space << "\"" << node.GetValue() << "\",";
		// No new line if the indent level is zero (if primitive array type).
		stream << (indent != 0 ? format.newLine : format.space);
	}
#endif

	// Output each property.
	for (auto it = node.GetProperties().begin(); it != node.GetProperties().end(); ++it) {
		const auto &[propertyName, property] = *it;
		// TODO: if this *it is in an array and there are elements missing between *(it-1) and *it fill with null.
		
		stream << indents;
		// Output name for property if it exists.
		if (!propertyName.empty()) {
			stream << '\"' << propertyName << "\":" << format.space;
		}

		bool isArray = false;
		if (!property.GetProperties().empty()) {
			// If all properties have no names, then this must be an array.
			// TODO: this does not look over all properties, handle where we have mixed mapped names and array elements.
			for (const auto &[name2, property2] : property.GetProperties()) {
				if (name2.empty()) {
					isArray = true;
					break;
				}
			}

			stream << (isArray ? '[' : '{') << format.newLine;
		} else if (property.GetType() == NodeType::Object) {
			stream << '{';
		} else if (property.GetType() == NodeType::Array) {
			stream << '[';
		}

		// If a node type is a primitive type.
		constexpr static auto IsPrimitive = [](const Node &type) {
			return type.GetProperties().empty() && type.GetType() != NodeType::Object && type.GetType() != NodeType::Array && type.GetType() != NodeType::Unknown;
		};

		// Shorten primitive array output length.
		if (isArray && format.inlineArrays && !property.GetProperties().empty() && IsPrimitive(property.GetProperty(0))) {
			stream << format.GetIndents(indent + 1);
			// New lines are printed a a space, no spaces are ever emitted by primitives.
			AppendData(property, stream, Format(0, '\0', '\0', false), indent);
			stream << '\n';
		} else {
			AppendData(property, stream, format, indent + 1);
		}

		if (!property.GetProperties().empty()) {
			stream << indents << (isArray ? ']' : '}');
		} else if (property.GetType() == NodeType::Object) {
			stream << '}';
		} else if (property.GetType() == NodeType::Array) {
			stream << ']';
		}

		// Separate properties by comma.
		if (it != std::prev(node.GetProperties().end()))
			stream << ',';
		// No new line if the indent level is zero (if primitive array type).
		stream << (indent != 0 ? format.newLine : format.space);
	}
}
}

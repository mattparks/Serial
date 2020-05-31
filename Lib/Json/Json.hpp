#pragma once

#include "Node.hpp"

namespace serial {
class Json {
public:
	Json() = delete;
	
	static void ParseString(Node &node, std::string_view string);
	static void WriteStream(const Node &node, std::ostream &stream, Node::Format format);

private:
	static void AddToken(std::string_view view, std::vector<Node::Token> &tokens);
	static void Convert(Node &current, const std::vector<Node::Token> &tokens, int32_t i, int32_t &r);

	static void AppendData(const Node &node, std::ostream &stream, Node::Format format, int32_t indent);
};
}

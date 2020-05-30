#pragma once

#include "Node.hpp"

namespace acid {
class Xml : public Node::Formatter {
public:
	explicit Xml(const std::string &rootName = "", Node::Format format = Node::Format::Minified);

	void ParseString(Node &node, std::string_view string) const override;
	void WriteStream(const Node &node, std::ostream &stream) const override;

private:
	static void AddToken(std::string_view view, std::vector<Node::Token> &tokens);
	static void Convert(Node &current, const std::vector<Node::Token> &tokens, int32_t i, int32_t &r);

	static void AppendData(const std::string &name, const Node &source, std::ostream &stream, Node::Format format, int32_t indent);

	std::string rootName;
	Node::Format format;
};
}

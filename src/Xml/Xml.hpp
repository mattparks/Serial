#pragma once

#include "Node.hpp"

namespace serial {
class XmlTokenizer;

class Xml : public NodeFormatType<Xml> {
public:
	static constexpr char AttributePrefix = '@';

	// Do not call Load and Write directly, use Node::ParseString<Xml> and Node::WriteStream<Xml>.
	static void Load(Node &node, std::string_view string);
	static void Write(const Node &node, std::ostream &stream, Format format = Minified);

private:
	static void Convert(Node &current, XmlTokenizer &tokenizer);
	static Node &CreateProperty(Node &current, const std::string &name);
	
	static void AppendData(const NodeKey &nodeKey, const Node &node, std::ostream &stream, Format format, int32_t indent);
};
}

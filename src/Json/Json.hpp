#pragma once

#include "Node.hpp"

namespace serial {
class JsonTokenizer;
class Json : public NodeFormatType<Json> {
public:
	// Do not call Load and Write directly, use Node::ParseString<Json> and Node::WriteStream<Json>.
	static void Load(Node &node, std::string_view string);
	static void Write(const Node &node, std::ostream &stream, Format format = Minified);

private:
	static void Convert(Node &current, JsonTokenizer &tokenizer);
	static void AppendData(const Node &node, std::ostream &stream, Format format, int32_t indent);
};
}

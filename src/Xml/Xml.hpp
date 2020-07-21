#pragma once

#include "NodeFormat.hpp"

namespace serial {
class Xml : public NodeFormatType<Xml> {
public:
	static Node ParseString(std::string_view string);
	static void WriteStream(const Node &node, std::ostream &stream, Format format = Minified);

private:
	static void AddToken(std::string_view view, std::vector<Token> &tokens);
	static void Convert(Node &current, const std::vector<Token> &tokens, int32_t &k);
	static Node &CreateProperty(Node &current, const std::string &name);
	
	static void AppendData(const Node &node, std::ostream &stream, Format format, int32_t indent);
};
}

#pragma once

#include "Serialized/Node.hpp"

namespace acid {
class Json : public Node {
public:
	Json() = default;
	Json(const Node &node);
	Json(Node &&node);

	void LoadString(std::string_view string) override;
	void WriteStream(std::ostream &stream, Format format = Format::Minified) const override;

private:
	static void AddToken(const char *start, const char *end, std::vector<Token> &tokens);
	static void Convert(Node &current, const std::vector<Token> &v, int32_t i, int32_t &r);

	static void AppendData(const Node &source, std::ostream &stream, int32_t indentation, Node::Format format);
};
}

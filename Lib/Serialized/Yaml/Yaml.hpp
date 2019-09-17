#pragma once

#include "Serialized/Node.hpp"

namespace acid {
class Yaml : public Node {
public:
	Yaml() = default;
	Yaml(const Node &node);
	Yaml(Node &&node);

	void LoadString(std::string_view string) override;
	void WriteStream(std::ostream &stream, Format format = Format::Minified) const override;

private:
	static void AppendData(const Node &source, std::ostream &stream, int32_t indentation, Node::Format format);
};
}

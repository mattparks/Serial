#pragma once

#include "Serialized/Node.hpp"

namespace acid {
class Xml : public Node {
public:
	Xml() = default;
	Xml(const Node &node);
	Xml(Node &&node);

	void LoadString(std::string_view string) override;
	void WriteStream(std::ostream &stream, Format format = Format::Minified) const override;

private:
};
}

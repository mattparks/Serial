#pragma once

#include "Serialized/Node.hpp"

namespace acid {
class Yaml :
	public Node {
public:
	Yaml() = default;
	explicit Yaml(const Node &node);

	void Load(std::istream &stream);
	void Load(const std::string &string);
	
	void Write(std::ostream &stream, Format format = Format::Beautified) const;
	std::string Write(Format format = Format::Beautified) const;

private:
	static void AppendData(const Node &source, std::ostream &stream, int32_t indentation, Format format);
};
}

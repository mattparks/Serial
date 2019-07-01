#pragma once

#include "Serialized/Node.hpp"

namespace acid
{
class Yaml :
	public Node
{
public:
	Yaml() = default;

	explicit Yaml(const Node &node);

	void Load(std::istream &stream) override;

	void Write(std::ostream &stream, const Format &format = Format::Beautified) const override;

	void Load(const std::string &string);

	std::string Write(const Format &format = Format::Beautified) const;

private:
	static void AppendData(const Node &source, std::ostream &stream, const int32_t &indentation, const Format &format);
};
}

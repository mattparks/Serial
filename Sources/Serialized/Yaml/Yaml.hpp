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

	void Load(std::istream &inStream) override;

	void Write(std::ostream &outStream, const Format &format = Format::Beautified) const override;

	void Load(const std::string &string);

	std::string Write(const Format &format = Format::Beautified) const;

private:
	static std::string GetIndents(const int32_t &indentation);

	static void AppendData(const Node &source, std::ostream &outStream, const int32_t &indentation, const Format &format);
};
}

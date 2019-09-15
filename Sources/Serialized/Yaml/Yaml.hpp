#pragma once

#include "Serialized/Node.hpp"

namespace acid {
class ACID_EXPORT Yaml : public Node {
public:
	Yaml() = default;
	explicit Yaml(const Node &node);
	explicit Yaml(Yaml &&node);

protected:
	void LoadStructure(const std::string &string) override;
	void WriteStructure(std::ostream &stream, Format format) const override;

private:
	static void AppendData(const Node &source, std::ostream &stream, int32_t indentation, Format format);
};
}

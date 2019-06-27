#pragma once

#include "Serialized/Node.hpp"

namespace acid
{
class Json :
	public Node
{
public:
	Json() = default;

	explicit Json(const Node &node);

	void Load(std::istream &inStream) override;

	void Write(std::ostream &outStream, const Format &format = Format::Beautified) const override;

	void Load(const std::string &string);

	std::string Write(const Format &format = Format::Beautified) const;

private:
	enum class Token
	{
		Unknown, String, Boolean, Number, Null, CurlyOpen, CurlyClose, SquareOpen, SquareClose, Comma, Colon
	};

	static void Convert(Node &current, const std::vector<std::pair<Token, std::string>> &v, const int32_t &i, int32_t &r);

	static void AppendData(const Node &source, std::ostream &outStream, const int32_t &indentation, const Format &format);
};
}

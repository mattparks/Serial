#pragma once

#include "Serialized/Node.hpp"

namespace acid {
class Json :
	public Node {
public:
	Json() = default;
	explicit Json(const Node &node);
	explicit Json(Node &&node);

	template<typename _Elem>
	void Load(std::basic_istream<_Elem> &stream);
	template<typename _Elem>
	void Load(const std::basic_string<_Elem> &string);

	template<typename _Elem>
	void Write(std::basic_ostream<_Elem> &stream, Format format = Format::Beautified) const;
	template<typename _Elem = char>
	std::basic_string<_Elem> Write(Format format = Format::Beautified) const;

private:
	static void AddToken(std::vector<std::pair<Type, std::string>> &tokens, std::string &current);
	static void Convert(Node &current, const std::vector<std::pair<Type, std::string>> &v, int32_t i, int32_t &r);

	template<typename _Elem>
	static void AppendData(const Node &source, std::basic_ostream<_Elem> &stream, int32_t indentation, Format format);
};
}

#include "Json.inl"

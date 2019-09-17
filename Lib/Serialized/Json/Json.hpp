#pragma once

#include "Serialized/DocumentParser.hpp"

namespace acid {
template<typename _Elem = char>
class Json : public DocumentParser<_Elem> {
public:
	Json() = default;

	void LoadString(Document &document, std::basic_string_view<_Elem> string) override;
	void WriteStream(const Document &document, std::basic_ostream<_Elem> &stream, Document::Format format = Document::Format::Minified) const override;

private:
	static void AddToken(std::vector<std::pair<Document::Type, std::string>> &tokens, std::string &current);
	static void Convert(Document &current, const std::vector<std::pair<Document::Type, std::string>> &v, int32_t i, int32_t &r);

	static void AppendData(const Document &source, std::basic_ostream<_Elem> &stream, int32_t indentation, Document::Format format);
};
}

#include "Json.inl"

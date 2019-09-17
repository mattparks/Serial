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
	class Token {
	public:
		Token(const char *start, const char *end, Document::Type type) :
			start(start),
			end(end),
			type(type) {
		}

		char GetChar() const {
			if (end - start != 1) return '\0';
			return *start;
		}
		
		std::string_view GetView() const {
			return std::string_view(start, end - start);
		}

		std::string GetString() const {
			return std::string(start, end - start);
		}

		const char *start;
		const char *end;
		Document::Type type;
	};
	
	static void AddToken(const char *start, const char *end, std::vector<Token> &tokens);
	static void Convert(Document &current, const std::vector<Token> &v, int32_t i, int32_t &r);

	static void AppendData(const Document &source, std::basic_ostream<_Elem> &stream, int32_t indentation, Document::Format format);
};
}

#include "Json.inl"

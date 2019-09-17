#pragma once

#include "Serialized/Document.hpp"

namespace acid {
class Json : public Document {
public:
	Json() = default;
	Json(const Document &document);
	Json(Document &&document);

	void LoadString(std::string_view string) override;
	void WriteStream(std::ostream &stream, Format format = Format::Minified) const override;

private:
	class Token {
	public:
		Token(const char *start, const char *end, Type type) :
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
		Type type;
	};
	
	static void AddToken(const char *start, const char *end, std::vector<Token> &tokens);
	static void Convert(Document &current, const std::vector<Token> &v, int32_t i, int32_t &r);

	static void AppendData(const Document &source, std::ostream &stream, int32_t indentation, Document::Format format);
};
}

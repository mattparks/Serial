#pragma once

#include "Document.hpp"

namespace acid {
template<typename _Elem>
class DocumentParser {
public:
	virtual ~DocumentParser() = default;

	virtual void LoadArray(Document &document, const _Elem *str, std::size_t length) = 0;
	virtual void WriteStream(const Document &document, std::basic_ostream<_Elem> &stream, Document::Format format = Document::Format::Minified) const = 0;
	
	void LoadString(Document &document, std::basic_string_view<_Elem> string) {
		LoadArray(document, string.data(), string.length());
	}
	
	void LoadStream(Document &document, std::basic_istream<_Elem> &stream) {
		// We must read as UTF8 chars.
		stream.imbue(std::locale(stream.getloc(), new std::codecvt_utf8<char>));

		// Reading into a string before iterating is much faster.
		std::string s(std::istreambuf_iterator<_Elem>(stream), {});
		LoadString(document, s);
	}
	
	std::basic_string<_Elem> WriteString(const Document &document, Document::Format format = Document::Format::Minified) const {
		std::basic_stringstream<_Elem> stream;
		WriteStream(document, stream, format);
		return stream.str();
	}
};
}

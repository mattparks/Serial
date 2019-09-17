#include "Xml.hpp"

#include "Helpers/String.hpp"

namespace acid {
Xml::Xml(const Node &node) :
	Node(node) {
	SetType(Type::Object);
}

Xml::Xml(Node &&node) :
	Node(std::move(node)) {
	SetType(Type::Object);
}

void Xml::LoadString(std::string_view string) {
}

void Xml::WriteStream(std::ostream &stream, Format format) const {
}
}

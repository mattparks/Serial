#include "Yaml.hpp"

#include "Helpers/String.hpp"

namespace acid {
Yaml::Yaml(const Node &node) :
	Node(node) {
	SetType(Type::Object);
}

Yaml::Yaml(Node &&node) :
	Node(std::move(node)) {
	SetType(Type::Object);
}

void Yaml::LoadString(std::string_view string) {
}

void Yaml::WriteStream(std::ostream &stream, Format format) const {
	stream << "---\n";
	AppendData(*this, stream, 0, format);
}

void Yaml::AppendData(const Node &source, std::ostream &stream, int32_t indentation, Format format) {
	// Creates a string for the indentation level.
	std::string indents(2 * indentation, ' ');

	// Only output the value if no properties exist.
	if (source.GetProperties().empty()) {
		if (source.GetType() == Type::String)
			stream << '\"' << String::FixReturnTokens(source.GetValue()) << '\"';
		else
			stream << source.GetValue();
	}

	// Output each property.
	for (auto it = source.GetProperties().begin(); it < source.GetProperties().end(); ++it) {
		stream << '\n' << indents;

		// Output name for property if it exists.
		if (!it->GetName().empty()) {
			if (String::IsNumber(it->GetName())) {
				stream << "\"" << it->GetName() << "\": ";
			} else {
				stream << it->GetName() << ": ";
			}
		} else if (it->GetValue().empty()) {
			stream << '-';
		}
		
		bool isArray = false;
		// If all properties have no names, then this must be an array.
		for (const auto &property2 : it->GetProperties()) {
			if (property2.GetName().empty()) {
				isArray = true;
				break;
			}
		}

		// Appends the current stream with the property data.
		AppendData(*it, stream, indentation + (isArray ? 0 : 1), format);
	}
}
}

#include "Yaml.hpp"

#include "Helpers/String.hpp"

namespace acid {
Yaml::Yaml(const Node &node) :
	Node{node} {
}

Yaml::Yaml(Yaml &&node) :
	Node(std::move(node)) {
	SetType(Type::Object);
}

void Yaml::LoadStructure(const std::string &string) {
}

void Yaml::WriteStructure(std::ostream &stream, Format format) const {
	stream << "---\n";
	AppendData(*this, stream, 0, format);
}

void Yaml::AppendData(const Node &source, std::ostream &stream, int32_t indentation, Format format) {
	// Creates a string for the indentation level.
	std::string indents(2 * indentation, ' ');

	// Only output the value if no properties exist.
	if (source.GetProperties().empty()) {
		auto value = source.GetValue();
		auto lines = String::Split(value, '\n');

		if (lines.size() > 1) {
			stream << "|\n";
		}

		for (const auto &line : lines) {
			if (lines.size() > 1) {
				stream << indents;
			}

			stream << line << "\n";
		}
	}

	// Output each property.
	for (auto it = source.GetProperties().begin(); it < source.GetProperties().end(); ++it) {
		stream << indents;

		// Output name for property if it exists.
		if (!it->GetName().empty()) {
			if (String::IsNumber(it->GetName())) {
				stream << "\"" << it->GetName() << "\": ";
			} else {
				stream << it->GetName() << ": ";
			}
		}

		// Appends the current stream with the property data.
		AppendData(*it, stream, indentation + 1, format);
	}
}
}

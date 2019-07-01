#include "Yaml.hpp"

#include "Helpers/String.hpp"

namespace acid
{
Yaml::Yaml(const Node &node) :
	Node{node}
{
}

void Yaml::Load(std::istream &stream)
{
}

void Yaml::Write(std::ostream &stream, const Format &format) const
{
	stream << "---\n";
	AppendData(*this, stream, 0, format);
}

void Yaml::Load(const std::string &string)
{
	std::stringstream stream{string};
	Load(stream);
}

std::string Yaml::Write(const Format &format) const
{
	std::stringstream stream;
	Write(stream, format);
	return stream.str();
}

void Yaml::AppendData(const Node &source, std::ostream &stream, const int32_t &indentation, const Format &format)
{
	// Creates a string for the indentation level.
	std::string indents(2 * indentation, ' ');

	// Only output the value if no properties exist.
	if (source.GetProperties().empty())
	{
		auto value{source.GetValue<std::string>()};
		auto lines{String::Split(value, '\n')};

		if (lines.size() > 1)
		{
			stream << "|\n";
		}

		for (const auto &line : lines)
		{
			if (lines.size() > 1)
			{
				stream << indents;
			}

			stream << line << "\n";
		}
	}

	// Output each property.
	for (auto it{source.GetProperties().begin()}; it < source.GetProperties().end(); ++it)
	{
		stream << indents;

		// Output name for property if it exists.
		if (!it->first.empty())
		{
			if (String::IsNumber(it->first))
			{
				stream << "\"" << it->first << "\": ";
			}
			else
			{
				stream << it->first << ": ";
			}
		}

		// Appends the current stream with the property data.
		AppendData(it->second, stream, indentation + 1, format);
	}
}
}

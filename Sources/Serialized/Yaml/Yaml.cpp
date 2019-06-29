#include "Yaml.hpp"

#include "Helpers/String.hpp"

namespace acid
{
Yaml::Yaml(const Node &node) :
	Node{node}
{
}

void Yaml::Load(std::istream &inStream)
{
}

void Yaml::Write(std::ostream &outStream, const Format &format) const
{
	outStream << "---\n";
	AppendData(*this, outStream, 0, format);
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

std::string Yaml::GetIndents(const int32_t &indentation)
{
	std::stringstream indents;

	for (int32_t i{}; i < indentation; i++)
	{
		indents << "  ";
	}

	return indents.str();
}

void Yaml::AppendData(const Node &source, std::ostream &outStream, const int32_t &indentation, const Format &format)
{
	// Creates a string for the indentation level.
	auto indents{GetIndents(indentation)};

	// Only output the value if no properties exist.
	if (source.GetProperties().empty())
	{
		auto value{source.GetValue<std::string>()};
		auto lines{String::Split(value, '\n')};

		if (lines.size() > 1)
		{
			outStream << "|\n";
		}

		for (const auto &line : lines)
		{
			if (lines.size() > 1)
			{
				outStream << indents;
			}

			outStream << line << "\n";
		}
	}

	// Output each property.
	for (auto it{source.GetProperties().begin()}; it < source.GetProperties().end(); ++it)
	{
		outStream << indents;

		// Output name for property if it exists.
		if (!it->first.empty())
		{
			if (String::IsNumber(it->first))
			{
				outStream << "\"" << it->first << "\": ";
			}
			else
			{
				outStream << it->first << ": ";
			}
		}

		// Appends the current stream with the property data.
		AppendData(it->second, outStream, indentation + 1, format);
	}
}
}

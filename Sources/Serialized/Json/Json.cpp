#include "Json.hpp"

#include "Helpers/String.hpp"
#include <iostream>

namespace acid
{
Json::Json(const Node &node) :
	Node{node}
{
}

void AddToken(std::vector<std::pair<Node::Type, std::string>> &tokens, std::stringstream &current)
{
	if (auto str{current.str()}; !str.empty())
	{
		// Finds the node value type of the string and adds it to the tokens vector.
		if (String::IsNumber(str))
		{
			tokens.emplace_back(Node::Type::Number, str);
		}
		else if (str == "null")
		{
			tokens.emplace_back(Node::Type::Null, str);
		}
		else if (str == "true" || str == "false")
		{
			tokens.emplace_back(Node::Type::Boolean, str);
		}
		else
		{
			tokens.emplace_back(Node::Type::String, str);
		}
	}

	// Clears the current summation stream.
	current.str({});
}

void Json::Load(std::istream &inStream)
{
	std::vector<std::pair<Type, std::string>> tokens;

	char c;
	std::stringstream current;
	bool inString{};

	// Read stream until end of file.
	while (!inStream.eof())
	{
		inStream.get(c);

		// On start of string switch in/out of stream space and ignore this char.
		if (c == '"' || c == '\'')
		{			 
			inString ^= 1;
			continue;
		}

		// When not reading a string tokens can be found.
		if (!inString)
		{
			// Tokens used to read json nodes.
			if (std::string{",{}[]:"}.find(c) != std::string::npos)
			{
				AddToken(tokens, current);
				tokens.emplace_back(Type::Unknown, std::string{c});
				continue;
			}

			// On whitespace save current as a string.
			if (String::IsWhitespace(c))
			{
				AddToken(tokens, current);
				continue;
			}
		}

		// Add this char to the current builder stream.
		current << c;
	}

	// Converts the list of tokens into nodes.
	int32_t k{};
	Convert(*this, tokens, 0, k);
}

void Json::Write(std::ostream &outStream, const Format &format) const
{
	// Json files are wrapped with an extra set of braces.
	AppendData({"", {{"", *this}}}, outStream, 0, format);
}

void Json::Load(const std::string &string)
{
	std::stringstream stream{string};
	Load(stream);
}

std::string Json::Write(const Format &format) const
{
	std::stringstream stream;
	Write(stream, format);
	return stream.str();
}

void Json::Convert(Node &current, const std::vector<std::pair<Type, std::string>> &v, const int32_t &i, int32_t &r)
{
	if (v[i].second == "{")
	{
		auto k{i + 1};

		while (v[k].second != "}")
		{
			auto key{v[k].second};
			k += 2; // k + 1 should be ':'
			Convert(current.AddProperty(key), v, k, k);

			if (v[k].second == ",")
			{
				k++;
			}
		}

		current.SetType(Type::Object);
		r = k + 1;
	}
	else if (v[i].second == "[")
	{
		auto k{i + 1};

		while (v[k].second != "]")
		{
			Convert(current.AddProperty(), v, k, k);

			if (v[k].second == ",")
			{
				k++;
			}
		}

		current.SetType(Type::Array);
		r = k + 1;
	}
	else
	{
		current.SetValue(v[i].second);
		current.SetType(v[i].first);
		r = i + 1;
	}
}

std::string GetIndents(const int32_t &indentation)
{
	std::stringstream indents;

	for (int32_t i{}; i < indentation; i++)
	{
		indents << "  ";
	}

	return indents.str();
}

void Json::AppendData(const Node &source, std::ostream &outStream, const int32_t &indentation, const Format &format)
{
	// Creates a string for the indentation level.
	auto indents{GetIndents(indentation)};

	// Only output the value if no properties exist.
	if (source.GetProperties().empty())
	{
		auto value{String::FixReturnTokens(source.GetValue<std::string>())};

		if (source.GetType() == Type::String)
		{
			outStream << '\"' << value << '\"';
		}
		else
		{
			outStream << value;
		}
	}

	// Output each property.
	for (auto it{source.GetProperties().begin()}; it < source.GetProperties().end(); ++it)
	{
		std::string openString;
		std::string closeString;

		// Gets array or object type braces.
		if (!it->second.GetProperties().empty())
		{
			openString = "{";
			closeString = "}";

			// Sets the braces to an array style if all properties have no names.
			for (const auto &[propertyName2, property2] : it->second.GetProperties())
			{
				if (propertyName2.empty())
				{
					openString = "[";
					closeString = "]";
					break;
				}
			}

			if (format != Format::Minified)
			{
				openString += '\n';
				closeString.insert(0, indents);
			}
		}

		// Separate properties by comma.
		if (it != source.GetProperties().end() - 1)
		{
			closeString += ',';
		}

		if (format != Format::Minified)
		{
			outStream << indents;
		}

		// Output name for property if it exists.
		if (!it->first.empty())
		{
			outStream << "\"" << it->first << "\":";

			if (format != Format::Minified)
			{
				outStream << ' ';
			}
		}

		// Appends the current stream with the property data.
		outStream << openString;
		AppendData(it->second, outStream, indentation + 1, format);
		outStream << closeString;

		if (format != Format::Minified)
		{
			outStream << '\n';
		}
	}
}
}

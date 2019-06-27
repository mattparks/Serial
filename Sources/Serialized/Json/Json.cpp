#include "Json.hpp"

#include "Helpers/String.hpp"
#include <cassert>

// https://github.com/amir-s/jute
// TODO: Split line whitespace right away.

namespace acid
{
Json::Json(const Node &node) :
	Node{node}
{
}

bool IsWhitespace(const char c)
{
	return std::string{" \n\r  "}.find(c) != std::string::npos;
}

int32_t SkipWhitespaces(const std::string &source, int32_t i = 0)
{
	// Gets the next index of the string that is not whitespace.
	while (i < static_cast<int32_t>(source.length()))
	{
		if (!IsWhitespace(source[i]))
		{
			return i;
		}

		i++;
	}

	// The entire string is whitespace.
	return -1;
}

int32_t NextWhitespace(const std::string &source, int32_t i)
{
	// Finds the index before the next whitespace, string whitespace is ignored.
	while (i < static_cast<int32_t>(source.length()))
	{
		if (source[i] == '"')
		{
			i++;

			// Go to the end of the string.
			while (i < static_cast<int32_t>(source.length()) && (source[i] != '"' || source[i - 1] == '\\'))
			{
				i++;
			}
		}

		if (source[i] == '\'')
		{
			i++;

			// Go to the end of the string.
			while (i < static_cast<int32_t>(source.length()) && (source[i] != '\'' || source[i - 1] == '\\'))
			{
				i++;
			}
		}

		if (IsWhitespace(source[i]))
		{
			return i;
		}

		i++;
	}

	// No whitespace found in the string.
	return source.length();
}

void Json::Load(std::istream &inStream)
{
	std::string tmp;
	std::vector<std::pair<Type, std::string>> tokens;

	while (std::getline(inStream, tmp))
	{
		// Gets the start of the line skipping whitespace.
		auto index{SkipWhitespaces(tmp, 0)};

		while (index >= 0)
		{
			// The next whitespace location after the last one.
			auto next{NextWhitespace(tmp, index)};
			auto str{tmp.substr(index, next - index)};

			std::size_t k{};

			while (k < str.length())
			{
				if (str[k] == '"')
				{
					auto tmpK{k + 1};

					while (tmpK < str.length() && (str[tmpK] != '"' || str[tmpK - 1] == '\\'))
					{
						tmpK++;
					}

					tokens.emplace_back(Type::String, str.substr(k + 1, tmpK - k - 1));
					k = tmpK + 1;
					continue;
				}
				if (str[k] == '\'')
				{
					auto tmpK{k + 1};

					while (tmpK < str.length() && (str[tmpK] != '\'' || str[tmpK - 1] == '\\'))
					{
						tmpK++;
					}

					tokens.emplace_back(Type::String, str.substr(k + 1, tmpK - k - 1));
					k = tmpK + 1;
					continue;
				}
				if (str[k] == '-' || (str[k] <= '9' && str[k] >= '0'))
				{
					auto tmpK{k};

					if (str[tmpK] == '-')
					{
						tmpK++;
					}

					while (tmpK < str.size() && ((str[tmpK] <= '9' && str[tmpK] >= '0') || str[tmpK] == '.'))
					{
						tmpK++;
					}

					tokens.emplace_back(Type::Number, str.substr(k, tmpK - k));
					k = tmpK;
					continue;
				}
				if (str[k] == 't' && k + 3 < str.length() && str.substr(k, 4) == "true")
				{
					tokens.emplace_back(Type::Boolean, "true");
					k += 4;
					continue;
				}
				if (str[k] == 'f' && k + 4 < str.length() && str.substr(k, 5) == "false")
				{
					tokens.emplace_back(Type::Boolean, "false");
					k += 5;
					continue;
				}
				if (str[k] == 'n' && k + 3 < str.length() && str.substr(k, 4) == "null")
				{
					tokens.emplace_back(Type::Null, "null");
					k += 4;
					continue;
				}
				if (str[k] == ',' || str[k] == '}' || str[k] == '{' || str[k] == ']' || str[k] == '[' || str[k] == ':')
				{
					tokens.emplace_back(Type::Unknown, str.substr(k, 1));
					k++;
					continue;
				}

				//tokens.emplace_back(str.substr(k));
				k = str.length();
			}

			index = SkipWhitespaces(tmp, next);
		}
	}

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

		if (source.GetType() == Node::Type::String)
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

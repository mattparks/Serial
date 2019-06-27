#pragma once

#include <istream>

namespace acid
{
/**
 * @brief Module used for managing files on engine updates.
 */
class Files
{
public:
	/**
	 * Gets the next line from a stream.
	 * @param is The input stream.
	 * @param t The next string.
	 * @return The input stream.
	 */
	static std::istream &SafeGetLine(std::istream &is, std::string &t);
};
}

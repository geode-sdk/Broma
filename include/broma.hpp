#pragma once

#include "ast.hpp"

namespace broma {
	/// @brief Pass a file for Broma to parse into an AST.
	///
	/// This function throws, so make sure to handle errors properly if needed.
	///
	/// @param fname The path of the file you want to parse, as a string.
	Root parse_file(std::string const& fname);

	/// @brief Parses a broma file safely by not exiting when the parser throws an error
	/// @param fname The path of the file you want to parse, as a string.
	/// @return A root result with a boolean value to check for errors.
	SafeRootResult parse_file_safely(std::string const& fname);

}

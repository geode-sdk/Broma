#pragma once

#include <Geode/Result.hpp>
#include <filesystem>

#include "ast.hpp"

namespace broma {
	/// @brief Pass a file for Broma to parse into an AST.
	///
	/// This function throws, so make sure to handle errors properly if needed.
	///
	/// @param fname The path of the file you want to parse, as a string.
	geode::Result<Root, ParseError> parse_file(std::filesystem::path const& fname);
}

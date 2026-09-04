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

	/// @brief Parse an `std::string_view` of Broma declarations in memory into an AST.
	///
	/// @param source The `std::string_view` of Broma declarations to parse.
	/// @param include_base Where relative `#include`s inside `source` are
	///   resolved from. Defaults to the current working directory.
	/// @param source_name A label used for the `source` field on
	///   parsed classes/functions/headers (see Root::sources()).
	///   Defaults to `<string>`.
	geode::Result<Root, ParseError> parse_string(
		std::string_view source,
		std::filesystem::path const& include_base = std::filesystem::current_path(),
		std::string const& source_name = "<string>"
	);
}

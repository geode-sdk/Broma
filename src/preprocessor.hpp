#pragma once

#include "basic_components.hpp"
#include "state.hpp"
#include "paths.hpp"

namespace broma {
	/// @brief The inner name of the file to be included.
	struct include_name : until<at<one<'>'>>> {};

	/// @brief A C++ include expression.
	///
	/// Currently, this only supports angle bracket includes.
	struct include_expr : seq<ascii::string<'#', 'i', 'n', 'c', 'l', 'u', 'd', 'e'>, sep, one<'<'>, include_name, one<'>'>> {};

	struct root_grammar;

	template <>
	struct run_action<include_name> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			std::filesystem::path name = input.string();
			// find relative to file path first
			if (std::filesystem::exists(scratch->include_path / name))
				name = scratch->include_path / name;
			else if (!std::filesystem::exists(name))
				throw parse_error("could not resolve path for included file: " + pathToString(name), input.position());

			std::filesystem::path canonical = std::filesystem::canonical(name);
			if (!scratch->included_files.insert(pathToString(canonical)).second)
				return;

			file_input<> include_input(name);

			parse<must<root_grammar>, broma::run_action>(include_input, root, scratch);
		}
	};
} // namespace broma

#pragma once

#include "basic_components.hpp"
#include "state.hpp"
#include "paths.hpp"

namespace broma {
	/// @brief The inner name of the file to be included.
	struct include_name : until<at<one<'>'>>> {};

	/// @brief A C++ include expression for Broma files.
	/// The declarations read from the included file are merged
	/// into the current result `Root` struct.
	/// Results can be filtered per source using `Root::filterBySource`.
	///
	/// Currently, this only supports angle bracket includes.
	struct include_expr :
		seq<
			ascii::string<'#', 'i', 'n', 'c', 'l', 'u', 'd', 'e'>, // #include
			sep,
			one<'<'>,
			include_name,
			one<'>'>
		> {};

	struct root_grammar;

	template <>
	struct run_action<include_name> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			std::filesystem::path name = input.string();
			// find relative to file path first
			if (std::filesystem::exists(scratch->include_path / name))
				name = scratch->include_path / name;
			else if (!std::filesystem::exists(name)) {
				// this is non-fatal because per-design, Broma is just a data container mini-language
				// so there's no real scenario where having a failed include expression could cause issues
				scratch->error("could not resolve path for included file: " + paths::pathToString(name), input.position());
				return;
			}

			// realistically a Broma file include either fatally errors or actually resolves
			// since its existence is already accounted for
			std::string const& canonical = scratch->canonicalizePath(name);
			if (!scratch->included_files.insert(canonical).second)
				return;

			file_input<> include_input(name);

			parse<must<root_grammar>, broma::run_action>(include_input, root, scratch);
		}
	};
} // namespace broma

#pragma once

#include "basic_components.hpp"
#include "state.hpp"

namespace broma {
	/// @brief The inner name of the file to be included.
	struct include_name : until<at<one<'>'>>> {};

	/// @brief A platform specifier for includes.
	struct include_platform : sor<keyword_win, keyword_mac, keyword_ios, keyword_android> {};

	/// @brief A C++ include expression.
	///
	/// Currently, this only supports angle bracket includes.
	struct include_expr : seq<ascii::string<'#', 'i', 'n', 'c', 'l', 'u', 'd', 'e'>, sep, opt<include_platform, sep>, one<'<'>, include_name, one<'>'>> {};

	struct root_grammar;

	template <>
	struct run_action<include_platform> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_include_platform = str_to_platform(input.string());
		}
	};

	template <>
	struct run_action<include_name> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			std::filesystem::path name = input.string();
			if (!std::filesystem::exists(name))
				name = scratch->include_path / name;
			if (!std::filesystem::exists(name)) {
				root->headers.push_back({
					.name = input.string(),
					.platform = scratch->wip_include_platform
				});
				return;
			}

			file_input<> file_input(name);

			parse<root_grammar, broma::run_action>(file_input, root, scratch);
		}
	};
} // namespace broma

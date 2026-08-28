#pragma once

#include "basic_components.hpp"
#include "state.hpp"

namespace broma {
	/// @brief The inner name of the file to be imported.
	struct import_name : until<at<one<'>'>>> {};

	/// @brief A C++ import expression for header files.
	/// Becomes a `Header` struct.
	///
	/// Currently, this only supports angle bracket imports.
	struct import_expr :
		seq<
			ascii::string<'#', 'i', 'm', 'p', 'o', 'r', 't'>, // '#import'
			sep,
			opt<tagged_platform<import_expr>, sep>,
			one<'<'>,
			import_name,
			one<'>'>
		> {};

	template <>
	struct run_action<tagged_platform<import_expr>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_import_platform = str_to_platform(input.string());
		}
	};

	template <>
	struct run_action<import_name> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			root->headers.push_back({
				.name = input.string(),
				.platform = scratch->wip_import_platform,
				.source = scratch->canonicalizePath(input.input().source()),
				.line = input.position().line
			});
			scratch->wip_import_platform = Platform::All;
		}
	};
} // namespace broma

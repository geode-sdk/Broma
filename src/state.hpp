#pragma once

#include <tao/pegtl.hpp>
#include <ast.hpp>
#include <unordered_map>

#include "paths.hpp"

namespace broma {
	/// @brief Base class for all Broma actions.
	///
	/// Template specializations of this class are used as "callbacks"
	/// so that the AST can be changed when the parser reaches an appropriate stage.
	/// See "How Broma uses PEGTL" in the Developer's Guide for more information.
	template <typename Rule>
	struct run_action {};

	/// @brief Temporary parsed data to be manipulated in `run_action` specializations.
	///
	/// If the parser successfully parses a grammar, data here will be written into the final AST.
	struct ScratchData {
		bool is_class;
		Class wip_class;
		FunctionProto wip_fn_proto;
		Field wip_field;
		PlatformNumber wip_bind;
		Platform wip_bind_platform;
		Type wip_type;
		MemberFunctionProto wip_mem_fn_proto;
		Attributes wip_attributes;
		std::string wip_fn_body;
		bool wip_has_explicit_inline;
		std::optional<Platform> wip_platform_block;
		Platform wip_import_platform = Platform::All;
		size_t next_field_id = 0;

		std::vector<tao::pegtl::parse_error> errors;
		std::vector<std::string> post_errors; // post_process errors
		std::unordered_set<std::string> included_files;
		const std::filesystem::path include_path;
		std::unordered_map<std::string, std::string> source_cache;

		/// @brief Centralized function with caching to handle file path strings.
		inline std::string const& canonicalizePath(std::filesystem::path const& path) {
			auto raw = paths::pathToString(path);
			auto [it, inserted] = source_cache.try_emplace(raw);
			if (inserted)
				it->second = paths::canonicalize(path);
			return it->second;
		}

		/// @brief Record a non-fatal error without position info.
		void error(std::string msg) {
			post_errors.push_back(std::move(msg));
		}

		/// @brief Record a non-fatal error during parsing.
		template <typename Pos>
		void error(std::string msg, Pos&& pos) {
			errors.push_back(tao::pegtl::parse_error(std::move(msg), std::forward<Pos>(pos)));
		}

		/// @brief Abort parsing immediately with a fatal error.
		template <typename Pos>
		[[noreturn]] void fatal(std::string msg, Pos&& pos) {
			throw tao::pegtl::parse_error(std::move(msg), std::forward<Pos>(pos));
		}
	};
} // namespace broma

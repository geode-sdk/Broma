#include <tao/pegtl.hpp>
#include <Geode/Result.hpp>
#include <iostream>

#include "attribute.hpp"
#include "basic_components.hpp"
#include "class.hpp"
#include "function.hpp"
#include "import.hpp"
#include "state.hpp"
#include "post_process.hpp"
#include "preprocessor.hpp"

using namespace tao::pegtl;

namespace broma {
	/// @brief Broma's top-level grammar.
	struct root_grammar : until<eof, sep, must<sor<import_expr, include_expr, seq<opt<attribute>, sor<class_statement, function>>>>, sep> {};

	// templated cause of memory_input and file_input being different types
	// but they're the same thing under the hood
	template <typename Input>
	static geode::Result<Root, ParseError> parse_input(Input& input, std::filesystem::path const& include_base) {
		Root root;
		ScratchData scratch { .include_path = include_base };

		try {
			parse<must<root_grammar>, run_action>(input, &root, &scratch);
		} catch (std::exception const& e) {
			return geode::Err(ParseError{ { e.what() } });
		}

		post_process(root, scratch);

		if (!scratch.errors.empty() || !scratch.post_errors.empty()) {
			std::vector<std::string> msgs;

			std::cerr << "[Broma] errors found while parsing:\n";
			for (auto& e : scratch.errors) {
				std::cerr << "\t" << e.what() << "\n";
				msgs.push_back(e.what());
			}
			for (auto& e : scratch.post_errors) {
				std::cerr << "\t" << e << "\n";
				msgs.push_back(e);
			}

			return geode::Err(ParseError{ std::move(msgs) });
		}

		return geode::Ok(std::move(root));
	}

	geode::Result<Root, ParseError> parse_file(std::filesystem::path const& fname) {
		try {
			file_input<> input(fname);
			return parse_input(input, fname.parent_path());
		} catch (std::exception const& e) {
			return geode::Err(ParseError{ { e.what() } });
		}
	}

	geode::Result<Root, ParseError> parse_string(
		std::string_view source,
		std::filesystem::path const& include_base,
		std::string const& source_name
	) {
		// having pure text would only fail at grammar-level parsing, probably
		memory_input<> input(source.data(), source.size(), source_name);
		return parse_input(input, include_base);
	}
} // namespace broma

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
#include "paths.hpp"

using namespace tao::pegtl;

namespace broma {
	struct top_comment_expr : comment_expr_base {};

	template <>
	struct run_action<top_comment_expr> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			auto parsed = extract_comment(input);

			Comment c;
			c.inner = std::move(parsed.text);
			c.multiline = parsed.multiline;
			c.trailing = parsed.trailing;
			c.source = input.input().source();
			c.line = input.position().line;

			root->comments.push_back(std::move(c));
		}
	};

	/// @brief Broma's top-level grammar.
	struct root_grammar : until<eof, sep, must<sor<import_expr, include_expr, top_comment_expr, seq<opt<attribute>, sor<class_statement, function>>>>, sep> {};

	geode::Result<Root, ParseError> parse_file(std::filesystem::path const& fname) {
		Root root;
		ScratchData scratch { .include_path = fname.parent_path() };

		try {
			file_input<> input(fname);
			parse<must<root_grammar>, run_action>(input, &root, &scratch);
		} catch (std::exception const& e) {
			return geode::Err(ParseError{ { e.what() } });
		}

		post_process(root);

		if (!scratch.errors.empty()) {
			std::vector<std::string> msgs;

			std::cerr << "[Broma] errors found while parsing file '" << pathToString(fname) << "' : \n";
			for (auto& e : scratch.errors) {
				std::cerr << "\t" << e.what() << "\n";
				msgs.push_back(e.what());
			}

			return geode::Err(ParseError{ std::move(msgs) });
		}

		return geode::Ok(std::move(root));
	}
} // namespace broma

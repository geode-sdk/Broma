#include <tao/pegtl.hpp>
#include <iostream>

#include "attribute.hpp"
#include "basic_components.hpp"
#include "class.hpp"
#include "function.hpp"
#include "member.hpp"
#include "state.hpp"
#include "post_process.hpp"
#include "preprocessor.hpp"

using namespace tao::pegtl;

namespace broma {
	/// @brief Broma's top-level grammar.
	struct root_grammar : until<eof, sep, must<sor<include_expr, seq<opt<attribute>, sor<class_statement, function>>>>, sep> {};

	Root parse_file(std::string const& fname) {
		file_input<> input(fname);

		Root root;
		ScratchData scratch;
		parse<must<root_grammar>, run_action>(input, &root, &scratch);
		post_process(root);

		if (scratch.errors.size()) {
			std::cerr << "[Broma] errors found on parsing: \n";

			for (auto& e : scratch.errors) {
				std::cerr << "\t" << e.what() << "\n";
			}

			std::exit(1);
		}

		return root;
	}

	SafeRootResult parse_file_safely(std::string const& fname){
		file_input<> input(fname);
		
		SafeRootResult srr;
		
		srr.has_errors = false;

		ScratchData scratch;
		parse<must<root_grammar>, run_action>(input, &srr.root, &scratch);
		post_process(srr.root);
		

		if (scratch.errors.size()){
			srr.has_errors = true;
			
			for (auto& e : scratch.errors) {
				srr.erros.emplace_back(e.what());
			}
		}
		return srr;
	}
} // namespace broma

#pragma once

#include <ast.hpp>
#include <algorithm>
#include "state.hpp"

namespace broma {
	/// @brief Sort classes by required dependencies.
	///
	/// @param cls The class whose requirements will be sorted.
	/// @param root The AST (must be the same AST where the class was found).
	/// @param scratch The ScratchData (must be the same ScratchData where this class was parsed).
	/// @param visiting A vector for keeping a scratch of currently visiting classes for sorting.
	/// @param output A vector to output the sorted classes to.
	inline void sort_class(Class cls, Root& root, ScratchData& scratch, std::vector<std::string>& visiting, std::vector<Class>& output) {
		root.classes.erase(std::remove(root.classes.begin(), root.classes.end(), cls), root.classes.end());
		visiting.push_back(cls.name);

		for (auto& name : cls.attributes.depends) {
			if (std::find(visiting.begin(), visiting.end(), name) != visiting.end()) {
				scratch.error(
					"cyclic dependency: '" + cls.name + "' (" + cls.source + ":" + std::to_string(cls.line) + ")"
					" depends (directly or indirectly) on '" + name + "', which depends back on '" + cls.name + "'"
				);
				continue;
			}

			if (auto* dep = root[name])
				sort_class(*dep, root, scratch, visiting, output);
		}

		visiting.pop_back();
		output.push_back(std::move(cls));
	}

	/// @brief Post process step after the AST is generated.
	///
	/// This step sorts all classes by their dependencies.
	/// This makes it easier to use the AST, as users of Broma do not need
	/// to worry about emitting classes in the right order.
	///
	/// @param root The AST (must be the same AST where the class was found).
	/// @param scratch The ScratchData (must be the same ScratchData where this class was parsed).
	inline void post_process(Root& root, ScratchData& scratch) {
		std::vector<Class> out;
		std::vector<std::string> visiting;

		while (root.classes.size())
			sort_class(root.classes[0], root, scratch, visiting, out);

		root.classes = out;
	}
}

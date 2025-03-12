#pragma once

#include <string>
#include <vector>
#include <variant>
#include <map>
#include <algorithm>
#include <iostream>

namespace broma {
	/// @brief The platform currently being processed in a bind statement.
	enum class Platform {
		None = 0,
		Windows = 2,
		iOS = 4,
		Android32 = 8,
		Android64 = 16,
		Android = 16 | 8, // group for Android32 and Android64
		MacIntel = 32,
		MacArm = 64,
		Mac = 64 | 32,  // group for MacIntel and MacArm
	};

	inline Platform str_to_platform(std::string const& str) {
		if (str == "mac") return Platform::Mac;
		if (str == "win") return Platform::Windows;
		if (str == "android") return Platform::Android;
		if (str == "ios") return Platform::iOS;
		if (str == "imac") return Platform::MacIntel;
		if (str == "m1") return Platform::MacArm;
		if (str == "android32") return Platform::Android32;
		if (str == "android64") return Platform::Android64;
		return Platform::None;
	}

	inline Platform operator|(Platform a, Platform b) {
		return static_cast<Platform>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline Platform& operator|=(Platform& a, Platform b) {
		a = a | b;
		return a;
	}

	inline Platform operator&(Platform a, Platform b) {
		return static_cast<Platform>(static_cast<int>(a) & static_cast<int>(b));
	}

	inline Platform& operator&=(Platform& a, Platform b) {
		a = a & b;
		return a;
	}

	/// @brief Binding offsets for each platform.
	struct PlatformNumber {
		ptrdiff_t imac = -1;
		ptrdiff_t m1 = -1;
		ptrdiff_t ios = -1;
		ptrdiff_t win = -1;
		ptrdiff_t android32 = -1;
		ptrdiff_t android64 = -1;
	};

	/// @brief A C++ type declaration.
	struct Type {
		bool is_struct = false;
		std::string name;

		bool operator==(Type const& t) const {
			return name == t.name;
		}
	};

	/// @brief A version comparison type.
	enum class VersionComparison {
		Exact,
		MoreEqual,
		LessEqual,
		More,
		Less,
	};

	/// @brief A version pre-release type.
	enum class VersionType {
		Alpha,
		Beta,
		Prerelease,
		Release,
	};

	/// @brief A version tag.
	struct VersionTag {
		VersionType type = VersionType::Release;
		std::optional<int> number;
		
		bool operator==(VersionTag const& t) const {
			return type == t.type && number == t.number;
		}

		bool operator<(VersionTag const& t) const {
			if (type < t.type)
				return true;
			if (type > t.type)
				return false;

			if (!number.has_value() && t.number.has_value())
				return true;
			if (number.has_value() && !t.number.has_value())
				return false;

			if (number.has_value() && t.number.has_value())
				return number.value() < t.number.value();

			return false;
		}

		bool operator>(VersionTag const& t) const {
			if (type > t.type)
				return true;
			if (type < t.type)
				return false;

			if (!number.has_value() && t.number.has_value())
				return false;
			if (number.has_value() && !t.number.has_value())
				return true;

			if (number.has_value() && t.number.has_value())
				return number.value() > t.number.value();

			return false;
		}

		bool operator<=(VersionTag const& t) const {
			return *this < t || *this == t;
		}

		bool operator>=(VersionTag const& t) const {
			return *this > t || *this == t;
		}
	};

	/// @brief Represents a semver version.
	struct Version {
		VersionComparison comparison = VersionComparison::MoreEqual;
		int major = 0;
		int minor = 0;
		int patch = 0;
		std::optional<VersionTag> tag;

		bool operator==(Version const& v) const {
			return major == v.major && minor == v.minor && patch == v.patch && tag == v.tag;
		}

		bool operator<(Version const& v) const {
			if (major < v.major)
				return true;
			if (major > v.major)
				return false;

			if (minor < v.minor)
				return true;
			if (minor > v.minor)
				return false;

			if (patch < v.patch)
				return true;
			if (patch > v.patch)
				return false;

			if (!tag.has_value() && v.tag.has_value())
				return false;
			if (tag.has_value() && !v.tag.has_value())
				return true;

			if (tag.has_value() && v.tag.has_value())
				return tag.value() < v.tag.value();

			return false;
		}

		bool operator>(Version const& v) const {
			if (major > v.major)
				return true;
			if (major < v.major)
				return false;

			if (minor > v.minor)
				return true;
			if (minor < v.minor)
				return false;

			if (patch > v.patch)
				return true;
			if (patch < v.patch)
				return false;

			if (!tag.has_value() && v.tag.has_value())
				return true;
			if (tag.has_value() && !v.tag.has_value())
				return false;

			if (tag.has_value() && v.tag.has_value())
				return tag.value() > v.tag.value();

			return false;
		}

		bool operator<=(Version const& v) const {
			return *this < v || *this == v;
		}

		bool operator>=(Version const& v) const {
			return *this > v || *this == v;
		}

		bool is_compatible(Version const& v) const {
			switch (comparison) {
				case VersionComparison::Exact:
					return v == *this;
				case VersionComparison::MoreEqual:
					return v >= *this;
				case VersionComparison::LessEqual:
					return v <= *this;
				case VersionComparison::More:
					return v > *this;
				case VersionComparison::Less:
					return v < *this;
			}
			return false;
		}
	};

	inline Version str_to_version(std::string const& str) {
		Version v;
		VersionTag tag;
		std::vector<std::string> parts = {};

		for (char c : str) {
			if (c == '.' || c == '-') {
				parts.push_back("");
			} else {
				if (parts.empty())
					parts.push_back("");
				parts.back() += c;
			}
		}

		if (parts.size() > 0) {
			if (parts[0].starts_with(">=")) {
				v.comparison = VersionComparison::MoreEqual;
				parts[0].erase(0, 2);
			} else if (parts[0].starts_with("<=")) {
				v.comparison = VersionComparison::LessEqual;
				parts[0].erase(0, 2);
			} else if (parts[0].starts_with(">")) {
				v.comparison = VersionComparison::More;
				parts[0].erase(0, 1);
			} else if (parts[0].starts_with("<")) {
				v.comparison = VersionComparison::Less;
				parts[0].erase(0, 1);
			} else if (parts[0].starts_with("=")) {
				v.comparison = VersionComparison::Exact;
				parts[0].erase(0, 1);
			}

			v.major = std::stoi(parts[0]);
		}
		if (parts.size() > 1) {
			v.minor = std::stoi(parts[1]);
		}
		if (parts.size() > 2) {
			v.patch = std::stoi(parts[2]);
		}
		if (parts.size() > 3) {
			if (parts[3].starts_with("alpha")) {
				tag.type = VersionType::Alpha;
			} else if (parts[3].starts_with("beta")) {
				tag.type = VersionType::Beta;
			} else if (parts[3].starts_with("prerelease")) {
				tag.type = VersionType::Prerelease;
			} else {
				tag.type = VersionType::Release;
			}
		}
		if (parts.size() > 4) {
			tag.number = std::stoi(parts[4]);
		}

		v.tag = tag;
		return v;
	}

	/// @brief List of attributes that could be applied to a class or a function
	struct Attributes {
		std::string docs; ///< Any docstring pulled from a `[[docs(...)]]` attribute.
		Platform links = Platform::None; ///< All the platforms that link the class or function
		Platform missing = Platform::None; ///< All the platforms that are missing the class or function
		std::vector<std::string> depends; ///< List of classes that this class or function depends on
		Version since; ///< The SDK version that this class or function was introduced in
	};

	struct FunctionProto {
		Attributes attributes; ///< Attributes associated with the function.
		Type ret; ///< The return type of the function.
		std::vector<std::pair<Type, std::string>> args; ///< All arguments, represented by their type and their name.
		std::string name; ///< The function's name.

		inline bool operator==(FunctionProto const& f) const {
			if (name != f.name || args.size() != f.args.size()) {
				return false;
			}

			for (size_t i = 0; i < args.size(); ++i) {
				if (!(args[i].first == f.args[i].first)) {
					return false;
				}
			}
			
			return true;
		}
	};

	/// @brief A member function's type.
	enum class FunctionType {
		Normal,
		Ctor, ///< A constructor.
		Dtor ///< A destructor.
	};

	/// @brief The signature of a member function.
	struct MemberFunctionProto : FunctionProto {
		FunctionType type = FunctionType::Normal;
		bool is_const = false;
		bool is_virtual = false;
		bool is_callback = false; ///< Whether or not this function is a callback.
								  ///< In Geode, this determines the function's calling convention (optcall or thiscall).
		bool is_static = false;

		inline bool operator==(MemberFunctionProto const& f) const {
			if (!FunctionProto::operator==(f))
				return false;

			if (is_const != f.is_const)
				return false;

			return true;
		}
	};

	/// @brief A class's member variables.
	struct MemberField {
		Platform platform; ///< For platform-specific members, all platforms this member is defined on 
		std::string name; ///< The name of the field.
		Type type; ///< The type of the field.
		size_t count = 0; ///< The number of elements in the field when it's an array (pretty much unused since we use std::array).
	};

	/// @brief Any class padding.
	struct PadField {
		PlatformNumber amount; ///< The amount of padding, separated per platform.
	};

	/// @brief A function that is bound to an offset.
	struct FunctionBindField {
		MemberFunctionProto prototype;
		PlatformNumber binds; ///< The offsets, separated per platform.
		std::string inner; ///< The (optional) inline body of the function as a raw string.
	};

	/// @brief A inline function body that should go in a header file (.hpp).
	struct InlineField {
		std::string inner; ///< The inline body of the function as a raw string.
	};

	/// @brief A class field.
	struct Field {
		size_t field_id; ///< The index of the field. This starts from 0 and counts up across all classes.
		std::string parent; ///< The name of the parent class.
		std::variant<InlineField, FunctionBindField, PadField, MemberField> inner;

		/// @brief Cast the field into a variant type. This is useful to extract data from the field.
		template <typename T>
		T* get_as() {
			return std::get_if<T>(&inner);
		}

		/// @brief Cast the field into a variant type. This is useful to extract data from the field.
		template <typename T>
		T const* get_as() const {
			return std::get_if<T>(&inner);
		}

		/// @brief Convenience function to get the function prototype of the field, if the field is a function of some sort.
		inline MemberFunctionProto* get_fn() {
			if (auto fn = get_as<FunctionBindField>()) {
				return &fn->prototype;
			} else return nullptr;
		}
	};

	/// @brief A top-level class declaration.
	struct Class {
		Attributes attributes;
		std::string name; ///< The name of the class.
		std::vector<std::string> superclasses; ///< Parent classes that the current class inherits.
										  ///< This includes parent classes, and any classes declared in a `[[depends(...)]]` attribute.
		std::vector<Field> fields; ///< All the fields parsed in the class.

		inline bool operator==(Class const& c) const {
			return name == c.name;
		}
		inline bool operator==(std::string const& n) const {
			return name == n;
		}
	};

	/// @brief A top-level free function binding.
	struct Function {
		FunctionProto prototype; ///< The free function's signature.
		PlatformNumber binds; ///< The offsets of free function, separated per platform.
		std::string inner; ///< The (optional) inline body of the function as a raw string.
	};

	/// @brief Broma's root grammar (the root AST).
	///
	/// See the user's guide for an example on how to traverse this AST.
	struct Root {
		std::vector<Class> classes;
		std::vector<Function> functions;

		inline Class* operator[](std::string const& name) {
			auto it = std::find_if(classes.begin(), classes.end(), [name](Class& cls) {
					return cls.name == name;
			});

			if (it == classes.end())
				return nullptr;

			return &*it;
		}
	};
} // namespace broma

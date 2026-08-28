// taken from geode::utils::string
// https://github.com/geode-sdk/geode/blob/main/loader/src/utils/string.cpp
#pragma once

#include <filesystem>
#include <string>

namespace broma {
	namespace paths {
		#ifdef _WIN32

		#include <Windows.h>
		#include <stringapiset.h>

		inline std::string wideToUtf8(std::wstring_view wstr) {
			int count = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wstr.size(), NULL, 0, NULL, NULL);
			std::string str(count, 0);
			WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wstr.size(), &str[0], count, NULL, NULL);
			return str;
		}

		#endif

		inline std::string pathToString(std::filesystem::path const& path) {
		#ifdef _WIN32
			return wideToUtf8(path.native());
		#else
			return path.string();
		#endif
		}

		inline std::string canonicalize(std::filesystem::path const& path) {
			// error handling lets us keep stuff like the default '<string>' as source name
			std::error_code ec;
			auto canon = std::filesystem::weakly_canonical(path, ec);
			return pathToString(ec ? path : canon);
		}
	}
} // namespace broma

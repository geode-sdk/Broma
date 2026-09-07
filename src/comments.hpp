#pragma once

#include <tao/pegtl.hpp>
using namespace tao::pegtl;

namespace broma {
    /// @brief C and C++-style comments.
    struct comment_expr_base : sor<
        seq<at<ascii::string<'/', '/'>, not_at<one<'/'>>>, until<eolf>>,
        seq<ascii::string<'/', '*'>, until<ascii::string<'*', '/'>>>
    > {};

    /// @brief A parsed comment, with metadata about its type and position.
    struct ParsedComment {
        std::string text; ///< The raw comment string.
        bool multiline = false; ///< Whether this was a `/* ... */` style comment, as opposed to a `//` one.
        bool trailing = false; ///< Whether the comment follows other code on the same line.
    };

    template <typename T>
    inline ParsedComment extract_comment(T& input) {
        ParsedComment out;
        std::string raw = input.string();

        out.multiline = raw.size() >= 2 && raw[0] == '/' && raw[1] == '*';

        // check if there's any non-whitespace characters before the comment on the same line
        auto pos = input.begin();
        auto file_begin = input.input().begin();
        while (pos != file_begin) {
            --pos;
            if (*pos == '\n') break;
            if (*pos != ' ' && *pos != '\t' && *pos != '\r') {
                out.trailing = true;
                break;
            }
        }

        // strip the comment delimiters
        if (out.multiline) {
            raw.erase(0, 2);
            if (raw.size() >= 2) {
                raw.erase(raw.size() - 2);
            }
        } else {
            raw.erase(0, 2);
        }

        // strip whitespace for single-line comments
        if (!out.multiline) {
            auto first = raw.find_first_not_of(" \t\r");
            if (first == std::string::npos) {
                raw.clear();
            } else {
                auto last = raw.find_last_not_of(" \t\r\n");
                raw = raw.substr(first, last - first + 1);
            }
        }

        out.text = std::move(raw);
        return out;
    }
}
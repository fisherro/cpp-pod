#include "libx.hpp"
#include <algorithm>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace abbreviations:

namespace s = std;
namespace r = std::ranges;
namespace v = std::views;

///////////////////////////////////////////////////////////////////////////////
// Line makes it easy to read lines from an istream.

struct Line {
    s::string text;
};

s::ostream& operator<<(s::ostream& out, const Line& line)
{
    out << line.text;
    return out;
}

s::istream& operator>>(s::istream& in, Line& line)
{
    s::getline(in, line.text);
    return in;
}

///////////////////////////////////////////////////////////////////////////////

int main(const int argc, const char** argv)
{
    // Turn argv into a vector:
    s::vector<s::string_view> args(argv + 1, argv + argc);

    if (args.size() < 2) {
        x::println(s::cerr, "Usage: {} pattern file...", argv[0]);
        return EXIT_FAILURE;
    }

    // Create regex:
    // Can't construct regex from a string_view, so use iterators.
    s::regex pattern = s::regex(begin(args[0]), end(args[0]));

    auto files = r::subrange(args.begin() + 1, args.end());
    auto lines = files | v::transform(
            [](s::string_view file)
            {
                auto stream = s::make_shared<s::ifstream>(s::string(file));
                return v::zip(
                        v::repeat(file),
                        v::repeat(stream), //needed?
                        v::iota(1),
                        r::istream_view<Line>(*stream)
                        | v::transform(&Line::text));
            });
    auto joined = lines | v::join;
#if 0
    auto filtered = joined | v::filter(
            [&pattern](auto tuple)
            {
                auto [file, stream, number, text] = tuple;
                return s::regex_search(text, pattern);
            });
    auto output = filtered | v::transform(
#endif
    auto output = joined | v::transform(
            [](auto tuple)
            {
                auto [file, _, number, text] = tuple;
                return s::format("{}({}): {}", file, number, text);
            });
    r::copy(output,
            s::ostream_iterator<s::string>(s::cout, "\n"));
#if 0
    // If a file name is given, open it; otherwise, use stdin.
    s::ifstream fin;
    if (args.size() > 1) {
        // Can't pass a string_view, so convert it to a string.
        fin.open(s::string{args[1]});
    }
    s::istream& in = fin.is_open()? fin: s::cin;

    // A helper function for the filter view below.
    auto match = [&pattern](const auto& tuple)
    {
        const auto& [number, line] = tuple;
        return s::regex_search(line, pattern);
    };

    // Turn an <int, string> tuple-like object into a string:
    auto stringify = [](const auto& tuple)
    {
        const auto& [number, line] = tuple;
        return s::format("{}: {}", number + 1, line);
    };

    // Do the work and write it to stdout:
    r::copy(r::istream_view<Line>(in)   // Read lines
            | v::transform(&Line::text) // Turn line objects into strings
            | v::enumerate              // Number the lines
            | v::filter(match)          // Do the regex match
            | v::transform(stringify),  // Format the output
            s::ostream_iterator<s::string>(s::cout, "\n"));
#endif
    return EXIT_SUCCESS;
}


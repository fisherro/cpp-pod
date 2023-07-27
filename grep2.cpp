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
    auto filtered = joined | v::filter(
            [&pattern](auto tuple)
            {
                auto [file, stream, number, text] = tuple;
                return s::regex_search(text, pattern);
            });
    auto output = filtered | v::transform(
            [](auto tuple)
            {
                auto [file, _, number, text] = tuple;
                return s::format("{}({}): {}", file, number, text);
            });
    r::copy(output,
            s::ostream_iterator<s::string>(s::cout, "\n"));

    return EXIT_SUCCESS;
}


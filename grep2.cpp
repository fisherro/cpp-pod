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
    s::vector<s::string_view> args(argv + 1, argv + argc);

    if (args.size() < 2) {
        x::println(s::cerr, "Usage: {} pattern file...", argv[0]);
        return EXIT_FAILURE;
    }

    s::regex pattern = s::regex(begin(args[0]), end(args[0]));

    r::copy(r::subrange(args.begin() + 1, args.end())
            | v::transform(
                [](s::string_view file)
                {
                auto stream = s::make_shared<s::ifstream>(s::string(file));
                return v::zip(
                        v::repeat(file),
                        v::repeat(stream), //needed?
                        v::iota(1),
                        r::istream_view<Line>(*stream)
                        | v::transform(&Line::text));
                })
            | v::join
            | v::filter(
                [&pattern](auto tuple)
                {
                [[maybe_unused]] auto [file, stream, number, text] = tuple;
                return s::regex_search(text, pattern);
                })
            | v::transform(
                [](auto tuple)
                {
                auto [file, _, number, text] = tuple;
                return s::format("{}({}): {}", file, number, text);
                }),
                s::ostream_iterator<s::string>(s::cout, "\n"));

            return EXIT_SUCCESS;
}


#include "libx.hpp"
#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace s = std;
namespace r = std::ranges;
namespace v = std::views;

struct Line {
    std::string text;
};

std::ostream& operator<<(std::ostream& out, const Line& line)
{
    out << line.text;
    return out;
}

std::istream& operator>>(std::istream& in, Line& line)
{
    std::getline(in, line.text);
    return in;
}

int main(const int argc, const char** argv)
{
    s::vector<s::string_view> args(argv + 1, argv + argc);

    s::regex pattern{"."};
    if (args.size() > 0) {
        pattern = s::regex(begin(args[0]), end(args[0]));
    }

    s::ifstream fin;
    if (args.size() > 1) {
        fin.open(s::string{args[1]});
    }
    s::istream& in = fin.is_open()? fin: s::cin;

    auto match = [&pattern](const auto& tuple)
    {
        const auto& [number, line] = tuple;
        return s::regex_search(line.text, pattern);
    };

    auto stringify = [](const auto& tuple)
    {
        const auto& [number, line] = tuple;
        return s::format("{}: {}", number + 1, line.text);
    };

    r::copy(r::istream_view<Line>(in)
            | v::enumerate
            | v::filter(match)
            | v::transform(stringify),
            s::ostream_iterator<s::string>(s::cout, "\n"));
}


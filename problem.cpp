#include <algorithm>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <sstream>
#include <format>

struct Line {
    std::string s;
};

std::istream& operator>>(std::istream& in, Line& line)
{
    std::getline(in, line.s);
    return in;
}

std::ostream& operator<<(std::ostream& out, const Line& line)
{
    out << line.s;
    return out;
}

void print_view(std::string_view name, std::ranges::viewable_range auto view)
{
    bool found = false;
    int i{1};
    for (const auto& item: view) {
        found = true;
        if constexpr (requires { std::cout << item; }) {
            std::cout << name << ": \"" << item << "\"\n";
        } else if constexpr (std::ranges::viewable_range<decltype(item)>) {
            print_view(std::format("{}[{}]", name, i), item);
            ++i;
        } else {
            std::cout << "I can't output that!\n";
        }
    }
    if (not found) std::cout << name << " is empty\n";
}

int main()
{
    std::istringstream in{"this\nis\na\ntest"};
    in.unsetf(std::ios_base::skipws);
    auto raw = std::ranges::istream_view<Line>(in);
    print_view("raw", raw);
#if 0
    auto split = raw | std::views::lazy_split('\n');
    //Why is split empty?
    //It works if I don't do it as a separate step.
    //It works if I don't print raw.
    print_view("split", split);
#endif
}


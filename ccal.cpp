#include "libx.hpp"

#include <iostream>
#include <ranges>
#include <iterator>
#include <algorithm>
#include <format>
#include <chrono>
#include <array>
#include <functional>

using namespace std::literals;

namespace s = std;
namespace c = std::chrono;
namespace r = std::ranges;
namespace v = std::views;

// Should use a concept for the view
void print_view(auto view, s::string_view delimiter = "")
{
    for (const auto& element: view) {
        x::print("{}{}", element, delimiter);
    }
    x::println("");
}

// Pad a string with spaces on the left to fit the given width.
// e.g. lpad("x", 5) => "    x"
s::string lpad(s::string_view sv, int width)
{
    return s::format("{:>{}}", sv, width);
}

int main()
{
    const c::year_month_day today{
        c::floor<c::days>(
                c::system_clock::now())};
    const c::weekday weekday{c::sys_days{today.year()/today.month()/1d}};
    const unsigned day_count {
        c::year_month_day_last{today.year()/today.month()/c::last}.day()};
    x::println("{:^21%B %Y}", today);
    const auto days = v::iota( 1U , day_count + 1 )
        | v::transform([](auto x){ return s::to_string(x); })
        | v::transform(x::bind_back(lpad, 3));
    s::vector<s::string> days_padded(weekday.c_encoding(), "   ");
    r::copy(days, s::back_inserter(days_padded));
    print_view(days_padded | v::chunk(7) | v::join_with("\n"));
}


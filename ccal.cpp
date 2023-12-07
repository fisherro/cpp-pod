// Note that this has improvements over cal.cpp

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
    x::println("{:^20%B %Y}", today);

    r::copy(v::iota(0U, 7U)
            | v::transform([](unsigned i){ return c::weekday{i}; })
            | v::transform([](c::weekday wd){ return s::format("{:%A}", wd); })
            | v::transform([](s::string s){ return s::format("{:.2}", s); })
            | v::join_with(' '),
            s::ostream_iterator<char>(s::cout));
    x::println("");

    const auto days = v::iota(1U , day_count + 1)
        | v::transform([](auto x){ return s::to_string(x); })
        | v::transform(x::bind_back(lpad, 2));
    s::vector<s::string> days_padded(weekday.c_encoding(), "  ");
    r::copy(days, s::back_inserter(days_padded));
    r::copy(days_padded
            | v::chunk(7)
            | v::transform([](auto r){ return r | v::join_with(' '); })
            | v::join_with('\n'),
            s::ostream_iterator<char>(s::cout));
    x::println("");
}


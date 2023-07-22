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

// Pad the given string out, right-aligned, within 3 spaces
// e.g. pad("5") => "  5"
s::string pad(s::string_view sv, int width)
{
    return s::format("{:>{}}", sv, width);
}

// Convert an int to a string
s::string itos(int n)
{
    return s::to_string(n);
}

void compact();

int main()
{
    // Get today's date:
    const c::year_month_day today{
        c::floor<c::days>(
                c::system_clock::now()
                )};

    // Get the first and last day of the month: (type = c::year_month_day)
    const auto first    { today.year() / today.month() / 1d      };
    const auto last_day { today.year() / today.month() / c::last };
    const auto day_count = static_cast<unsigned>(last_day.day());

    // Get the day-of-the-week for the first day of the month:
    const c::weekday weekday{ c::sys_days{first} };
    // Print the month and year as a title, centered:
    x::println("{:^21%B %Y}", today);
    // Create a range of ints from 1 to last_day:
    const auto ns = v::iota( 1U , day_count + 1 );
    // Convert the ints to strings and pad them out whith spaces:
    const auto strs = ns | v::transform(itos) |
        v::transform([](const auto& x){return pad(x, 3);});
    // x::bind_back(pad, 3)
    // Create a range of spaces to represent the "empty" days
    // at the beginning of the month:
    const auto blanks = v::repeat("   "s, weekday.c_encoding());
    // We don't yet have r::concat.
    // So we convert strs and blanks into vectors.
    // We put them together in an array.
    // Then we use v::join to concatenate them.
    // There may be a better way to do this.
#if 1
    s::vector<s::string> bstrs(r::begin(blanks), r::end(blanks));
#else
    // C++23 should make this possible:
    s::vector<s::string> bstrs(s::from_range_t, blanks);
#endif
    s::vector<s::string> vstrs(r::begin(strs), r::end(strs));
    auto a = s::array{bstrs, vstrs};
    auto combined = v::join(a);
    // Split the combined array into chunks of 7:
    auto chunks = combined | v::chunk(7);
    // Join the chunks back together with newline as a delimiter:
    auto joined = chunks | v::join_with("\n");
    // Print the result:
    print_view(joined);
    compact();
    auto f = s::bind_front(pad, "x");
    auto foutput = f(3);
    x::println("\"{}\"", foutput);
#if 0
    auto g = x::bind_back(pad, 3);
    auto goutput = g("y");
    x::println("\"{}\"", goutput);
#endif
}

void compact()
{
    const c::year_month_day today{
        c::floor<c::days>(
                c::system_clock::now()
                )};
    const c::weekday weekday{c::sys_days{today.year()/today.month()/1d}};
    const unsigned day_count {
        c::year_month_day_last{today.year()/today.month()/c::last}.day()};
    x::println("{:^21%B %Y}", today);
    const auto strs = v::iota( 1U , day_count + 1 )
        | v::transform(itos)
        | v::transform([](const auto& x){return pad(x, 3);});
    const auto blanks = v::repeat("   "s, weekday.c_encoding());
    s::vector<s::string> bstrs(r::begin(blanks), r::end(blanks));
    s::vector<s::string> vstrs(r::begin(strs), r::end(strs));
    auto a = s::array{bstrs, vstrs};
    auto joined = v::join(a) | v::chunk(7) | v::join_with("\n");
    print_view(joined);
}


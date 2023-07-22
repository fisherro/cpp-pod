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

void compact();

int main()
{
    // Get today's date:
    const auto now = c::system_clock::now();
    const auto days = c::floor<c::days>(now);
    const c::year_month_day today{days};

    // Get the first and last day of the month: (type = c::year_month_day)
    const auto first    { today.year() / today.month() / 1d      };
    const auto last_day { today.year() / today.month() / c::last };

    // Get the number of days in this month:
    const auto day_count = static_cast<unsigned>(last_day.day());

    // Get the day-of-the-week for the first day of the month:
    const c::weekday weekday{ c::sys_days{first} };

    // Print the month and year as a title, centered:
    x::println("{:^21%B %Y}", today);

    // Create a range of ints from 1 to last_day:
    const auto ns = v::iota( 1U , day_count + 1 );

    // Select the to_string overload for ints:
    auto itos = static_cast<s::string(*)(int)>(s::to_string);

    // Convert the ints to strings and pad them out whith spaces:
    const auto strs = ns | v::transform(itos) |
        v::transform(x::bind_back(lpad, 3));
    
    // Create a range of spaces to represent the "empty" days
    // at the beginning of the month:
    const auto blanks = v::repeat("   "s, weekday.c_encoding());

    // In the future, we should get concat that will concatenate two views.
    // So, we'll use a vector instead:

    s::vector<s::string> combined(r::begin(blanks), r::end(blanks));
    r::copy(strs, s::back_inserter(combined));

#if 0
    // In C++23, we should also get these options:
    s::vector<s::string> combined(s::from_range_t, blanks);
    combined.append_range(strs);
#endif

    // Split the combined array into chunks of 7:
    auto chunks = combined | v::chunk(7);

    // Join the chunks back together with newline as a delimiter:
    auto joined = chunks | v::join_with("\n");

    // Print the result:
    print_view(joined);
    compact();
}

void compact()
{
#if 0
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
        | v::transform([](const auto& x){return lpad(x, 3);});
    const auto blanks = v::repeat("   "s, weekday.c_encoding());
    s::vector<s::string> bstrs(r::begin(blanks), r::end(blanks));
    s::vector<s::string> vstrs(r::begin(strs), r::end(strs));
    auto a = s::array{bstrs, vstrs};
    auto joined = v::join(a) | v::chunk(7) | v::join_with("\n");
    print_view(joined);
#endif
}


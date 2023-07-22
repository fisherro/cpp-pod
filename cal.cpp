
#include <iostream>
#include <ranges>
#include <iterator>
#include <algorithm>
#include <format>
#include <chrono>
#include <array>
using namespace std::literals;
// Abbreviated namespaces for compactness in Godbolt
namespace s = std;
namespace c = std::chrono;
namespace r = std::ranges;
namespace v = std::views;
decltype(auto) bind_front(auto&& f, auto&&... frontArgs)
{
    return [f=std::forward<decltype(f)>(f), 
            frontArgs = std::make_tuple(std::forward<decltype(frontArgs)>(frontArgs)...)]
            (auto&&...backArgs) 
        {
            return std::apply(
                f,
                std::tuple_cat(
                    std::forward<decltype(frontArgs)>(frontArgs),
                    std::forward_as_tuple(backArgs...)));
        };
}
decltype(auto) bind_back(auto&& f, auto&&... backArgs)
{
    return [f = std::forward<decltype(f)>(f),
            backArgs = std::make_tuple(
                std::forward<decltype(backArgs)>(backArgs)...)]
            (auto&&... frontArgs)
        {
            return std::apply(
                f,
                std::tuple_cat(
                    std::forward_as_tuple(frontArgs...),
                    std::forward<decltype(backArgs)>(backArgs)));
        };
}
// C++23 function that is not yet available:
template <typename... Args>
void print(std::format_string<Args...> fmt, Args&&... args)
{
    s::cout << s::format(fmt, args...);
}
// C++23 function that is not yet available:
template <typename... Args>
void println(std::format_string<Args...> fmt, Args&&... args)
{
    s::cout << s::format(fmt, args...) << '\n';
}
// Should use a concept for the view
void print_view(auto view, s::string_view delimiter = "")
{
    for (const auto& element: view) {
        print("{}{}", element, delimiter);
    }
    println("");
}
// Pad the given string out, right-aligned, within 3 spaces
// e.g. pad("5") => "  5"
std::string pad(std::string_view sv, int width)
{
    return s::format("{:>{}}", sv, width);
}
// Convert an int to a string
std::string itos(int n)
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
    println("{:^21%B %Y}", today);
    // Create a range of ints from 1 to last_day:
    const auto ns = v::iota( 1U , day_count + 1 );
    // Convert the ints to strings and pad them out whith spaces:
    const auto strs = ns | v::transform(itos) |
        v::transform([](const auto& x){return pad(x, 3);});
    // s::bind_back(pad, 3)
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
    auto f = bind_front(pad, "x");
    auto foutput = f(3);
    println("\"{}\"", foutput);
#if 0
    auto g = bind_back(pad, 3);
    auto goutput = g("y");
    println("\"{}\"", goutput);
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
    println("{:^21%B %Y}", today);
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


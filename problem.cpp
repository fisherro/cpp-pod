#include <algorithm>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <sstream>
#include <format>

void print_view(std::string_view name, std::ranges::viewable_range auto view)
{
    bool found = false;
    int i{1};
    for (const auto& item: view) {
        found = true;
        if constexpr (requires { std::cout << item; }) {
            std::cout << std::format("{}: \"{}\"\n", name, item);
        } else if constexpr (std::ranges::viewable_range<decltype(item)>) {
            print_view(std::format("{}[{}]", name, i), item);
            ++i;
        } else {
#if 0
            std::cout << "I can't output that!\n";
#else
            // A var-arg template can << a tuple
            auto [a, b] = item;
            std::cout << std::format("{}: <\"{}\", \"{}\">\n", name, a, b);
#endif
        }
    }
    if (not found) std::cout << name << " is empty\n";
}

int main()
{
    std::istringstream in{
        "There will be a test.\n"
            "This is a test.\n"
            "That was a test.\n"
    };
    in.unsetf(std::ios_base::skipws);

    auto raw = std::ranges::istream_view<char>(in);
#if 0
    print_view("raw", raw);
#endif

    // Views::split requires a forward_range; istream_view is an input_range.
    // Views::lazy_split can take an input_range.
    auto split = raw | std::views::lazy_split('\n');
    // If raw was printed above, split will be empty.
    // If raw wasn't printed above, all is well.
#if 0
    print_view("split", split);
#endif

    auto strings = split | std::views::transform([](auto range)
            {
            std::ranges::common_view cv(range);
            return std::string(std::ranges::begin(cv), std::ranges::end(cv));
            });
#if 0
    print_view("strings", strings);
#endif

    auto enumerated = strings | std::views::enumerate;
#if 0
    print_view("enumerated", enumerated);
#endif

    // The strings are lost...even if we never called print_view.
    auto filtered = enumerated | std::views::filter([](auto tuple)
            {
            auto [number, text] = tuple;
            return number % 2;
            });
    print_view("filtered", filtered);
}


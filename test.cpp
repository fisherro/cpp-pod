#include "libx.hpp"
#include <vector>
#include <string_view>
#include <functional>

using namespace std::literals;

int main(int argc, const char **argv)
{
	auto fooit = std::bind_front(std::plus<std::string>{}, "foo"s);
	auto barit = x::bind_back(std::plus<std::string>{}, "bar"s);
	x::println("{} -> {}", fooit("test"), barit("test"));

	if (argc < 2) {
		x::println("No parameters given");
		return 0;
	} else {
		std::vector<std::string_view> args{argv + 1, argv + argc};
		for (auto arg: args) {
			x::println("\t{}", arg);
		}
	}
}

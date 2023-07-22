// A library for things that are standardized or being standardized but that
// aren't in GCC's library yet.

#pragma once
#include <format>
#include <iostream>
#include <tuple>
#include <utility>

namespace x {

#if 0 // Now available
      // C++20
    decltype(auto) bind_front(auto&& f, auto&&... frontArgs)
    {
        return [
            f=std::forward<decltype(f)>(f), 
            frontArgs =
                std::make_tuple(
                        std::forward<decltype(frontArgs)>(frontArgs)...)]
                (auto&&...backArgs) 
                {
                    return std::apply(
                            f,
                            std::tuple_cat(
                                std::forward<decltype(frontArgs)>(frontArgs),
                                std::forward_as_tuple(backArgs...)));
                };
    }
#endif

    // C++23
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

    // C++23
    template <typename... Args>
        void print(std::ostream& s, std::format_string<Args...> fmt, Args&&... args)
        {
            s << std::format(fmt, std::forward<decltype(args)>(args)...);
        }

    // C++23
    template <typename... Args>
        void print(std::format_string<Args...> fmt, Args&&... args)
        {
            print(std::cout, fmt, std::forward<decltype(args)>(args)...);
        }

    // C++23
    template <typename... Args>
        void print(std::FILE* f, std::format_string<Args...> fmt, Args&&... args)
        {
            std::string s = std::format(fmt, args...);
            std::fwrite(s.c_str(), 1, s.size(), f);
        }

    // C++23
    template <typename... Args>
        void println(std::ostream& s, std::format_string<Args...> fmt, Args&&... args)
        {
            print(fmt, std::forward<decltype(args)>(args)...);
            s << '\n';
        }

    // C++23
    template <typename... Args>
        void println(std::format_string<Args...> fmt, Args&&... args)
        {
            println(std::cout, fmt, std::forward<decltype(args)>(args)...);
        }

    // C++23
    template <typename... Args>
        void println(std::FILE* f, std::format_string<Args...> fmt, Args&&... args)
        {
            print(f, fmt, std::forward<decltype(args)>(args)...);
            std::fputc('\n', f);
        }
}


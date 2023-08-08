#include <iostream>
#include <utility>
#include <format>
#include <cxxabi.h>

#define NO_COPIES

#if 0
std::ostream& debug = std::clog;
#else
std::ostream& debug = std::cout;
#endif

struct movable {
    static inline int counter{1};
    const int serial_number{-1};
    bool specified{true};

    std::string class_name() const
    {
        int status;
        char* demangled = abi::__cxa_demangle(typeid(*this).name(), 0, 0, &status);
        std::string output(demangled);
        free(demangled);
        return output;
    }

    void dump(std::string_view message) const
    {
        debug << std::format("{}({},{}): {}\n",
                class_name(), serial_number, specified, message);
    }

    movable(): serial_number{counter++} { dump("CTOR"); }

#ifndef NO_COPIES
    movable(const movable& that): serial_number{counter++}
    {
        if (not that.specified) {
            debug << "Copying from unspecified object!\n";
        }
        that.dump("CTOR COPIED FROM");
        dump("COPY CTOR");
    }

    movable& operator=(const movable& that)
    {
        if (not that.specified) {
            debug << "Copying from unspecified object!\n";
        }
        specified = true;
        that.dump("OP= COPIED FROM");
        dump("COPY OP=");
        return *this;
    }
#endif

    movable(movable&& that): serial_number{counter++}
    {
        if (not that.specified) {
            debug << "Moving from unspecified object!\n";
        }
        that.specified = false;
        that.dump("CTOR MOVED FROM");
        dump("MOVE CTOR");
    }

    movable& operator=(movable&& that)
    {
        if (not that.specified) {
            debug << "Moving from unspecified object!\n";
        }
        that.specified = false;
        specified = true;
        that.dump("OP= MOVED FROM");
        dump("MOVE OP=");
        return *this;
    }

    ~movable() { dump("DTOR"); }
};

std::ostream& operator<<(std::ostream& out, const movable&)
{ out << "OUTPUT\n"; return out; }

struct move_holder {
    movable m;
    move_holder() = default;
    move_holder(movable&& m): m(std::move(m)) {}
    movable get() && { return std::move(m); }
    void set(movable&& m) { this->m = std::move(m); }
};

int main()
{
    {
        // Move in from a temporary via ctor; move out via ctor.
        std::cout << "TEST 1\n";
        move_holder holder{movable{}};
        movable m = std::move(holder).get();
        std::cout << m;
    }
    {
        // Move in from a named variable via ctor; move out via ctor.
        std::cout << "TEST 2\n";
        movable m1;
        move_holder holder{std::move(m1)};
        movable m2 = std::move(holder).get();
        std::cout << m2;
    }
    {
        // Move in from a temporary via a member function; move out via ctor.
        std::cout << "TEST 3\n";
        move_holder holder;
        holder.set(movable{});
        movable m = std::move(holder).get();
        std::cout << m;
    }
    {
        // Move in from a named variable via a member function;
        // move out via ctor.
        std::cout << "TEST 4\n";
        movable m1;
        move_holder holder;
        holder.set(std::move(m1));
        movable m2 = std::move(holder).get();
        std::cout << m2;
    }
    {
        // Move in from a named variable via ctor; move out to same variable.
        std::cout << "TEST 5\n";
        movable m;
        // The variable m is in a specified state.
        move_holder holder{std::move(m)};
        // The variable m is in an "unspecified" state.
        m = std::move(holder).get();
        // The variable m is in a specified state.
        std::cout << m;
    }
}

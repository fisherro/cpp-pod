CXXFLAGS+=-std=c++23 -Werror -Wpedantic -Wall -Wextra

progs=test cal ccal grep grep2 grep2-debug problem movable

all: $(progs)

clean:
	-rm $(progs)

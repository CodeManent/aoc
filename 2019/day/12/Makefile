CXXFLAGS= -g -std=c++17 -Wall -Wextra -pedantic -Werror -fvar-tracking-assignments -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -ubsan -fsanitize=address -fno-omit-frame-pointer -Werror=format-security -Werror=array-bounds

all: run_day12

.PHONY: run_day12
run_day12: day12 input
	./day12 < input

clean:
	$(RM) day12
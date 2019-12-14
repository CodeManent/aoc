CXXFLAGS= -g -std=c++17 -Wall -Wextra -pedantic -Werror -fvar-tracking-assignments -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -ubsan -fsanitize=address -fno-omit-frame-pointer -Werror=format-security -Werror=array-bounds

all: run_day11

.PHONY: run_day11
run_day11: day11 input
	./day11 < input

clean:
	$(RM) day11 test
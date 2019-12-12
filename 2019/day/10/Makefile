CXXFLAGS= -g -std=c++17 -Wall -Wextra -pedantic -Werror -fvar-tracking-assignments -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -ubsan -fsanitize=address -fno-omit-frame-pointer -lasan -Werror=format-security -Werror=array-bounds

all: run_day10

.PHONY: run_day10
run_day10: day10 input
	./day10 < input

clean:
	$(RM) day10
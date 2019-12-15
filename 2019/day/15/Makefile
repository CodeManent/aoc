CXXFLAGS= -g -std=c++17 -Wall -Wextra -pedantic -Werror -fvar-tracking-assignments -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -ubsan -fsanitize=address -fno-omit-frame-pointer -Werror=format-security -Werror=array-bounds

all: run_day15

.PHONY: run_day15
run_day15: day15 input
	./day15 < input

clean:
	$(RM) day15

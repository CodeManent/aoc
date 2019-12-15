CXXFLAGS= -g -std=c++17 -Wall -Wextra -pedantic -Werror -fvar-tracking-assignments -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -ubsan -fsanitize=address -fno-omit-frame-pointer -Werror=format-security -Werror=array-bounds

all: run_day13

.PHONY: run_day13
run_day13: day13 input
	./day13 < input

clean:
	$(RM) day13

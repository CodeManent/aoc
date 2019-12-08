CXXFLAGS= -std=c++17 -Wall -Wextra -pedantic -Werror -fvar-tracking-assignments -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -ubsan -fsanitize=address -fno-omit-frame-pointer -lasan -Werror=format-security -Werror=array-bounds

all: run_day8

.PHONY: run_day8
run_day8: day8 input
	./day8 < input

clean:
	$(RM) day8